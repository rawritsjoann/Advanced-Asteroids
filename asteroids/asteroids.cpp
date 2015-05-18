//CS335 Spring 2015
//
//Program: Advanced Asteroids
//Authors:  Drew Cody, JoAnn Tuazon, Michael Wines, Amador Joao Silva
//Date:    2015
//
// Possible additions:
// ----------------------
// control of bullet launch point
// score keeping
// levels of difficulty
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "solidSphere.h"
#include "structures.h"
#include "drewC.cpp"
#include "ajS.cpp"
#include "michaelW.cpp"
#include "joannT.cpp"
extern "C" {
#include "fonts.h"
}

#include <FMOD/fmod.h>
#include <FMOD/wincompat.h>
#include "fmod.h"

using namespace std;

//constants
const float timeslice = 1.0f;
const float gravity = -0.2f;

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;


//-----------------------------------------------------------------------------
//Setup timers
const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
struct timespec timeStart, timeCurrent;
struct timespec timePause;
double physicsCountdown=0.0;
double timeSpan=0.0;
double timeDiff(struct timespec *start, struct timespec *end) {
    return (double)(end->tv_sec - start->tv_sec ) +
	(double)(end->tv_nsec - start->tv_nsec) * oobillion;
}
void timeCopy(struct timespec *dest, struct timespec *source) {
    memcpy(dest, source, sizeof(struct timespec));
}
//-----------------------------------------------------------------------------

int xres=1250, yres=900;

GLuint asteroidtext;
int play_sounds = 0;

int keys[65536];

//function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_resize(XEvent *e);
int check_keys(XEvent *e);
void init(Game *g);
void init_sounds(void);
void physics(Game *game);
void render(Game *game);
bool endGame(Game *game);
void endMenu(Game *game);

int main(void)
{
    initXWindows();
    init_opengl();
    init_sounds();
    getShipTexture();
    Game game;
    srand(time(NULL));
    int done=0;

    glBindTexture(GL_TEXTURE_2D, bgTexture);
    glBegin(GL_QUADS);
    glColor3f(1.0f,0.0f,0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex2i(0, yres);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(xres, yres);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(xres, 0);
    glEnd();

    while (done != 1) {
	while (XPending(dpy)) {
	    XEvent e;
	    XNextEvent(dpy, &e);
	    check_resize(&e);
	    done = check_keys(&e);
	    Rect r;
	    r.bot = yres - 100;
	    r.left = 500;
	    r.center = 0;
	    ggprint16(&r, 36, 0x00ffff00, "ADVANCED ASTEROIDS");
	    r.bot = yres - 500;
	    r.left = 250;
	    ggprint16(&r, 36, 0x00ffff00, "S: START GAME          C: CONTROLS            H: HOW TO           ESC: Exit Game");
	    if(done == 2){
		r.bot = yres - 200;
		r.left = 250;
		ggprint16(&r, 36, 0x00ffff00, "Arrow <-                Turn ship left");
		ggprint16(&r, 36, 0x00ffff00, "Arrow ->                Turn ship right");
		ggprint16(&r, 36, 0x00ffff00, "Arrow ^                 Accelerate ship");
		ggprint16(&r, 36, 0x00ffff00, "Arrow v                 Brakes/Reverse");
		ggprint16(&r, 36, 0x00ffff00, "Space                    Shoot Gun");

	    }
	    if(done == 5){
		r.bot = yres - 200;
		r.left = 675;
		ggprint16(&r, 36, 0x00ffff00, " - Shoot asteroids to make them explode");
		ggprint16(&r, 36, 0x00ffff00, " - When hit, asteroids break into smaller asteroids");
		ggprint16(&r, 20, 0x00ffff00, " - Some asteroids will revert to starting size");
		ggprint16(&r, 36, 0x00ffff00, "   at a set interval");
		ggprint16(&r, 20, 0x00ffff00, " - After hitting a set number of asteroids your ship");
		ggprint16(&r, 36, 0x00ffff00, "   will go into SUPERMODE for a short time");
		ggprint16(&r, 36, 0x00ffff00, " - Eliminate all asteroids to win");
	    }
	    if(done == 3){
		play_music(0);
		init(&game);
		break;
	    }
	}
	glXSwapBuffers(dpy, win);
	if(done == 3) { break; }
    }
    clock_gettime(CLOCK_REALTIME, &timePause);
    clock_gettime(CLOCK_REALTIME, &timeStart);
    while (done != 1) {
	while (XPending(dpy)) {
	    XEvent e;
	    XNextEvent(dpy, &e);
	    check_resize(&e);
	    done = check_keys(&e);
	}
	clock_gettime(CLOCK_REALTIME, &timeCurrent);
	timeSpan = timeDiff(&timeStart, &timeCurrent);
	timeCopy(&timeStart, &timeCurrent);
	physicsCountdown += timeSpan;
	while (physicsCountdown >= physicsRate) {
	    physics(&game);
	    physicsCountdown -= physicsRate;
	}
	render(&game);
	glXSwapBuffers(dpy, win);
    }
    cleanupXWindows();
    cleanup_fonts();
    fmod_cleanup();
    return 0;
}
void cleanupXWindows(void)
{
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

void set_title(void)
{
    //Set the window title bar.
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "CS335 - Advanced Asteroids");
}

void setup_screen_res(const int w, const int h)
{
    xres = w;
    yres = h;
}

void initXWindows(void)
{
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    XSetWindowAttributes swa;
    setup_screen_res(xres, yres);
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
	std::cout << "\n\tcannot connect to X server" << std::endl;
	exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if (vi == NULL) {
	std::cout << "\n\tno appropriate visual found\n" << std::endl;
	exit(EXIT_FAILURE);
    } 
    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
	StructureNotifyMask | SubstructureNotifyMask;
    win = XCreateWindow(dpy, root, 0, 0, xres, yres, 0,
	    vi->depth, InputOutput, vi->visual,
	    CWColormap | CWEventMask, &swa);
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
}

void reshape_window(int width, int height)
{
    //window has been resized.
    setup_screen_res(width, height);
    glViewport(0, 0, (GLint)width, (GLint)height);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glOrtho(0, xres, 0, yres, -1, 1);
    set_title();
}

void init_opengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, xres, yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //This sets 2D mode (no perspective)
    glOrtho(0, xres, 0, yres, -1, 1);
    //
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);
    //
    //Clear the screen to black
    glClearColor(0.0, 0.0, 0.0, 1.0);
    //Do this to allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();
    load_background();
}


void check_resize(XEvent *e)
{
    //The ConfigureNotify is sent by the
    //server if the window is resized.
    if (e->type != ConfigureNotify)
	return;
    XConfigureEvent xce = e->xconfigure;
    if (xce.width != xres || xce.height != yres) {
	//Window size did change.
	reshape_window(xce.width, xce.height);
    }
}

void init(Game *g) {
    // start with 10 asteroids
    for (int j=0; j<10; j++) {
	Asteroid *a = new Asteroid;
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
	a->pos[0] = ((Flt)(rand() % xres))+500+200;
	a->pos[1] = ((Flt)(rand() % yres))+500+200;
	a->pos[2] = 0.0f;
	a->angle = 0.0;
	a->rotate = rnd() * 4.0 - 2.0;
	a->color[0] = 0.75;
	a->color[1] = 0.75;
	a->color[2] = 0.75;
	a->vel[0] = (Flt)(rnd()*2.0-1.0);
	a->vel[1] = (Flt)(rnd()*2.0-1.0);
	a->next = g->ahead;
	if (g->ahead != NULL)
	    g->ahead->prev = a;
	g->ahead = a;
	g->nasteroids++;
    }
    asteroidtext = getPpm();
    clock_gettime(CLOCK_REALTIME, &g->bulletTimer);
    clock_gettime(CLOCK_REALTIME, &g->asteroidTimer);
    memset(keys, 0, 65536);
}

void normalize(Vec v) {
    Flt len = v[0]*v[0] + v[1]*v[1];
    if (len == 0.0f) {
	v[0] = 1.0;
	v[1] = 0.0;
	return;
    }
    len = 1.0f / sqrt(len);
    v[0] *= len;
    v[1] *= len;
}

int check_keys(XEvent *e)
{
    //keyboard input?
    int key = XLookupKeysym(&e->xkey, 0);
    if (e->type == KeyRelease) {
	keys[key]=0;
	return 0;
    }
    if (e->type == KeyPress) {
	keys[key]=1;
    } else {
	return 0;
    }
    switch(key) {
	case XK_Escape:
	    return 1;
	case XK_c:
	    return 2;
	case XK_s:
	    return 3;
	case XK_r:
	    return 4;
	case XK_h:
	    return 5;
	case XK_minus:
	    break;
    }
    return 0;
}

void deleteBullet(Game *g, Bullet *node)
{
    //remove a node from linked list
    if (node->prev == NULL) {
	if (node->next == NULL) {
	    g->bhead = NULL;
	} else {
	    node->next->prev = NULL;
	    g->bhead = node->next;
	}
    } else {
	if (node->next == NULL) {
	    node->prev->next = NULL;
	} else {
	    node->prev->next = node->next;
	    node->next->prev = node->prev;
	}
    }
    delete node;
    node = NULL;
    g->nbullets--;
}

void deleteAsteroid(Game *g, Asteroid *node)
{
    if (node->prev == NULL) {
	if (node->next == NULL) {
	    g->ahead = NULL;
	} else {
	    node->next->prev = NULL;
	    g->ahead = node->next;
	}
    } else {
	if (node->next == NULL) {
	    node->prev->next = NULL;
	} else {
	    node->prev->next = node->next;
	    node->next->prev = node->prev;
	}
    }
    delete node;
    node = NULL;
}

void buildAsteroidFragment(Asteroid *ta, Asteroid *a)
{
    //build ta from a
    ta->nverts = 4;
    ta->radius = a->radius / 1.5;
    if(ta->radius < 20.0f) {
	ta->radius = 20.0f;
    }
    Flt r2 = ta->radius / 2.0;
    Flt angle = 0.0f;
    Flt inc = (PI * 2.0) / (Flt)ta->nverts;
    for (int i=0; i<ta->nverts; i++) {
	ta->vert[i][0] = sin(angle) * (r2 + rnd() * ta->radius);
	ta->vert[i][1] = cos(angle) * (r2 + rnd() * ta->radius);
	angle += inc;
    }
    ta->pos[0] = a->pos[0] + rnd()*10.0-5.0;
    ta->pos[1] = a->pos[1] + rnd()*10.0-5.0;
    ta->pos[2] = 0.0f;
    ta->angle = 0.0;
    ta->rotate = a->rotate + (rnd() * 4.0 - 2.0);
    ta->color[0] = 0.8;
    ta->color[1] = 0.8;
    ta->color[2] = 0.7;
    ta->color[3] = 0;
    ta->vel[0] = a->vel[0] + (rnd()*2.0-1.0);
    ta->vel[1] = a->vel[1] + (rnd()*2.0-1.0);
    //std::cout << "frag" << std::endl;
}

void physics(Game *g)
{
    Flt d0,d1,dist;
    //Update ship position
    g->ship.pos[0] += g->ship.vel[0];
    g->ship.pos[1] += g->ship.vel[1];
    //Check for collision with window edges
    if (g->ship.pos[0] < 0.0) {
	g->ship.pos[0] += (float)xres;
    }
    else if (g->ship.pos[0] > (float)xres) {
	g->ship.pos[0] -= (float)xres;
    }
    else if (g->ship.pos[1] < 0.0) {
	g->ship.pos[1] += (float)yres;
    }
    else if (g->ship.pos[1] > (float)yres) {
	g->ship.pos[1] -= (float)yres;
    }


    //Ship collision with asteroids
    Asteroid *a = g->ahead;
    asteroidCollision(a,g);



    //Update bullet positions
    struct timespec bt;
    clock_gettime(CLOCK_REALTIME, &bt);
    Bullet *b = g->bhead;
    while (b) {
	//How long has bullet been alive?
	double ts = timeDiff(&b->time, &bt);
	if (ts > 2.5) {
	    //time to delete the bullet.
	    Bullet *saveb = b->next;
	    deleteBullet(g, b);
	    b = saveb;
	    g->nbullets--;
	    continue;
	}
	//move the bullet
	b->pos[0] += b->vel[0];
	b->pos[1] += b->vel[1];
	//Check for collision with window edges
	if (b->pos[0] < 0.0) {
	    b->pos[0] += (float)xres;
	}
	else if (b->pos[0] > (float)xres) {
	    b->pos[0] -= (float)xres;
	}
	else if (b->pos[1] < 0.0) {
	    b->pos[1] += (float)yres;
	}
	else if (b->pos[1] > (float)yres) {
	    b->pos[1] -= (float)yres;
	}
	b = b->next;
    }

    //Update asteroid positions
    a = g->ahead;
    while (a) {
	a->pos[0] += a->vel[0];
	a->pos[1] += a->vel[1];
	//Check for collision with window edges
	if (a->pos[0] < -50) {
	    a->pos[0] += (float)xres+100;
	}
	else if (a->pos[0] > (float)xres+50) {
	    a->pos[0] -= (float)xres+100;
	}
	else if (a->pos[1] < -50) {
	    a->pos[1] += (float)yres+100;
	}
	else if (a->pos[1] > (float)yres+50) {
	    a->pos[1] -= (float)yres+100;
	}
	a->angle += a->rotate;
	a = a->next;
    }

    //Asteroid collision with bullets
    a = g->ahead;
    while (a) {
	//is there a bullet within its radius?
	glColor3fv(a->color);
	Bullet *b = g->bhead;
	while (b) {
	    d0 = b->pos[0] - a->pos[0];
	    d1 = b->pos[1] - a->pos[1];
	    dist = (d0*d0 + d1*d1);
	    if (dist < (a->radius*a->radius)) {
		//this asteroid is hit.
		g->bulletsHit++;
		g->ship.superMode++;
		if (abs(g->ship.vel[0] + g->ship.vel[1]) > 0.5f) {
		    g->score += 10;
		} else {
		    g->score += 5;
		}
		if (a->radius > 30.0) {
		    //break it into pieces.
		    Asteroid *ta = a;
		    buildAsteroidFragment(ta, a);
		    int r;
		    if (g->nasteroids > 100) { r = 2; }
		    else if (g->nasteroids > 50) { r = 3; }
		    else { r = 5; }
		    //int r = rand() % 2+3; //# OF FRAGMENTS
		    for (int k=0; k<r; k++) {
			//get the next asteroid position in the array
			Asteroid *ta = new Asteroid;
			buildAsteroidFragment(ta, a);
			//add to front of asteroid linked list
			ta->next = g->ahead;
			if (g->ahead != NULL)
			    g->ahead->prev = ta;
			g->ahead = ta;
			g->nasteroids++;
		    }
		} else {
		    //asteroid is too small to break up
		    //delete the asteroid and bullet
		    Asteroid *savea = a->next;
		    deleteAsteroid(g, a);
		    a = savea;
		    g->nasteroids--;
		}
		//delete the bullet...
		Bullet *saveb = b->next;
		deleteBullet(g, b);
		b = saveb;
		if (a == NULL)
		    break;
		continue;
	    }
	    b = b->next;
	}
	if (a == NULL)
	    break;
	a = a->next;
    }
    //---------------------------------------------------
    //check keys pressed now
    if (keys[XK_Left]) {
	g->ship.angle += 4.0;
	if (g->ship.angle >= 360.0f)
	    g->ship.angle -= 360.0f;
    }
    if (keys[XK_Right]) {
	g->ship.angle -= 4.0;
	if (g->ship.angle < 0.0f)
	    g->ship.angle += 360.0f;
    }
    if (keys[XK_Up]) {
	//apply thrust
	//convert ship angle to radians
	Flt rad = ((g->ship.angle+90.0) / 360.0f) * PI * 2.0;
	Flt xdir = cos(rad);
	Flt ydir = sin(rad);
	g->ship.vel[0] += xdir*0.05f;
	g->ship.vel[1] += ydir*0.05f;
	Flt speed = sqrt(g->ship.vel[0]*g->ship.vel[0]+
		g->ship.vel[1]*g->ship.vel[1]);
	if (speed > 7.0f) {
	    speed = 7.0f;
	    normalize(g->ship.vel);
	    g->ship.vel[0] *= speed;
	    g->ship.vel[1] *= speed;
	}
    }
    if (keys[XK_Down]) {
	//apply breaks!
	Flt rad = ((g->ship.angle+90.0) / 360.0f) * PI * 2.0;
	Flt xdir = cos(rad);
	Flt ydir = sin(rad);
	if (g->ship.superMode >= 50) {
	    g->ship.vel[0] = 0.0f;
	    g->ship.vel[1] = 0.0f;
	} else {
	    g->ship.vel[0] += xdir * -0.025f;
	    g->ship.vel[1] += ydir * -0.025f;
	    Flt speed = sqrt(g->ship.vel[0]*g->ship.vel[0]+
		    g->ship.vel[1]*g->ship.vel[1]);
	    if (speed > 5.0f) {
		speed = 5.0f;
		normalize(g->ship.vel);
		g->ship.vel[0] *= speed;
		g->ship.vel[1] *= speed;
		cout << "VelocityX: " << g->ship.vel[0] << "   VelocityY: " << g->ship.vel[1] << endl;
	    }
	}
    }
    if (keys[XK_space]) {
	//a little time between each bullet
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	double ts = timeDiff(&g->bulletTimer, &bt);
	if (ts > 0.1) {
	    timeCopy(&g->bulletTimer, &bt);
	    //shoot a bullet...
	    Bullet *b = new Bullet;
	    timeCopy(&b->time, &bt);
	    b->pos[0] = g->ship.pos[0];
	    b->pos[1] = g->ship.pos[1];
	    b->vel[0] = g->ship.vel[0];
	    b->vel[1] = g->ship.vel[1];
	    //convert ship angle to radians
	    Flt rad = ((g->ship.angle+90.0) / 360.0f) * PI * 2.0;
	    //convert angle to a vector
	    Flt xdir = cos(rad);
	    Flt ydir = sin(rad);
	    b->pos[0] += xdir*20.0f;
	    b->pos[1] += ydir*20.0f;
	    b->vel[0] += xdir*6.0f + rnd()*0.1;
	    b->vel[1] += ydir*6.0f + rnd()*0.1;
	    b->color[0] = 1.0f;
	    b->color[1] = 1.0f;
	    b->color[2] = 1.0f;
	    //add to front of bullet linked list
	    b->next = g->bhead;
	    if (g->bhead != NULL)
		g->bhead->prev = b;
	    g->bhead = b;
	    g->nbullets++;
	    g->bulletsFired++;
	}
    }
}

void render(Game *g)
{
    draw_background();
    bool gameOver = endGame(g);

    if (!gameOver) {
	struct timespec at;
	int yellow = 0x00ffff00;
	clock_gettime(CLOCK_REALTIME, &at);
	g->gameTimer = timeDiff(&g->asteroidTimer, &at);

	Rect r;
	r.bot = yres - 30;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0x00ff0000, "cs335 - Advanced Asteroids");
	ggprint8b(&r, 16, yellow, "n asteroids: %i", g->nasteroids);
	ggprint8b(&r, 16, yellow, "Game time: %i", g->gameTimer);
	ggprint8b(&r, 16, yellow, "Super Mode: %i", g->ship.superMode);
	ggprint8b(&r, 16, yellow, "Score: %i", g->score);
	ggprint8b(&r, 16, yellow, "Damage: %i", g->ship.damageTaken);
	//ggprint16(&r, 20, yellow, "Hit: %i", g->bulletsHit);
	//ggprint16(&r, 20, yellow, "Fired: %i", g->bulletsFired);
	//ggprint16(&r, 20, yellow, "Accuracy: %f%", g->accuracy);



	//-------------------------------------------------------------------------
	//Draw the ship

	if( g->ship.superMode >= 50 ) {
		play_music(1);
	    int x, y, z;
	    x = random(3);
	    y = random(3);
	    z = random(3);
	    glColor3f(x,y,z);
	    if(g->ship.superMode >= 150) {
	    	//fmod_stopsound(1); /*Not a Function*/
		g->ship.superMode = 0;
	    }
	} else {
	    glColor3fv(g->ship.color);
	}
	setShipTexture(g);
	glColor3f(1.0f,0.0f,0.0f);
	glBegin(GL_POINTS);
	glVertex2f(0.0f,0.0f);
	glEnd();
	glPopMatrix();
	if (keys[XK_Up]) {
	    int i;
	    //draw thrust
	    Flt rad = ((g->ship.angle+90.0) / 360.0f) * PI * 2.0;
	    //convert angle to a vector
	    Flt xdir = cos(rad);
	    Flt ydir = sin(rad);
	    Flt xs,ys,xe,ye,r;
	    for (i=0; i<16; i++) {
		glBegin(GL_LINES);
		xs = -xdir * 11.0f + rnd() * 4.0 - 2.0;
		ys = -ydir * 11.0f + rnd() * 4.0 - 2.0;

		r = rnd()*50.0+50.0;

		xe = -xdir * r + rnd() * 18.0 - 9.0;
		ye = -ydir * r + rnd() * 18.0 - 9.0;
		if(r < 26.0) {
		    glColor3f(9.0f,9.0f,9.0f);
		} else {
		    if(r >26 && r <= 52) {
			glColor3f(9.0f,9.0f,0.0f);
		    }
		    else {
			glColor3f(9.0f,0.0f,0.0f);
		    }
		}
		glVertex2f(g->ship.pos[0]+xs,g->ship.pos[1]+ys);
		glVertex2f(g->ship.pos[0]+xe,g->ship.pos[1]+ye);
		glEnd();
	    }
	}
	//-------------------------------------------------------------------------
	//Draw the asteroids
	{

	    Asteroid *a = g->ahead;
	    while (a) {
		if( g->gameTimer%15 == 0 
			&& g->nasteroids <= 20 
			&& g->gameTimer != 0 
			&& g->score > 100) {
		    resizeAsteroid(a);
		}

		glColor4fv(a->color);
		glBindTexture(GL_TEXTURE_2D, asteroidtext);
		glPushMatrix();
		glTranslatef(a->pos[0], a->pos[1], a->pos[2]);
		glRotatef(a->angle, 0.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
		glTexCoord2f(0,1);
		glVertex2f(a->vert[0][0], a->vert[0][1]);
		glTexCoord2f(1,1);
		glVertex2f(a->vert[1][0], a->vert[1][1]);
		glTexCoord2f(1,0);
		glVertex2f(a->vert[2][0], a->vert[2][1]);
		glTexCoord2f(0,0);
		glVertex2f(a->vert[3][0], a->vert[3][1]);
		glEnd();
		glPopMatrix();

		//Center Point Dot
		glBindTexture(GL_TEXTURE_2D, 0);
		glBegin(GL_POINTS);
		glVertex2f(a->pos[0], a->pos[1]);
		glEnd();
		//next asteroid
		a = a->next;
	    }
	}
	//-------------------------------------------------------------------------
	//Draw the bullets
	{
	    Bullet *b = g->bhead;
	    while (b) {
		float size = 1.0;
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_POINTS);
		glVertex2f(b->pos[0],      b->pos[1]);
		glVertex2f(b->pos[0]-size, b->pos[1]);
		glVertex2f(b->pos[0]+size, b->pos[1]);
		glVertex2f(b->pos[0],      b->pos[1]-size);
		glVertex2f(b->pos[0],      b->pos[1]+size);
		glColor3f(0.0, 1.0, 0.0);
		glVertex2f(b->pos[0]-size, b->pos[1]-size);
		glVertex2f(b->pos[0]-size, b->pos[1]+size);
		glVertex2f(b->pos[0]+size, b->pos[1]-size);
		glVertex2f(b->pos[0]+size, b->pos[1]+size);
		glEnd();
		b = b->next;
	    }
	}
    }
    if (gameOver) {
	Asteroid *a2 = g->ahead;
	while(a2){
	    Asteroid *savea = a2->next;
	    deleteAsteroid(g, a2);
	    a2 = savea;
	    g->nasteroids--;
	}
	endMenu(g);
    }
}

