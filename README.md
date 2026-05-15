# ZMK config for beekeeb Toucan Keyboard

[The beekeeb Toucan Keyboard](https://beekeeb.com/toucan-keyboard/) is a wireless split 42-key column‑stagger keyboard with a display and a trackpad, with an aggressive stagger on the pinky columns.

This is a personal fork of [`beekeeb/zmk-keyboard-toucan`](https://github.com/beekeeb/zmk-keyboard-toucan) with customizations for my own use. See **Differences from upstream** below if you arrived here looking for the original.

## Build

CI builds via GitHub Actions on every push (see `.github/workflows/build.yml`). The matrix is in [`build.yaml`](build.yaml). Local builds use [west](https://docs.zephyrproject.org/latest/develop/west/index.html):

```sh
west init -l config
west update
west zephyr-export
west build -s zmk/app -b xiao_ble//zmk -- -DSHIELD="toucan_left rgbled_adapter nice_view_gem"
```

Built variants:

| Artifact | What it is |
|---|---|
| `toucan_left rgbled_adapter nice_view_gem` | Central half (with display) |
| `toucan_left_studio` | Same as above plus ZMK Studio support |
| `toucan_right rgbled_adapter` | Peripheral half (with trackpad) |
| `toucan_right_debug` | Same as right, plus USB logging + `INPUT_EVENT_DUMP` for trackpad debugging |
| `settings_reset` | Standard ZMK settings-reset firmware |

## Differences from upstream

- **ZMK main + Zephyr 4.1**: migrated off the v0.3 release. Board name is `xiao_ble//zmk` (Zephyr's renamed `seeeduino_xiao_ble`).
- **Native Zephyr Pinnacle trackpad driver**: dropped the [`geeksville/cirque-input-module`](https://github.com/geeksville/cirque-input-module) fork. The native driver fixes the interrupt re-enable bug that caused stuck button state on double-tap-and-hold and properly supports `primary-tap-enable`.
- **Tap-to-click enabled** on the trackpad.
- **Positional hold-tap home-row mods** (GACS layout) on the base layer.
- **APP layer** (left thumb) with a yabai-style window manager grid + Raycast/system shortcuts on the right.
- **Tuned BLE split parameters** for trackpad throughput (7.5 ms connection interval on the right).
- **Display restoration**: full Toucan widgets work after the LVGL v9 / Zephyr 4.1 migration; trackpad-deadlock fix in place.
- **BLE status indicator** correctly shows "BLE" instead of NULL.
- **Deep sleep disabled on the central half** as a workaround for [zmkfirmware/zmk#3207](https://github.com/zmkfirmware/zmk/issues/3207) (central fails to wake from deep sleep on ZMK main, drains battery). Re-enable once that issue is fixed.

# License

The code in this repo is available under the MIT license.

The included shield nice_view_gem is modified from https://github.com/M165437/nice-view-gem licensed under the MIT License.

ZMK code snippets are taken from the ZMK documentation under the MIT license.

The embedded font QuinqueFive is designed by GGBotNet, licensed under under the SIL Open Font License, Version 1.1.
