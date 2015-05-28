/* JoAnn Tuazon       *
 * CMPS 335           *
 * Advanced-Asteroids *
 * Spring 2015        */

#ifndef JOANNT_H
#define JOANNT_H

#include "structures.h"
#include <FMOD/wincompat.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "fmod.h"
#include "ppm.h"

Ppmimage *bgImg = NULL;
GLuint bgTxtr;
extern Display *dpy;
extern Window win;
extern XEvent *e;

void init_sounds(void);
void play_music(int a);
void load_background(void);
void draw_background(void);
void set_mouse_position(int x, int y);
void show_mouse_cursor(const int onoff);
//void check_mouse(XEvent *e, Game *game);

#endif
