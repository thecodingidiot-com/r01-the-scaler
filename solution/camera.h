#ifndef CAMERA_H
# define CAMERA_H

# include "vec2.h"

typedef struct s_camera
{
    t_vec2  pos;
    float   angle;
    t_vec2  forward;
    t_vec2  right;
}   t_camera;

void    camera_init(t_camera *cam, float x, float y, float angle);
void    camera_turn(t_camera *cam, float delta_angle);
void    camera_move(t_camera *cam, float delta_forward);

#endif
