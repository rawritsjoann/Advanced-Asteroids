//Author: Drew Cody
//Class: CMPS 335
//Assignment: HW3

#include <cmath>
#include "structures.h"


void resizeAsteroid(Asteroid *a)
{
    if( a->radius < 40 && a->radius > 20 ) {
	a->nverts = 8;
	a->radius = rnd()*80.0 + 40.0;
	Flt r2 = a->radius / 2.0;
	Flt angle = 0.0f;
	Flt inc = (PI * 2.0) / (Flt)a->nverts;
	for (int i=0; i<a->nverts; i++) {
	    a->vert[i][0] = sin(angle) * (r2 + rnd() * a->radius);
	    a->vert[i][1] = cos(angle) * (r2 + rnd() * a->radius);
	    angle += inc;
	}
	a->color[0] = 1.0;
	a->color[1] = 0.0;
	a->color[2] = 0.0;
    }
}

void asteroidCollision(Asteroid *a, Game *g)
{
    while(a) {
	Flt d0, d1, dist;
	d0 = g->ship.pos[0] - a->pos[0];
	d1 = g->ship.pos[1] - a->pos[1];
	dist = (d0*d0 + d1*d1);
	if(dist < (a->radius * a->radius)) {
	    g->ship.pos[0] = a->pos[0] + d0 * 1.5;
	    g->ship.pos[1] = a->pos[1] + d1 * 1.5;
	    g->ship.vel[0] *= -0.5f;
	    g->ship.vel[1] *= -0.5f;
	    a->vel[0] += g->ship.vel[0] * -0.5;
	    a->vel[1] += g->ship.vel[1] * -0.5;
	}
	a = a->next;
    }
    return;
}
