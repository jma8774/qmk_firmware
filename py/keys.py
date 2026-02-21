import pydirectinput

from timing import jitter, sleep_ms

pydirectinput.PAUSE = 0

DEFAULT_TAP_MS = 60
DEFAULT_TAP_JITTER_PCT = 15

_held: set[str] = set()


def _do_tap(key: str, ms: int):
    pydirectinput.keyDown(key)
    try:
        sleep_ms(ms)
    finally:
        pydirectinput.keyUp(key)


def tap(key: str):
    """TAP(kc) — default ~60 ms with 15 % jitter."""
    _do_tap(key, jitter(DEFAULT_TAP_MS, DEFAULT_TAP_JITTER_PCT))


def tap_d(key: str, duration_ms: int):
    """TAP_D(kc, ms) — custom duration with 15 % jitter."""
    _do_tap(key, jitter(duration_ms, DEFAULT_TAP_JITTER_PCT))


def tap_raw(key: str, duration_ms: int):
    """Tap with an exact duration (caller handles jitter)."""
    _do_tap(key, duration_ms)


def press(key: str):
    pydirectinput.keyDown(key)
    _held.add(key)


def release(key: str):
    pydirectinput.keyUp(key)
    _held.discard(key)


def release_all():
    for k in list(_held):
        pydirectinput.keyUp(k)
    _held.clear()
