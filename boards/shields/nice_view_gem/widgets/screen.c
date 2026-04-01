#include <zephyr/kernel.h>
#include <stdio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/split_peripheral_status_changed.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/battery.h>
#include <zmk/ble.h>
#include <zmk/display.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>
#include <zmk/usb.h>
#include <zmk/split/central.h>

#include "screen.h"

struct battery_status_state {
    uint8_t level;
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    bool usb_present;
#endif
};

struct battery_peripheral_status_state {
    uint8_t level;
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    bool usb_present;
#endif
};

struct layer_status_state {
    uint8_t index;
};

struct output_status_state {
    struct zmk_endpoint_instance selected_endpoint;
    int active_profile_index;
    bool active_profile_connected;
    bool active_profile_bonded;
};

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static bool sleep_active = false;

/**
 * Update all labels from current state
 **/

static void update_labels(struct zmk_widget_screen *widget) {
    const struct status_state *state = &widget->state;

    if (sleep_active) {
        lv_label_set_text(widget->battery_label, "");
        lv_label_set_text(widget->battery_p_label, "");
        lv_label_set_text(widget->output_label, "");
        lv_label_set_text(widget->layer_label, "SLEEP");
        lv_label_set_text(widget->profile_label, "");
        return;
    }

    /* Battery L */
    char bat_buf[16];
    if (state->charging) {
        snprintf(bat_buf, sizeof(bat_buf), "L:%d%%+", state->battery);
    } else {
        snprintf(bat_buf, sizeof(bat_buf), "L:%d%%", state->battery);
    }
    lv_label_set_text(widget->battery_label, bat_buf);

    /* Battery R (peripheral) */
    char bat_p_buf[16];
    if (state->charging_p) {
        snprintf(bat_p_buf, sizeof(bat_p_buf), "R:%d%%+", state->battery_p);
    } else {
        snprintf(bat_p_buf, sizeof(bat_p_buf), "R:%d%%", state->battery_p);
    }
    lv_label_set_text(widget->battery_p_label, bat_p_buf);

    /* Output status */
    switch (state->selected_endpoint.transport) {
    case ZMK_TRANSPORT_USB:
        lv_label_set_text(widget->output_label, "USB");
        break;
    case ZMK_TRANSPORT_BLE:
        if (state->active_profile_bonded) {
            if (state->active_profile_connected) {
                lv_label_set_text(widget->output_label, "BLE");
            } else {
                lv_label_set_text(widget->output_label, "BLE !");
            }
        } else {
            lv_label_set_text(widget->output_label, "BLE ?");
        }
        break;
    default:
        lv_label_set_text(widget->output_label, "---");
        break;
    }

    /* Layer name */
    const char *layer_name =
        zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(state->layer_index));
    if (layer_name == NULL || layer_name[0] == '\0') {
        char fallback[16];
        snprintf(fallback, sizeof(fallback), "L#%u", state->layer_index);
        lv_label_set_text(widget->layer_label, fallback);
    } else {
        lv_label_set_text(widget->layer_label, layer_name);
    }

    /* Profile index */
    char prof_buf[16];
    snprintf(prof_buf, sizeof(prof_buf), "P:%d", state->active_profile_index + 1);
    lv_label_set_text(widget->profile_label, prof_buf);
}

/**
 * Battery status (local)
 **/

static void set_battery_status(struct zmk_widget_screen *widget,
                               struct battery_status_state state) {
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    widget->state.charging = state.usb_present;
#endif
    widget->state.battery = state.level;
    update_labels(widget);
}

static void battery_status_update_cb(struct battery_status_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_battery_status(widget, state); }
}

static struct battery_status_state battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);
    return (struct battery_status_state){
        .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        .usb_present = zmk_usb_is_powered(),
#endif
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_status_state,
                            battery_status_update_cb, battery_status_get_state);

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_battery_status, zmk_usb_conn_state_changed);
#endif

/**
 * Battery status (peripheral)
 **/

static void set_battery_peripheral_status(struct zmk_widget_screen *widget,
                                          struct battery_peripheral_status_state state) {
    uint8_t level;
    zmk_split_central_get_peripheral_battery_level(0, &level);
    widget->state.battery_p = level;
    update_labels(widget);
}

static void
battery_peripheral_status_update_cb(struct battery_peripheral_status_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_battery_peripheral_status(widget, state);
    }
}

static struct battery_peripheral_status_state
battery_peripheral_status_get_state(const zmk_event_t *eh) {
    const struct zmk_peripheral_battery_state_changed *ev =
        as_zmk_peripheral_battery_state_changed(eh);
    return (struct battery_peripheral_status_state){
        .level = ev->state_of_charge,
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        .usb_present = zmk_usb_is_powered(),
#endif
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_peripheral_status,
                            struct battery_peripheral_status_state,
                            battery_peripheral_status_update_cb,
                            battery_peripheral_status_get_state);

ZMK_SUBSCRIPTION(widget_battery_peripheral_status, zmk_peripheral_battery_state_changed);

/**
 * Layer status
 **/

static void set_layer_status(struct zmk_widget_screen *widget, struct layer_status_state state) {
    widget->state.layer_index = zmk_keymap_highest_layer_active();
    update_labels(widget);
}

static void layer_status_update_cb(struct layer_status_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_layer_status(widget, state); }
}

static struct layer_status_state layer_status_get_state(const zmk_event_t *eh) {
    uint8_t index = zmk_keymap_highest_layer_active();
    return (struct layer_status_state){.index = index};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct layer_status_state, layer_status_update_cb,
                            layer_status_get_state)

ZMK_SUBSCRIPTION(widget_layer_status, zmk_layer_state_changed);

/**
 * Output status
 **/

static void set_output_status(struct zmk_widget_screen *widget,
                              const struct output_status_state *state) {
    widget->state.selected_endpoint = state->selected_endpoint;
    widget->state.active_profile_index = state->active_profile_index;
    widget->state.active_profile_connected = state->active_profile_connected;
    widget->state.active_profile_bonded = state->active_profile_bonded;
    update_labels(widget);
}

static void output_status_update_cb(struct output_status_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_output_status(widget, &state); }
}

static struct output_status_state output_status_get_state(const zmk_event_t *_eh) {
    return (struct output_status_state){
        .selected_endpoint = zmk_endpoint_get_selected(),
        .active_profile_index = zmk_ble_active_profile_index(),
        .active_profile_connected = zmk_ble_active_profile_is_connected(),
        .active_profile_bonded = !zmk_ble_active_profile_is_open(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_output_status, struct output_status_state,
                            output_status_update_cb, output_status_get_state)
ZMK_SUBSCRIPTION(widget_output_status, zmk_endpoint_changed);

#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_output_status, zmk_usb_conn_state_changed);
#endif
#if defined(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(widget_output_status, zmk_ble_active_profile_changed);
#endif

/**
 * Activity state handling for sleep screen
 **/

static void force_redraw_all_widgets(void) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { update_labels(widget); }
}

static int display_activity_event_handler(const zmk_event_t *eh) {
    struct zmk_activity_state_changed *ev = as_zmk_activity_state_changed(eh);
    if (ev == NULL) {
        return -ENOTSUP;
    }

    switch (ev->state) {
    case ZMK_ACTIVITY_ACTIVE:
        sleep_active = false;
        break;
    case ZMK_ACTIVITY_SLEEP:
        sleep_active = true;
        force_redraw_all_widgets();
        lv_timer_handler();
        lv_refr_now(lv_display_get_default());
        break;
    default:
        break;
    }
    return 0;
}

ZMK_LISTENER(nice_view_gem_display, display_activity_event_handler);
ZMK_SUBSCRIPTION(nice_view_gem_display, zmk_activity_state_changed);

/**
 * Initialization — create LVGL label widgets
 **/

static lv_obj_t *create_label(lv_obj_t *parent, const lv_font_t *font, lv_coord_t x,
                               lv_coord_t y) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, "");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_pos(label, x, y);
    return label;
}

int zmk_widget_screen_init(struct zmk_widget_screen *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_style_bg_color(widget->obj, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(widget->obj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(widget->obj, 0, 0);
    lv_obj_set_style_pad_all(widget->obj, 0, 0);

    /* Battery labels at top */
    widget->battery_label = create_label(widget->obj, &lv_font_montserrat_14, 4, 4);
    widget->battery_p_label = create_label(widget->obj, &lv_font_montserrat_14, 80, 4);

    /* Output status */
    widget->output_label = create_label(widget->obj, &lv_font_montserrat_14, 4, 30);

    /* Layer name — centered, larger font */
    widget->layer_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->layer_label, "");
    lv_obj_set_style_text_color(widget->layer_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(widget->layer_label, &lv_font_montserrat_18, 0);
    lv_obj_set_width(widget->layer_label, SCREEN_WIDTH);
    lv_obj_set_style_text_align(widget->layer_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(widget->layer_label, 0, 70);

    /* Profile index at bottom */
    widget->profile_label = create_label(widget->obj, &lv_font_montserrat_14, 4, 140);

    sys_slist_append(&widgets, &widget->node);

    return 0;
}

lv_obj_t *zmk_widget_screen_obj(struct zmk_widget_screen *widget) { return widget->obj; }

