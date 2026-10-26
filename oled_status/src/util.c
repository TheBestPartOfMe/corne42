#include "util.h"

void draw_rect(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h,
               lv_color_t color) {
    lv_draw_rect_dsc_t dsc;
    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_color = color;
    lv_canvas_draw_rect(canvas, x, y, w, h, &dsc);
}

void draw_text(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y, lv_coord_t w, const lv_font_t *font,
               lv_text_align_t align, const char *text) {
    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = COLOR_INK;
    dsc.font = font;
    dsc.align = align;
    lv_canvas_draw_text(canvas, x, y, w, &dsc, text);
}

/* Draws a bitmap of up to 8 px width; each row is one byte, MSB = leftmost pixel. */
static void draw_bitmap(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y, const uint8_t *rows,
                        int h, int w) {
    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            if (rows[r] & (0x80 >> c)) {
                lv_canvas_set_px_color(canvas, x + c, y + r, COLOR_INK);
            }
        }
    }
}

void draw_icon_ok(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y) {
    static const uint8_t rows[] = {0x02, 0x06, 0x0c, 0x98, 0xf0, 0x60, 0x00};
    draw_bitmap(canvas, x, y, rows, 7, 7);
}

void draw_icon_cross(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y) {
    static const uint8_t rows[] = {0x82, 0x44, 0x28, 0x10, 0x28, 0x44, 0x82};
    draw_bitmap(canvas, x, y, rows, 7, 7);
}

void draw_icon_pairing(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y) {
    /* three dots: "searching" */
    static const uint8_t rows[] = {0x00, 0x00, 0x00, 0x92, 0x00, 0x00, 0x00};
    draw_bitmap(canvas, x, y, rows, 7, 7);
}

void draw_battery(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y, uint8_t level, bool charging) {
    if (level > 100) {
        level = 100;
    }

    draw_rect(canvas, x, y, 15, 8, COLOR_INK);
    draw_rect(canvas, x + 1, y + 1, 13, 6, COLOR_BG);
    draw_rect(canvas, x + 15, y + 2, 2, 4, COLOR_INK);

    if (charging) {
        /* lightning bolt instead of the fill level */
        static const uint8_t bolt[] = {0x18, 0x30, 0x7c, 0x18, 0x30, 0x20};
        draw_bitmap(canvas, x + 4, y + 1, bolt, 6, 7);
    } else {
        draw_rect(canvas, x + 2, y + 2, (level * 11 + 50) / 100, 4, COLOR_INK);
    }
}
