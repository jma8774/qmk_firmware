"""Lint: every tap/press/release must be followed by a delay.

Checks two patterns:
  1. Back-to-back key actions with no sleep_ms or delay function between
  2. Key action as the last statement in a block, or right before return

Usage:
    python lint_delays.py [file ...]

Scans all .py files under py/ if no arguments given.
Append '# noqa' to a line to suppress the warning.
Exit code 1 if violations found.
"""

import ast
import sys
from pathlib import Path

KEY_ACTIONS = {"tap", "tap_d", "tap_raw", "press", "release"}

DELAY_FUNCS = {
    "sleep_ms",
    "flash_jump", "shoot", "dash", "rope",
    "jump_attack", "jump_attack_delay", "jump_down_delay",
    "teleport_reset", "teleport_setup_try",
    "full_reset_carcion", "full_reset_nr1",
    "rune_flash_jump", "rune_jump", "rune_rope",
    "rune_jump_down", "rune_protect", "rune_interact",
}

SKIP_FILES = {"keys.py", "lint_delays.py"}


def _call_name(stmt: ast.stmt):
    """Return the function/method name if stmt is a bare Call expression."""
    if isinstance(stmt, ast.Expr) and isinstance(stmt.value, ast.Call):
        func = stmt.value.func
        if isinstance(func, ast.Name):
            return func.id
        if isinstance(func, ast.Attribute):
            return func.attr
    return None


def _collect_bodies(node: ast.AST):
    """Recursively collect every statement-list body in the AST."""
    bodies = []

    if isinstance(node, (ast.Module, ast.FunctionDef, ast.AsyncFunctionDef)):
        bodies.append(node.body)
    elif isinstance(node, ast.If):
        bodies.append(node.body)
        if node.orelse:
            bodies.append(node.orelse)
    elif isinstance(node, (ast.For, ast.While)):
        bodies.append(node.body)
        if node.orelse:
            bodies.append(node.orelse)
    elif isinstance(node, (ast.With, ast.AsyncWith)):
        bodies.append(node.body)
    elif isinstance(node, ast.Try):
        bodies.append(node.body)
        for handler in node.handlers:
            bodies.append(handler.body)
        if node.orelse:
            bodies.append(node.orelse)
        if node.finalbody:
            bodies.append(node.finalbody)

    for child in ast.iter_child_nodes(node):
        bodies.extend(_collect_bodies(child))

    return bodies


def lint_file(path: Path):
    source = path.read_text()
    lines = source.splitlines()
    try:
        tree = ast.parse(source, filename=str(path))
    except SyntaxError:
        return []

    violations = []

    for body in _collect_bodies(tree):
        for i, stmt in enumerate(body):
            name = _call_name(stmt)
            if name not in KEY_ACTIONS:
                continue
            if stmt.lineno <= len(lines) and "# noqa" in lines[stmt.lineno - 1]:
                continue

            if i + 1 < len(body):
                nxt = body[i + 1]
                nxt_name = _call_name(nxt)
                if nxt_name in KEY_ACTIONS:
                    violations.append(
                        (stmt.lineno, f"{name}() followed by {nxt_name}() with no delay")
                    )
                elif isinstance(nxt, ast.Return):
                    violations.append(
                        (stmt.lineno, f"{name}() immediately before return with no delay")
                    )
            else:
                violations.append(
                    (stmt.lineno, f"{name}() at end of block with no delay after")
                )

    return violations


def main():
    if len(sys.argv) > 1:
        files = [Path(f) for f in sys.argv[1:]]
    else:
        files = sorted(Path(__file__).parent.glob("**/*.py"))

    files = [f for f in files if f.name not in SKIP_FILES]

    total = 0
    for f in files:
        for lineno, msg in lint_file(f):
            print(f"{f}:{lineno}: {msg}")
            total += 1

    if total:
        print(f"\n{total} violation(s) found.")
        sys.exit(1)
    else:
        print("No violations found.")


if __name__ == "__main__":
    main()
