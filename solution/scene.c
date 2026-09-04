#include <stdio.h>
#include "scene.h"

/*
** No tci_getline here. This is the first chapter in the curriculum
** that drops libtci entirely -- from here on the console ports ahead
** (Mega Drive, SNES, PlayStation, Dreamcast) have their own C
** libraries, and libtci was never going anywhere but a Linux box.
** fgets() reads one line into a fixed buffer; sscanf() parses it.
** Both are ordinary libc, both exist on every target this curriculum
** will ever build for.
*/
int scene_load(t_scene *scene, char const *path)
{
    FILE    *fp;
    char    line[256];
    int     n;

    fp = fopen(path, "r");
    if (!fp)
        return (0);
    if (!fgets(line, sizeof(line), fp) || sscanf(line, "%d", &n) != 1) {
        fclose(fp);
        return (0);
    }
    if (n < 0 || n > MAX_BILLBOARDS) {
        fclose(fp);
        return (0);
    }
    scene->count = 0;
    while (scene->count < n) {
        if (!fgets(line, sizeof(line), fp)) {
            fclose(fp);
            return (0);
        }
        if (sscanf(line, "%f %f %d",
                &scene->items[scene->count].pos.x,
                &scene->items[scene->count].pos.y,
                &scene->items[scene->count].sprite_id) != 3) {
            fclose(fp);
            return (0);
        }
        scene->count++;
    }
    fclose(fp);
    return (1);
}
