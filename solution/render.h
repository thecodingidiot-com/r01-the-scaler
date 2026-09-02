#ifndef RENDER_H
# define RENDER_H

# include <SDL2/SDL.h>
# include "scene.h"
# include "camera.h"

# define SPRITE_COUNT 2

void    render_backdrop(SDL_Renderer *ren);
void    render_scene(t_scene const *scene, t_camera const *cam,
            SDL_Renderer *ren, SDL_Texture *sprites[SPRITE_COUNT]);

#endif
