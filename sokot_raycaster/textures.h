#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdint.h>


#include "upng.h"
#define NUM_TEXTURES 4
#define NUM_CHARTEXTURES 4

class Texture
{
public:
	Texture() = default;

	upng_t* textures[NUM_TEXTURES];
	upng_t* characterTextures[NUM_CHARTEXTURES];

	// to determine whether a texture contains transparency
	bool bIsTransparent[NUM_TEXTURES];

	void loadTextures();
	void freeTextures();

public:
	const char* textureFileNames[NUM_TEXTURES] = {
        "./images/redbrick.png",                   //  0
        "./images/purplestone.png",                //  1
        "./images/mossystone.png",                 //  2
        "./images/graystone.png",                  //  3
       
	};



	const char* characterFileNames[NUM_CHARTEXTURES] = {
		
		"troopertest.png",
		"trooperT.png",
	};
};

#endif

