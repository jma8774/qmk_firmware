#!/usr/bin/env python3
"""
Keyboard event monitor for QMK firmware validation.

Listens for key press/release events while the firmware macro runs and reports:
  - Ghost releases (key released without a matching press)
  - Inhuman hold durations (< MIN_HOLD_MS)
  - Per-key timing statistics when done

Inter-event interval is NOT checked — fast typists naturally overlap keys,
producing sub-millisecond gaps between events from different fingers. Only
per-key hold duration is a meaningful liveness signal.

Press F8 to trigger the firmware macro, Esc or Ctrl+C to stop monitoring.

Requirements:
    pip install pynput

macOS note: grant Terminal (or your IDE) Accessibility access in
System Settings → Privacy & Security → Accessibility.
"""

import time
from collections import defaultdict
from pynput import keyboard

# ---------------------------------------------------------------------------
# Thresholds
# ---------------------------------------------------------------------------

MIN_HOLD_MS = 20   # below this a tap is suspiciously fast (firmware minimum is 40 ms)

# ---------------------------------------------------------------------------
# State
# ---------------------------------------------------------------------------

pressed: dict[str, float] = {}   # key_name -> press timestamp (ms)
events: list[dict] = []
violations: list[str] = []

last_event_time: float | None = None


def _now_ms() -> float:
    return time.perf_counter() * 1000


def _key_name(key) -> str:
    try:
        return key.char if key.char else str(key)
    except AttributeError:
        return str(key)


def _inter_ms() -> float | None:
    global last_event_time
    now = _now_ms()
    delta = (now - last_event_time) if last_event_time is not None else None
    last_event_time = now
    return delta


def _flag(msg: str):
    violations.append(msg)
    print(f"  ❌  {msg}")


# ---------------------------------------------------------------------------
# Handlers
# ---------------------------------------------------------------------------

def on_press(key):
    name  = _key_name(key)
    now   = _now_ms()
    inter = _inter_ms()

    inter_str = f"+{inter:.1f} ms" if inter is not None else "      —"
    print(f"   PRESS   {name:<20}  {inter_str:>12} since last")

    pressed[name] = now
    events.append({"type": "press", "key": name, "time": now, "inter_ms": inter})


def on_release(key):
    name  = _key_name(key)
    now   = _now_ms()
    inter = _inter_ms()

    inter_str = f"+{inter:.1f} ms" if inter is not None else "      —"

    if name in pressed:
        hold_ms = now - pressed.pop(name)
        inhuman = hold_ms < MIN_HOLD_MS
        tag = "❌" if inhuman else "  "
        print(f"{tag}  RELEASE {name:<20}  {inter_str:>12} since last  hold={hold_ms:.1f} ms")
        if inhuman:
            _flag(f"RELEASE {name}: hold {hold_ms:.1f} ms < {MIN_HOLD_MS} ms (inhuman tap speed)")
    else:
        print(f"❌  RELEASE {name:<20}  {inter_str:>12} since last  (ghost — never pressed!)")
        _flag(f"RELEASE {name}: ghost release — key was not in pressed set")
        hold_ms = None

    events.append({"type": "release", "key": name, "time": now, "inter_ms": inter, "hold_ms": hold_ms})

    if key == keyboard.Key.esc:
        return False


# ---------------------------------------------------------------------------
# Summary
# ---------------------------------------------------------------------------

def print_summary():
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    print(f"Total events : {len(events)}")
    print(f"Violations   : {len(violations)}")

    if not events:
        return

    # Per-key hold stats
    holds: dict[str, list[float]] = defaultdict(list)
    for e in events:
        if e["type"] == "release" and e.get("hold_ms") is not None:
            holds[e["key"]].append(e["hold_ms"])

    if holds:
        print("\nPer-key hold durations (ms):")
        print(f"  {'Key':<20}  {'Count':>5}  {'Min':>8}  {'Avg':>8}  {'Max':>8}")
        for k, vals in sorted(holds.items()):
            flag = " ❌" if min(vals) < MIN_HOLD_MS else ""
            print(f"  {k:<20}  {len(vals):>5}  {min(vals):>8.1f}  {sum(vals)/len(vals):>8.1f}  {max(vals):>8.1f}{flag}")

    # Inter-event stats (informational only, not used for violations)
    inters = [e["inter_ms"] for e in events if e["inter_ms"] is not None]
    if inters:
        print(f"\nInter-event intervals (ms) [informational]:")
        print(f"  min={min(inters):.1f}  avg={sum(inters)/len(inters):.1f}  max={max(inters):.1f}")

    if violations:
        print(f"\nViolations ({len(violations)}):")
        for v in violations:
            print(f"  ❌  {v}")
    else:
        print("\nNo violations.")


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main():
    print("Key monitor started — press F8 to trigger firmware macro, Esc or Ctrl+C to stop.")
    print(f"Threshold: hold < {MIN_HOLD_MS} ms → inhuman  (firmware minimum tap is 40 ms)\n")
    print(f"  {'Event':<30}  {'Since last':>12}  {'Hold':>10}")
    print("-" * 62)

    try:
        with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
            listener.join()
    except KeyboardInterrupt:
        pass

    print_summary()


if __name__ == "__main__":
    main()
