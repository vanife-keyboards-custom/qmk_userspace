# Copyright 2026 Ivan Melnychuk (@vanife)
# SPDX-License-Identifier: GPL-2.0-or-later
# Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

# Wire this keymap to the Miryoku engine (users/manna_harbour_miryoku) without
# duplicating any layout code — same as the BM40/Corne `vanos` keymaps.
# No flash budget concerns: RP2040, 2 MB. The board's single WS2812 (RGBLIGHT)
# stays on; Miryoku's RGB keycodes on the Media layer drive it — but only
# because config.h re-shims them to UG_* (the userspace sends them to RGB Matrix).
USER_NAME = manna_harbour_miryoku
