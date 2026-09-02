#include <math.h>
#include "camera.h"

static void camera_rebuild_axes(t_camera *cam)
{
    cam->forward.x = cosf(cam->angle);
    cam->forward.y = sinf(cam->angle);
    cam->right.x = cam->forward.y;
    cam->right.y = -cam->forward.x;
}

void    camera_init(t_camera *cam, float x, float y, float angle)
{
    cam->pos.x = x;
    cam->pos.y = y;
    cam->angle = angle;
    camera_rebuild_axes(cam);
}

void    camera_turn(t_camera *cam, float delta_angle)
{
    cam->angle += delta_angle;
    camera_rebuild_axes(cam);
}

void    camera_move(t_camera *cam, float delta_forward)
{
    cam->pos = vec2_add(cam->pos, vec2_scale(cam->forward, delta_forward));
}
