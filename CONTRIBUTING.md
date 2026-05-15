# Contributing to zmk-keyboard-toucan (personal fork)

This is a personal fork of [beekeeb/zmk-keyboard-toucan](https://github.com/beekeeb/zmk-keyboard-toucan)
with my own customizations. PRs and issues are welcome but the keymap and
default behaviour reflect personal preferences — see
[README.md](README.md#differences-from-upstream).

## Local build

You need:
- Python 3.10+ with [`west`](https://docs.zephyrproject.org/latest/develop/west/index.html)
  (`pipx install west` works fine)
- Either:
  - The Zephyr SDK (toolchain + headers), or
  - Docker, in which case `zmkfirmware/zmk-build-arm:stable` mirrors CI exactly

### One-time workspace setup

```sh
git clone https://github.com/1313/zmk-keyboard-toucan.git
cd zmk-keyboard-toucan
west init -l config
west update
west zephyr-export
```

This populates `zephyr/`, `zmk/`, `zmk-rgbled-widget/`, and `modules/` as
sibling directories. They're all gitignored.

### Build a firmware variant

```sh
./scripts/build.sh left          # Central half (no Studio)
./scripts/build.sh left_studio   # Central with ZMK Studio support
./scripts/build.sh right         # Peripheral half with trackpad
./scripts/build.sh right_debug   # Peripheral with USB logging + INPUT_EVENT_DUMP
./scripts/build.sh reset         # Settings-reset firmware
```

The resulting `*.uf2` is at `build/<target>/zephyr/zmk.uf2`.

### Flashing

1. Double-tap the reset button on the xiao_ble. A `XIAO-BOOT` USB drive appears.
2. Drag the `.uf2` onto it. The drive disconnects and the keyboard reboots.

For a fresh device or to clear settings, flash `settings_reset` first, then
the firmware you want.

## Coding style

- C code follows the `.clang-format` (LLVM, 4-space indent, 100 col).
  This matches upstream `zmkfirmware/zmk`.
- Run `clang-format -i <file>` before committing C changes.
- Don't reformat large generated arrays (asset bitmaps, fonts);
  guard them with `// clang-format off`/`on`.

## Pre-commit hooks (optional but recommended)

```sh
pipx install pre-commit
pre-commit install
```

This installs trailing-whitespace, end-of-file-fixer, YAML validation,
and clang-format hooks that run on `git commit`.

## Submitting changes

- Branch off `main`.
- Commit messages: subject ≤72 chars, imperative mood, body explains the
  why (look at recent commits for examples).
- Include the `Co-authored-by: Copilot` trailer if AI-assisted, per repo
  convention.
- CI must pass on the PR (it builds all five firmware variants).

## Reporting issues

If something's broken with the firmware (BLE drops, trackpad behaviour,
display glitches), open an issue with:
- Which build variant
- ZMK / Zephyr revision (visible in CI logs)
- Steps to reproduce
- Whether the same thing reproduces on upstream `beekeeb/zmk-keyboard-toucan`
  (helps localise to this fork's customisations)
