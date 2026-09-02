#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "camera.h"
#include "scaler.h"
#include "scene.h"
#include "render.h"

#define TURN_SPEED  0.035f
#define MOVE_SPEED  1.5f

static void handle_input(t_camera *cam, Uint8 const *keys)
{
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_H])
        camera_turn(cam, -TURN_SPEED);
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_L])
        camera_turn(cam, TURN_SPEED);
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

    if (argc < 2)
    {
        printf("usage: %s <scene_file>\n", argv[0]);
        return (1);
    }
    if (!scene_load(&scene, argv[1]))
    {
        printf("failed to load scene: %s\n", argv[1]);
        return (1);
    }
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return (1);
    }
    win = SDL_CreateWindow("r01", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, 0);
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    IMG_Init(IMG_INIT_PNG);
    sprites[0] = IMG_LoadTexture(ren, "assets/tree.png");
    sprites[1] = IMG_LoadTexture(ren, "assets/rock.png");
    camera_init(&cam, 0.0f, 0.0f, 0.0f);
    running = 1;
    while (running)
    {
        while (SDL_PollEvent(&ev))
        {
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
