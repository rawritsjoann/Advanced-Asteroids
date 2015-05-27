#ifndef STRUCTURES_H
#define STRUCTURES_H
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define rnd() (((double)rand())/(double)RAND_MAX)
#define random(a) (rand()%a)
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)     ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
							 (c)[1]=(a)[1]-(b)[1]; \
(c)[2]=(a)[2]-(b)[2]

#define PI 3.141592653589793
#define ALPHA 1

typedef float Flt;
typedef float Vec[3];
typedef Flt Matrix[4][4];
extern int xres;
extern int yres;
extern Display *dpy;
extern Window win;
extern XEvent *e;

struct Ship {
	Vec dir;
	Vec pos;
	Vec vel;
	float angle;
	float color[3];
	int superMode;
	struct timespec superTimer;
	int damageTaken;
	Ship() {
		VecZero(dir);
		pos[0] = (Flt)(xres/2);
		pos[1] = (Flt)(yres/2);
		pos[2] = 0.0f;
		VecZero(vel);
		angle = 0.0;
		color[0] = 1.0;
		color[1] = 1.0;
		color[2] = 1.0;
		superMode = 0;
		damageTaken = 0;
	}
};

struct Bullet {
	Vec pos;
	Vec vel;
	float color[3];
	struct timespec time;
	struct Bullet *prev;
	struct Bullet *next;
	Bullet() {
		prev = NULL;
		next = NULL;
		color[0] = 1.0f;
		color[1] = 1.0f;
		color[2] = 1.0f;
	}
};

struct Asteroid {
	Vec pos;
	Vec vel;
	int nverts;
	Flt radius;
	Vec vert[4];
	float angle;
	float rotate;
	float color[3];
	struct Asteroid *prev;
	struct Asteroid *next;
	Asteroid() {
		prev = NULL;
		next = NULL;
	}
};

struct Game {
	Ship ship;
	Asteroid *ahead;
	Bullet *bhead;
	int nasteroids;
	int nbullets;
	int gameTimer;
	float bulletsFired;
	float bulletsHit;
	float accuracy;
	bool gameOver;
	unsigned long int score;
	struct timespec bulletTimer;
	struct timespec asteroidTimer;
	struct timespec mouseThrustTimer;
	bool mouseThrustOn;
	Game() {
		ahead = NULL;
		bhead = NULL;
		nasteroids = 0;
		nbullets = 0;
		gameTimer = 0;
		score = 0;
		gameOver = false;
		bulletsFired = 0.0;
		bulletsHit = 0.0;
		accuracy = 0.0;
		mouseThrustOn = false;
	}
};

#endif
