// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

#pragma once
// vanos = miryoku:default engine + vanos tweaks.
// Encoder push = a custom keycode on every layer; vanos_encoder.h gives it
// meaning (layer-aware tap, long press = pomodoro). Must precede the adapter,
// which only defaults it (to KC_MPLY) when unset. QK_USER_0 = VANOS_ENC_PUSH.
#define CHEAPINO_ENCODER_PUSH QK_USER_0
#include "../manna_harbour_miryoku/config.h"   // reuse this board's LAYOUT_miryoku adapter
#include "vanos.h"                          // shared vanos customizations (timing tune)

// LED colour order. The vendored board config sets WS2812_BYTE_ORDER_RGB (tompi's
// RP2040-Zero), but the LED on our AliExpress clones is the standard GRB: with
// RGB, red and green came out swapped (SYM's green showed red, FUN's orange
// showed green; blue and yellow, symmetric in R/G, looked right). With GRB both
// units show identical, correct colours (confirmed 2026-10-30). GRB is QMK's
// own default.
#undef WS2812_BYTE_ORDER
#define WS2812_BYTE_ORDER WS2812_BYTE_ORDER_GRB

// MEDIA layer RGB keys → RGBLIGHT. The shared custom_config.h shims Miryoku's
// RGB_* names to RM_* (RGB Matrix) for the BM40/Corne; the Cheapino has only
// RGBLIGHT (its one WS2812), so on this board RM_* do nothing. This config.h is
// read after the userspace one, so these win.
#undef RGB_TOG
#undef RGB_MOD
#undef RGB_HUI
#undef RGB_SAI
#undef RGB_VAI
#define RGB_TOG UG_TOGG
#define RGB_MOD UG_NEXT
#define RGB_HUI UG_HUEU
#define RGB_SAI UG_SATU
#define RGB_VAI UG_VALU

// VIA defaults to 4 dynamic layers; layers beyond it read as KC_NO. Miryoku needs all
// of them (stock Miryoku's 10; no FLAT layer on the Cheapino). RP2040: 4 KB emulated EEPROM; 10 layers × 8×12 matrix × 2 B = 1920 B (measured: keymaps = 0x780).
#define DYNAMIC_KEYMAP_LAYER_COUNT 10

// ── Typing-state indicators on the status LED (users/…/vanos_status.h) ───────
// Comment a line out to drop that indicator. Colours: VANOS_IND_*_COL in
// vanos_encoder.h (COL_WHITE by default; patterns tell the states apart).
#define VANOS_IND_ONESHOT   // one-shot Shift pending: white, fast blink
#define VANOS_IND_CAPS_WORD // Caps Word on: white, double blink
#define VANOS_IND_MEDIA     // Media held: live colour blinks until an RGB key is pressed
#define VANOS_IND_MODS      // Gui/Alt/Ctrl held: dim white (add Shift via VANOS_IND_MODS_MASK)
