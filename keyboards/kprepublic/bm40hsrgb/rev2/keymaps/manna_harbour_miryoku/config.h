// Copyright 2019 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

// MIRYOKU_MAPPING picks the physical mapping (Miryoku's "subset mappings"). It is set per
// keymap in rules.mk and turned into -DMIRYOKU_MAPPING_<X> by users/.../post_rules.mk.

#if defined MIRYOKU_MAPPING_ROT180_EXTENDED_THUMBS
// vanife addition (2026-11-18), used by keymaps/vanos_rot180. The board is turned 180°
// (USB at the front edge). The old top row (12 x 1u) becomes the thumb row, and the old 2u
// space bar ends up unused in the middle of the new top row. That frees columns 6-7 of the
// thumb row, so upstream's ortho_4x12 EXTENDED_THUMBS grid fits: thumbs one column further
// in (4-6 / 7-9 instead of 3-5 / 8-10).
// Rotating by 180° maps (r, c) -> (3-r, 11-c). LAYOUT_planck_mit lists its 47 keys
// row-major and the 2u row is symmetric (5 | 2u | 5), so the result is upstream's
// EXTENDED_THUMBS argument list read backwards. Doc: kboards docs/devices/bm40-rot180.md.
#define LAYOUT_miryoku(\
K00,   K01,   K02,   K03,   K04,                 K05,   K06,   K07,   K08,   K09,\
K10,   K11,   K12,   K13,   K14,                 K15,   K16,   K17,   K18,   K19,\
K20,   K21,   K22,   K23,   K24,                 K25,   K26,   K27,   K28,   K29,\
N30,   N31,   K32,   K33,   K34,                 K35,   K36,   K37,   N38,   N39\
)\
LAYOUT_planck_mit(\
KC_NO, KC_NO, KC_NO, K37,   K36,   K35,   K34,   K33,   K32,   KC_NO, KC_NO, KC_NO,\
K29,   K28,   K27,   K26,   K25,   KC_NO, KC_NO, K24,   K23,   K22,   K21,   K20,\
K19,   K18,   K17,   K16,   K15,   KC_NO, KC_NO, K14,   K13,   K12,   K11,   K10,\
K09,   K08,   K07,   K06,   K05,      KC_NO,     K04,   K03,   K02,   K01,   K00\
)

#else // default: upstream planck_mit mapping, board in its normal orientation

#define LAYOUT_miryoku(\
K00,   K01,   K02,   K03,   K04,                 K05,   K06,   K07,   K08,   K09,\
K10,   K11,   K12,   K13,   K14,                 K15,   K16,   K17,   K18,   K19,\
K20,   K21,   K22,   K23,   K24,                 K25,   K26,   K27,   K28,   K29,\
N30,   N31,   K32,   K33,   K34,                 K35,   K36,   K37,   N38,   N39\
)\
LAYOUT_planck_mit(\
K00,   K01,   K02,   K03,   K04,   KC_NO, KC_NO, K05,   K06,   K07,   K08,   K09,\
K10,   K11,   K12,   K13,   K14,   KC_NO, KC_NO, K15,   K16,   K17,   K18,   K19,\
K20,   K21,   K22,   K23,   K24,   KC_NO, KC_NO, K25,   K26,   K27,   K28,   K29,\
KC_NO, KC_NO, K32,   K33,   K34,      KC_NO,     K35,   K36,   K37,   KC_NO, KC_NO\
)

#endif
