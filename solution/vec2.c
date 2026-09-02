#include "vec2.h"

t_vec2  vec2_add(t_vec2 a, t_vec2 b)
{
    t_vec2  r;

    r.x = a.x + b.x;
    r.y = a.y + b.y;
    return (r);
}

t_vec2  vec2_sub(t_vec2 a, t_vec2 b)
{
    t_vec2  r;

    r.x = a.x - b.x;
    r.y = a.y - b.y;
    return (r);
}

t_vec2  vec2_scale(t_vec2 a, float s)
{
    t_vec2  r;

    r.x = a.x * s;
    r.y = a.y * s;
    return (r);
}

float   vec2_dot(t_vec2 a, t_vec2 b)
{
    return (a.x * b.x + a.y * b.y);
}
