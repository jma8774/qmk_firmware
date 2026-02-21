import random
import threading
import time


class StopRequested(Exception):
    pass


_stop = threading.Event()


def request_stop():
    _stop.set()


def clear_stop():
    _stop.clear()


def sleep_ms(ms: int):
    """Interruptible sleep. Raises StopRequested if the bot is stopped."""
    if ms <= 0:
        return
    if _stop.wait(timeout=ms / 1000.0):
        raise StopRequested()


# ---------------------------------------------------------------------------
# Jitter helpers (match engine/timing.c exactly)
# ---------------------------------------------------------------------------

def jitter(base_ms: int, pct: int) -> int:
    """base_ms +/- pct%. E.g. jitter(5000, 15) -> [4250, 5750]."""
    if pct == 0:
        return base_ms
    pct = min(pct, 100)
    max_offset = base_ms * pct // 100
    return base_ms - max_offset + random.randint(0, max_offset * 2)


def jitter_down(base_ms: int, pct: int) -> int:
    """base_ms minus up to pct%. Never over base_ms."""
    if pct == 0:
        return base_ms
    pct = min(pct, 100)
    max_offset = base_ms * pct // 100
    if max_offset == 0:
        return base_ms
    return base_ms - random.randint(0, max_offset)


def jitter_up(base_ms: int, pct: int) -> int:
    """base_ms plus up to pct%. Never under base_ms."""
    if pct == 0:
        return base_ms
    pct = min(pct, 100)
    max_offset = base_ms * pct // 100
    if max_offset == 0:
        return base_ms
    return base_ms + random.randint(0, max_offset)


def chance(pct: int) -> bool:
    if pct <= 0:
        return False
    if pct >= 100:
        return True
    return random.randint(0, 99) < pct


def random_range(interval_ms: int, min_pct: int, max_pct: int) -> int:
    """Scale interval_ms by a random percentage in [min_pct, max_pct]."""
    min_pct = min(min_pct, 100)
    max_pct = min(max_pct, 100)
    if min_pct > max_pct:
        min_pct, max_pct = max_pct, min_pct
    pct = random.randint(min_pct, max_pct)
    return int(interval_ms * pct / 100)
