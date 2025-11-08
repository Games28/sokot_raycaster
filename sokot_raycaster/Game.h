#pragma once
#ifndef GAME_H
#define GAME_H

#define SOKOL_IMPL
#define SOKOL_GLCORE
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_gp.h"
#include "sokol/sokol_app.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_log.h"
#include "Pixel.h"
#include <string>

#define NUM_KEYS 512
#define WINDOW_WDITH 800
#define WINDOW_HEIGHT 600
#define PI 3.1415926535f
#define COL_CEIL    DARK_BLUE
#define COL_FLOOR   DARK_YELLOW
#define COL_WALL    GREY
#define COL_TEXT    MAGENTA

#define SPEED_ROTATE      60.0f  
#define SPEED_MOVE         5.0f  
#define SPEED_STRAFE       5.0f 


//generalized struct initializer
template<typename T>
void zeroMem(T& t) {
	std::memset(&t, 0, sizeof(T));
}


struct Game
{
	bool _keys_old[NUM_KEYS],
		_keys_curr[NUM_KEYS],
		_keys_new[NUM_KEYS];

	std::string sMap;
	int nMapX = 16;
	int nMapY = 16;

	float fMaxDistance = 25.0f;

	float fPlayerX = 2.0f;
	float fPlayerY = 2.0f;
	float fPlayerA_deg = 0.0f;
	float fPlayerH = 0.5f;
	float fPlayerFoV_deg = 60.0f;

	float fDistToProjPlane;

	void userInit()
	{
		//            0123456789012345
		sMap.append("################");
		sMap.append("#..............#");
		sMap.append("#........####..#");
		sMap.append("#..............#");
		sMap.append("#...#.....#....#");
		sMap.append("#...#..........#");
		sMap.append("#...####.......#");
		sMap.append("#..............#");
		sMap.append("#..............#");
		sMap.append("#..............#");
		sMap.append("#......##.##...#");
		sMap.append("#......#...#...#");
		sMap.append("#......#...#...#");
		sMap.append("#.......###....#");
		sMap.append("#..............#");
		sMap.append("################");

		// work out distance to projection plane. This is a constant depending on the width of the projection plane and the field of view.
		fDistToProjPlane = ((WINDOW_WDITH / 2.0f) / sin((fPlayerFoV_deg / 2.0f) * PI / 180.0f)) * cos((fPlayerFoV_deg / 2.0f) * PI / 180.0f);

	}



	void init()
	{
		sg_desc desc; zeroMem(desc);
		desc.environment = sglue_environment();
		sg_setup(desc);

		userInit();
		std::memset(_keys_old, false, sizeof(bool) * NUM_KEYS);
		std::memset(_keys_curr, false, sizeof(bool) * NUM_KEYS);
		std::memset(_keys_new, false, sizeof(bool) * NUM_KEYS);

	}

	void cleanup() {
		sg_shutdown();
	}

	void input(const sapp_event* e)
	{
		switch (e->type)
		{
		case SAPP_EVENTTYPE_KEY_DOWN:
		{
			_keys_new[e->key_code] = true;
		}break;

		case SAPP_EVENTTYPE_KEY_UP:
		{
			_keys_new[e->key_code] = false;
		}break;


		}

	}

	struct keyState { bool bPressed, bHeld, bReleased; };
	keyState GetKey(const sapp_keycode& kc) const
	{
		bool bheld = _keys_curr[kc];
		bool bpressed = bheld && !_keys_old[kc];
		return { bpressed, bheld,!bpressed };
	}

	void handleUserInput(float dt)
	{
		// rotate - collision detection not needed
		if (GetKey(SAPP_KEYCODE_D).bHeld) { fPlayerA_deg += SPEED_ROTATE * dt; if (fPlayerA_deg >= 360.0f) fPlayerA_deg -= 360.0f; }
		if (GetKey(SAPP_KEYCODE_A).bHeld) { fPlayerA_deg -= SPEED_ROTATE * dt; if (fPlayerA_deg < 0.0f) fPlayerA_deg += 360.0f; }

		float fNewX = fPlayerX;
		float fNewY = fPlayerY;

		// walk forward - CD checked
		if (GetKey(SAPP_KEYCODE_W).bHeld) {
			fNewX += cos(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * dt;
			fNewY += sin(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * dt;
		}
		// walk backwards - CD checked
		if (GetKey(SAPP_KEYCODE_S).bHeld) {
			fNewX -= cos(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * dt;
			fNewY -= sin(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * dt;
		}
		// strafe left - CD checked
		if (GetKey(SAPP_KEYCODE_Q).bHeld) {
			fNewX += sin(fPlayerA_deg * PI / 180.0f) * SPEED_STRAFE * dt;
			fNewY -= cos(fPlayerA_deg * PI / 180.0f) * SPEED_STRAFE * dt;
		}
		// strafe right - CD checked
		if (GetKey(SAPP_KEYCODE_E).bHeld) {
			fNewX -= sin(fPlayerA_deg * PI / 180.0f) * SPEED_STRAFE * dt;
			fNewY += cos(fPlayerA_deg * PI / 180.0f) * SPEED_STRAFE * dt;
		}
		// collision detection - check if out of bounds or inside occupied tile
		// only update position if no collision
		if (fNewX >= 0 && fNewX < nMapX &&
			fNewY >= 0 && fNewY < nMapY &&
			sMap[int(fNewY) * nMapX + int(fNewX)] != '#') {
			fPlayerX = fNewX;
			fPlayerY = fNewY;
		}


		//exit
		if (GetKey(SAPP_KEYCODE_ESCAPE).bPressed) sapp_request_quit();
	}

	void userUpdate(float dt)
	{
		handleUserInput(dt);
	}

	void userRender()
	{


	}

	void frame()
	{
		//deltaTime
		const float dt = sapp_frame_duration();

		userUpdate(dt);

		userRender();

		//update prev key values
	    std:memcpy(_keys_old, _keys_curr, sizeof(bool) * NUM_KEYS);

	}

};



#endif // !GAME_H


