#ifndef SCENE_H
# define SCENE_H

# include "vec2.h"

# define MAX_BILLBOARDS 64

typedef struct s_billboard
{
    t_vec2  pos;
    int     sprite_id;
}   t_billboard;

typedef struct s_scene
{
    t_billboard items[MAX_BILLBOARDS];
    int         count;
}   t_scene;

int     scene_load(t_scene *scene, char const *path);

#endif
