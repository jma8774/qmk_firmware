# CLAUDE.md — QMK Firmware (Keychron V3, custom keymap)

## Project overview

This is a QMK firmware repo for the **Keychron V3 TKL (ANSI)** keyboard. The primary custom code lives at:

```
keyboards/keychron/v3/ansi/keymaps/custom/
```

The custom keymap implements an **automated bot for MapleStory** targeting the **Marksman** job class. It runs entirely on-device — no host software required. The keyboard emits keypresses autonomously on a schedule driven by the QMK matrix scan loop.

## How the bot works

### Top-level control (`keymap.c`)

- **F7** — start the bot in the current mode (`running = true`)
- **F8** — stop the bot (`running = false`, stops runner, releases stuck keys)
- **F9** — cycle bot mode (only when stopped): `tallahart → limbo → cubing → tallahart`
- `matrix_scan_user()` is the policy layer, called every ~1 ms.

**Bot modes (`bot_mode_t`):**
| Mode | Behaviour |
|---|---|
| `BOT_TALLAHART` | Full rotation: setup → loot → buff → human → rotation loop |
| `BOT_LIMBO` | Loops `LIMBO_SCRIPT` forever (spam Space with jitter) |
| `BOT_CUBING` | Loops `CUBING_SCRIPT` forever (4× Space taps); **spacebar pauses** |

**Tallahart policy intervals:**
| Script | Interval | Effective range |
|---|---|---|
| Setup | `random_range(120000, 83, 100)` | 100–120 s |
| Loot | `random_range(80000, 75, 100)` | 60–80 s (suppressed on fresh start) |
| Buff | `random_range(15000, 70, 100)` | 10.5–15 s |
| Human | `random_range(160000, 56, 100)` | 90–160 s |
| Rotation cooldown | `random_range(8500, 71, 100)` | 6–8.5 s |
| End pause | `jitter(70, 20)` | 56–84 ms |

All intervals randomised via `random_range()` to avoid fixed patterns.

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
| `WAIT_JITTER(ms, pct)` | delay base_ms ± pct% |
| `WAIT_JITTER_UP(ms, pct)` | delay in [ms, ms*(1+pct/100)] |
| `WAIT_JITTER_DOWN(ms, pct)` | delay in [ms*(1-pct/100), ms] |
| `WAIT_JITTER_UP_D(ms)` | `WAIT_JITTER_UP(ms, 20)` shorthand |
| `CALL_NAMED(name, fn)` | call a bool function; result in `last_result` |
| `CHANCE(pct)` | set `last_result` = true with pct% probability |
| `SKIP_TRUE(n)` / `SKIP_FALSE(n)` | conditional forward skip by n instructions |
| `END()` | terminate script |

All waits yield (return from `runner_task`) so the keyboard USB stack keeps running.

### Marksman job (`scripts/marksman/`)

**Key bindings (in-game):**
| Key | Skill | Cooldown |
|---|---|---|
| Q | Main attack (Snipe / basic shot) | — |
| D | Bolt Burst | 6 s |
| R | Surgebolt | 18 s |
| E | Jump | — |
| X | Teleport | — |
| C | Rope Lift | — |
| Alt | Dash | — |
| N | Janus (3 independent charges) | 57 s each |
| B | Erda Fountain | 57 s |
| 4 | Web | 250 s |
| 5 | Solar Crest | 250 s |
| Page Up | Boss Buffs | 119 s |
| F1 | Guild Buff | 30–32 min |
| F11 | Check Familiars (open/look 1.5–3s/close) | 1.5–3 min |
| Down+X×3 | Teleport setup (blink anchor) | 30–45 s |

**Cooldown system:** `job.c` tracks each skill with a `static uint32_t` timer. `TRY_*` macros call the function and skip the follow-up wait if the skill didn't fire (CD not ready).

**Key action macros (job.h):**
- `TELEPORT_RESET()` — probabilistic X taps: **10% = 1 tap, 60% = 2 taps, 30% = 3 taps** (10 cmd_t, uses CHANCE/SKIP internally)
- `TELEPORT_RESET_2X()` — fixed 2 taps of X (4 cmd_t)
- `FLASH_JUMP()` — double-tap E with timing
- `FLASH_UP()` — hold UP, double-tap E, release UP

**Dynamic script builders** (call at script-start time, return pointer to static buffer):
- `make_rotation_tallahart()` — weighted random picker (weighted_rotation_t table)
- `make_setup_tallahart()` — 50/50 between `make_setup_tallahart_up()` and `make_setup_tallahart_reset()`
- `make_loot_full()` — 2 UP positions with optional extra UP each, then TELEPORT_RESET_2X
- `make_random_human()` — picks from HUMAN_1–HUMAN_8 with equal weight

**Rotation weights (tallahart):**
- SHOOT: 39/65 = 60%
- BOLT_BURST: 13/65 = 20%
- Other 13 variants: 1/65 each = 20% total

**Scripts:**
- `rotation_script.c` — `ROTATION_TALLAHART_*` arrays + `make_rotation_tallahart()`
- `setup_script.c` — `make_setup_tallahart_up/reset/()` builders
- `loot_script.c` — `LOOT_EMPTY`, `make_loot_full()`
- `buff_script.c` — `BUFF_SCRIPT`: TRY_TELEPORT_SETUP → TRY_BOSS_BUFFS → TRY_GUILD_BUFF → TRY_CHECK_FAMILIARS
- `random_human_script.c` — HUMAN_1–HUMAN_8 + `make_random_human()`
- `limbo_script.c` — `LIMBO_SCRIPT` (Space + WAIT_JITTER_UP(1000, 150))
- `cubing_script.c` — `CUBING_SCRIPT` (4× Space + WAIT_JITTER_UP(100, 300))

### Timing / RNG (`engine/timing.c`)

- `timing_seed_rng()` — seeds `rand()` from `timer_read32()` at boot
- `jitter(base, pct)` — ± pct%
- `jitter_up(base, pct)` — [base, base*(1+pct/100)]
- `jitter_down(base, pct)` — [base*(1-pct/100), base]
- `random_range(base, min_pct, max_pct)` — base * random_in[min_pct, max_pct] / 100
- `timing_chance(pct)` — boolean with pct% probability

## Build

```bash
qmk compile -kb keychron/v3/ansi -km custom
```

Flash with QMK Toolbox or `qmk flash`.

For IDE support (resolve red squiggles), generate a compilation database:

```bash
qmk compile -kb keychron/v3/ansi -km default --compiledb
```

Then use the **clangd** language server — it will pick up `compile_commands.json` automatically.
Note: clangd will show false-positive errors for QMK-specific identifiers (KC_*, timer_*, uprintf, etc.) — these are harmless.

## File map

```
keyboards/keychron/v3/ansi/keymaps/custom/
  keymap.c                        — layer definitions, F7/F8/F9 toggle, policy loop, bot_mode_t
  common.h / common.c             — random_range(), keycode_to_string()
  rules.mk                        — adds all .c files to SRC, enables CONSOLE
  engine/
    script_engine.h / .c          — cmd_t, runner_t, runner_start/stop/task
    timing.h / .c                 — jitter variants, RNG seed, timing_chance
  scripts/
    cmd_macros.h                  — TAP/PRESS/RELEASE/WAIT_*/CALL/CHANCE/SKIP/END macros
    marksman/
      job.h / job.c               — per-skill cooldown functions + stateless action macros
      rotation_script.h / .c      — ROTATION_TALLAHART_* arrays + make_rotation_tallahart()
      setup_script.h / .c         — make_setup_tallahart*() builders
      loot_script.h / .c          — LOOT_EMPTY, make_loot_full()
      buff_script.h / .c          — BUFF_SCRIPT array
      random_human_script.h / .c  — HUMAN_1–8 + make_random_human()
      limbo_script.h / .c         — LIMBO_SCRIPT
      cubing_script.h / .c        — CUBING_SCRIPT
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

**`tap_code_delay` in job.c:** the `delay` parameter is the hold duration only — no post-release pause. Always add a `wait_ms(...)` between consecutive `tap_code_delay` calls.

### Linter

```bash
python3 lint_c_delays.py
```

Scans all `.c` and `.h` files under `keyboards/keychron/v3/ansi/keymaps/custom/`. Exit code 1 if violations found.

**Always run the linter after any code changes to the custom keymap.**

## Buffer sizing for dynamic builders

When TELEPORT_RESET() is used in a static `cmd_t[]` initialiser inside a builder function, its size (10 cmd_t) is computed at compile time via `sizeof(array) / sizeof(cmd_t)`. Always verify static buffer sizes cover the worst case when adding new commands.

- `TELEPORT_RESET()` = 10 cmd_t (probabilistic)
- `TELEPORT_RESET_2X()` = 4 cmd_t (fixed 2 taps)

## Adding a new map

1. Add `ROTATION_<MAP>`, `SETUP_<MAP>`, `LOOT_<MAP>` arrays/builders in the respective `*_script.c` files and declare them in the headers.
2. Update `keymap.c`: add a new `bot_mode_t` entry and handle it in `matrix_scan_user()`.
