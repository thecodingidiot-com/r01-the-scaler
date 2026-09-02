#!/bin/bash
# r01 — The Scaler / test.sh
#
# Builds the game, then checks the projection math deterministically --
# compiled and linked WITHOUT SDL2 at all (vec2.c, camera.c, scaler.c,
# and scene.c never call an SDL2 function), plus a headless smoke test
# of the real binary.
#
# Copy this file and fixtures/scene1.txt into your working directory,
# build with 'make re', then run:
#
#   bash test.sh

set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FIXTURES="${SCRIPT_DIR}/fixtures"

# ── colour ────────────────────────────────────────────────────────────────────

if [[ ! -t 1 ]]; then
    C_GREEN=""
    C_RED=""
    C_BOLD=""
    C_RESET=""
else
    C_GREEN="\033[0;32m"
    C_RED="\033[0;31m"
    C_BOLD="\033[1m"
    C_RESET="\033[0m"
fi

# ── state ─────────────────────────────────────────────────────────────────────

pass_count=0
fail_count=0
WORK_DIR=$(mktemp -d)

cleanup() {
    rm -rf "$WORK_DIR"
}
trap cleanup EXIT

# ── helpers ───────────────────────────────────────────────────────────────────

hr() {
    echo "────────────────────────────────────────────────────────────────"
}

banner() {
    hr
    echo "  r01 — The Scaler / test.sh"
    hr
}

pass() {
    local label="$1"
    printf "  ${C_GREEN}PASS${C_RESET}  %s\n" "$label"
    pass_count=$((pass_count + 1))
}

fail() {
    local label="$1"
    local detail="${2:-}"
    printf "  ${C_RED}FAIL${C_RESET}  %s\n" "$label"
    if [[ -n "$detail" ]]; then
        echo "        $detail"
    fi
    fail_count=$((fail_count + 1))
}

banner

# ── build the real game ───────────────────────────────────────────────────────

echo "Building..."
build_log=$(make re 2>&1)
build_status=$?
if [[ "$build_status" -ne 0 ]]; then
    fail "build succeeds" "make re failed:"
    echo "$build_log"
    exit 1
fi
pass "build succeeds"

if echo "$build_log" | grep -qi "warning"; then
    fail "build produces no warnings" "$(echo "$build_log" | grep -i warning)"
else
    pass "build produces no warnings"
fi

if [[ -x ./scaler ]]; then
    pass "scaler binary exists"
else
    fail "scaler binary exists"
fi

# ── build the SDL2-free projection tester ────────────────────────────────────

if [[ ! -f "${FIXTURES}/scene1.txt" ]]; then
    fail "fixtures/scene1.txt found" "keep the r01-the-scaler clone alongside your working directory"
    exit 1
fi
cp "${FIXTURES}/scene1.txt" "$WORK_DIR/scene1.txt"

cat > "$WORK_DIR/test_logic.c" <<'TESTC'
#include <math.h>
#include <stdio.h>
#include "vec2.h"
#include "camera.h"
#include "scaler.h"
#include "scene.h"

static int  g_pass = 0;
static int  g_fail = 0;

static void check_int(char const *label, int got, int want)
{
    if (got == want)
    {
        printf("PASS  %s (got %d)\n", label, got);
        g_pass++;
    }
    else
    {
        printf("FAIL  %s (got %d, want %d)\n", label, got, want);
        g_fail++;
    }
}

static void check_float_near(char const *label, float got, float want, float eps)
{
    if (fabsf(got - want) <= eps)
    {
        printf("PASS  %s (got %f)\n", label, got);
        g_pass++;
    }
    else
    {
        printf("FAIL  %s (got %f, want %f)\n", label, got, want);
        g_fail++;
    }
}

int main(void)
{
    t_camera        cam;
    t_projection    proj;
    t_scene         scene;

    /* facing along +x, a billboard straight ahead: depth is exactly the
     * x distance, side is exactly zero. */
    camera_init(&cam, 0.0f, 0.0f, 0.0f);
    proj = scaler_project(&cam, (t_vec2){10.0f, 0.0f});
    check_int("straight-ahead billboard is visible", proj.visible, 1);
    check_float_near("straight-ahead depth == distance", proj.depth, 10.0f, 0.001f);
    check_float_near("straight-ahead side == 0", proj.side, 0.0f, 0.001f);
    check_int("h = WINDOW_H / depth", proj.size, (int)(WINDOW_H / 10.0f));

    /* the same billboard twice as far away scales to half the size --
     * the whole point of the chapter, checked numerically. */
    camera_init(&cam, 0.0f, 0.0f, 0.0f);
    proj = scaler_project(&cam, (t_vec2){20.0f, 0.0f});
    check_int("double the distance halves the size", proj.size,
        (int)(WINDOW_H / 20.0f));

    /* a billboard behind the near plane is not visible, and nothing
     * about its position is trusted after that. */
    camera_init(&cam, 0.0f, 0.0f, 0.0f);
    proj = scaler_project(&cam, (t_vec2){1.0f, 0.0f});
    check_int("closer than the near plane is not visible", proj.visible, 0);

    /* turning 90 degrees swaps which axis is "ahead": a billboard that
     * was dead ahead is now directly to one side, at the same depth. */
    camera_init(&cam, 0.0f, 0.0f, 0.0f);
    camera_turn(&cam, (float)M_PI / 2.0f);
    proj = scaler_project(&cam, (t_vec2){0.0f, 10.0f});
    check_float_near("after a 90-degree turn, depth tracks the new forward axis",
        proj.depth, 10.0f, 0.01f);

    /* moving the camera forward shortens the depth to a fixed billboard
     * by exactly the distance travelled. */
    camera_init(&cam, 0.0f, 0.0f, 0.0f);
    camera_move(&cam, 6.0f);
    proj = scaler_project(&cam, (t_vec2){10.0f, 0.0f});
    check_float_near("moving forward shortens depth by the distance moved",
        proj.depth, 4.0f, 0.001f);

    /* the test fixture itself: six billboards, parsed straight off disk
     * with fgets()/sscanf(), no libtci anywhere in this file's call
     * chain (vec2.c/camera.c/scaler.c/scene.c link with no SDL2 and no
     * libtci -- see the Makefile's SRCS list for scaler itself). */
    if (!scene_load(&scene, "scene1.txt"))
    {
        printf("FAIL  scene_load\n");
        return (1);
    }
    check_int("scene1.txt loads six billboards", scene.count, 6);

    printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return (g_fail > 0);
}
TESTC

logic_build_log=$(gcc -Wall -Wextra -I . -c "$WORK_DIR/test_logic.c" -o "$WORK_DIR/test_logic.o" 2>&1 \
    && gcc "$WORK_DIR/test_logic.o" vec2.o camera.o scaler.o scene.o -lm -o "$WORK_DIR/test_logic" 2>&1)
logic_build_status=$?

if [[ "$logic_build_status" -ne 0 ]]; then
    fail "logic tester builds without SDL2" "$logic_build_log"
    exit 1
fi
pass "logic tester builds without SDL2 (vec2.o/camera.o/scaler.o/scene.o only)"

echo
echo "Running the logic tester..."
cd "$WORK_DIR"
logic_out=$(./test_logic)
logic_status=$?
cd - > /dev/null

echo "$logic_out" | grep "^PASS\|^FAIL" | while read -r line; do
    echo "  $line"
done

logic_pass_count=$(echo "$logic_out" | grep -c "^PASS")
logic_fail_count=$(echo "$logic_out" | grep -c "^FAIL")
pass_count=$((pass_count + logic_pass_count))
fail_count=$((fail_count + logic_fail_count))

if [[ "$logic_status" -ne 0 ]]; then
    fail "all logic assertions pass" "see failures above"
fi

# ── headless smoke test of the real binary ───────────────────────────────────

echo
echo "Running scaler headless (2s)..."
SDL_VIDEODRIVER=dummy timeout 2 ./scaler "$FIXTURES/scene1.txt"
scaler_status=$?
if [[ "$scaler_status" -eq 124 ]]; then
    pass "scaler runs its event loop for 2s without crashing"
else
    fail "scaler runs its event loop for 2s without crashing" "exit code: $scaler_status"
fi

# ── regression: a missing assets/ must fail loudly, not silently ────────────
#
# A real bug, found by an actual reader: without assets/ (gen_assets.sh
# never run), IMG_LoadTexture used to return NULL unchecked, SDL_RenderCopy
# silently no-op'd, and the binary ran forever showing only the flat
# sky/tarmac backdrop -- no billboards, no visible sign anything was wrong,
# indistinguishable from a genuinely broken camera. Fixed by checking both
# textures and exiting with a clear message. This must never regress to
# silent again.

if [[ -d assets ]]; then
    mv assets "$WORK_DIR/assets-backup"
fi
missing_assets_out=$(./scaler "$FIXTURES/scene1.txt" 2>&1)
missing_assets_status=$?
if [[ -d "$WORK_DIR/assets-backup" ]]; then
    mv "$WORK_DIR/assets-backup" assets
fi

if [[ "$missing_assets_status" -eq 1 ]] && echo "$missing_assets_out" | grep -qi "gen_assets.sh"; then
    pass "missing assets/ fails loudly with a clear message (not silently)"
else
    fail "missing assets/ fails loudly with a clear message (not silently)" \
        "exit code: $missing_assets_status, output: $missing_assets_out"
fi

# ── summary ───────────────────────────────────────────────────────────────────

echo
hr
printf "  ${C_BOLD}%d passed, %d failed${C_RESET}\n" "$pass_count" "$fail_count"
hr

if [[ "$fail_count" -gt 0 ]]; then
    exit 1
fi
exit 0
