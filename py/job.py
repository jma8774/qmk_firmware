import time

from keys import tap, tap_raw, press, release
from timing import jitter, jitter_up, sleep_ms, random_range

DJ = 10  # default wait-jitter percentage (matches DEFAULT_WAIT_JITTER_PCT)


# ---------------------------------------------------------------------------
# Cooldown-tracked skills (mirrors job.c)
# ---------------------------------------------------------------------------

class Cooldown:
    def __init__(self, name: str, key: str, cd_ms: int):
        self.name = name
        self.key = key
        self.cd_ms = cd_ms
        self._ready_at = 0.0

    def try_use(self) -> bool:
        now = time.monotonic()
        if now < self._ready_at:
            return False
        tap(self.key)
        self._ready_at = now + self.cd_ms / 1000.0
        print(f"    [{self.name}] fired")
        return True


bolt_burst    = Cooldown("bolt_burst",    "d",      6_000)
surgebolt     = Cooldown("surgebolt",     "r",     18_000)
web           = Cooldown("web",           "4",    250_000)
solar_crest   = Cooldown("solar_crest",   "5",    250_000)
erda_fountain = Cooldown("erda_fountain", "b",     57_000)
janus1        = Cooldown("janus",         "n",     57_000)
janus2        = Cooldown("janus2",        "n",     57_000)
janus3        = Cooldown("janus3",        "n",     57_000)
boss_buffs    = Cooldown("boss_buffs",    "pageup", 119_000)


# ---------------------------------------------------------------------------
# Teleport setup (special combo with variable cooldown)
# ---------------------------------------------------------------------------

_teleport_setup_ready_at = 0.0


def teleport_setup_try() -> bool:
    global _teleport_setup_ready_at
    now = time.monotonic()
    if now < _teleport_setup_ready_at:
        return False

    press("down")
    try:
        sleep_ms(jitter(100, 20))
        tap_raw("x", jitter(60, 20))
        sleep_ms(jitter(60, 20))
        tap_raw("x", jitter(60, 20))
        sleep_ms(jitter(60, 20))
        tap_raw("x", jitter(60, 20))
        sleep_ms(jitter(70, 20))
    finally:
        release("down")

    _teleport_setup_ready_at = now + random_range(35_000, 67, 100) / 1000.0
    print("    [teleport_setup] fired")
    return True


# ---------------------------------------------------------------------------
# Stateless actions (mirrors job.h macros)
# ---------------------------------------------------------------------------

def shoot():
    tap("q")
    sleep_ms(jitter_up(600, DJ))


def rope():
    tap("c")
    sleep_ms(jitter_up(1600, DJ))


def dash():
    tap("alt")
    sleep_ms(jitter_up(600, 5))


def flash_jump():
    tap("e")
    sleep_ms(jitter_up(70, DJ))
    tap("e")
    sleep_ms(jitter_up(580, 5))


def jump_attack():
    tap("e")
    sleep_ms(jitter_up(70, DJ))
    tap("e")
    sleep_ms(jitter(200, DJ))
    tap("q")
    sleep_ms(jitter_up(620, 5))


def jump_down_delay(ms: int):
    press("down")
    try:
        sleep_ms(jitter(70, DJ))
        tap("e")
        sleep_ms(jitter(50, DJ))
    finally:
        release("down")
    sleep_ms(jitter_up(ms, DJ))


def teleport_reset():
    tap("x")
    sleep_ms(jitter(60, DJ))
    tap("x")
    sleep_ms(jitter(60, DJ))
    tap("x")
    sleep_ms(jitter_up(650, DJ))
