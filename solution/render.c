#include <stdlib.h>
#include "render.h"
#include "scaler.h"

typedef struct s_draw_item
{
    t_projection    proj;
    int             sprite_id;
}   t_draw_item;

/* Painter's algorithm: farthest first, so a near billboard overdraws
** a far one exactly where they overlap. Same qsort()-driven approach
** as depthvis back in c06 -- there it sorted sprites by a fixed depth
** field, here the depth is computed fresh from the camera every
** frame, but the draw order problem, and the fix, are the same one. */
static int  compare_draw_items(void const *a, void const *b)
{
    t_draw_item const  *ia = a;
    t_draw_item const  *ib = b;

    if (ib->proj.depth > ia->proj.depth)
        return (1);
    if (ib->proj.depth < ia->proj.depth)
        return (-1);
    return (0);
}

void    render_backdrop(SDL_Renderer *ren)
{
    SDL_Rect    sky;
    SDL_Rect    ground;

    sky.x = 0;
    sky.y = 0;
    sky.w = WINDOW_W;
    sky.h = HORIZON_Y;
    ground.x = 0;
    ground.y = HORIZON_Y;
    ground.w = WINDOW_W;
    ground.h = WINDOW_H - HORIZON_Y;
    SDL_SetRenderDrawColor(ren, 0x5c, 0x9d, 0xe8, 0xff);
    SDL_RenderFillRect(ren, &sky);
    SDL_SetRenderDrawColor(ren, 0x4a, 0x4a, 0x4a, 0xff);
    SDL_RenderFillRect(ren, &ground);
}

void    render_scene(t_scene const *scene, t_camera const *cam,
        SDL_Renderer *ren, SDL_Texture *sprites[SPRITE_COUNT])
{
    t_draw_item items[MAX_BILLBOARDS];
    int         visible;
    int         i;
    SDL_Rect    dst;

    visible = 0;
    i = 0;
    while (i < scene->count)
    {
        items[visible].proj = scaler_project(cam, scene->items[i].pos);
        items[visible].sprite_id = scene->items[i].sprite_id;
        if (items[visible].proj.visible)
            visible++;
        i++;
    }
    qsort(items, visible, sizeof(items[0]), compare_draw_items);
    i = 0;
    while (i < visible)
    {
        dst.x = items[i].proj.screen_x;
        dst.y = items[i].proj.screen_y;
        dst.w = items[i].proj.size;
        dst.h = items[i].proj.size;
        SDL_RenderCopy(ren, sprites[items[i].sprite_id], NULL, &dst);
        i++;
    }
}
