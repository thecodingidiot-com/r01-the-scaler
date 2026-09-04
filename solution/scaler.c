#include "scaler.h"

/*
** The scaler. Given the camera and a billboard's world position:
**   1. project the billboard into camera space (depth = how far
**      ahead, side = how far left/right of centre)
**   2. h = WINDOW_H / depth  -- the whole technique in one line
**   3. reuse the same divide for the horizontal position, so a
**      billboard drifts toward screen centre as it gets close, the
**      same way its height grows
** Anything behind the near plane is marked not visible and every
** other field is left unset -- the caller must check `visible` first.
*/
t_projection    scaler_project(t_camera const *cam, t_vec2 world_pos)
{
    t_projection    proj;
    t_vec2          rel;

    rel = vec2_sub(world_pos, cam->pos);
    proj.depth = vec2_dot(rel, cam->forward);
    proj.side = vec2_dot(rel, cam->right);
    if (proj.depth < NEAR_PLANE) {
        proj.visible = 0;
        return (proj);
    }
    proj.visible = 1;
    proj.size = (int)((float)WINDOW_H / proj.depth);
    proj.screen_x = WINDOW_W / 2
        + (int)((float)WINDOW_H * proj.side / proj.depth) - proj.size / 2;
    proj.screen_y = HORIZON_Y - proj.size;
    return (proj);
}
