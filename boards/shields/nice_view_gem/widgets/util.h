#pragma once

#include <lvgl.h>
#include <zmk/endpoints.h>

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168

#define CANVAS_SIZE 68
#define CANVAS_COLOR_FORMAT LV_COLOR_FORMAT_L8
#define CANVAS_BUF_SIZE \
    LV_CANVAS_BUF_SIZE(CANVAS_SIZE, CANVAS_SIZE, LV_COLOR_FORMAT_GET_BPP(CANVAS_COLOR_FORMAT), \
                       LV_DRAW_BUF_STRIDE_ALIGN)

#define LVGL_BACKGROUND \
    IS_ENABLED(CONFIG_NICE_VIEW_WIDGET_INVERTED) ? lv_color_black() : lv_color_white()
#define LVGL_FOREGROUND \
    IS_ENABLED(CONFIG_NICE_VIEW_WIDGET_INVERTED) ? lv_color_white() : lv_color_black()

void fill_background(lv_obj_t *canvas);
void init_label_dsc(lv_draw_label_dsc_t *label_dsc, lv_color_t color, const lv_font_t *font,
                    lv_text_align_t align);
void init_rect_dsc(lv_draw_rect_dsc_t *rect_dsc, lv_color_t bg_color);
void canvas_draw_rect(lv_obj_t *canvas, int32_t x, int32_t y, int32_t w, int32_t h,
                      lv_draw_rect_dsc_t *rect_dsc);
void canvas_draw_text(lv_obj_t *canvas, int32_t x, int32_t y, int32_t max_w,
                      lv_draw_label_dsc_t *label_dsc, const char *text);
