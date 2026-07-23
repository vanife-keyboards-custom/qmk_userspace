# Copyright 2019 Manna Harbour
# https://github.com/manna-harbour/miryoku
#
# Modified for the vanife unified-layout userspace (2026-07): userspace-wide
# Miryoku defaults moved here from custom_config.h, plus the full option catalogue.

# ═════════════════════════════════════════════════════════════════════════════
#  MIRYOKU OPTIONS — userspace-level defaults
# ═════════════════════════════════════════════════════════════════════════════
#
#  HOW THIS WORKS
#    post_rules.mk turns each MIRYOKU_<OPT> make variable into a -DMIRYOKU_<OPT>_<VALUE>
#    compiler define. Setting them as make variables (rather than #define in
#    custom_config.h) is what makes them overridable per keymap and per build.
#
#  THE THREE LEVELS, weakest to strongest
#    1. userspace  — this file, using `?=`. Applies to every board and every keymap.
#    2. keymap     — keyboards/<kb>/keymaps/<km>/rules.mk, using `=`.
#                    Read at build_keyboard.mk:146, i.e. BEFORE this file (line 429),
#                    which is exactly why the defaults below must use `?=` and not `=`.
#    3. build      — make <kb>:<km> MIRYOKU_ALPHAS=DVORAK
#                    qmk compile -kb <kb> -km <km> -e MIRYOKU_ALPHAS=DVORAK
#
#  DO NOT set these as #define in custom_config.h. Miryoku resolves each option
#  with a first-match #elif chain in ALPHABETICAL order (miryoku_babel/
#  miryoku_layer_selection.h), so a hardcoded #define does not lose to a make
#  variable — both symbols end up defined and whichever sits earlier in the chain
#  wins silently. Make variables are last-write-wins and have no such hazard.
#
# ═════════════════════════════════════════════════════════════════════════════


# ── ACTIVE: the vanife unified layout ────────────────────────────────────────
# The only deliberate deviations from stock Miryoku. Applies to every board built
# from this userspace, which is what keeps BM40 and Corne in sync.
# Recovered from the BM40 — see docs/devices/bm40/recovered-layout.md.

MIRYOKU_ALPHAS ?= QWERTY        # stock default: COLEMAKDH
MIRYOKU_NAV    ?= INVERTEDT     # stock default: vi-style row


# ═════════════════════════════════════════════════════════════════════════════
#  FULL OPTION CATALOGUE — uncomment to change. Values are exhaustive.
#  These eight are the complete set consumed by post_rules.mk; nothing else
#  is read. Leaving an option unset selects Miryoku's stock default.
# ═════════════════════════════════════════════════════════════════════════════

# ── Alpha block ──────────────────────────────────────────────────────────────
# Base layer alphas. Default: COLEMAKDH
# AZERTY | BEAKL15 | COLEMAK | COLEMAKDH | COLEMAKDHK | DVORAK | HALMAK
# QWERTY | QWERTZ | WORKMAN
#MIRYOKU_ALPHAS ?= COLEMAKDH

# Extra layer — a second alpha layer, reachable from Media. Same 10 values.
#MIRYOKU_EXTRA ?= QWERTY

# Tap layer — alphas with no home-row mods (gaming / fast bursts). Same 10 values.
MIRYOKU_TAP ?= QWERTY

# ── Navigation block ─────────────────────────────────────────────────────────
# Arrow-cluster arrangement. Applies consistently to the Nav, Mouse AND Media
# layers — all three key off this one option.
#   VI        — h/j/k/l row (Miryoku stock)
#   INVERTEDT — inverted-T arrow cluster
#MIRYOKU_NAV ?= VI

# ── Clipboard block ──────────────────────────────────────────────────────────
# Which cut/copy/paste/undo/redo keycodes the Nav layer emits.
#   FUN — KC_AGIN/KC_PSTE/KC_COPY/KC_CUT/KC_UNDO (dedicated keys)
#   MAC — Cmd-based
#   WIN — Ctrl-based
#   unset — Miryoku's portable default (KC_AGIN, S(KC_INS), C(KC_INS), …)
#MIRYOKU_CLIPBOARD ?= WIN

# ── Handedness ───────────────────────────────────────────────────────────────
# FLIP mirrors the layer assignments left<->right across the thumb cluster.
# Only value: FLIP. Leave unset for stock handedness.
#MIRYOKU_LAYERS ?= FLIP

# ── Physical mapping ─────────────────────────────────────────────────────────
# How Miryoku's canonical 10x3+4 grid lands on the board's real matrix.
# BOARD-DEPENDENT: a value only works if that board's LAYOUT_miryoku adapter
# implements it (see keyboards/<kb>/keymaps/<km>/config.h). Setting an
# unimplemented value silently falls through to the adapter's #else branch.
#   EXTENDED_THUMBS | LITE | NOREVERSEANGLE | PEAK_ERGODOX | ROTATE
#   SHIFTED_ROWS | SHIFTED_ROWS_EXTENDED_THUMBS
#   SHIFTED_ROWS_EXTENDED_THUMBS_PINKIE_STAGGER | SPLIT
# Prefer setting this per keymap, not here — it is a property of the board.
#MIRYOKU_MAPPING ?= EXTENDED_THUMBS

# ── Kludges ──────────────────────────────────────────────────────────────────
# Thumb combos: recovers keys lost on boards with fewer than 3 thumb keys per
# hand by comboing adjacent thumbs. Forces COMBO_ENABLE = yes.
# Only value: yes
#MIRYOKU_KLUDGE_THUMBCOMBOS ?= yes


# ═════════════════════════════════════════════════════════════════════════════
#  PER-KEYMAP VARIANTS
#  To ship a second Miryoku flavour under its own name, create a keymap that
#  borrows this engine — no code duplication:
#
#    keyboards/crkbd/rev4_1/standard/keymaps/vanife_colemak/
#      keymap.c   # license header only, zero code (makes the dir discoverable)
#      config.h   # copy the LAYOUT_miryoku adapter from the sibling keymap
#      rules.mk   # USER_NAME = manna_harbour_miryoku
#                 # MIRYOKU_ALPHAS = COLEMAKDH      <- plain '=', overrides the ?= above
#                 # MIRYOKU_NAV    = VI
#
#  Then: qmk userspace-add -kb crkbd/rev4_1/standard -km vanife_colemak
#
#  USER_NAME is what wires the keymap to this userspace; without it the build
#  looks for users/<keymap name>, finds nothing, and silently omits Miryoku.
# ═════════════════════════════════════════════════════════════════════════════
