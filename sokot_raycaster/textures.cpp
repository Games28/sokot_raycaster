#include "textures.h"
#include <stdio.h>

void Texture::loadTextures()
{
	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		upng_t* upng = upng_new_from_file(textureFileNames[i]);
		if (upng != NULL) {
			upng_decode(upng);
			if (upng_get_error(upng) == UPNG_EOK) {
				textures[i] = upng;
			}
			else
			{
				printf("error decoding texture file %s \n", textureFileNames[i]);
			}

		}
		else
		{
			printf("error loading texture %s \n", textures[i]);
		}
	}

	for (int i = 0; i < NUM_CHARTEXTURES; i++)
	{

		upng_t* upng = upng_new_from_file(characterFileNames[i]);
		if (upng != NULL) {
			upng_decode(upng);
			if (upng_get_error(upng) == UPNG_EOK) {
				characterTextures[i] = upng;
			}
			else
			{
				printf("error decoding texture file %s \n", characterFileNames[i]);
			}

		}
		else
		{
			printf("error loading texture %s \n", characterTextures[i]);
		}
	}
}

void Texture::freeTextures()
{
	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		upng_free(textures[i]);
	}

	for (int i = 0; i < NUM_CHARTEXTURES; i++)
	{
		upng_free(characterTextures[i]);
	}
}

