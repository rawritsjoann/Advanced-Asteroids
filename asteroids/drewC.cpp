//Author: Drew Cody
//Class: CMPS 335
//Assignment: HW3

#include <cmath>
extern "C" {
	#include "fonts.h"
}
#include "structures.h"

void resizeAsteroid(Asteroid *a)
{
	if (a->radius < 50 && a->radius > 25) {
		a->nverts = 4;
		a->radius = rnd()*80.0 + 40.0;
		Flt r2 = a->radius / 2.0;
		Flt angle = 0.0f;
		Flt inc = (PI * 2.0) / (Flt)a->nverts;
		for (int i=0; i<a->nverts; i++) {
			a->vert[i][0] = sin(angle) * (r2 + rnd() * a->radius);
			a->vert[i][1] = cos(angle) * (r2 + rnd() * a->radius);
			angle += inc;
		}
		a->color[0] = 1;
		a->color[1] = 0.0;
		a->color[2] = 0.0;
	} 
}

void asteroidCollision(Asteroid *a, Game *g)
{
	while (a) {
		float d0, d1, dist, shipVelocity, asteroidVelocity;
		d0 = g->ship.pos[0] - a->pos[0];
		d1 = g->ship.pos[1] - a->pos[1];
		dist = (d0*d0 + d1*d1);
		if (dist < (a->radius * a->radius) * 0.5) {
			//Calculate damage taken from impact with asteroid
			shipVelocity = abs(g->ship.vel[0] + g->ship.vel[1]);
			asteroidVelocity = abs(a->vel[0] + a->vel[1]);

			if (shipVelocity >= 2.5 || asteroidVelocity >= 2.5) {
				g->ship.damageTaken += 5;
			}
			else if (shipVelocity >= 1.5 || asteroidVelocity >= 1.5) {
				g->ship.damageTaken += 2;
			} else {
				g->ship.damageTaken++;
			}
			if ( g->ship.damageTaken >= 100 ) {
				g->gameOver = true;
			}
			//Alter ships position and velocity	
			g->ship.pos[0] = a->pos[0] + d0 * 1.5;
			g->ship.pos[1] = a->pos[1] + d1 * 1.5;
			g->ship.vel[0] *= -0.5f;
			g->ship.vel[1] *= -0.5f;
			a->vel[0] += g->ship.vel[0] * -0.5;
			a->vel[1] += g->ship.vel[1] * -0.5;
		}
		a = a->next;
	}
}

bool endGame(Game *g)
{
	if (g->ship.damageTaken >= 10) {
		return true;
	} else {
		return false;
	}
}

void endMenu(Game *g)
{
	int yellow = 0x00ffff00;
	int red = 0x00ff0000;
	Rect r;
	r.bot = yres - 300;
	r.left = 600;
	ggprint16(&r, 50, red, "GAME OVER");
	ggprint16(&r, 50, yellow, "Your Score: %i", g->score);
	ggprint16(&r, 50, yellow, "Your Time: %i seconds",g->gameTimer);
	r.bot = yres - 600;
	ggprint16(&r, 50, yellow, "Press ESC to Exit");
}

