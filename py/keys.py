import pydirectinput

from timing import jitter, sleep_ms
from map_check import map_check, admin_check

pydirectinput.PAUSE = 0

DEFAULT_TAP_MS = 60
DEFAULT_TAP_JITTER_PCT = 15

_held: set[str] = set()


def _do_tap(key: str, ms: int, check: bool):
    if check:
        map_check(type_string, tap)
        admin_check(type_string, tap)
    pydirectinput.keyDown(key)
    try:
        sleep_ms(ms)
    finally:
        pydirectinput.keyUp(key)


def tap(key: str, check=True):
    """TAP(kc) — default ~60 ms with 15 % jitter."""
    _do_tap(key, jitter(DEFAULT_TAP_MS, DEFAULT_TAP_JITTER_PCT), check)


def tap_d(key: str, duration_ms: int, check=True):
    """TAP_D(kc, ms) — custom duration with 15 % jitter."""
    _do_tap(key, jitter(duration_ms, DEFAULT_TAP_JITTER_PCT), check)


def tap_raw(key: str, duration_ms: int, check=True):
    """Tap with an exact duration (caller handles jitter)."""
    _do_tap(key, duration_ms, check)


def press(key: str, check=True):
    if check:
        map_check(type_string, tap)
        admin_check(type_string, tap)
    pydirectinput.keyDown(key)
    _held.add(key)


def release(key: str, check=True):
    if check:
        map_check(type_string, tap)
        admin_check(type_string, tap)
    pydirectinput.keyUp(key)
    _held.discard(key)


def release_all():
    for k in list(_held):
        pydirectinput.keyUp(k)
    _held.clear()

def type_string(string: str, check=True):
    for char in string:
        if char == " ":
            tap("space", check)
        else:
            tap(char, check)
