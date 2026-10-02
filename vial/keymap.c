/*
 * Vial port of config/corne.keymap (ZMK) for the corne42 with Pro Micro RP2040s.
 *
 * Bindings assume a German (QWERTZ) host layout, same as the ZMK keymap:
 * RALT() = AltGr, S() = Shift.
 */

#include QMK_KEYBOARD_H

enum layers { _BASE, _LOWER, _RAISE, _NUM };

// ^ and ` are dead keys on the German host layout, so producing the bare
// character means tapping the dead key then Space. Exposed to Vial as
// custom keycodes (see customKeycodes in vial.json).
enum custom_keycodes {
    DE_CIRC_S = QK_KB_0,
    DE_GRV_S,
};

// Home row mods (ZMK HMR_L / HMR_R / hmr_l_layer)
#define HM_C    LSFT_T(KC_C)
#define HM_T    LCTL_T(KC_T)
#define HM_O    LT(_NUM, KC_O)
#define HM_S    LCTL_T(KC_S)
#define HM_G    LSFT_T(KC_G)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_ESC,    KC_J,    KC_D,    KC_U,    KC_A,    KC_X,                         KC_P,    KC_H,    KC_L,    KC_M,    KC_W,  KC_DEL,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
       KC_TAB,    HM_C,    HM_T,    KC_I,    KC_E,    HM_O,                         KC_B,    KC_N,    KC_R,    HM_S,    HM_G,    KC_Q,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,    KC_F,    KC_V, KC_LBRC, KC_QUOT, KC_SCLN,                         KC_Z,    KC_Y, KC_COMM,  KC_DOT,    KC_K, KC_MINS,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, MO(_RAISE), KC_SPC,   KC_ENT, KC_BSPC, MO(_LOWER)
                                      //`--------------------------'  `--------------------------'
  ),

  [_LOWER] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      S(KC_0),    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                      _______, KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, _______,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, XXXXXXX, XXXXXXX, _______, _______, _______,                      _______, _______, _______, _______, _______, _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

  // Neo2 "Ebene 3" symbols
  //  …  _  [  ]  ^      !  <  >  =  &
  //  \  /  {  }  *      ?  (  )  -  :  @
  //  #  $  |  ~  `      +  %  "  '  ;
  [_RAISE] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      S(KC_0), RALT(KC_DOT), S(KC_SLSH), RALT(KC_8), RALT(KC_9), DE_CIRC_S,        S(KC_1), KC_NUBS, S(KC_NUBS), S(KC_0), S(KC_6), XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, RALT(KC_MINS), S(KC_7), RALT(KC_7), RALT(KC_0), S(KC_RBRC),      S(KC_MINS), S(KC_8), S(KC_9), KC_SLSH, S(KC_DOT), RALT(KC_Q),
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT, KC_NUHS, S(KC_4), RALT(KC_NUBS), RALT(KC_RBRC), DE_GRV_S,         KC_RBRC, S(KC_5), S(KC_2), S(KC_NUHS), S(KC_COMM), XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, XXXXXXX
                                      //`--------------------------'  `--------------------------'
  ),

  // Numpad; + = RBRC, - = SLSH, * = S(RBRC), / = S(7), = = S(0) on German layout
  [_NUM] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      _______, _______, _______, _______, _______, _______,                      _______,    KC_7,    KC_8,    KC_9, KC_RBRC, KC_SLSH,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                         KC_0,    KC_4,    KC_5,    KC_6, S(KC_RBRC), S(KC_7),
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                      _______,    KC_1,    KC_2,    KC_3, S(KC_0), _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, _______, _______,    _______, _______, _______
                                      //`--------------------------'  `--------------------------'
  )
};

// Chordal Hold: a home row mod only becomes a hold when the next key is on
// the opposite hand (ZMK hold-trigger-key-positions). Thumbs count for both.
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM =
    LAYOUT_split_3x6_3(
        'L', 'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R', 'R',
        'L', 'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R', 'R',
        'L', 'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R', 'R',
                       '*', '*', '*',  '*', '*', '*'
    );

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return true;
    }
    switch (keycode) {
        case DE_CIRC_S:
            tap_code(KC_GRV);
            tap_code(KC_SPC);
            return false;
        case DE_GRV_S:
            tap_code16(S(KC_EQL));
            tap_code(KC_SPC);
            return false;
    }
    return true;
}

#ifdef OLED_ENABLE
// Left (master) OLED in portrait (32x128): a rocket flying through a starfield
// that speeds up with WPM, layer + WPM below. The right side keeps the
// keyboard-level logo.

#define SCENE_H 86
#define FRAME_MS 40
#define STAR_COUNT 14

// '#' = lit, '.' = hull interior (cleared so stars don't shine through)
static const char rocket[][17] PROGMEM = {
    "       ##       ",
    "      ####      ",
    "     ##..##     ",
    "     #....#     ",
    "    ##....##    ",
    "    #......#    ",
    "    #..##..#    ",
    "    #.#..#.#    ",
    "    #.#..#.#    ",
    "    #..##..#    ",
    "    #......#    ",
    "    #......#    ",
    "    #.####.#    ",
    "   ##......##   ",
    "  #.#......#.#  ",
    " #..#......#..# ",
    " #..#......#..# ",
    " #.##.####.##.# ",
    " ###  #..#  ### ",
    "      ####      ",
};
#define ROCKET_H (sizeof(rocket) / sizeof(rocket[0]))

typedef struct {
    uint8_t  x;
    uint16_t y; // 1/16 px
    uint8_t  depth;
} star_t;

static star_t   stars[STAR_COUNT];
static uint32_t rng_state = 0x2545F491;
static uint32_t frame_timer;
static uint16_t frame;

static uint8_t rnd(uint8_t n) {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 17;
    rng_state ^= rng_state << 5;
    return rng_state % n;
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_180;
    }
    for (uint8_t i = 0; i < STAR_COUNT; i++) {
        stars[i] = (star_t){rnd(32), rnd(SCENE_H) * 16, 1 + i % 3};
    }
    return OLED_ROTATION_270;
}

static void render_scene(uint8_t wpm) {
    // Stars: three parallax depths, streaks once they get fast
    uint8_t speed = 2 + wpm / 6;
    for (uint8_t i = 0; i < STAR_COUNT; i++) {
        star_t *s = &stars[i];
        s->y += speed * s->depth;
        if (s->y >= SCENE_H * 16) {
            s->y -= SCENE_H * 16;
            s->x = rnd(32);
        }
        uint8_t y   = s->y / 16;
        uint8_t len = MIN(6, speed * s->depth / 24);
        for (uint8_t k = 0; k <= len && k <= y; k++) {
            oled_write_pixel(s->x, y - k, true);
        }
    }

    // Rocket: bobs when idle, shakes when you're flying
    uint8_t x0 = 8;
    uint8_t y0 = 36;
    if (wpm < 10) {
        y0 += (frame / 6) % 2;
    } else if (wpm > 80) {
        x0 += rnd(3) - 1;
    }
    for (uint8_t r = 0; r < ROCKET_H; r++) {
        for (uint8_t c = 0; c < 16; c++) {
            char px = pgm_read_byte(&rocket[r][c]);
            if (px != ' ') {
                oled_write_pixel(x0 + c, y0 + r, px == '#');
            }
        }
    }

    // Flame grows with WPM and flickers
    if (wpm > 0) {
        uint8_t len = 2 + MIN(wpm, 120) / 10 + rnd(3);
        for (uint8_t k = 0; k < len; k++) {
            uint8_t half = k < len * 2 / 3 ? 2 : 1;
            for (uint8_t x = x0 + 8 - half; x < x0 + 8 + half; x++) {
                if (rnd(len + 1) >= k / 2) {
                    oled_write_pixel(x, y0 + ROCKET_H + k, true);
                }
            }
        }
    }

    for (uint8_t x = 0; x < 32; x += 2) {
        oled_write_pixel(x, SCENE_H + 2, true);
    }
}

static void render_status(uint8_t wpm) {
    static const char names[][6] PROGMEM = {"BASE ", "LOWER", "RAISE", " NUM "};
    bool caps = host_keyboard_led_state().caps_lock;

    oled_set_cursor(0, 12);
    oled_write_P(names[MIN(get_highest_layer(layer_state), _NUM)], caps);
    oled_set_cursor(0, 14);
    oled_write_P(PSTR(" WPM "), false);
    oled_set_cursor(0, 15);
    oled_write_P(PSTR(" "), false);
    oled_write(get_u8_str(wpm, ' '), false);
    oled_write_P(PSTR(" "), false);
}

bool oled_task_user(void) {
    if (!is_keyboard_master()) {
        return true; // keyboard-level logo
    }
    if (timer_elapsed32(frame_timer) < FRAME_MS) {
        return false;
    }
    frame_timer = timer_read32();
    frame++;

    uint8_t wpm = get_current_wpm();
    oled_clear();
    render_scene(wpm);
    render_status(wpm);
    return false;
}
#endif
