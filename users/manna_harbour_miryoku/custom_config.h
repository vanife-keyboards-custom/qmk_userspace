// Copyright 2019 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

// ═══════════════════════════════════════════════════════════════════════════
//  LAYOUT OVERRIDES  →  moved to custom_rules.mk (2026-07)
//
//  MIRYOKU_ALPHAS_QWERTY and MIRYOKU_NAV_INVERTEDT used to be #defined here.
//  They are now make variables in custom_rules.mk:
//      MIRYOKU_ALPHAS ?= QWERTY
//      MIRYOKU_NAV    ?= INVERTEDT
//
//  Why: Miryoku resolves each option with a first-match #elif chain in
//  ALPHABETICAL order (miryoku_babel/miryoku_layer_selection.h). A #define here
//  is unconditional and userspace-wide, so a per-keymap MIRYOKU_ALPHAS=... would
//  define a SECOND symbol rather than replace this one, and whichever sits
//  earlier in the chain would win silently. QWERTY is 9th of 10, so e.g. QWERTZ
//  would have lost to it with no diagnostic. Make variables are last-write-wins.
//
//  Set layout options in custom_rules.mk (userspace) or a keymap's rules.mk
//  (per layout/board) — not here.
// ═══════════════════════════════════════════════════════════════════════════

// ── non-layout customizations go below (timings, thumb HOKP fix, etc.) ──


// ═══════════════════════════════════════════════════════════════════════════
//  RGB MATRIX  — behaviour of the Media layer's RGB keys.
//
//  Miryoku's Media layer carries RGB_TOG / RGB_MOD / RGB_HUI / RGB_SAI / RGB_VAI
//  (miryoku_babel/miryoku_layer_alternatives.h:316-340). Those names are shimmed
//  to the RM_* keycodes further down this file. The step sizes below decide how
//  far one press of those keys moves hue/saturation/value.
//
//  The two boards differ in which RGB backends they compile:
//    crkbd/rev4_1   info.json     "rgblight": false, "rgb_matrix": true
//    bm40hsrgb/rev2 keyboard.json "rgblight": true,  "rgb_matrix": true  (both)
//  So RGBLIGHT_* defines are dead on the Corne but live on the BM40. The shims
//  further down route Miryoku's RGB keys to RM_* (RGB Matrix) on both boards, so
//  the BM40's underglow is not driven from the Media layer. Set RGBLIGHT_* only
//  in the BM40's own keymap config.h, never here.
//
//  The step values below were carried over from the stock Corne keymap's inert
//  #ifdef RGBLIGHT_ENABLE block (that orphaned config.h deleted 2026-07).
//
//  Shared settings belong here; per-board settings (brightness caps, which
//  animations get compiled in) belong in keyboards/<kb>/keymaps/<km>/config.h.
// ═══════════════════════════════════════════════════════════════════════════
#define RGB_MATRIX_HUE_STEP 10
#define RGB_MATRIX_SAT_STEP 17
#define RGB_MATRIX_VAL_STEP 17
#define RGB_MATRIX_SPD_STEP 10

// Brightness cap — PER BOARD, do not set it here.
//   crkbd/rev4_1 already emits RGB_MATRIX_MAXIMUM_BRIGHTNESS 50 from its
//   keyboard.json (bus-powered split; 120 would be optimistic), so redefining it
//   needs an #undef first or the build warns. Set it in the board's keymap
//   config.h if a board actually needs a different cap:
//     #undef  RGB_MATRIX_MAXIMUM_BRIGHTNESS
//     #define RGB_MATRIX_MAXIMUM_BRIGHTNESS 120

// Animations — PER BOARD, deliberately not enabled here.
//   Each animation costs flash. The BM40 is an atmega32u4 with a real budget,
//   the Corne v4 (RP2040) has room to spare, so this is not a shared decision.
//   Enable in the board's keymap config.h, or via rgb_matrix.animations in
//   keyboard.json. The stock Corne keymap's old RGBLIGHT list mapped to:
//     #define ENABLE_RGB_MATRIX_BREATHING
//     #define ENABLE_RGB_MATRIX_CYCLE_ALL              // was RAINBOW_MOOD
//     #define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT       // was RAINBOW_SWIRL
//     #define ENABLE_RGB_MATRIX_BAND_SAT               // was SNAKE / KNIGHT
//     #define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN       // was STATIC_GRADIENT
//     #define ENABLE_RGB_MATRIX_ALTERNATING
//     #define ENABLE_RGB_MATRIX_PIXEL_RAIN             // was TWINKLE
//   (RGBLIGHT effects have no 1:1 RGB Matrix equivalents — these are the
//    nearest matches, not a faithful port. CHRISTMAS and RGB_TEST have none.)


// ═══════════════════════════════════════════════════════════════════════════
//  BUILD COMPATIBILITY  — NOT layout choices. Plumbing so this vendored (older)
//  Miryoku compiles against our newer qmk_firmware. Remove if Miryoku is re-vendored.
// ═══════════════════════════════════════════════════════════════════════════
// Mouse-keycode rename (QMK 2024): KC_MS_*/KC_BTN*/KC_WH_* → MS_*
#define KC_MS_U MS_UP
#define KC_MS_D MS_DOWN
#define KC_MS_L MS_LEFT
#define KC_MS_R MS_RGHT
#define KC_BTN1 MS_BTN1
#define KC_BTN2 MS_BTN2
#define KC_BTN3 MS_BTN3
#define KC_WH_U MS_WHLU
#define KC_WH_D MS_WHLD
#define KC_WH_L MS_WHLL
#define KC_WH_R MS_WHLR
// RGB: old RGBLIGHT keycodes → RGB Matrix (the BM40/Corne use RGB_MATRIX)
#define RGB_TOG RM_TOGG
#define RGB_MOD RM_NEXT
#define RGB_HUI RM_HUEU
#define RGB_SAI RM_SATU
#define RGB_VAI RM_VALU

