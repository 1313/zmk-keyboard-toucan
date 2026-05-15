#!/usr/bin/env bash
# Local build helper for the Toucan firmware.
# Usage: ./scripts/build.sh <left|left_studio|right|right_debug|reset>
# Requires a working west workspace (see README.md "Build" section).

set -euo pipefail

target="${1:-}"
case "$target" in
    left)
        shield="toucan_left rgbled_adapter nice_view_gem"
        extra=(--snippet nrf52840-nosd)
        ;;
    left_studio)
        shield="toucan_left rgbled_adapter nice_view_gem"
        extra=(--snippet "nrf52840-nosd studio-rpc-usb-uart" -- -DCONFIG_ZMK_STUDIO=y)
        ;;
    right)
        shield="toucan_right rgbled_adapter"
        extra=(--snippet nrf52840-nosd)
        ;;
    right_debug)
        shield="toucan_right rgbled_adapter"
        extra=(--snippet "nrf52840-nosd zmk-usb-logging" -- -DCONFIG_INPUT_EVENT_DUMP=y)
        ;;
    reset)
        shield="settings_reset"
        extra=(--snippet nrf52840-nosd)
        ;;
    *)
        echo "usage: $0 <left|left_studio|right|right_debug|reset>" >&2
        exit 2
        ;;
esac

build_dir="build/${target}"
rm -rf "$build_dir"
west build -d "$build_dir" -s zmk/app -b xiao_ble//zmk -- -DSHIELD="$shield" "${extra[@]}"
echo
echo "Firmware: $build_dir/zephyr/zmk.uf2"
