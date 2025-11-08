#include "sprite.h"

void Sprites::init( Map *mapPtr )
{
	sprites[0] = { 300, 400, 1, 0,0,0,PI / 2,100, 45 * (PI / 180) };
	sprites[0].Slift = 0;

	pMap = mapPtr;
}

void Sprites::update(float deltatime, thePhysics& phs)
{

	//for (int i = 0; i < NUM_SPRITES; i++)
	//{
	//	Sprite* sprite = &sprites[i];
	//
	//	if (islifting)
	//	{
	//		sprite->Slift -= 5.0f;
	//	}
	//	else
	//	{
	//		phs.physicssetup(sprite->Slift);
	//		phs.physicsobjectlift(deltatime, sprite->isfalling);
	//	}
	//}



}

void Sprites::renderSpriteProjection(graphics& gfx, Player & player, Rays& ray, Texture& texture)
{
	Sprite visibleSprites[NUM_SPRITES];
	int numVisibleSprites = 0;


	for (int i = 0; i < NUM_SPRITES; i++) {
		float angleSpritePlayer = player.rotationAngle - atan2(sprites[i].y - player.y, sprites[i].x - player.x);

		if (angleSpritePlayer > PI)
			angleSpritePlayer -= TWO_PI;
		if (angleSpritePlayer < -PI)
			angleSpritePlayer += TWO_PI;

		angleSpritePlayer = fabs(angleSpritePlayer);

		const float EPSILON = 0.2f;
		if (angleSpritePlayer < (FOV_ANGLE / 2) + EPSILON) {
			sprites[i].visible = true;
			sprites[i].angle = angleSpritePlayer;
			sprites[i].distance = distanceBetweenPoints(sprites[i].x, sprites[i].y, player.x, player.y);
			visibleSprites[numVisibleSprites] = sprites[i];
			numVisibleSprites++;
		}
		else {
			sprites[i].visible = false;
		}
	}

	for (int i = 0; i < numVisibleSprites - 1; i++)
	{
		for (int j = i + 1; j < numVisibleSprites; j++)
		{
			if (visibleSprites[i].distance < visibleSprites[j].distance)
			{
				Sprite temp = visibleSprites[i];
				visibleSprites[i] = visibleSprites[j];
				visibleSprites[j] = temp;
			}
		}
	}

	for (int i = 0; i < numVisibleSprites; i++)
	{
		Sprite sprite = visibleSprites[i];

		float prepDistance = sprite.distance * cos(sprite.angle);

		float spriteHeight = (TILE_SIZE / prepDistance) * Dist_PROJ_PLANE;
		float spriteWidth = spriteHeight;
		
		

		float spriteTopY = (WINDOW_HEIGHT / 2) - (spriteHeight / 2);
		spriteTopY = (spriteTopY < 0) ? 0 : spriteTopY;

		float spriteBottomY = (WINDOW_HEIGHT / 2) + (spriteHeight / 2);
		spriteBottomY = (spriteBottomY > WINDOW_HEIGHT) ? WINDOW_HEIGHT : spriteBottomY;

		float spriteAngle = atan2(sprite.y - player.y, sprite.x - player.x) - player.rotationAngle;
		float spriteScreenPosX = tan(spriteAngle) * Dist_PROJ_PLANE;

		float spriteLeftX = (WINDOW_WIDTH / 2) + spriteScreenPosX - spriteWidth / 2;

		
		
		float spriteRightX = spriteLeftX + spriteWidth;
		
		

		int textureWidth = upng_get_width(texture.characterTextures[sprite.texture]);
		int textureHeight = upng_get_height(texture.characterTextures[sprite.texture]);

		float texelWidth = (textureWidth / spriteWidth) * 0.25f;
		for (int x = spriteLeftX ; x < spriteRightX; x++)
		{

			
			//float texWidthsubset = texelWidth ;
			int textureOffSetX = (x - spriteLeftX) * texelWidth;

			
			for (int y = spriteTopY; y < spriteBottomY; y++)
			{
				int distanceFromTop = y + (spriteHeight / 2) - (WINDOW_HEIGHT / 2);
				int textureOffSetY = distanceFromTop * (textureHeight / spriteHeight);

				if (x > 0 && x < WINDOW_WIDTH && y > 0 && y < WINDOW_HEIGHT)
				{

					color_t* spriteTextureBuffer = (color_t*)upng_get_buffer(texture.characterTextures[sprite.texture]);
					color_t texelColor = spriteTextureBuffer[(textureWidth * textureOffSetY) + (textureOffSetX)];

					if (sprite.distance < ray.rays[x][0].frontdistance && texelColor != 0xffff00ff)
						gfx.drawPixel(x, y, texelColor);
				}

			}
		}

	}
}

void Sprites::renderMapSprites(graphics& gfx)
{
	for (int i = 0; i < NUM_SPRITES; i++)
	{
		for (int i = 0; i < NUM_SPRITES; i++) {
			gfx.drawRect(
				sprites[i].x * MINIMAP_SCALE_FACTOR,
				sprites[i].y * MINIMAP_SCALE_FACTOR,
				4,
				4,
				(sprites[i].visible) ? 0xFF00FFFF : 0xFF444444
			);
		}
	}
	for (int i = 0; i < NUM_SPRITES; i++)
	{
		Sprite sprite = sprites[i];
		gfx.drawLine(sprite.x * MINIMAP_SCALE_FACTOR,
			sprite.y * MINIMAP_SCALE_FACTOR,
			(sprite.x + cos(sprite.rotationAngle) * 40) * MINIMAP_SCALE_FACTOR,
			(sprite.y + sin(sprite.rotationAngle) * 40) * MINIMAP_SCALE_FACTOR,
			0xff00ffff);
	}
}

float Sprites::distanceBetweenPoints(float x1, float y1, float x2, float y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void Sprites::moveSprite(float deltatime)
{
	for (int i = 0; i < NUM_SPRITES; i++)
	{
		Sprite* sprite = &sprites[i];
		sprite->rotationAngle += sprite->turnDirection * sprite->turnSpeed * deltatime;
		normalizeAngle(&sprite->rotationAngle);

		float moveStep = sprite->walkDirection * sprite->walkSpeed * deltatime;

		float newPlayerX = sprite->x + cos(sprite->rotationAngle) * moveStep;
		float newPlayerY = sprite->y + sin(sprite->rotationAngle) * moveStep;

		//wall collision
		if (!pMap->mapHasWallAt(newPlayerX, newPlayerY)) {

				sprite->x = newPlayerX;
				sprite->y = newPlayerY;

		}
		else
		{
			sprite->walkDirection *= -1;
		}

	}
}

void Sprites::normalizeAngle(float* angle)
{
	*angle = remainder(*angle, TWO_PI);
	if (*angle < 0) {
		*angle = TWO_PI + *angle;
	}
}
