"""Shared screen-capture and template-matching utilities."""

from pathlib import Path

import cv2
import dxcam
import numpy as np

_IMAGES_DIR = Path(__file__).parent / "images"

DEFAULT_THRESHOLD = 0.8

_camera = dxcam.create(output_color="BGR")


def grab_screen(grayscale: bool = False) -> np.ndarray:
    """Capture the full screen. Returns grayscale if requested, BGR otherwise."""
    frame = _camera.grab()
    while frame is None:
        frame = _camera.grab()
    if grayscale:
        return cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    return frame


def grab_region(region: dict, grayscale: bool = False) -> np.ndarray:
    """Capture a specific region (dict with top/left/width/height)."""
    left, top = region["left"], region["top"]
    r = (left, top, left + region["width"], top + region["height"])
    frame = _camera.grab(region=r)
    while frame is None:
        frame = _camera.grab(region=r)
    if grayscale:
        return cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    return frame


def match_template(frame: np.ndarray, template: np.ndarray, threshold: float = DEFAULT_THRESHOLD):
    """Return (cx, cy) centre of the best match, or None if below *threshold*."""
    result = cv2.matchTemplate(frame, template, cv2.TM_CCOEFF_NORMED)
    _, max_val, _, max_loc = cv2.minMaxLoc(result)
    if max_val < threshold:
        return None
    h, w = template.shape[:2]
    return (max_loc[0] + w // 2, max_loc[1] + h // 2)


def is_template_on_screen(template: np.ndarray, threshold: float = DEFAULT_THRESHOLD, grayscale: bool = False) -> bool:
    """Grab the full screen and return True if *template* is found."""
    if template is None:
        return False
    return match_template(grab_screen(grayscale=grayscale), template, threshold) is not None


def is_template_in_region(template: np.ndarray, region: dict, threshold: float = DEFAULT_THRESHOLD, grayscale: bool = False) -> bool:
    """Grab a region and return True if *template* is found."""
    if template is None:
        return False
    return match_template(grab_region(region, grayscale=grayscale), template, threshold) is not None


def load_template(name: str) -> np.ndarray:
    """Load an image from the shared images/ directory. Raises if missing."""
    path = _IMAGES_DIR / name
    tmpl = cv2.imread(str(path))
    if tmpl is None:
        raise FileNotFoundError(f"Missing {path}")
    return tmpl


# ---------------------------------------------------------------------------
# Screen detection helpers
# ---------------------------------------------------------------------------

_enfolding3_tmpl = load_template("enfolding3_teleport_reset.png")
def is_enfolding3_teleport_reset_on_screen(threshold: float = 0.75) -> bool:
    """Return True if the enfolding-3 teleport-reset icon is visible on screen."""
    ret = is_template_on_screen(_enfolding3_tmpl, threshold)
    if ret:
        print("    [is_enfolding3_teleport_reset_on_screen] found")
    else:
        print("    [is_enfolding3_teleport_reset_on_screen] not found")
    return ret
