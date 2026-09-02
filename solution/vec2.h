#ifndef VEC2_H
# define VEC2_H

typedef struct s_vec2
{
    float   x;
    float   y;
}   t_vec2;

t_vec2  vec2_add(t_vec2 a, t_vec2 b);
t_vec2  vec2_sub(t_vec2 a, t_vec2 b);
t_vec2  vec2_scale(t_vec2 a, float s);
float   vec2_dot(t_vec2 a, t_vec2 b);

#endif
