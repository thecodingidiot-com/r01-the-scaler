# r01-the-scaler

Companion repository for **r01 — The Scaler on x86** at
[thecodingidiot.com](https://thecodingidiot.com) — the first chapter of
Part III, The Rendering Journey.

---

## Follow my journey

Working through r01 alongside the implementation pages? Build `scaler`
step by step, then run the tester.

Clone this repository:

```bash
git clone https://github.com/thecodingidiot-com/r01-the-scaler.git r01-practice
cd r01-practice/solution
bash gen_assets.sh
make re
bash ../test.sh
```

All tests must pass before the chapter is complete.

---

## Follow your journey

Building `scaler` independently? Here is the full project brief.

A sprite distance scaler in the style of Sega's mid-80s super-scaler
arcade boards (Hang-On, Out Run, Space Harrier): billboards that always
face the camera, scaled by `h = WINDOW_H / depth`. No true 3D geometry
— every billboard is a flat rectangle whose size is recomputed every
frame from one division.

- A camera with a world position, a facing angle, and the two axes
  derived from it (`forward`, `right`) — a 2D vector camera, not just
  an x-coordinate.
- A scene file (`width height` on the first line is not used here —
  instead: a billboard count, then one `x y sprite_id` line per
  billboard), loaded with `fopen`/`fgets`/`sscanf`.
- The scaler itself: project each billboard into camera space
  (`depth`, `side`), cull anything behind the near plane, then
  `h = WINDOW_H / depth` for size and the same divide for horizontal
  position.
- Painter's algorithm draw order — farthest billboard first — sorted
  with `qsort()`, the same tool `depthvis` used back in c06.

This is also the first chapter in the curriculum with **no `libtci` at
all**. Every console port ahead (Mega Drive, SNES, PlayStation,
Dreamcast) has its own C library, and `libtci` was never going to be
one of them — scene parsing uses plain `fopen`/`fgets`/`sscanf`, not
`tci_getline`.

Source is split by concern, one file per module:

| File | Contents |
| --- | --- |
| `main.c` | SDL2 init, the game loop (event → update → render), cleanup |
| `vec2.c` / `vec2.h` | a small 2D vector type: add, subtract, scale, dot |
| `camera.c` / `camera.h` | position, facing angle, and the derived `forward`/`right` axes — no SDL2 anywhere |
| `scaler.c` / `scaler.h` | the projection itself: world position → depth, side, screen size, screen position — no SDL2 anywhere |
| `scene.c` / `scene.h` | load a billboard list from a scene file — no SDL2 anywhere |
| `render.c` / `render.h` | the only file that calls actual SDL2 drawing functions |

`vec2.c`, `camera.c`, `scaler.c`, and `scene.c` never call an SDL2
function, so they link into a test binary with no SDL2 library at all.

Build and test your own version first. Use `solution/` to compare once
you are done, not before.

---

## Building the solution

```bash
cd solution
bash gen_assets.sh
make re
./scaler ../fixtures/scene1.txt
```

Controls: Left/Right arrows or `h`/`l` to turn, Up/Down arrows or
`k`/`j` to drive forward/backward, Escape or `q` to quit.

`gen_assets.sh` needs Python3 + Pillow:

```bash
sudo apt install python3-pil
```

---

## What the tester checks

**Build** — the real game compiles and links with zero warnings.

**A standalone projection tester** — `vec2.o`, `camera.o`, `scaler.o`,
and `scene.o` compiled and linked with no SDL2 at all, asserting real
numbers against `fixtures/scene1.txt`:

- A billboard straight ahead of the camera has `depth` equal to its
  distance and `side` equal to zero.
- Doubling the distance to a billboard exactly halves its projected
  size — `h = WINDOW_H / depth`, checked as a number, not eyeballed.
- A billboard closer than the near plane is marked not visible.
- Turning the camera 90 degrees swaps which world axis reads as
  "ahead"; moving the camera forward shortens depth to a fixed
  billboard by exactly the distance travelled.
- The fixture scene file parses into exactly six billboards.

**`scaler`** — runs its event loop for two seconds under a headless
(`SDL_VIDEODRIVER=dummy`) video driver without crashing. A smoke test,
not a visual check — actually driving down the road and watching
billboards grow as they approach is done by running it yourself.

---

## License

MIT License. See [LICENSE](LICENSE).
