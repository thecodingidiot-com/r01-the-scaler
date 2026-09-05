#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "camera.h"
#include "scaler.h"
#include "scene.h"
#include "render.h"

#define STRAFE_SPEED    0.3f
#define MOVE_SPEED      0.6f
#define MIN_SIDE        -10.0f
#define MAX_SIDE        10.0f

/*
** Steering used to call camera_turn() here -- true rotation, the
** raycaster's own model, not Hang-On/Out Run/Space Harrier's. None of
** the three cabinets this chapter itself names ever rotate the
** camera to steer; verified by actually playing a real Space Harrier
** ROM (in g03-the-getaway, this project's own capstone), not by
** re-reading this file. `cam->right` never changes once nothing here
** ever calls camera_turn() -- so this is a plain vector add, not a
** rotation, fenced by MIN_SIDE/MAX_SIDE so steering has an actual
** edge instead of an unbounded empty plane either side of the road.
**
** Forward/backward stays exactly as it was: MOVE_SPEED, player-held,
** either direction. That part was never a technique claim -- nothing
** here says this demonstrates a real cabinet's throttle -- and free
** forward/backward is what lets a reader park next to any billboard
** and watch it scale, not just drive past it once.
*/
static void handle_input(t_camera *cam, Uint8 const *keys)
{
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_H])
        cam->pos = vec2_add(cam->pos, vec2_scale(cam->right, -STRAFE_SPEED));
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_L])
        cam->pos = vec2_add(cam->pos, vec2_scale(cam->right, STRAFE_SPEED));
    if (cam->pos.y > MAX_SIDE)
        cam->pos.y = MAX_SIDE;
    if (cam->pos.y < MIN_SIDE)
        cam->pos.y = MIN_SIDE;
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_K])
        camera_move(cam, MOVE_SPEED);
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_J])
        camera_move(cam, -MOVE_SPEED);
}

int main(int argc, char **argv)
{
    SDL_Window      *win;
    SDL_Renderer    *ren;
    SDL_Event       ev;
    SDL_Texture     *sprites[SPRITE_COUNT];
    t_scene         scene;
    t_camera        cam;
    Uint8 const     *keys;
    int             running;

    if (argc < 2) {
        printf("usage: %s <scene_file>\n", argv[0]);
        return (1);
    }
    if (!scene_load(&scene, argv[1])) {
        printf("failed to load scene: %s\n", argv[1]);
        return (1);
    }
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return (1);
    }
    win = SDL_CreateWindow("r01", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, 0);
    ren = SDL_CreateRenderer(win, -1, 0);
    IMG_Init(IMG_INIT_PNG);
    sprites[0] = IMG_LoadTexture(ren, "assets/tree.png");
    sprites[1] = IMG_LoadTexture(ren, "assets/rock.png");
    if (!sprites[0] || !sprites[1]) {
        printf("failed to load a sprite: %s\n", IMG_GetError());
        printf("did you run 'bash gen_assets.sh' first?\n");
        return (1);
    }
    camera_init(&cam, 0.0f, 0.0f, 0.0f);
    running = 1;
    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT)
                running = 0;
            if (ev.type == SDL_KEYDOWN && (ev.key.keysym.sym == SDLK_ESCAPE
                    || ev.key.keysym.sym == SDLK_q))
                running = 0;
        }
        keys = SDL_GetKeyboardState(NULL);
        handle_input(&cam, keys);
        render_backdrop(ren);
        render_scene(&scene, &cam, ren, sprites);
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
    SDL_DestroyTexture(sprites[0]);
    SDL_DestroyTexture(sprites[1]);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
    return (0);
}
