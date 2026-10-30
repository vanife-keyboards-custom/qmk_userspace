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
// showed green; blue and yellow, symmetric in R/G, looked right). Observed on the
// unit flashed 2026-10-30 — check the other unit too, a different clone batch
// could differ. GRB is QMK's own default.
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
