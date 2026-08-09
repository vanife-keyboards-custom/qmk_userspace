# Copyright 2026 Ivan Melnychuk (@vanife)
# SPDX-License-Identifier: GPL-2.0-or-later
# Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

# Wire this keymap to the Miryoku engine (users/manna_harbour_miryoku) without
# duplicating any layout code. Layout options come from custom_rules.mk (?=);
# vanos tweaks come from vanos.h (via config.h).
USER_NAME = manna_harbour_miryoku

# ── RGB: left compiled IN, deliberately ───────────────────────────────────────
# crkbd/rev4_1 enables RGB Matrix in info.json:18 (23 LEDs per half), and the
# Media layer's RGB keys reach it. The LEDs are off at runtime, so this is NOT the
# cause of the "right half dies after a KVM switch" brown-out theory — dark WS2812s
# draw ~1 mA each, single-digit mA per half, not the hundreds a lit half would.
#
# Uncomment to compile it out entirely: saves flash and that idle current, and is
# the right move if RGB is never going to be used (the BM40 already does this in
# its own rules.mk:38). Cost: Miryoku's Media-layer RGB keys become no-ops, and
# the halves lose the only visible sign of life they have — with the case closed
# and no reachable reset button, lighting the LEDs is currently the only way to
# tell "slave MCU alive, link dead" from "slave MCU hung".
#RGB_MATRIX_ENABLE = no
