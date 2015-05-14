/*Amador Joao Silva
 CMPS 335
 Advanced Asteroids
 Spring 2015
  */
#include "structures.h"
#include "ppm.h"


GLuint getPpm()
{
    //load images into a ppm structure
    //Ppm tmp;
    Ppmimage *asteroidTex=NULL;
    asteroidTex = ppm6GetImage((char*)"./images/generic.ppm");
    int w = asteroidTex->width;
    int h = asteroidTex->height;
    unsigned char tmpArr[w*h*3];
    unsigned char *t = asteroidTex->data;
    unsigned char dataWithAlpha[w*h*4];

    for(int i=0; i<(w*h*3); i++){
	tmpArr[i] = *(t+i);
    }
    // apply the alpha channel
    for(int i=0; i<(w*h); i++){
	// copy color to new array
	dataWithAlpha[i*4] = tmpArr[3 * i];
	dataWithAlpha[i*4 + 1] = tmpArr[ 3 * i + 1];
	dataWithAlpha[i*4 + 2] = tmpArr[ 3 * i + 2];
	// set alpha
	dataWithAlpha[i*4+3]=((int)tmpArr[i*3] | (int)tmpArr[i*3+1] | (int)tmpArr[i*3+2] );
    }


    GLuint returningTex;
    //
    glGenTextures(1, &returningTex);
    glBindTexture(GL_TEXTURE_2D, returningTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataWithAlpha);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return returningTex;
}

