"""RuneWalker — detect a rune on the minimap and navigate to it.

Uses OpenCV template-matching on an mss screen-grab instead of pyautogui.
"""

import math
from pathlib import Path

import cv2
import mss
import numpy as np

from runewalker.rune_abstract import RuneWalkerPilot
from keys import press, release
from timing import sleep_ms, is_stopped

_IMAGES_DIR = Path(__file__).parent / "images"

# Minimap defaults (top-left corner of the screen).
_DEFAULT_MINIMAP = {"top": 0, "left": 0, "width": 350, "height": 300}

_MATCH_THRESHOLD = 0.8
_MARGIN_PX = 5


class RuneWalker:
    def __init__(self, pilot: RuneWalkerPilot, minimap_region: dict | None = None):
        self.pilot = pilot
        self.minimap_region = minimap_region or dict(_DEFAULT_MINIMAP)
        self._move_seq = 0

        self._me_tmpl = cv2.imread(str(_IMAGES_DIR / "me_minimap.png"))
        self._rune_tmpl = cv2.imread(str(_IMAGES_DIR / "rune_minimap.png"))
        self._rune_buff_tmpl = cv2.imread(str(_IMAGES_DIR / "rune_buff.png"))
        if self._me_tmpl is None:
            raise FileNotFoundError(f"Missing {_IMAGES_DIR / 'me_minimap.png'}")
        if self._rune_tmpl is None:
            raise FileNotFoundError(f"Missing {_IMAGES_DIR / 'rune_minimap.png'}")
        if self._rune_buff_tmpl is None:
            raise FileNotFoundError(f"Missing {_IMAGES_DIR / 'rune_buff.png'}")

        self._sct: mss.mss | None = None

    # ------------------------------------------------------------------
    # Logging
    # ------------------------------------------------------------------

    @staticmethod
    def _log(msg: str):
        print(f"[RuneWalker] {msg}")

    # ------------------------------------------------------------------
    # Screen capture + template matching
    # ------------------------------------------------------------------

    def _get_sct(self) -> mss.mss:
        """Lazy-init so the GDI handle belongs to the calling thread."""
        if self._sct is None:
            self._sct = mss.mss()
        return self._sct

    def _grab_minimap(self) -> np.ndarray:
        raw = self._get_sct().grab(self.minimap_region)
        return cv2.cvtColor(np.array(raw), cv2.COLOR_BGRA2BGR)

    @staticmethod
    def _match(frame: np.ndarray, template: np.ndarray, threshold: float = _MATCH_THRESHOLD):
        """Return (cx, cy) centre of the best match, or None."""
        result = cv2.matchTemplate(frame, template, cv2.TM_CCOEFF_NORMED)
        _, max_val, _, max_loc = cv2.minMaxLoc(result)
        if max_val < threshold:
            return None
        h, w = template.shape[:2]
        return (max_loc[0] + w // 2, max_loc[1] + h // 2)

    def find_me(self):
        try:
            return self._match(self._grab_minimap(), self._me_tmpl)
        except Exception as e:
            self._log(f"find_me error: {e}")
            return None

    def find_rune(self):
        try:
            return self._match(self._grab_minimap(), self._rune_tmpl)
        except Exception as e:
            self._log(f"find_rune error: {e}")
            return None

    def _has_rune_buff(self) -> bool:
        """Check the top-left buff region for the rune-buff icon."""
        try:
            frame = self._grab_minimap()
            return self._match(frame, self._rune_buff_tmpl, threshold=0.75) is not None
        except Exception as e:
            self._log(f"_has_rune_buff error: {e}")
            return False

    # ------------------------------------------------------------------
    # Navigation
    # ------------------------------------------------------------------

    def go(self) -> bool:
        """Walk to the rune, protect, and interact.  Returns True on success."""
        self._move_seq = 0
        rune_loc = self.find_rune()
        if rune_loc is None:
            self._log("No rune detected on minimap")
            return False

        me_loc = self.find_me()
        me_miss_count = 0
        try_left = True
        self._log(f"Rune at {rune_loc}")

        while not is_stopped() and rune_loc is not None:
            self._log(f"Me: {me_loc}  Rune: {rune_loc}")

            if me_loc is None:
                me_miss_count += 1
                if me_miss_count > 1:
                    self._log("Lost player position, aborting")
                    return False
                # Nudge left/right to become visible
                direction = "left" if try_left else "right"
                try_left = not try_left
                self.pilot.rune_flash_jump(direction=direction)
                sleep_ms(650)
                me_loc = self.find_me()
                rune_loc = self.find_rune()
                continue

            me_miss_count = 0
            vec = self._vector(me_loc, rune_loc)
            self._log(f"Vector: {vec}")
            self._make_move(vec)
            me_loc = self.find_me()
            rune_loc = self.find_rune()

        if is_stopped():
            return False

        self._log("Reached rune location")
        self.pilot.rune_protect()
        self.pilot.rune_interact()

        max_attempts = 10
        for attempt in range(max_attempts):
            if is_stopped():
                return False
            sleep_ms(750)
            if self._has_rune_buff():
                self._log("Rune buff detected, done")
                return True
            self._log(f"No rune buff yet, retrying interact ({attempt + 1}/{max_attempts})")
            self.pilot.rune_interact()

        self._log("Gave up waiting for rune buff")
        return False

    # ------------------------------------------------------------------
    # Movement helpers
    # ------------------------------------------------------------------

    def _vector(self, me, rune):
        dx = self._snap(rune[0] - me[0])
        dy = self._snap(rune[1] - me[1])
        return (dx, dy)

    def _make_move(self, vector):
        self._move_seq += 1
        dx, dy = vector

        # Periodic jump to unstick from ropes/ladders
        if dx > 0:
            press("right")
            if self._move_seq % 3 == 0:
                self.pilot.rune_jump()
            release("right")
        elif dx < 0:
            press("left")
            if self._move_seq % 3 == 0:
                self.pilot.rune_jump()
            release("left")

        # Flash-jump when far away
        if abs(dx) > 40:
            n = math.ceil(abs(dx) / 40)
            direction = "right" if dx > 0 else "left"
            self._log(f"Flash-jump {n}x {direction}")
            for _ in range(n):
                self.pilot.rune_flash_jump(direction=direction)
            return

        # Walk when closer
        walk_ms = int(abs(dx) / 20.0 * 1000)
        if walk_ms > 0:
            key = "right" if dx > 0 else "left"
            press(key)
            sleep_ms(walk_ms)
            release(key)

        # Vertical movement
        if dy > 0:
            self.pilot.rune_jump_down()
        elif dy < 0:
            self.pilot.rune_rope()

    @staticmethod
    def _snap(value, margin=_MARGIN_PX):
        return 0 if -margin <= value <= margin else value
