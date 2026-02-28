from ssl import RAND_add
import time
import threading

import keyboard

from timing import StopRequested, request_stop, clear_stop, random_range, jitter_up, jitter, sleep_ms
from keys import tap, tap_d, press, release, release_all
from job import flash_jump, rope, jump_down_delay, web, solar_crest, teleport_reset, teleport_setup_try, shoot, jump_attack
from scripts import rotation_carcion, setup_carcion, loot_carcion, buff_script, full_reset_carcion, setup_nr1, full_reset_nr1, loot_nr1
from runewalker import RuneWalker, RuneWalkerPilot
from common import load_template, is_template_in_region, is_enfolding3_teleport_reset_on_screen, is_nr1_teleport_reset_on_screen
from job import teleport_setup_try
from map_check import stop_alert
from notify import notify
import random

SETUP_INTERVAL_S = 52.0
LOOT_INTERVAL_MS = 80_000
BUFF_INTERVAL_MS = 10_000
RUNE_CHECK_INTERVAL_S = 15.0


class MarksPilot(RuneWalkerPilot):
    def rune_flash_jump(self, direction=None):
        if direction:
            press(direction)
            sleep_ms(jitter(50, 20))
        flash_jump()
        if direction:
            release(direction)
            sleep_ms(jitter(50, 20))

    def rune_jump(self):
        tap("e")
        sleep_ms(jitter(50, 20))

    def rune_rope(self):
        rope(delayAfter=3000)

    def rune_jump_down(self):
        jump_down_delay(1000)

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
        sleep_ms(jitter(200, 20))

    def done(self):
        def do():
            if random.random() < 0.5:
                press('right')
                sleep_ms(jitter(100, 20))
                tap('e')
                sleep_ms(jitter(100, 20))
                release('right')
                sleep_ms(jitter_up(500, 20))
                teleport_reset(delayAfter=800)
            else:
                press('left')
                sleep_ms(jitter(100, 20))
                tap('e')
                sleep_ms(jitter(100, 20))
                release('left')
                sleep_ms(jitter_up(500, 20))
                tap('right')
                sleep_ms(jitter(50, 20))
                shoot()
                jump_attack()
                teleport_reset(delayAfter=800)

        sleep_ms(jitter(200, 20))
        do()
        attempts = 0
        while not is_nr1_teleport_reset_on_screen():
            attempts += 1
            if attempts % 20 == 0:
                print(f"[done] -> full reset ({attempts})")
                full_reset_nr1()
            do()

_thread: threading.Thread | None = None
_first_run = True
_last_setup = 0.0
_last_loot = 0.0
_last_buff = 0.0
_last_rune_check = 0.0
_last_rune_check_2 = 0.0
_last_ui_check = 0.0

_rune_walker = RuneWalker(MarksPilot())
cancel_tmpl = load_template("cancel.png")
confirmed_tmpl = load_template("confirm.png")


def _policy_loop():
    global _first_run, _last_setup, _last_loot, _last_buff, _last_rune_check, _last_rune_check_2, _last_ui_check

    if _first_run:
        print("[policy] initial buff")
        buff_script()
        _first_run = False
    else:
        print("[policy] resuming")

    while True:
        now = time.monotonic()

        if _last_ui_check == 0 or (now - _last_ui_check) >= 10.0:
            print("\n[policy] -> checking for UI")
            _last_ui_check = now
            UI_REGION = {"top": 424, "left": 511, "width": 1494-511, "height": 831-424}
            if is_template_in_region(cancel_tmpl, UI_REGION, threshold=0.8, name="cancel"):
                tap('esc')
                sleep_ms(jitter(500, 20))
                continue
            if is_template_in_region(confirmed_tmpl, UI_REGION, threshold=0.8, name="confirmed"):
                tap('esc')
                sleep_ms(jitter(500, 20))
                continue

        # Rune check (highest priority — avoid penalty)
        if (now - _last_rune_check) >= RUNE_CHECK_INTERVAL_S:
            _last_rune_check = now
            if _rune_walker.is_rune_inactive():
                print("\n[policy] -> rune on cooldown, skipping")
            elif _rune_walker.find_rune():
                print("\n[policy] -> rune detected, walking to it")
                if teleport_setup_try(ignore_cooldown=True):
                    sleep_ms(jitter_up(675, 10))
                _rune_walker.go()
                continue
            else:
                print("\n[policy] -> no rune detected")

        if _last_setup == 0 or (now - _last_setup) >= SETUP_INTERVAL_S:
            print("\n[policy] -> setup")
            setup_nr1()
            _last_setup = time.monotonic()
            continue

        # Only loot if setup has been done after last 15 seconds and not 10 seconds before the next setup and the loot interval has elapsed
        setup_time_elapsed = now - _last_setup
        time_until_next_setup = SETUP_INTERVAL_S - setup_time_elapsed
        # print(f"[policy] -> setup time elapsed: {setup_time_elapsed}, time until next setup: {time_until_next_setup}")
        if setup_time_elapsed > random_range(15, 70, 100) and time_until_next_setup > 10 and (now - _last_loot) >= random_range(LOOT_INTERVAL_MS, 80, 100) / 1000.0:
            print("\n[policy] -> loot")
            loot_nr1(_rune_walker, _last_rune_check_2)
            _last_rune_check_2 = now
            _last_loot = time.monotonic()
            continue
        if _last_buff == 0 or (now - _last_buff) >= random_range(BUFF_INTERVAL_MS, 70, 100) / 1000.0:
            print("\n[policy] -> buff")
            buff_script()
            _last_buff = time.monotonic()
            continue

        print("\n[policy] -> rotation")
        rotation_carcion()


def _run():
    try:
        _policy_loop()
    except StopRequested:
        pass
    finally:
        release_all()
        notify("Bot paused")
        print("[bot] paused")


def start():
    global _thread
    stop_alert()
    if _thread and _thread.is_alive():
        return
    clear_stop()
    _thread = threading.Thread(target=_run, daemon=True)
    _thread.start()
    print("[bot] started — press F8 to pause")


def stop():
    stop_alert()
    request_stop()
    print("[bot] stopping...")


if __name__ == "__main__":
    keyboard.add_hotkey("f7", start, suppress=True)
    keyboard.add_hotkey("f8", stop, suppress=True)

    print("Maple Marksman Bot (Tallahart)")
    print("  F7 = start")
    print("  F8 = pause")
    print("  Ctrl+C = exit")

    try:
        keyboard.wait()
    except KeyboardInterrupt:
        request_stop()
        print("\nExiting.")
