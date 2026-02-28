import json
import time
from pathlib import Path

from keys import tap, tap_raw, press, release
from timing import jitter, jitter_up, sleep_ms, random_range

DJ = 10  # default wait-jitter percentage (matches DEFAULT_WAIT_JITTER_PCT)

_COOLDOWN_DB = Path(__file__).parent / ".cooldowns.json"

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
        # No need for sleep here
        self._ready_at = now + self.cd_ms / 1000.0
        print(f"    [{self.name}] fired")
        return True


class PersistedCooldown:
    """Cooldown that survives restarts via a JSON file on disk.

    Uses wall-clock time (time.time) so the saved timestamp remains
    meaningful across process lifetimes.
    """

    def __init__(self, name: str, key: str, cd_ms: int, delayAfter: int = 100):
        self.name = name
        self.key = key
        self.cd_ms = cd_ms
        self._ready_at = self._load()
        self.delayAfter = delayAfter

    def try_use(self) -> bool:
        now = time.time()
        if now < self._ready_at:
            return False
        tap(self.key)
        sleep_ms(jitter(self.delayAfter, 20))
        self._ready_at = now + self.cd_ms / 1000.0
        self._save()
        print(f"    [{self.name}] fired")
        return True

    def reset(self):
        """Clear the persisted cooldown so it fires on next try_use()."""
        self._ready_at = 0.0
        self._save()
        print(f"    [{self.name}] cooldown reset")

    def remaining_ms(self) -> int:
        """Milliseconds until this cooldown is ready (0 if ready now)."""
        return max(0, int((self._ready_at - time.time()) * 1000))

    def _load(self) -> float:
        try:
            db = json.loads(_COOLDOWN_DB.read_text())
            return float(db.get(self.name, 0.0))
        except (FileNotFoundError, json.JSONDecodeError, ValueError):
            return 0.0

    def _save(self):
        try:
            db = json.loads(_COOLDOWN_DB.read_text())
        except (FileNotFoundError, json.JSONDecodeError):
            db = {}
        db[self.name] = self._ready_at
        _COOLDOWN_DB.write_text(json.dumps(db))


bolt_burst    = Cooldown("bolt_burst",    "d",      6_000)
surgebolt     = Cooldown("surgebolt",     "r",     18_000)
web           = Cooldown("web",           "4",    250_000)
solar_crest   = Cooldown("solar_crest",   "5",    250_000)
erda_fountain = Cooldown("erda_fountain", "b",     57_000)
janus1        = Cooldown("janus",         "n",     57_000)
janus2        = Cooldown("janus2",        "n",     57_000)
janus3        = Cooldown("janus3",        "n",     57_000)
boss_buffs    = Cooldown("boss_buffs",    "pageup", 119_000)

guild_crit_buff = PersistedCooldown("guild_crit_buff", "f5", (30 * 60 * 1000) + (5 * 1000)) # 30 minutes and 10 seconds
exp_buff = PersistedCooldown("exp_buff", "f9", (30 * 60 * 1000) + (5 * 1000)) # 30 minutes and 10 seconds
legion_meso_buff = PersistedCooldown("legion_meso_buff", "f10", (30 * 60 * 1000) + (5 * 1000)) # 30 minutes and 10 seconds
wap_buff = PersistedCooldown("wap_buff", "f12", (30 * 60 * 1000) + (5 * 1000)) # 30 minutes and 10 seconds

# ---------------------------------------------------------------------------
# Teleport setup (special combo with variable cooldown)
# ---------------------------------------------------------------------------

_teleport_setup_ready_at = 0.0


def teleport_setup_try(ignore_cooldown: bool = False) -> bool:
    global _teleport_setup_ready_at
    now = time.monotonic()
    if not ignore_cooldown and now < _teleport_setup_ready_at:
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
    # No need for sleep here

    _teleport_setup_ready_at = now + random_range(25_000, 67, 100) / 1000.0
    print("    [teleport_setup] fired")
    return True


# ---------------------------------------------------------------------------
# Stateless actions (mirrors job.h macros)
# ---------------------------------------------------------------------------

def shoot():
    tap("q")
    sleep_ms(jitter_up(610, DJ))


def rope(delayAfter: int = 1600):
    tap("c")
    sleep_ms(jitter_up(delayAfter, DJ))


def dash(delayAfter: int = 620):
    tap("alt")
    sleep_ms(jitter_up(620, 5))


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

def jump_attack_delay(delayAfter, double_tap=False):
    tap("e")
    sleep_ms(jitter_up(70, DJ))
    if double_tap:
        tap("e")
        sleep_ms(jitter(30, DJ))
    tap("e")
    sleep_ms(jitter(200, DJ))
    tap("q")
    sleep_ms(jitter_up(delayAfter, 5))


def jump_down_delay(ms: int):
    press("down")
    try:
        sleep_ms(jitter(70, DJ))
        tap("e")
        sleep_ms(jitter(50, DJ))
    finally:
        release("down")
    sleep_ms(jitter_up(ms, DJ))


def teleport_reset(delayAfter: int = 650):
    tap("x")
    sleep_ms(jitter(60, DJ))
    tap("x")
    sleep_ms(jitter(60, DJ))
    tap("x")
    sleep_ms(jitter_up(delayAfter, DJ))
