#!/usr/bin/env python3
"""
Lint C script arrays and #define macros: every TAP / TAP_D / PRESS / RELEASE
must be immediately followed by a WAIT* macro at the same level.

Two contexts are checked:
  1. const cmd_t NAME[] = { ... }  — script arrays
  2. #define NAME(...)  TOKEN, TOKEN, ...  — compound action macros in headers

Compound macros used inside arrays (JUMP_ATTACK, SHOOT, etc.) are treated as
single opaque tokens — their internals are checked separately via #define linting.

Usage:
    python lint_c_delays.py [file ...]

With no arguments, scans all .c and .h files under
keyboards/keychron/v3/ansi/keymaps/maple/.
Append '// noqa' to the offending source line to suppress a warning.
Exit code 1 if violations found.
"""

import re
import sys
from pathlib import Path

ROOT = Path(__file__).parent

KEY_ACTION_MACROS = {"TAP", "TAP_D", "PRESS", "RELEASE"}

MIN_TAP_MS = 40  # below this a TAP_D duration looks inhuman

# WAIT_JITTER variants that take an explicit pct argument (not the _D defaults)
EXPLICIT_JITTER_MACROS = {"WAIT_JITTER", "WAIT_JITTER_UP", "WAIT_JITTER_DOWN"}


# ---------------------------------------------------------------------------
# Source pre-processing
# ---------------------------------------------------------------------------

def strip_comments(text: str) -> str:
    """Remove comments while preserving line count (for accurate line numbers)."""
    # Replace // comments with spaces (keeps line structure intact)
    text = re.sub(r"//[^\n]*", lambda m: " " * len(m.group()), text)
    # Replace /* */ comments with matching newlines only
    text = re.sub(
        r"/\*.*?\*/",
        lambda m: "\n" * m.group().count("\n"),
        text,
        flags=re.DOTALL,
    )
    return text


# ---------------------------------------------------------------------------
# Array extraction
# ---------------------------------------------------------------------------

def extract_defines(source: str):
    """
    Yield (macro_name, body, line_no) for every #define whose expansion
    contains at least one cmd_t-like token.  Handles multi-line macros
    by joining backslash-continuation lines.
    """
    lines = source.splitlines()
    i = 0
    while i < len(lines):
        m = re.match(r"\s*#define\s+(\w+)(?:\([^)]*\))?\s+(.*)", lines[i])
        if m:
            name = m.group(1)
            line_no = i + 1
            parts = [lines[i].rstrip()]
            while parts[-1].rstrip().endswith("\\") and i + 1 < len(lines):
                i += 1
                parts.append(lines[i].rstrip())
            # Strip backslash continuations and join
            body = " ".join(p.rstrip("\\").strip() for p in parts)
            # Only keep the part after the macro signature
            sig_m = re.match(r"\s*#define\s+\w+(?:\([^)]*\))?\s+(.*)", body, re.DOTALL)
            if sig_m:
                yield name, sig_m.group(1).strip(), line_no
        i += 1


def extract_arrays(source: str):
    """
    Yield (array_name, body, body_line) for every const cmd_t NAME[] = { ... }.
    body_line is the 1-based line number of the opening '{'.
    """
    pattern = re.compile(r"const\s+cmd_t\s+(\w+)\s*\[\s*\]\s*=\s*\{", re.DOTALL)
    for m in pattern.finditer(source):
        name = m.group(1)
        body_start = m.end()
        body_line = source[:body_start].count("\n") + 1

        # Walk forward to find the matching '}'
        depth = 1
        i = body_start
        while i < len(source) and depth > 0:
            if source[i] == "{":
                depth += 1
            elif source[i] == "}":
                depth -= 1
            i += 1

        body = source[body_start : i - 1]
        yield name, body, body_line


# ---------------------------------------------------------------------------
# Tokenizer
# ---------------------------------------------------------------------------

def _macro_name(token: str) -> str | None:
    m = re.match(r"(\w+)\s*\(", token.strip())
    return m.group(1) if m else None


def tokenize_body(body: str, body_line: int):
    """
    Split the array body on top-level commas (respecting paren depth).
    Returns list of (macro_name, line_no, raw_token).
    line_no is 1-based within the file.
    """
    tokens = []
    depth = 0
    buf: list[str] = []
    lines_so_far = 0    # newlines seen in body so far
    tok_line = 0        # newlines at start of current token (relative to body)

    for ch in body:
        if ch == "\n":
            lines_so_far += 1

        if ch == "(":
            depth += 1
            buf.append(ch)
        elif ch == ")":
            depth -= 1
            buf.append(ch)
        elif ch == "," and depth == 0:
            raw = "".join(buf).strip()
            if raw:
                tokens.append((_macro_name(raw), body_line + tok_line, raw))
            buf = []
            tok_line = lines_so_far  # next token starts around here
        else:
            buf.append(ch)

    raw = "".join(buf).strip()
    if raw:
        tokens.append((_macro_name(raw), body_line + tok_line, raw))

    return tokens


# ---------------------------------------------------------------------------
# Lint logic
# ---------------------------------------------------------------------------

def _check_tokens(tokens, source_lines, path, label):
    """Shared violation checker for both array and define token lists."""
    violations = []
    for i, (name, line, _) in enumerate(tokens):
        if name not in KEY_ACTION_MACROS:
            continue

        orig = source_lines[line - 1] if 0 < line <= len(source_lines) else ""
        if "// noqa" in orig:
            continue

        if i + 1 >= len(tokens):
            violations.append((path, line, label,
                f"{name}() at end of definition with no WAIT* after"))
        else:
            next_name, _, _ = tokens[i + 1]
            if not next_name or not next_name.startswith("WAIT"):
                violations.append((path, line, label,
                    f"{name}() followed by {next_name or '?'}() — expected WAIT*"))
    return violations


def _extract_args(raw: str) -> list[str]:
    """Split the arguments inside a macro call on top-level commas."""
    m = re.match(r"\w+\s*\((.*)\)\s*$", raw.strip(), re.DOTALL)
    if not m:
        return []
    depth, buf, args = 0, [], []
    for ch in m.group(1):
        if ch == "(":
            depth += 1
            buf.append(ch)
        elif ch == ")":
            depth -= 1
            buf.append(ch)
        elif ch == "," and depth == 0:
            args.append("".join(buf).strip())
            buf = []
        else:
            buf.append(ch)
    if buf:
        args.append("".join(buf).strip())
    return args


def _try_int(s: str) -> int | None:
    try:
        return int(s.strip())
    except (ValueError, AttributeError):
        return None


def _noqa(source_lines, line):
    orig = source_lines[line - 1] if 0 < line <= len(source_lines) else ""
    return "// noqa" in orig


def _check_tap_duration(tokens, source_lines, path, label):
    """TAP_D(kc, ms) — ms must be >= MIN_TAP_MS (or 0 which uses the engine default)."""
    violations = []
    for name, line, raw in tokens:
        if name != "TAP_D" or _noqa(source_lines, line):
            continue
        args = _extract_args(raw)
        if len(args) >= 2:
            ms = _try_int(args[1])
            if ms is not None and ms != 0 and ms < MIN_TAP_MS:
                violations.append((path, line, label,
                    f"TAP_D duration {ms} ms is below minimum {MIN_TAP_MS} ms — looks inhuman"))
    return violations


ALL_WAIT_MACROS = {
    "WAIT",
    "WAIT_JITTER", "WAIT_JITTER_UP", "WAIT_JITTER_DOWN",
    "WAIT_JITTER_D", "WAIT_JITTER_UP_D", "WAIT_JITTER_DOWN_D",
}


def _check_zero_jitter(tokens, source_lines, path, label):
    """
    Three sub-checks, all suppressible with // noqa:
      1. WAIT(ms) — fixed delay with no jitter, always flagged.
      2. WAIT_JITTER*(ms, 0) — explicit zero pct, flagged.
      3. Any WAIT*(0, ...) or WAIT*(0) — zero ms duration, flagged.
    """
    violations = []
    for name, line, raw in tokens:
        if name not in ALL_WAIT_MACROS:
            continue
        if _noqa(source_lines, line):
            continue

        args = _extract_args(raw)

        # Rule: zero ms duration on any WAIT variant
        if args and _try_int(args[0]) == 0:
            violations.append((path, line, label,
                f"{name}(0, ...) has zero ms duration"))
            continue  # no need to also check pct for a zero-ms wait

        # Rule: WAIT has no jitter at all
        if name == "WAIT":
            violations.append((path, line, label,
                "WAIT() has no jitter — use WAIT_JITTER_UP_D / WAIT_JITTER_D instead"))

        # Rule: explicit zero pct on WAIT_JITTER variants
        elif name in EXPLICIT_JITTER_MACROS and len(args) >= 2 and _try_int(args[1]) == 0:
            violations.append((path, line, label,
                f"{name}({args[0]}, 0) has zero jitter — pct must be > 0"))

    return violations


def lint_file(path: Path):
    raw = path.read_text()
    source_lines = raw.splitlines()
    source = strip_comments(raw)
    violations = []

    for array_name, body, body_line in extract_arrays(source):
        tokens = tokenize_body(body, body_line)
        violations.extend(_check_tokens(tokens, source_lines, path, array_name))
        violations.extend(_check_tap_duration(tokens, source_lines, path, array_name))
        violations.extend(_check_zero_jitter(tokens, source_lines, path, array_name))

    for macro_name, body, line_no in extract_defines(source):
        tokens = tokenize_body(body, line_no)
        violations.extend(_check_tokens(tokens, source_lines, path, f"#define {macro_name}"))
        violations.extend(_check_tap_duration(tokens, source_lines, path, f"#define {macro_name}"))
        violations.extend(_check_zero_jitter(tokens, source_lines, path, f"#define {macro_name}"))

        # TRY_* macros must end with a WAIT*
        if macro_name.startswith("TRY_") and tokens:
            last_name, last_line, _ = tokens[-1]
            orig = source_lines[last_line - 1] if 0 < last_line <= len(source_lines) else ""
            if "// noqa" not in orig and (not last_name or not last_name.startswith("WAIT")):
                violations.append((path, line_no, f"#define {macro_name}",
                    f"TRY_* macro must end with WAIT* but ends with {last_name or '?'}()"))

    return violations


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main():
    if len(sys.argv) > 1:
        files = [Path(f) for f in sys.argv[1:]]
    else:
        maple = ROOT / "keyboards/keychron/v3/ansi/keymaps/maple"
        files = sorted([*maple.rglob("*.c"), *maple.rglob("*.h")])

    total = 0
    for f in files:
        for path, line, array, msg in lint_file(f):
            print(f"{path}:{line}: [{array}] {msg}")
            total += 1

    if total:
        print(f"\n{total} violation(s) found.")
        sys.exit(1)
    else:
        print("No violations found.")


if __name__ == "__main__":
    main()
