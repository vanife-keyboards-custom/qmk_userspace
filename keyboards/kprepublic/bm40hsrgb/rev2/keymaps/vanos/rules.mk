# Copyright 2026 Ivan Melnychuk (@vanife)
# SPDX-License-Identifier: GPL-2.0-or-later
# Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

# Wire this keymap to the Miryoku engine (users/manna_harbour_miryoku) without
# duplicating any layout code. Layout options come from custom_rules.mk (?=);
# vanos tweaks come from vanos.h (via config.h).
USER_NAME = manna_harbour_miryoku

# ═════════════════════════════════════════════════════════════════════════════
#  FLASH BUDGET — RGB off (2026-07-28)
#
#  The BM40 is an atmega32u4 with 28672 B usable (atmel-dfu). The `vanos` build
#  sat at 27894 B = 97%, 778 B free — too tight to land CHORDAL_HOLD's handedness
#  map, the thumb HOKP callback, combos, or anything else.
#
#  Measured cost of the RGB stacks (docs/devices/bm40/size-matrix.md, 45 builds):
#    RGB_MATRIX (per-key driver + animations)   7690 B   ← animations alone 4836 B
#    RGBLIGHT   (6-LED underglow stack)         1622 B
#    both together (LTO overlap −516 B)         9828 B  →  18066 B = 63%, 10606 B free
#
#  Turning the driver off takes its animations with it, so there is no separate
#  animation switch to set — the matrix line below covers both.
#
#  This is deliberately the blunt version: ALL lighting off, on the safe side.
#  Re-enabling selectively later (layer indicator, combo/Caps-Word status) means
#  RGB_MATRIX_ENABLE = yes + a hand-picked animation list in config.h — budget
#  ~2900 B for the driver with animations trimmed, per the size matrix.
#
#  Note: Miryoku's Media layer keeps its RGB keycodes (RM_TOGG/RM_NEXT/...); QMK
#  defines those keycodes unconditionally, so they still compile — they just do
#  nothing on this board now. Volume/media transport on that layer is unaffected.
#
#  Board default (keyboards/kprepublic/bm40hsrgb/rev2/keyboard.json) enables both
#  stacks; a keymap rules.mk overrides it — the mechanism the size matrix used for
#  all 45 measurement builds.
# ═════════════════════════════════════════════════════════════════════════════
RGB_MATRIX_ENABLE = no
RGBLIGHT_ENABLE   = no
