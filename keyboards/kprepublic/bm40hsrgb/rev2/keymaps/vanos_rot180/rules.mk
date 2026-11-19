# Copyright 2026 Ivan Melnychuk (@vanife)
# SPDX-License-Identifier: GPL-2.0-or-later
# Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

# vanos_rot180 = the vanos keymap for a BM40 turned 180° (USB at the front edge), so the
# thumbs move one column toward the centre (Miryoku EXTENDED_THUMBS).
# Doc: kboards docs/devices/bm40-rot180.md.
#
# Everything comes from ../vanos (USER_NAME, the flash budget, RGB off). The only
# difference is the physical mapping, which is picked here and implemented in the board
# adapter ../manna_harbour_miryoku/config.h. This is a separate keymap rather than a build
# flag so it gets its own hex file and cannot be flashed by mistake as the normal
# orientation (or the other way round).
include $(KEYMAP_PATH)/../vanos/rules.mk

MIRYOKU_MAPPING = ROT180_EXTENDED_THUMBS

# ── Layer indicator on the per-key LEDs (2026-11-19) ─────────────────────────
# ../vanos/rules.mk switches both lighting stacks off for the flash budget. This keymap turns
# the per-key stack (RGB_MATRIX, IS31FL3733 driver) back on for a small layer indicator,
# with ALL 38 animations compiled out in config.h: the driver costs ~3.3 KB, the animations
# would cost another ~4.8 KB (docs/QMK-FIRMWARE-SIZE.md). Underglow (RGBLIGHT) stays off:
# its 6 LEDs sit under the PCB and only light the desk.
RGB_MATRIX_ENABLE = yes
