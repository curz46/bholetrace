#ifndef KEYS__H
#define KEYS__H

#include <GL/glut.h>

// not sure why, but can't find a good definiton of this. 700 should be sufficient.
#define GLUT_KEY_LAST 700 

static int keys[GLUT_KEY_LAST];

void key_toggle(int keycode, int val, int tick)
{
    if (val)
        keys[keycode] = tick+1;
    else
        keys[keycode] = 0;
}

int key_pressed(int keycode)
{
    if (keycode < 0 || keycode > GLUT_KEY_LAST)
        return 0;
    return keys[keycode] != 0;
}

int key_pressed_tick(int keycode, int tick)
{
    if (keycode < 0 || keycode > GLUT_KEY_LAST)
        return 0;
    return keys[keycode] == tick+1;
}

#endif // KEYS__H
