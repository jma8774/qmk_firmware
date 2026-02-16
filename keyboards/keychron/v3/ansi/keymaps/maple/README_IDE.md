# Fixing "errors" in keymap C code (IDE only)

The red squiggles in `keymap.c` are **not real compile errors**. They appear because the editor’s C/C++ linter doesn’t know QMK’s include paths and macros. The firmware builds fine with QMK.

## Fix: use a compilation database

Generate a `compile_commands.json` so the IDE uses the same includes and defines as QMK.

### 1. Generate `compile_commands.json`

In your **QMK MSYS2** (or other terminal where `qmk` works), from the **qmk_firmware** directory run:

```bash
qmk compile -kb keychron/v3/ansi -km default --compiledb
```

This creates `compile_commands.json` in the `qmk_firmware` folder. The first build may take a few minutes.

### 2. Use the right language server

- **clangd** (recommended by QMK): it will find `compile_commands.json` automatically for files under `qmk_firmware`. Install the “clangd” extension if you don’t have it, then run **“clangd: Restart Language Server”** from the Command Palette (Ctrl+Shift+P).
- **Microsoft C/C++**: if your workspace root is `product` (parent of `qmk_firmware`), add the config in the next section so it uses the compilation database.

### 3. If errors persist

- Run **“clangd: Restart Language Server”** after generating `compile_commands.json`.
- If clangd still shows odd macro errors, see [QMK #23968](https://github.com/qmk/qmk_firmware/issues/23968): you may need to remove the `__has_include` / `__has_include_next` entries from `compile_commands.json`.

Your keymap code is valid QMK; the IDE just needs this database to understand it.
