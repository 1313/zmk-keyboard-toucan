# Changelog

All notable changes to this fork are tracked here. Format roughly follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/). Upstream
beekeeb/zmk-keyboard-toucan history is intentionally not duplicated.

## [Unreleased]

### Added
- `.pre-commit-config.yaml`: trailing-whitespace, end-of-file-fixer, YAML
  validation, and clang-format hooks.
- `CONTRIBUTING.md`: local build instructions, flashing notes, and PR style.
- `CHANGELOG.md` (this file).

### Changed
- All C sources in `boards/shields/nice_view_gem/` reformatted with the
  repo `.clang-format`. The `sleep_icon_map` byte array is wrapped in
  `// clang-format off`/`on` to preserve its 12-bytes-per-line layout
  (one source line ↔ one image row).

## 2026-05-15 — Polish, perf and post-migration cleanup

### Removed
- Orphan `snippets/toucan-sdc/` (left over from the SoftDevice experiment).
- `widgets/wpm.c`, `widgets/wpm.h` (literal stub files; never compiled).
- Two unused `#include`s in `widgets/screen.c`
  (`split_peripheral_status_changed.h`, `wpm_state_changed.h`).
- 5.5 GB local `.build-workspace/` scratch directory (gitignored).
- Reverted the SoftDevice Controller (SDC) integration experiment after 9
  iterations confirmed it requires forking sdk-nrf or upstreaming to ZMK.
  Detailed postmortem is in commit `b8e1a94`.

### Added
- `.gitignore` covering `.west/`, `build/`, `firmware-artifacts/`,
  west-managed module dirs, and macOS noise.
- `nrf52840-nosd` snippet on every build variant: recovers ~152 KB of
  flash that the default `xiao_ble` layout reserves for the unused
  SoftDevice region (788 KB → 844 KB code, 32 KB → 128 KB storage).
- `release.yml` GitHub Actions workflow: pushes a `v*` tag → publishes
  a release with the firmware UF2s.
- `scripts/build.sh`: single-command local builds for each variant.
- `.clang-format` matching upstream zmkfirmware/zmk style.
- Battery percentage text under each battery icon on the display.
- `toucan_left` build variant without ZMK Studio (Studio is now in a
  separate `toucan_left_studio` artifact).
- README sections for Build, Variants, and "Differences from upstream".
- `pre-cleanup-2026-05` git tag pointing at the pre-cleanup state.
- `archive/button-release-stuck` tag on `1313/cirque-input-module`
  preserving the trackpad bug fix that shipped in the deleted custom
  cirque module.

### Performance
- Trackpad SPI clock 1 MHz → 6 MHz (Pinnacle 1CA027 supports up to
  13 MHz; 1 MHz had been there since the initial commit and was never
  tuned). Each register read drops from ~100 µs to ~17 µs.
- Host-side BLE preferred connection params on the LEFT (central):
  `BT_PERIPHERAL_PREF_MIN_INT=6`, `MAX_INT=6`, `LATENCY=0`. Keypress
  reports go out on the very next 7.5 ms connection event.
- `CONFIG_PM_DEVICE_RUNTIME=y` on both halves; SPI bus and other
  devices can suspend between transfers (with `PM_DEVICE_SYSTEM_MANAGED=y`
  workaround so ZMK's deep-sleep path still has `zmk_pm_suspend_devices`).
- `CONFIG_ZMK_HID_REPORT_TYPE_NKRO=y`: full N-key rollover.
- `CONFIG_BOOT_BANNER=n`, `CONFIG_BT_DIS_PNP=n`: drop unused subsystems.
- `CONFIG_ZMK_DISPLAY_BLANK_ON_IDLE=y`: blank the display after 30 s
  idle to save power.
- Dropped the 4096-byte override of
  `CONFIG_ZMK_DISPLAY_DEDICATED_THREAD_STACK_SIZE`; default 2048 is
  plenty for the simple draw_text/draw_img widgets. Saves 2 KB RAM.

### Fixed
- 16 distinct bugs in `boards/shields/nice_view_gem/`:
  - NULL-pointer deref in `battery_peripheral_status_get_state` (the
    matching `battery_status_get_state` already null-checked; the
    peripheral version did not).
  - Uninitialized read of `level` after
    `zmk_split_central_get_peripheral_battery_level` ignored its
    return code.
  - Battery icon disappeared at 0% and 1% (the chained `else if`
    ladder ended with `level > 1`, so nothing was drawn at the most
    critical state).
  - `bolt`/`charging` scaffolding was dead — declared but never drawn,
    `state.charging` was never read. Cleaned up the scaffolding (will
    be re-implemented properly later when an actual bolt asset exists).
  - `&mo TAB` in the ADJ layer was `&mo 43` (TAB keycode), a no-op
    binding. Fixed to `&kp TAB` in both the user keymap and the shield
    default keymap.
  - `set_battery_peripheral_status` re-fetched via the central API
    instead of using the event's `state.level` (race + uninitialized
    read combined). Fixed to use the event.
  - `LVGL_BACKGROUND`/`LVGL_FOREGROUND` ternary macros lacked outer
    parens (footgun).
  - `sprintf` → `snprintf` in `layer.c`; added explicit
    `<inttypes.h>` include for `PRIu8`.
  - `canvas_draw_text` clipped to full canvas height instead of the
    label height; fixed bbox calculation.
  - Hardcoded scroller layer indexes (`<1 2>`) now have an inline
    comment naming NAV/SYM so future reordering flags the dependency.
  - Removed ~35 lines of commented-out v8-API draw code in `output.c`.
  - Removed dead `peripheral_status_state` struct in `output.h`.
  - Removed dead `connection_status_state` struct and unused
    `<zmk/usb.h>` include in `screen.c`.
  - "reconenect" → "reconnect" typo in conf comments.

### Repo hygiene
- Repo went 4.4 GB → 2.6 MB after deleting all the downloaded west
  modules (`zephyr/`, `zmk/`, `modules/`, `optional/`, `nice-view-*`,
  `cirque-input-module/`).
- Removed broken root-level `toucan_*.conf` symlinks (same broken
  pattern that the migration commit had removed from `config/`).
- `main` 38 commits ahead of upstream beekeeb (intentional fork
  divergence with personal customizations).

## Pre-fork — see upstream `beekeeb/zmk-keyboard-toucan`

For history before this fork started diverging, see the upstream
repository.
