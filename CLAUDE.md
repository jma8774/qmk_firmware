# CLAUDE.md — QMK Firmware (Keychron V3, maple keymap)

## Project overview

This is a QMK firmware repo for the **Keychron V3 TKL (ANSI)** keyboard. The primary custom code lives at:

```
keyboards/keychron/v3/ansi/keymaps/maple/
```

The maple keymap implements an **automated bot for MapleStory** targeting the **Marksman** job class. It runs entirely on-device — no host software required. The keyboard emits keypresses autonomously on a schedule driven by the QMK matrix scan loop.

## How the bot works

### Top-level control (`keymap.c`)

- **F7** — start the bot (`running = true`, begins with `BUFF_SCRIPT`)
- **F8** — stop the bot (`running = false`)
- `matrix_scan_user()` is the policy layer, called every ~1 ms. It drives a state machine:
  - **Setup** (`SETUP_CARCION`) — runs first, then every ~46 s, repositions the character
  - **Loot** (`LOOT_CARCION`) — every ~60 s (randomised), picks up dropped items
  - **Buff** (`BUFF_SCRIPT`) — every ~15 s (randomised), activates buff skills
  - **Rotation** (`ROTATION_CARCION`) — all other time, main attack loop

All intervals are randomised via `random_range()` with a percentage window to avoid fixed patterns.

### Script engine (`engine/`)

A non-blocking bytecode interpreter. Scripts are `const cmd_t[]` arrays stored in flash.

- `runner_start()` — load a script and begin execution
- `runner_task()` — advance execution; call once per matrix scan
- `runner_stop()` — halt immediately

**Command types:**
| Macro | Behaviour |
|---|---|
| `TAP(kc)` | tap a key with ~60 ms jittered hold |
| `PRESS(kc)` / `RELEASE(kc)` | hold / release for combos |
| `WAIT_JITTER(ms, pct)` | delay base_ms +/- pct% |
| `WAIT_JITTER_UP(ms, pct)` | delay in [ms, ms*(1+pct/100)] |
| `WAIT_JITTER_DOWN(ms, pct)` | delay in [ms*(1-pct/100), ms] |
| `CALL_NAMED(name, fn)` | call a bool function; result in `last_result` |
| `CHANCE(pct)` | set `last_result` = true with pct% probability |
| `SKIP_TRUE(n)` / `SKIP_FALSE(n)` | conditional forward skip |
| `END()` | terminate script |

All waits yield (return from `runner_task`) so the keyboard USB stack keeps running.

### Marksman job (`scripts/marksman/`)

**Key bindings (in-game):**
| Key | Skill |
|---|---|
| Q | Main attack (Snipe / basic shot) |
| D | Bolt Burst (6 s CD) |
| R | Surgebolt (18 s CD) |
| E | Jump |
| X | Teleport |
| C | Rope Lift |
| Alt | Dash |
| N | Janus (3 independent charges, 57 s each) |
| B | Erda Fountain (57 s CD) |
| 4 | Web (250 s CD) |
| 5 | Solar Crest (250 s CD) |
| Page Up | Boss Buffs (119 s CD) |
| Down+X+X+X | Teleport setup (blink anchor, ~30–45 s CD) |

**Cooldown system:** `job.c` tracks each skill with a `static uint32_t` timer. `TRY_*` macros call the function and skip the follow-up wait if the skill didn't fire (CD not ready).

**Scripts:**
- `rotation_script.c` — map-specific rotation arrays (`ROTATION_CARCION`, `ROTATION_CERNIUM`, `ROTATION_ODIUM`, etc.)
- `setup_script.c` — repositioning sequences for each map
- `loot_script.c` — item-pickup traversals
- `buff_script.c` — teleport setup + boss buffs

### Timing / RNG (`engine/timing.c`)

- `timing_seed_rng()` — seeds `rand()` from `timer_read32()` at boot
- `jitter(base, pct)` — +/- pct%
- `jitter_up(base, pct)` — [base, base*(1+pct/100)]
- `jitter_down(base, pct)` — [base*(1-pct/100), base]
- `timing_chance(pct)` — boolean with pct% probability

## Build

```bash
qmk compile -kb keychron/v3/ansi -km maple
```

Flash with QMK Toolbox or `qmk flash`.

For IDE support (resolve red squiggles), generate a compilation database:

```bash
qmk compile -kb keychron/v3/ansi -km default --compiledb
```

Then use the **clangd** language server — it will pick up `compile_commands.json` automatically.

## File map

```
keyboards/keychron/v3/ansi/keymaps/maple/
  keymap.c                        — layer definitions, F7/F8 toggle, policy loop
  common.h / common.c             — random_range(), keycode_to_string()
  rules.mk                        — adds all .c files to SRC, enables CONSOLE
  engine/
    script_engine.h / .c          — cmd_t, runner_t, runner_start/stop/task
    timing.h / .c                 — jitter variants, RNG seed, timing_chance
  scripts/
    cmd_macros.h                  — TAP/PRESS/RELEASE/WAIT_*/CALL/CHANCE/SKIP/END macros
    marksman/
      job.h / job.c               — per-skill cooldown functions + stateless action macros
      rotation_script.h / .c      — ROTATION_* arrays (one cycle of attack)
      setup_script.h / .c         — SETUP_* arrays (repositioning)
      loot_script.h / .c          — LOOT_* arrays (item pickup)
      buff_script.h / .c          — BUFF_SCRIPT array (buffs + teleport setup)
```

## Key-action delay rule

Every `TAP` / `TAP_D` / `PRESS` / `RELEASE` in a `const cmd_t[]` array or `#define` macro body **must** be immediately followed by a `WAIT*` macro or a compound macro that is known to end with a delay.

**The only acceptable follower is an explicit `WAIT*` token.** Compound macros (even ones that happen to end with a wait internally) are not exempt — a `WAIT*` must appear at the call site between the key action and whatever comes next.

**Not acceptable** — bare key actions with no delay:
```c
TAP(KC_RIGHT),
JUMP_ATTACK(),   // wrong — JUMP_ATTACK starts with TAP internally but TAP(KC_RIGHT) has no wait after it
```

**Correct:**
```c
TAP(KC_RIGHT),
WAIT_JITTER_UP_D(100),
JUMP_ATTACK(),
```

**`TRY_*` macros must end with `WAIT*`:** every `#define TRY_*` macro must have a `WAIT_JITTER*` as its last token. This ensures that when a skill fires, there is always a post-cast delay before the next action. The linter enforces this explicitly.

**Minimum tap duration:** `TAP_D(kc, ms)` where `ms` is non-zero and below 40 ms is flagged as inhuman. `TAP(kc)` uses the 60 ms default and is always fine.

**No zero-jitter waits:** `WAIT(ms)` (fixed, no jitter) and `WAIT_JITTER*(ms, 0)` (explicit zero pct) are flagged. Always use a `_D` variant or supply a non-zero pct. Suppress with `// noqa` if intentional.

**Suppressing false positives:** append `// noqa` to the line to silence a specific warning (e.g. for intentional building-block macros like `HOLD` that are always followed by a wait at the call site).

### Linter

```bash
python3 lint_c_delays.py
```

Scans all `.c` and `.h` files under `keyboards/keychron/v3/ansi/keymaps/maple/`. Exit code 1 if violations found.

**Always run the linter after any code changes to the maple keymap.**

## Adding a new map

1. Add `ROTATION_<MAP>`, `SETUP_<MAP>`, `LOOT_<MAP>` arrays in the respective `*_script.c` files and declare them in the headers.
2. Update `keymap.c`: set `carcion_time_ms` (or equivalent interval) and swap the script pointers passed to `runner_start()` in `matrix_scan_user()`.
