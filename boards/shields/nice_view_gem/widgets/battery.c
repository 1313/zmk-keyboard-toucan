#include <zephyr/kernel.h>
#include <stdio.h>

#include "battery.h"
#include "../assets/custom_fonts.h"

LV_IMAGE_DECLARE(l_battery_100);
LV_IMAGE_DECLARE(l_battery_90);
LV_IMAGE_DECLARE(l_battery_75);
LV_IMAGE_DECLARE(l_battery_50);
LV_IMAGE_DECLARE(l_battery_25);
LV_IMAGE_DECLARE(l_battery_10);


static void draw_level(lv_obj_t *canvas, const struct status_state *state) {
    lv_draw_image_dsc_t img_dsc_l;
    lv_draw_image_dsc_init(&img_dsc_l);

    uint8_t level = state->battery;
    if (level > 90) {
        canvas_draw_img(canvas, 8, 10, &l_battery_100, &img_dsc_l);
    } else if (level > 75) {
        canvas_draw_img(canvas, 8, 10, &l_battery_90, &img_dsc_l);
    } else if (level > 50) {
        canvas_draw_img(canvas, 8, 10, &l_battery_75, &img_dsc_l);
    } else if (level > 25) {
        canvas_draw_img(canvas, 8, 10, &l_battery_50, &img_dsc_l);
    } else if (level > 10) {
        canvas_draw_img(canvas, 8, 10, &l_battery_25, &img_dsc_l);
    } else {
        canvas_draw_img(canvas, 8, 10, &l_battery_10, &img_dsc_l);
    }

    char pct[5];
    snprintf(pct, sizeof(pct), "%u%%", state->battery);
    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &quinquefive_8, LV_TEXT_ALIGN_LEFT);
    canvas_draw_text(canvas, 8, 42, 56, &label_dsc, pct);
}

void draw_battery_status(lv_obj_t *canvas, const struct status_state *state) {
    draw_level(canvas, state);
}