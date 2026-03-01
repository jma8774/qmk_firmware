# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Running the bot

```bash
# From the py/ directory
python main.py           # Normal run (with map/admin checks, consumable buffs)
python main.py nocheck   # Skip map/admin detection
python main.py noconsumable  # Skip consumable buff usage (guild/exp/legion/wap)
python main.py resetconsumable  # Reset persisted consumable cooldown timestamps, then run
python main.py log       # Enable template-matching timing logs

# Starcatching sub-bot (standalone)
python auto-starcatching/main.py
```

Hotkeys at runtime: **F7** = start, **F8** = pause, **Ctrl+C** = exit.

## Dependencies

```bash
pip install pydirectinput keyboard opencv-python mss dxcam
```

`dxcam` is used for screen capture (Windows only). `winsound` (stdlib, Windows only) handles audio alerts.

## Architecture

This is a MapleStory automation bot (Marksman class, Tallahart map). The policy loop in `main.py` orchestrates all behavior.

### Module responsibilities

| File | Role |
|---|---|
| `timing.py` | `sleep_ms` (interruptible via `StopRequested`), jitter helpers (`jitter`, `jitter_up`, `jitter_down`, `random_range`, `chance`) |
| `keys.py` | Low-level key I/O via `pydirectinput` — `tap`, `tap_d`, `press`, `release`, `release_all`, `type_string`. Every key op calls `map_check`/`admin_check` as a side effect. |
| `common.py` | Screen capture (`grab_screen`, `grab_region` via dxcam), OpenCV template matching (`match_template`, `is_template_on_screen`, `is_template_in_region`), `load_template`. Also hosts named screen-detection helpers (`is_nr1_teleport_reset_on_screen`, `is_enfolding3_teleport_reset_on_screen`). |
| `job.py` | Skill definitions: in-memory `Cooldown` and disk-persisted `PersistedCooldown` objects, stateless actions (`shoot`, `flash_jump`, `rope`, `dash`, `teleport_reset`, etc.). Cooldowns are persisted to `.cooldowns.json`. |
| `scripts.py` | High-level map scripts — `rotation_*`, `setup_*`, `loot_*`, `full_reset_*`, `buff_script`. Currently active maps: **nr1** (primary) and **carcion** (legacy). |
| `map_check.py` | Safety guards called on every keypress: minimap check (bot pauses if tallahart icon disappears) and admin detection (maple admin eye/text templates trigger stop + Discord alert + audio). |
| `notify.py` | Discord webhook notifications (`notify(message, numberOfSends)`). Sends asynchronously via a daemon thread. |
| `runewalker/` | Detects runes on the minimap and navigates to them. `RuneWalkerPilot` (abstract) defines the movement interface; `MarksPilot` in `main.py` implements it for Marksman. |

### Policy loop (main.py)

The loop runs in a daemon thread and follows this priority order each iteration:
1. UI dismiss (cancel/confirm dialogs every 10 s)
2. Rune detection and walking (every 15 s, highest gameplay priority)
3. Setup rotation (every ~52 s)
4. Loot run (every ~80 s, only in a safe window relative to setup)
5. Buff (every ~10 s)
6. Attack rotation (fallback)

### Adding a new map/character

1. Add screen-detection helpers to `common.py` (new template images go in `images/`).
2. Write `setup_*`, `loot_*`, `full_reset_*`, `rotation_*` functions in `scripts.py`.
3. Subclass `RuneWalkerPilot` and implement all abstract movement methods.
4. Update the policy loop in `main.py` to call the new scripts and use the new pilot.

### Jitter convention

All timing calls use `jitter(base_ms, pct)` (±pct%), `jitter_up` (never under base), or `jitter_down` (never over base). The default key-tap duration is 60 ms with 15% jitter (`DEFAULT_TAP_MS`, `DEFAULT_TAP_JITTER_PCT` in `keys.py`).
