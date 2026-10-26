/*
 * Status screen for the 128x32 OLED on the left half (the central).
 *
 *   +---------+-------------------------------------+
 *   |         | Base                        BT1  ok |  y  0
 *   | Mimikyu | WPM  62    bar graph of recent WPM  |  y 12
 *   |  23x30  | L [bat] 87         R [bat] 64       |  y 23
 *   +---------+-------------------------------------+
 *   x 0        x 26                                 x 127
 */

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>

#include <zmk/battery.h>
#include <zmk/ble.h>
#include <zmk/display.h>
#include <zmk/endpoints.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/keymap.h>
#include <zmk/split/central.h>
#include <zmk/usb.h>
#include <zmk/wpm.h>

#include "util.h"

/* The OLED is on the left half (central); the right half is the peripheral. */
#define OWN_LABEL "L"
#define PERIPHERAL_LABEL "R"

#define TEXT_X 26
#define WPM_HISTORY 16
#define WPM_GRAPH_MIN_SCALE 40

LV_IMG_DECLARE(mimikyu);

static struct {
    uint8_t battery;
    bool charging;
    uint8_t peripheral_battery; /* 0 = other half not connected */
    struct zmk_endpoint_instance endpoint;
    int profile;
    bool profile_connected;
    bool profile_bonded;
    uint8_t wpm[WPM_HISTORY];
    zmk_keymap_layer_index_t layer_index;
    const char *layer_label;
} st;

static lv_obj_t *canvas;
static lv_color_t cbuf[SCREEN_W * SCREEN_H];

static void draw_battery_block(lv_coord_t x, const char *label, uint8_t level, bool charging,
                               bool connected) {
    const lv_coord_t y = 23;
    char pct[4];

    draw_text(canvas, x, y, 8, &lv_font_unscii_8, LV_TEXT_ALIGN_LEFT, label);
    if (connected) {
        draw_battery(canvas, x + 8, y, level, charging);
        snprintf(pct, sizeof(pct), "%u", level);
        draw_text(canvas, x + 26, y, 24, &lv_font_unscii_8, LV_TEXT_ALIGN_RIGHT, pct);
    } else {
        draw_icon_cross(canvas, x + 10, y);
        draw_text(canvas, x + 26, y, 24, &lv_font_unscii_8, LV_TEXT_ALIGN_RIGHT, "weg");
    }
}

static void rotate_180(void) {
    /* One lv_color_t per pixel: reversing the buffer rotates the picture by 180 degrees. */
    for (int i = 0, j = SCREEN_W * SCREEN_H - 1; i < j; i++, j--) {
        lv_color_t tmp = cbuf[i];
        cbuf[i] = cbuf[j];
        cbuf[j] = tmp;
    }
    lv_obj_invalidate(canvas);
}

static void draw_screen(void) {
    char text[16];

    draw_rect(canvas, 0, 0, SCREEN_W, SCREEN_H, COLOR_BG);

    lv_draw_img_dsc_t img_dsc;
    lv_draw_img_dsc_init(&img_dsc);
    lv_canvas_draw_img(canvas, 0, 1, &mimikyu, &img_dsc);

    /* Row 1: layer, output */
    if (st.layer_label == NULL || st.layer_label[0] == '\0') {
        snprintf(text, sizeof(text), "Layer %u", st.layer_index);
        draw_text(canvas, TEXT_X, 0, 64, &lv_font_unscii_8, LV_TEXT_ALIGN_LEFT, text);
    } else {
        draw_text(canvas, TEXT_X, 0, 64, &lv_font_unscii_8, LV_TEXT_ALIGN_LEFT, st.layer_label);
    }

    if (st.endpoint.transport == ZMK_TRANSPORT_USB) {
        draw_text(canvas, 96, 0, 32, &lv_font_unscii_8, LV_TEXT_ALIGN_RIGHT, "USB");
    } else {
        snprintf(text, sizeof(text), "BT%d", st.profile + 1);
        draw_text(canvas, 90, 0, 28, &lv_font_unscii_8, LV_TEXT_ALIGN_RIGHT, text);
        if (!st.profile_bonded) {
            draw_icon_pairing(canvas, 121, 0); /* open profile, waiting for pairing */
        } else if (st.profile_connected) {
            draw_icon_ok(canvas, 121, 0);
        } else {
            draw_icon_cross(canvas, 121, 0);
        }
    }

    /* Row 2: WPM and history */
    draw_text(canvas, TEXT_X, 12, 24, &lv_font_unscii_8, LV_TEXT_ALIGN_LEFT, "WPM");
    snprintf(text, sizeof(text), "%u", st.wpm[WPM_HISTORY - 1]);
    draw_text(canvas, 52, 12, 24, &lv_font_unscii_8, LV_TEXT_ALIGN_RIGHT, text);

    int max = WPM_GRAPH_MIN_SCALE;
    for (int i = 0; i < WPM_HISTORY; i++) {
        if (st.wpm[i] > max) {
            max = st.wpm[i];
        }
    }
    for (int i = 0; i < WPM_HISTORY; i++) {
        int h = st.wpm[i] * 9 / max;
        /* a dot for "typing, but slow", so the graph shows activity */
        if (h == 0 && st.wpm[i] > 0) {
            h = 1;
        }
        if (h > 0) {
            draw_rect(canvas, 80 + i * 3, 21 - h, 2, h, COLOR_INK);
        }
    }

    /* Row 3: batteries */
    draw_battery_block(TEXT_X, OWN_LABEL, st.battery, st.charging, true);
    draw_battery_block(78, PERIPHERAL_LABEL, st.peripheral_battery, false,
                       st.peripheral_battery > 0);

    if (IS_ENABLED(CONFIG_CORNE42_OLED_ROTATE_180)) {
        rotate_180();
    }
}

/* Own battery */

struct battery_state {
    uint8_t level;
    bool usb_present;
};

static void battery_update_cb(struct battery_state state) {
    st.battery = state.level;
    st.charging = state.usb_present;
    draw_screen();
}

static struct battery_state battery_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);
    return (struct battery_state){
        .level = ev != NULL ? ev->state_of_charge : zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        .usb_present = zmk_usb_is_powered(),
#endif
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery, struct battery_state, battery_update_cb,
                            battery_get_state)
ZMK_SUBSCRIPTION(widget_battery, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_battery, zmk_usb_conn_state_changed);
#endif

/* Other half's battery. ZMK reports 0 when it disconnects. */

#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING)

struct peripheral_battery_state {
    uint8_t level;
};

static void peripheral_battery_update_cb(struct peripheral_battery_state state) {
    st.peripheral_battery = state.level;
    draw_screen();
}

static struct peripheral_battery_state peripheral_battery_get_state(const zmk_event_t *eh) {
    const struct zmk_peripheral_battery_state_changed *ev =
        as_zmk_peripheral_battery_state_changed(eh);
    uint8_t level = 0;

    if (ev != NULL) {
        level = ev->state_of_charge;
    } else {
        zmk_split_central_get_peripheral_battery_level(0, &level);
    }
    return (struct peripheral_battery_state){.level = level};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_peripheral_battery, struct peripheral_battery_state,
                            peripheral_battery_update_cb, peripheral_battery_get_state)
ZMK_SUBSCRIPTION(widget_peripheral_battery, zmk_peripheral_battery_state_changed);

#endif

/* Output: USB or BT profile */

struct output_state {
    struct zmk_endpoint_instance endpoint;
    int profile;
    bool connected;
    bool bonded;
};

static void output_update_cb(struct output_state state) {
    st.endpoint = state.endpoint;
    st.profile = state.profile;
    st.profile_connected = state.connected;
    st.profile_bonded = state.bonded;
    draw_screen();
}

static struct output_state output_get_state(const zmk_event_t *eh) {
    return (struct output_state){
        .endpoint = zmk_endpoints_selected(),
        .profile = zmk_ble_active_profile_index(),
        .connected = zmk_ble_active_profile_is_connected(),
        .bonded = !zmk_ble_active_profile_is_open(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_output, struct output_state, output_update_cb,
                            output_get_state)
ZMK_SUBSCRIPTION(widget_output, zmk_endpoint_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_output, zmk_usb_conn_state_changed);
#endif
#if IS_ENABLED(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(widget_output, zmk_ble_active_profile_changed);
#endif

/* WPM */

struct wpm_state {
    uint8_t wpm;
};

static void wpm_update_cb(struct wpm_state state) {
    memmove(st.wpm, st.wpm + 1, WPM_HISTORY - 1);
    st.wpm[WPM_HISTORY - 1] = state.wpm;
    draw_screen();
}

static struct wpm_state wpm_get_state(const zmk_event_t *eh) {
    return (struct wpm_state){.wpm = zmk_wpm_get_state()};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_wpm, struct wpm_state, wpm_update_cb, wpm_get_state)
ZMK_SUBSCRIPTION(widget_wpm, zmk_wpm_state_changed);

/* Layer */

struct layer_state {
    zmk_keymap_layer_index_t index;
    const char *label;
};

static void layer_update_cb(struct layer_state state) {
    st.layer_index = state.index;
    st.layer_label = state.label;
    draw_screen();
}

static struct layer_state layer_get_state(const zmk_event_t *eh) {
    zmk_keymap_layer_index_t index = zmk_keymap_highest_layer_active();
    return (struct layer_state){
        .index = index,
        .label = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index)),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer, struct layer_state, layer_update_cb, layer_get_state)
ZMK_SUBSCRIPTION(widget_layer, zmk_layer_state_changed);

/* Called by ZMK when CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM is set. */
lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    canvas = lv_canvas_create(screen);
    lv_obj_align(canvas, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_canvas_set_buffer(canvas, cbuf, SCREEN_W, SCREEN_H, LV_IMG_CF_TRUE_COLOR);

    widget_battery_init();
#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING)
    widget_peripheral_battery_init();
#endif
    widget_output_init();
    widget_wpm_init();
    widget_layer_init();

    return screen;
}
