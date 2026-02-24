import time
import threading

import keyboard

from timing import StopRequested, request_stop, clear_stop, random_range, jitter_up, jitter, sleep_ms
from keys import tap, press, release, release_all
from job import flash_jump, rope, jump_down_delay, web, solar_crest, teleport_reset
from scripts import rotation_carcion, setup_carcion, loot_carcion, buff_script
from runewalker import RuneWalker, RuneWalkerPilot

SETUP_INTERVAL_S = 46.0
LOOT_INTERVAL_MS = 60_000
BUFF_INTERVAL_MS = 15_000
RUNE_CHECK_INTERVAL_S = 60.0


class MarksPilot(RuneWalkerPilot):
    def rune_flash_jump(self, direction=None):
        if direction:
            press(direction)
        flash_jump()
        if direction:
            release(direction)

    def rune_jump(self):
        tap("e")

    def rune_rope(self):
        rope()

    def rune_jump_down(self):
        jump_down_delay(500)

    def rune_protect(self):
        pass

    def rune_interact(self):
        tap("y")
        sleep_ms(jitter_up(750, 20))
        # Always left down down up
        tap("left")
        sleep_ms(jitter(120, 20))
        tap("down")
        sleep_ms(jitter(120, 20))
        tap("down")
        sleep_ms(jitter(120, 20))
        tap("up")
        sleep_ms(jitter(1000, 20))
        teleport_reset()


_thread: threading.Thread | None = None
_first_run = True
_last_setup = 0.0
_last_loot = 0.0
_last_buff = 0.0
_last_rune_check = 0.0

_rune_walker = RuneWalker(MarksPilot())


def _policy_loop():
    global _first_run, _last_setup, _last_loot, _last_buff, _last_rune_check

    if _first_run:
        print("[policy] initial buff")
        buff_script()
        _first_run = False
    else:
        print("[policy] resuming")

    while True:
        now = time.monotonic()

        # Rune check (highest priority — avoid penalty)
        if (now - _last_rune_check) >= RUNE_CHECK_INTERVAL_S:
            _last_rune_check = now
            if _rune_walker.find_rune() is not None:
                print("\n[policy] -> rune detected, walking to it")
                _rune_walker.go()
                continue

        if _last_setup == 0 or (now - _last_setup) >= SETUP_INTERVAL_S:
            print("\n[policy] -> setup")
            setup_carcion()
            _last_setup = time.monotonic()

        elif _last_loot == 0 or (now - _last_loot) >= random_range(LOOT_INTERVAL_MS, 80, 100) / 1000.0:
            print("\n[policy] -> loot")
            loot_carcion()
            _last_loot = time.monotonic()

        elif _last_buff == 0 or (now - _last_buff) >= random_range(BUFF_INTERVAL_MS, 70, 100) / 1000.0:
            print("\n[policy] -> buff")
            buff_script()
            _last_buff = time.monotonic()

        else:
            print("\n[policy] -> rotation")
            rotation_carcion()


def _run():
    try:
        _policy_loop()
    except StopRequested:
        pass
    finally:
        release_all()
        print("[bot] paused")


def start():
    global _thread
    if _thread and _thread.is_alive():
        return
    clear_stop()
    _thread = threading.Thread(target=_run, daemon=True)
    _thread.start()
    print("[bot] started — press F8 to pause")


def stop():
    request_stop()
    print("[bot] stopping...")


if __name__ == "__main__":
    keyboard.add_hotkey("f7", start, suppress=True)
    keyboard.add_hotkey("f8", stop, suppress=True)

    print("Maple Marksman Bot (Carcion)")
    print("  F7 = start")
    print("  F8 = pause")
    print("  Ctrl+C = exit")

    try:
        keyboard.wait()
    except KeyboardInterrupt:
        request_stop()
        print("\nExiting.")
