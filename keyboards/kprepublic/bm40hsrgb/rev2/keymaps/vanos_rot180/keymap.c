// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos_rot180: the vanos keymap, unchanged. The CHORDAL_HOLD map, thumb hold and
// permissive hold all go through LAYOUT_miryoku or work from keycodes, so they follow the
// rotated mapping on their own. Doc: kboards docs/devices/bm40-rot180.md.
#include "../vanos/keymap.c"

#ifdef RGB_MATRIX_ENABLE

// ═════════════════════════════════════════════════════════════════════════════
//  LAYER INDICATOR on the per-key LEDs (2026-11-19)
//
//  Which LEDs. Positions are the board AS IT SITS turned 180° (USB toward you), counted
//  row 1 = top (Q W E R T), row 4 = thumb row (nearest you), col 1 = left. LED numbers are
//  the RGB-matrix indices = order of keyboard.json's layout = matrix row*12 + col (row 3
//  skips the 2u's second half: [3,5]=41, [3,7]=42). Rotation: physical (r,c) -> matrix
//  (3-r, 11-c), 0-based. Every position below carries NO key in the layout, so the
//  switch above it can be pulled (hot-swap) and the LED shines bare through the PCB.
//
//    centre 2x2  (rows 2-3, cols 6-7) ..... row 2: 30 29    row 3: 18 17
//    2u "space bar" (row 1, cols 6-7) ..... 41   (one LED, centred under the 2u switch) <- DEFAULT
//    thumb row, outer left  (cols 1 2 3) .. 11 10 9
//    thumb row, outer right (cols 10 11 12)  2  1 0
//
//  Options, by editing VANOS_IND_LEDS (no other change needed, cost +0-2 B per LED):
//    { 30, 29, 18, 17 }                   centre square
//    { 41, 30, 29, 18, 17 }               centre square + the 2u position above it
//    { 41 }                               the 2u position only (default)
//    { 11, 10, 9, 2, 1, 0 }               thumb-row corners (under your hands: easy to cover)
//    { 41, 30, 29, 18, 17, 11, 10, 9, 2, 1, 0 }   every unused position
//  For reference: on the board in its NORMAL orientation the same LEDs are the 2u space
//  bar (41) and the centre square at rows 2-3, cols 6-7 again (the square is symmetric).
//
//  What it shows — the Cheapino's status-LED palette (keyboards/cheapino/keymaps/vanos/
//  vanos_encoder.h), so a colour means the same layer on both boards:
//    power-up, first 1.5 s ... one rainbow sweep = "firmware started"
//    base layer .............. dark
//    layer held .............. NAV cyan · MOUSE yellow · NUM blue · SYM green ·
//                              FUN magenta · BUTTON white
//    MEDIA held .............. the live RGB colour, so its RGB_HUI/SAI/VAI keys are visible
//    layer locked ............ that layer's colour, slow blink (600 ms); and the key to
//      (via its TD key)        double-tap for Base double-blinks white every 2 s (VANOS_IND_EXIT_HINT)
//  Everything else on the board stays dark: the indicator blanks all 47 LEDs every frame.
//
//  Notes:
//  - The solid-colour mode, hue, sat and VANOS_IND_VAL are forced at power-up WITHOUT
//    writing EEPROM, because the RGB area of EEPROM was never initialised by the vanos
//    builds (RGB was compiled out). RGB_TOG on Media turns the indicator off/on; it does
//    write EEPROM, but the next power-up forces it on again.
//  - Indicators only run while the matrix is enabled and the host is awake (USB suspend
//    turns everything off — QMK's own behaviour).
// ═════════════════════════════════════════════════════════════════════════════

#    ifndef VANOS_IND_LEDS
#        define VANOS_IND_LEDS { 41 }
#    endif
#    ifndef VANOS_IND_VAL
#        define VANOS_IND_VAL 16
#    endif
#    define VANOS_IND_MIN_VAL 16   // never so dim that a held layer looks like "base"
#    define VANOS_IND_BOOT_MS 1500 // power-up rainbow sweep
#    define VANOS_IND_BLINK_MS 600 // locked-base blink half-period (same as the Cheapino)
#    define VANOS_IND_HINT_MS 2000 // exit-hint double-blink repeats this often

static const uint8_t vanos_ind_leds[] = VANOS_IND_LEDS;

// Full-channel colours (R=4, G=2, B=1), as on the Cheapino: unambiguous even when dim.
static uint8_t vanos_layer_col(uint8_t layer) {
    switch (layer) {
        case U_NAV:   return 2 | 1; // cyan
        case U_MOUSE: return 4 | 2; // yellow
        case U_NUM:   return 1;     // blue
        case U_SYM:   return 2;     // green
        case U_FUN:   return 4 | 1; // magenta
        default:      return 4 | 2 | 1; // white: BUTTON, TAP, EXTRA
    }
}

void keyboard_post_init_user(void) {
    // Turn the LED coordinates 180° (x -> 224-x, y -> 64-y; keyboard.json spans exactly
    // 0..224 x 0..64) so directional animations would run the right way if any were ever
    // compiled back in. LED <-> key (matrix_co) is unchanged: each LED is still under
    // the same switch. Done on QMK's weak, generated g_led_config in RAM, so the 47-LED
    // table is not copied here and cannot drift from the board definition.
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        g_led_config.point[i].x = 224 - g_led_config.point[i].x;
        g_led_config.point[i].y = 64 - g_led_config.point[i].y;
    }
    // Known state regardless of what EEPROM holds (see Notes above).
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(rgb_matrix_get_hue(), rgb_matrix_get_sat(), VANOS_IND_VAL);
}

bool rgb_matrix_indicators_user(void) {
    uint32_t now = timer_read32();
    uint8_t  val = rgb_matrix_get_val();
    if (val < VANOS_IND_MIN_VAL) val = VANOS_IND_MIN_VAL;

    rgb_t   c   = {0};
    uint8_t col = 0;
    uint8_t top = get_highest_layer(layer_state);

    // Typing states come from users/…/vanos_status.h and are compiled in only when
    // config.h switches them on (VANOS_IND_ONESHOT / _CAPS_WORD / _MEDIA / _MODS);
    // all are off on the BM40 today, so the branches below fold away.
    if (now < VANOS_IND_BOOT_MS) {
        c = hsv_to_rgb((hsv_t){(uint8_t)(now * 255 / VANOS_IND_BOOT_MS), 255, val});
    } else if (vanos_ind_oneshot()) {
        col = vanos_ind_fast_blink(now) ? (4 | 2 | 1) : 0; // white, fast blink
    } else if (vanos_ind_caps_word()) {
        col = vanos_ind_double_blink(now) ? (4 | 2 | 1) : 0; // white, double blink
    } else if (layer_state && top == U_MEDIA) {
        // Live colour, so the RGB keys' effect shows; blinks until one is used
        // when VANOS_IND_MEDIA is on (otherwise steady, as before).
        if (!vanos_ind_media_held() || !vanos_ind_media_blinking() || vanos_ind_media_blink(now)) {
            c = hsv_to_rgb((hsv_t){rgb_matrix_get_hue(), rgb_matrix_get_sat(), val});
        }
    } else if (layer_state && top != U_BASE) {
        col = vanos_layer_col(top);
    } else if (vanos_ind_mods()) {
        uint8_t dim = val / 3 < VANOS_IND_MIN_VAL ? VANOS_IND_MIN_VAL : val / 3;
        c           = (rgb_t){dim, dim, dim}; // dim white
    } else if (get_highest_layer(default_layer_state) != U_BASE && (now / VANOS_IND_BLINK_MS) % 2 == 0) {
        col = vanos_layer_col(get_highest_layer(default_layer_state));
    }
    if (col) {
        c.r = (col & 4) ? val : 0;
        c.g = (col & 2) ? val : 0;
        c.b = (col & 1) ? val : 0;
    }

    rgb_matrix_set_color_all(0, 0, 0);
    for (uint8_t i = 0; i < sizeof(vanos_ind_leds); i++) {
        rgb_matrix_set_color(vanos_ind_leds[i], c.r, c.g, c.b);
    }

    // The key HOLDING the layer lights up in the same colour (transparent thumb caps).
    // Not a table of thumb positions: any held key whose base-layer keycode is a layer-tap
    // to the active layer (LT(top, …)) is lit. So it follows the mapping by itself, and
    // also covers BUTTON, which Miryoku puts on the Z and / keys. A thumb that is only
    // tapped never activates its layer, so it stays dark. On this board the thumbs are
    // Esc=LED 8 (Media), Spc=7 (Nav), Tab=6 (Mouse), Ent=5 (Sym), Bsp=4 (Num), Del=3 (Fun).
    // Flag: VANOS_IND_HOLD_KEY in config.h (on by default; +200 B).
#    ifdef VANOS_IND_HOLD_KEY
    if (layer_state && top != U_BASE) {
        uint8_t base = get_highest_layer(default_layer_state);
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            for (uint8_t colm = 0; colm < MATRIX_COLS; colm++) {
                uint8_t led = g_led_config.matrix_co[row][colm];
                if (led == NO_LED || !matrix_is_on(row, colm)) continue;
                uint16_t kc = keymap_key_to_keycode(base, (keypos_t){.row = row, .col = colm});
                if (IS_QK_LAYER_TAP(kc) && QK_LAYER_TAP_GET_LAYER(kc) == top) {
                    rgb_matrix_set_color(led, c.r, c.g, c.b);
                }
            }
        }
    }
#    endif // VANOS_IND_HOLD_KEY

    // EXIT HINT: while a layer is LOCKED (it replaced Base via its TD key), the key to
    // double-tap to get back to Base blinks WHITE (indicator brightness) as a double-tap
    // pattern: on-off-on, then dark, repeating every VANOS_IND_HINT_MS (2 s).
    // Which key: the one whose keycode is Miryoku's TD(U_TD_U_BASE), searched in the
    // locked layer itself (Nav/Mouse/Media -> R, Num/Sym/Fun -> U, on the hand that holds
    // that layer). Tap and Extra have no Base key of their own (you hold a left thumb
    // first), so the hint falls back to where Base sits on Nav (R). Searched rather than a
    // table, so it follows the mapping. TD(U_TD_U_BASE) = QK_TAP_DANCE | (1 + U_BASE):
    // Miryoku's enum is private to manna_harbour_miryoku.c and is U_TD_BOOT followed by
    // one entry per layer in MIRYOKU_LAYER_LIST order.
    // Flag: VANOS_IND_EXIT_HINT in config.h.
#    ifdef VANOS_IND_EXIT_HINT
    uint8_t  locked = get_highest_layer(default_layer_state);
    uint16_t ph     = now % VANOS_IND_HINT_MS;
    if (locked != U_BASE && now >= VANOS_IND_BOOT_MS && (ph < 150 || (ph >= 300 && ph < 450))) {
        uint8_t hv = val; // white, same brightness as the indicator: follows RGB_VAI
        uint8_t layers[2] = {locked, U_NAV};
        for (uint8_t l = 0; l < 2; l++) {
            bool found = false;
            for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
                for (uint8_t colm = 0; colm < MATRIX_COLS; colm++) {
                    uint8_t led = g_led_config.matrix_co[row][colm];
                    if (led == NO_LED) continue;
                    if (keymap_key_to_keycode(layers[l], (keypos_t){.row = row, .col = colm}) == (QK_TAP_DANCE | (1 + U_BASE))) {
                        rgb_matrix_set_color(led, hv, hv, hv);
                        found = true;
                    }
                }
            }
            if (found) break;
        }
    }
#    endif // VANOS_IND_EXIT_HINT
    return false;
}

#endif // RGB_MATRIX_ENABLE
