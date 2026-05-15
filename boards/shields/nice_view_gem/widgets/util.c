#include <zephyr/kernel.h>
#include "util.h"

void fill_background(lv_obj_t *canvas) { lv_canvas_fill_bg(canvas, LVGL_BACKGROUND, LV_OPA_COVER); }

void init_label_dsc(lv_draw_label_dsc_t *label_dsc, lv_color_t color, const lv_font_t *font,
                    lv_text_align_t align) {
    lv_draw_label_dsc_init(label_dsc);
    label_dsc->color = color;
    label_dsc->font = font;
    label_dsc->align = align;
}

void init_rect_dsc(lv_draw_rect_dsc_t *rect_dsc, lv_color_t bg_color) {
    lv_draw_rect_dsc_init(rect_dsc);
    rect_dsc->bg_color = bg_color;
}

void canvas_draw_rect(lv_obj_t *canvas, int32_t x, int32_t y, int32_t w, int32_t h,
                      lv_draw_rect_dsc_t *rect_dsc) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_area_t coords = {x, y, x + w - 1, y + h - 1};
    lv_draw_rect(&layer, rect_dsc, &coords);
    lv_canvas_finish_layer(canvas, &layer);
}

void canvas_draw_text(lv_obj_t *canvas, int32_t x, int32_t y, int32_t max_w,
                      lv_draw_label_dsc_t *label_dsc, const char *text) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    label_dsc->text = text;
    int32_t right = x + max_w - 1;
    if (right > CANVAS_W - 1)
        right = CANVAS_W - 1;
    int32_t bottom = y + label_dsc->font->line_height - 1;
    if (bottom > CANVAS_H - 1)
        bottom = CANVAS_H - 1;
    lv_area_t coords = {x, y, right, bottom};
    lv_draw_label(&layer, label_dsc, &coords);
    lv_canvas_finish_layer(canvas, &layer);
}

void canvas_draw_img(lv_obj_t *canvas, int32_t x, int32_t y, const lv_image_dsc_t *src,
                     lv_draw_image_dsc_t *img_dsc) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    img_dsc->src = src;
    lv_area_t coords = {x, y, x + src->header.w - 1, y + src->header.h - 1};
    lv_draw_image(&layer, img_dsc, &coords);
    lv_canvas_finish_layer(canvas, &layer);
}
