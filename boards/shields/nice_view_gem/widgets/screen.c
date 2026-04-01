#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include "screen.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

int zmk_widget_screen_init(struct zmk_widget_screen *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_style_bg_color(widget->obj, LVGL_BACKGROUND, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(widget->obj, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t *canvas = lv_canvas_create(widget->obj);
    lv_obj_align(canvas, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_canvas_set_buffer(canvas, widget->cbuf, CANVAS_W, CANVAS_H, CANVAS_COLOR_FORMAT);

    fill_background(canvas);

    /* Border around entire canvas */
    lv_draw_rect_dsc_t rect_fg;
    init_rect_dsc(&rect_fg, LVGL_FOREGROUND);
    canvas_draw_rect(canvas, 0, 0, CANVAS_W, CANVAS_H, &rect_fg);

    lv_draw_rect_dsc_t rect_bg;
    init_rect_dsc(&rect_bg, LVGL_BACKGROUND);
    canvas_draw_rect(canvas, 2, 2, CANVAS_W - 4, CANVAS_H - 4, &rect_bg);

    /* Top: "ZMK" */
    lv_draw_label_dsc_t label_lg;
    init_label_dsc(&label_lg, LVGL_FOREGROUND, &lv_font_montserrat_18, LV_TEXT_ALIGN_CENTER);
    canvas_draw_text(canvas, 0, 10, CANVAS_W, &label_lg, "ZMK");

    /* Middle: "TOUCAN" */
    canvas_draw_text(canvas, 0, 70, CANVAS_W, &label_lg, "TOUCAN");

    /* Bottom: "144x168" */
    canvas_draw_text(canvas, 0, 130, CANVAS_W, &label_lg, "144x168");

    sys_slist_append(&widgets, &widget->node);
    return 0;
}

lv_obj_t *zmk_widget_screen_obj(struct zmk_widget_screen *widget) { return widget->obj; }
