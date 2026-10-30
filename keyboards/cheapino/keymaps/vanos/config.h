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
