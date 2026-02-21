import time
import threading

import keyboard

from timing import StopRequested, request_stop, clear_stop, random_range
from keys import release_all
from scripts import rotation_odium, setup_odium, buff_script

SETUP_INTERVAL_S = 49.0
BUFF_INTERVAL_MS = 15_000

_thread: threading.Thread | None = None


def _policy_loop():
    last_setup = 0.0
    last_buff = 0.0

    print("[policy] initial buff")
    buff_script()

    while True:
        now = time.monotonic()

        if last_setup == 0 or (now - last_setup) >= SETUP_INTERVAL_S:
            print("\n[policy] -> setup")
            setup_odium()
            last_setup = time.monotonic()

        elif last_buff == 0 or (now - last_buff) >= random_range(BUFF_INTERVAL_MS, 70, 100) / 1000.0:
            print("\n[policy] -> buff")
            buff_script()
            last_buff = time.monotonic()

        else:
            print("\n[policy] -> rotation")
            rotation_odium()


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

    print("Maple Marksman Bot (Odium)")
    print("  F7 = start")
    print("  F8 = pause")
    print("  Ctrl+C = exit")

    try:
        keyboard.wait()
    except KeyboardInterrupt:
        request_stop()
        print("\nExiting.")
