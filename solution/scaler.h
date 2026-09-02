#ifndef SCALER_H
# define SCALER_H

# include "vec2.h"
# include "camera.h"

# define WINDOW_W       800
# define WINDOW_H       480
# define HORIZON_Y      340
# define NEAR_PLANE     2.0f

typedef struct s_projection
{
    float   depth;
    float   side;
    int     visible;
    int     size;
    int     screen_x;
    int     screen_y;
}   t_projection;

t_projection    scaler_project(t_camera const *cam, t_vec2 world_pos);

#endif
