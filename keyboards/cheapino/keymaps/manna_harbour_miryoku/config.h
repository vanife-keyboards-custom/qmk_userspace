// Copyright 2019 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

// Cheapino v2 — the board is not in upstream QMK; its definition lives in
// vendor/keyboards/cheapino and is symlinked into qmk_firmware by
// `mise run build:cheapino*` (see docs/devices/cheapino/qmk/README.md).
//
// Its LAYOUT_split_3x5_3 has 37 positions: the 36 keys plus the encoder push,
// sixth in the top row. Miryoku has no slot for it, so it is a macro: the
// keymap gets a keycode, and any LAYOUT_miryoku table that is NOT a keymap
// (vanos_chordal_hold.h's handedness map) redefines it to '*' around its
// include.
#ifndef CHEAPINO_ENCODER_PUSH
#    define CHEAPINO_ENCODER_PUSH KC_MPLY
#endif

#define LAYOUT_miryoku(\
K00, K01, K02, K03, K04,      K05, K06, K07, K08, K09,\
K10, K11, K12, K13, K14,      K15, K16, K17, K18, K19,\
K20, K21, K22, K23, K24,      K25, K26, K27, K28, K29,\
N30, N31, K32, K33, K34,      K35, K36, K37, N38, N39\
)\
LAYOUT_split_3x5_3(\
K00, K01, K02, K03, K04, CHEAPINO_ENCODER_PUSH, K05, K06, K07, K08, K09,\
K10, K11, K12, K13, K14,                        K15, K16, K17, K18, K19,\
K20, K21, K22, K23, K24,                        K25, K26, K27, K28, K29,\
               K32, K33, K34,                   K35, K36, K37\
)
