import threading

import keyboard

import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from timing import StopRequested, request_stop, clear_stop, sleep_ms
from keys import tap, release_all


_thread: threading.Thread | None = None


def _run_starcatching():
    while True:
        tap("space")
        sleep_ms(3000)
        tap("space")
        sleep_ms(5000)

def _run():
    try:
        _run_starcatching()
    except StopRequested:
        pass
    finally:
        release_all()
        print("[starcatch] paused")


def start():
    global _thread
    if _thread and _thread.is_alive():
        return
    clear_stop()
    _thread = threading.Thread(target=_run, daemon=True)
    _thread.start()
    print("[starcatch] started — press F8 to pause")


def stop():
    request_stop()
    print("[starcatch] stopping...")


if __name__ == "__main__":
    keyboard.add_hotkey("f7", start, suppress=True)
    keyboard.add_hotkey("f8", stop, suppress=True)

    print("Auto Starcatching")
    print("  F7 = start")
    print("  F8 = pause")
    print("  Ctrl+C = exit")

    try:
        keyboard.wait()
    except KeyboardInterrupt:
        request_stop()
        print("\nExiting.")
