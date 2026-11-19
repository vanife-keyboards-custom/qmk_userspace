// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

#pragma once
// Same as vanos. The adapter it includes switches to the rotated mapping because rules.mk
// sets MIRYOKU_MAPPING = ROT180_EXTENDED_THUMBS.
#include "../vanos/config.h"

// Bootmagic Lite: QMK checks matrix [0,0] at power-up (quantum/bootmagic/bootmagic.h
// default). On the rotated board that key is the bottom-right corner, which this layout
// leaves unused. Move the check to matrix [3,11]: the old bottom-right key, now the
// physical TOP-LEFT key (Q). That matches the board readme's "hold the top-left key while
// plugging in" and Boot† (hold a left thumb + double-tap Q).
#define BOOTMAGIC_ROW    3
#define BOOTMAGIC_COLUMN 11

// Underglow (RGBLIGHT, 6 LEDs) — only used if RGBLIGHT_ENABLE is switched back on.
// Reversing the index order makes chained animations (rainbow swirl, knight) run the same
// way as on the normal board. This assumes the strip is laid out symmetrically under the
// board (LED i opposite LED 5-i) [open — check against the PCB if RGB is ever used].
#define RGBLIGHT_LED_MAP { 5, 4, 3, 2, 1, 0 }

// ── Layer indicator: per-key LEDs on, every animation compiled out ──────────
// rules.mk switches RGB_MATRIX back on just for the indicator in keymap.c. The board's
// keyboard.json enables 38 animations (~4.8 KB); none is needed, so all of them are
// #undef'd here. A keymap config.h is the only place such an #undef takes effect (it is
// read after the generated info_config.h). What remains is QMK's always-present solid
// colour mode, which the indicator paints over every frame.
// List generated from keyboard.json (same method as scripts/bm40-size-matrix.sh):
#undef ENABLE_RGB_MATRIX_ALPHAS_MODS
#undef ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
#undef ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#undef ENABLE_RGB_MATRIX_BREATHING
#undef ENABLE_RGB_MATRIX_BAND_SAT
#undef ENABLE_RGB_MATRIX_BAND_VAL
#undef ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
#undef ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
#undef ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#undef ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
#undef ENABLE_RGB_MATRIX_CYCLE_ALL
#undef ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#undef ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#undef ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#undef ENABLE_RGB_MATRIX_CYCLE_OUT_IN
#undef ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
#undef ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
#undef ENABLE_RGB_MATRIX_CYCLE_SPIRAL
#undef ENABLE_RGB_MATRIX_DUAL_BEACON
#undef ENABLE_RGB_MATRIX_RAINBOW_BEACON
#undef ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
#undef ENABLE_RGB_MATRIX_RAINDROPS
#undef ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
#undef ENABLE_RGB_MATRIX_HUE_BREATHING
#undef ENABLE_RGB_MATRIX_HUE_PENDULUM
#undef ENABLE_RGB_MATRIX_HUE_WAVE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
#undef ENABLE_RGB_MATRIX_SPLASH
#undef ENABLE_RGB_MATRIX_MULTISPLASH
#undef ENABLE_RGB_MATRIX_SOLID_SPLASH
#undef ENABLE_RGB_MATRIX_SOLID_MULTISPLASH

// Indicator brightness at power-up (0-150; the board caps at 150). The Media layer's
// RGB_VAI raises it for the session (Shift+RGB_VAI lowers it). QMK does write that change to
// EEPROM, but keymap.c forces this value again at every power-up, so a replug resets it.
#define VANOS_IND_VAL 16 // the minimum (VANOS_IND_MIN_VAL in keymap.c); RGB_VAI steps up by 16

// Also light the key that is HOLDING the active layer (the layer thumb, or Z / for Button)
// in the indicator colour — made for transparent thumb caps. +200 B. Comment out to keep
// only the indicator LED(s) from VANOS_IND_LEDS in keymap.c.
#define VANOS_IND_HOLD_KEY

// While a layer is LOCKED, double-blink (every 2 s) the key to double-tap to get back to
// Base, in WHITE at the indicator brightness (VANOS_IND_VAL, follows RGB_VAI). Comment out
// to drop it.
#define VANOS_IND_EXIT_HINT
