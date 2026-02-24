#!/usr/bin/env python3
"""Rune arrow direction detector.

Detects the four directional arrows in MapleStory rune activation screenshots
and determines their facing direction (up/down/left/right).

Two-stage approach:
  1. Locate arrow centers via the bright-green center square (tight HSV threshold,
     with fallback to a wider range for faint arrows).
  2. For each center, detect the red/orange arrowhead tip within a local ROI and
     compute direction from center → tip.

Usage:
    python main.py                    # process all images in images/
    python main.py path/to/image.png  # process specific image(s)
"""

import cv2
import numpy as np
import sys
from pathlib import Path

# ---------------------------------------------------------------------------
# Stage 1 – locate arrow centers
# ---------------------------------------------------------------------------
TIGHT_HSV_LOW = np.array([50, 200, 220])
TIGHT_HSV_HIGH = np.array([75, 255, 255])
WIDE_HSV_LOW = np.array([40, 150, 180])
WIDE_HSV_HIGH = np.array([80, 255, 255])

MIN_CENTER_AREA = 5
MAX_CENTER_AREA = 500
Y_FRAC_MIN = 0.4
X_MARGIN_FRAC = 0.07
Y_TOLERANCE = 50
MERGE_DIST = 40
MIN_GAP = 30
EXPECTED_ARROWS = 4

# ---------------------------------------------------------------------------
# Stage 2 – detect arrow direction via red/orange tip
# ---------------------------------------------------------------------------
ROI_PAD = 40
RED_RANGES = [
    (np.array([0, 100, 130]), np.array([12, 255, 255])),
    (np.array([168, 100, 130]), np.array([180, 255, 255])),
]
ORANGE_RANGE = (np.array([12, 120, 150]), np.array([22, 255, 255]))
MIN_TIP_PIXELS = 3


# ---------------------------------------------------------------------------
# Stage 1 implementation
# ---------------------------------------------------------------------------

def _extract_blobs(mask, y_min, x_min, x_max):
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    blobs = []
    for c in contours:
        area = cv2.contourArea(c)
        if MIN_CENTER_AREA <= area <= MAX_CENTER_AREA:
            M = cv2.moments(c)
            if M["m00"] == 0:
                continue
            cx = int(M["m10"] / M["m00"])
            cy = int(M["m01"] / M["m00"])
            if cy >= y_min and x_min < cx < x_max:
                blobs.append((cx, cy, int(area)))
    return blobs


def find_arrow_centers(image):
    """Return list of (cx, cy, area) for the 4 arrow centers, sorted L→R."""
    h, w = image.shape[:2]
    y_min = int(h * Y_FRAC_MIN)
    x_margin = max(50, int(w * X_MARGIN_FRAC))
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))

    tight = cv2.morphologyEx(
        cv2.inRange(hsv, TIGHT_HSV_LOW, TIGHT_HSV_HIGH), cv2.MORPH_CLOSE, kernel
    )
    result = _extract_blobs(tight, y_min, x_margin, w - x_margin)

    if len(result) < EXPECTED_ARROWS:
        anchor_y = int(np.median([b[1] for b in result])) if result else int(h * 0.65)
        wide = cv2.morphologyEx(
            cv2.inRange(hsv, WIDE_HSV_LOW, WIDE_HSV_HIGH), cv2.MORPH_CLOSE, kernel
        )
        candidates = _extract_blobs(wide, y_min, x_margin, w - x_margin)
        candidates = [
            b
            for b in candidates
            if abs(b[1] - anchor_y) <= Y_TOLERANCE
            and not any(
                abs(b[0] - e[0]) < MERGE_DIST and abs(b[1] - e[1]) < MERGE_DIST
                for e in result
            )
        ]
        candidates.sort(key=lambda b: b[2], reverse=True)
        for b in candidates:
            if len(result) >= EXPECTED_ARROWS:
                break
            if all(abs(b[0] - e[0]) >= MIN_GAP for e in result):
                result.append(b)

    result.sort(key=lambda b: b[0])
    return result


# ---------------------------------------------------------------------------
# Stage 2 implementation
# ---------------------------------------------------------------------------

def detect_direction(image, cx, cy, pad=ROI_PAD):
    """Determine arrow direction by locating the red/orange tip relative to center."""
    h, w = image.shape[:2]
    x1, y1 = max(0, cx - pad), max(0, cy - pad)
    x2, y2 = min(w, cx + pad), min(h, cy + pad)

    roi = image[y1:y2, x1:x2]
    hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)

    tip_mask = np.zeros(roi.shape[:2], dtype=np.uint8)
    for lo, hi in RED_RANGES:
        tip_mask |= cv2.inRange(hsv, lo, hi)
    tip_mask |= cv2.inRange(hsv, ORANGE_RANGE[0], ORANGE_RANGE[1])

    pts = np.where(tip_mask > 0)
    if len(pts[0]) < MIN_TIP_PIXELS:
        return "unknown"

    tip_y = np.mean(pts[0])
    tip_x = np.mean(pts[1])
    roi_cy = cy - y1
    roi_cx = cx - x1

    dx = tip_x - roi_cx
    dy = tip_y - roi_cy

    if abs(dx) > abs(dy):
        return "right" if dx > 0 else "left"
    else:
        return "down" if dy > 0 else "up"


# ---------------------------------------------------------------------------
# Pipeline
# ---------------------------------------------------------------------------

def process_image(image_path, debug_dir=None):
    """Process one image; return list of direction strings."""
    img = cv2.imread(str(image_path))
    if img is None:
        print(f"  ERROR: could not load {image_path}")
        return []

    centers = find_arrow_centers(img)
    dirs = [detect_direction(img, cx, cy) for cx, cy, _ in centers]

    if debug_dir is not None:
        _save_debug(img, centers, dirs, image_path, debug_dir)

    return dirs


def _save_debug(img, centers, dirs, image_path, debug_dir):
    out = Path(debug_dir)
    out.mkdir(exist_ok=True)
    stem = Path(image_path).stem
    debug = img.copy()

    for i, (cx, cy, _) in enumerate(centers):
        h, w = img.shape[:2]
        pad = ROI_PAD
        x1, y1 = max(0, cx - pad), max(0, cy - pad)
        x2, y2 = min(w, cx + pad), min(h, cy + pad)

        roi = img[y1:y2, x1:x2]
        hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
        tip_mask = np.zeros(roi.shape[:2], dtype=np.uint8)
        for lo, hi in RED_RANGES:
            tip_mask |= cv2.inRange(hsv, lo, hi)
        tip_mask |= cv2.inRange(hsv, ORANGE_RANGE[0], ORANGE_RANGE[1])

        pts = np.where(tip_mask > 0)
        cv2.circle(debug, (cx, cy), 4, (0, 255, 0), -1)
        cv2.rectangle(debug, (x1, y1), (x2, y2), (128, 128, 128), 1)

        if len(pts[0]) >= MIN_TIP_PIXELS:
            tip_y = int(np.mean(pts[0])) + y1
            tip_x = int(np.mean(pts[1])) + x1
            cv2.circle(debug, (tip_x, tip_y), 4, (0, 0, 255), -1)
            cv2.arrowedLine(debug, (cx, cy), (tip_x, tip_y), (255, 255, 0), 2)

        label = dirs[i] if i < len(dirs) else "?"
        cv2.putText(
            debug, f"{i}:{label}", (cx - 25, y1 - 5),
            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2,
        )

    cv2.imwrite(str(out / f"{stem}_debug.png"), debug)


def main():
    here = Path(__file__).parent
    images_dir = here / "images"
    debug_dir = here / "debug"

    if len(sys.argv) > 1:
        paths = [Path(p) for p in sys.argv[1:]]
    else:
        paths = sorted(
            images_dir.glob("*.png"),
            key=lambda p: int(p.stem) if p.stem.isdigit() else 0,
        )

    if not paths:
        print("No images found. Place .png files in images/ or pass paths as arguments.")
        return

    for p in paths:
        dirs = process_image(p, debug_dir=debug_dir)
        arrow_str = ", ".join(dirs) if dirs else "(none detected)"
        print(f"{p.name:>10s}: [{arrow_str}]")


if __name__ == "__main__":
    main()
