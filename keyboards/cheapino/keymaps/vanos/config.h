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
