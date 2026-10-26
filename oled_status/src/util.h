#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

#define SCREEN_W 128
#define SCREEN_H 32

/*
 * The corne OLED runs with `inversion-on` (see ZMK's corne.dtsi), so in LVGL a black
 * pixel is a lit pixel. We draw "ink" in black on a white background.
 */
#define COLOR_BG lv_color_white()
#define COLOR_INK lv_color_black()

void draw_text(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y, lv_coord_t w, const lv_font_t *font,
               lv_text_align_t align, const char *text);
void draw_rect(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h,
               lv_color_t color);

/* Battery icon, 17x8 px with its top-left corner at (x, y). Shows a bolt while charging. */
void draw_battery(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y, uint8_t level, bool charging);

/* 7x7 status icons. */
void draw_icon_ok(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y);
void draw_icon_cross(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y);
void draw_icon_pairing(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y);
