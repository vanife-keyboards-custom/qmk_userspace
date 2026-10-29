// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later

// Encoder rotation for the Cheapino — include once, from a keymap.c.
//
// The board code (vendor/keyboards/cheapino/cheapino.c, fix_encoder_action) reads
// the encoder's A/B contacts through the matrix and guesses which order means
// "clockwise". On our units the guess is reversed: unit #1 lowered the volume when
// turned clockwise, and unit #2 scrolled up on stock Vial, whose map is
// [WH_U, WH_D] = [CCW, CW]. QMK's ENCODER_DIRECTION_FLIP cannot fix it, because the
// board bypasses QMK's encoder driver. So we flip it here and leave the vendored
// board code untouched.
//
// Behaviour = QMK's default encoder_update_kb (volume, EXTRAKEY_ENABLE) with the
// direction corrected. Returning false skips that default.

#pragma once

bool encoder_update_user(uint8_t index, bool clockwise) {
    clockwise = !clockwise;
    tap_code_delay(clockwise ? KC_VOLU : KC_VOLD, 10);
    return false;
}
