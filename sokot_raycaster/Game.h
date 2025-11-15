#define SOKOL_IMPL
#define SOKOL_GLCORE
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_gp.h"
#include "sokol/sokol_app.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_log.h"
#include "sokol/Pixel.h"
#include <string>
#include <vector>
#include <iostream>

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
	unsigned int pixel_count = 0;
	float fDistToProjPlane;
	std::vector<sgp_vec2> pixel_coord_buffer;
	std::vector<Pixel> pixel_buffer;
	std::vector<sgp_vertex> vertex_buffer;

	int increaseX = 100;
	int increasey = 100;

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

		sgp_desc sgpdesc;  zeroMem(sgpdesc);
		sgp_setup(&sgpdesc);
		if (!sgp_is_valid()) {
			fprintf(stderr, "Failed to create Sokol GP context: %s\n", sgp_get_error_message(sgp_get_last_error()));
			exit(-1);
		}
		

		userInit();
		std::memset(_keys_old, false, sizeof(bool) * NUM_KEYS);
		std::memset(_keys_curr, false, sizeof(bool) * NUM_KEYS);
		std::memset(_keys_new, false, sizeof(bool) * NUM_KEYS);

	}

	void cleanup() {
		sgp_shutdown();
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
		//if (GetKey(SAPP_KEYCODE_W).bHeld) {
		//	fNewX += cos(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * dt;
		//	fNewY += sin(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * dt;
		//}
		//// walk backwards - CD checked
		//if (GetKey(SAPP_KEYCODE_S).bHeld) {
		//	fNewX -= cos(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * dt;
		//	fNewY -= sin(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * dt;
		//}
		//// strafe left - CD checked
		//if (GetKey(SAPP_KEYCODE_Q).bHeld) {
		//	fNewX += sin(fPlayerA_deg * PI / 180.0f) * SPEED_STRAFE * dt;
		//	fNewY -= cos(fPlayerA_deg * PI / 180.0f) * SPEED_STRAFE * dt;
		//}
		//// strafe right - CD checked
		//if (GetKey(SAPP_KEYCODE_E).bHeld) {
		//	fNewX -= sin(fPlayerA_deg * PI / 180.0f) * SPEED_STRAFE * dt;
		//	fNewY += cos(fPlayerA_deg * PI / 180.0f) * SPEED_STRAFE * dt;
		//}
		// collision detection - check if out of bounds or inside occupied tile
		// only update position if no collision
		if (fNewX >= 0 && fNewX < nMapX &&
			fNewY >= 0 && fNewY < nMapY &&
			sMap[int(fNewY) * nMapX + int(fNewX)] != '#') {
			fPlayerX = fNewX;
			fPlayerY = fNewY;
		}

		//size increase test
		if (GetKey(SAPP_KEYCODE_W).bHeld) {
			increasey += 1;
		}
		if (GetKey(SAPP_KEYCODE_DOWN).bHeld) {
			increasey -= 1;
		}
		if (GetKey(SAPP_KEYCODE_LEFT).bHeld) {
			increaseX -= 1;
		}
		if (GetKey(SAPP_KEYCODE_RIGHT).bHeld) {
			increaseX += 1;
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
		vertex_buffer.clear();
		int width = sapp_width(), height = sapp_height();
		sgp_begin(width, height);
		// draw background
		sgp_set_color(0.05f, 0.05f, 0.05f, 1.0f);
		sgp_clear();
		sgp_reset_color();

		sgp_viewport(0, 0, width, height);
		sgp_push_transform();
		for (int y = 0; y < increasey; y++)
		{
			for (int x = 0; x < increaseX; x++)
			{
				Pixel p = CYAN;
				if (x < 50 && y < 50)
				{
					p = GREEN;
				}
				if (x > 100 && y > 100)
				{
					p = BLUE;
				}
				
				set_pixel(x, y, p);
			}
		}
		draw_pixels();
		sgp_pop_transform();
		

		sg_pass pass; zeroMem(pass);
		pass.swapchain = sglue_swapchain();
		sg_begin_pass(&pass);
		sgp_flush();
		sgp_end();
		sg_end_pass();
		sg_commit();


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


	//raycast test

	 void set_pixel(int coordx, int coordy, Pixel p)
	{ 
		 sgp_vertex tmp;
		 
		 sgp_vec2 pos = { coordx,coordy };
		 sgp_color_ub4 pix = {p.r, p.g, p.b, 255};
		 tmp.position  = pos;
		 tmp.color = pix;
		 vertex_buffer.push_back(tmp);
		
	}

	 void draw_pixels()
	 {
		
		sgp_vector_draw(SG_PRIMITIVETYPE_POINTS, vertex_buffer, vertex_buffer.size());

	 }

	

	void RenderWall()
	{
		int ScreenWidth = sapp_width(), ScreenHeight = sapp_height();
		int nHalfScreenWidth = ScreenWidth / 2;
		float fAngleStep = fPlayerFoV_deg / float(ScreenWidth);

		for (int x = 0; x < ScreenWidth; x++)
		{
			float fViewAngle = float(x - nHalfScreenWidth) * fAngleStep;
			float fCurAngle = fPlayerA_deg + fViewAngle;

			float fRawDistToWall, fCorrectDistToWall;
			float fX_hit, fY_hit;
			int nX_hit, nY_hit;

			int nWallCeil, nWallFloor;

			float fLighting = 1.0f;

			if (GetDistanceToWall(fCurAngle, fX_hit, fY_hit, fRawDistToWall, nX_hit, nY_hit))
			{

				auto my_float_equal = [=](float a, float b)-> bool
					{
						float fEpilon = 0.00001f;
						return abs(a - b) < fEpilon;
					};

				if (my_float_equal(fX_hit, float(nX_hit))) { fLighting = 1.0f; }
				else    // west  face was hit
					if (my_float_equal(fY_hit, float(nY_hit))) { fLighting = 0.8f; }
					else    // north
						if (my_float_equal(fX_hit, float(nX_hit + 1))) { fLighting = 0.6f; }
						else    // east
							if (my_float_equal(fY_hit, float(nY_hit + 1))) { fLighting = 0.4f; }
							else {  // south
								std::cout << "ERROR: OnUserUpdate() --> this situation should not occur" << std::endl;
								fLighting = 0.2f;
							}

				fCorrectDistToWall = fRawDistToWall * cos(fViewAngle * PI / 180.0f);
				int nSliceHeight = int((1.0f / fCorrectDistToWall) * fDistToProjPlane);
				nWallCeil = (ScreenHeight / 2.0f) - (nSliceHeight / 2.0f);
				nWallFloor = (ScreenHeight / 2.0f) + (nSliceHeight / 2.0f);
			}
			else {
				// no wall was hit - set bottom and top value for wall at the horizon
				nWallCeil = ScreenHeight / 2;
				nWallFloor = ScreenHeight / 2;
			}
				
			for (int y = 0; y < ScreenHeight; y++) {
				if (y < nWallCeil) {
					set_pixel(x, y, COL_CEIL);                    // render ceiling
				}
				else if (y > nWallFloor) {
					set_pixel (x, y, COL_FLOOR);                   // render floor
				}
				else {
					
					set_pixel(x, y, COL_WALL);        // render wall
				}
			}
		}
		
	}

	bool GetDistanceToWall(float fRayAngle, float& fHitX, float& fHitY, float& fDistIfFound, int& nMapCoordX, int& nMapCoordY) {

		// The player's position is the "from point"
		float fFromX = fPlayerX;
		float fFromY = fPlayerY;
		// Calculate the "to point" using it's angle and fMaxDistance
		float fToX = fPlayerX + fMaxDistance * cos(fRayAngle * PI / 180.0f);
		float fToY = fPlayerY + fMaxDistance * sin(fRayAngle * PI / 180.0f);
		// work out the direction vector (fDX, fDY) and normalize it
		float fDX = fToX - fFromX;
		float fDY = fToY - fFromY;
		float fRayLen = sqrt(fDX * fDX + fDY * fDY);
		fDX /= fRayLen;
		fDY /= fRayLen;

		// calculate the scaling factors for the ray increments per unit in x resp y direction
		// this calculation takes division by 0.0f into account
		float fSX = (fDX == 0.0f) ? FLT_MAX : sqrt(1.0f + (fDY / fDX) * (fDY / fDX));
		float fSY = (fDY == 0.0f) ? FLT_MAX : sqrt(1.0f + (fDX / fDY) * (fDX / fDY));

		float fLengthPartialRayX = 0.0f;
		float fLengthPartialRayY = 0.0f;
		// work out if line is going right or left resp. down or up
		int nGridStepX = (fDX > 0.0f) ? +1 : -1;
		int nGridStepY = (fDY > 0.0f) ? +1 : -1;
		int nCurX = int(fFromX);
		int nCurY = int(fFromY);

		// work out the first intersections with the grid
		if (nGridStepX < 0) { // ray is going left - get scaled difference between start point and left cell border
			fLengthPartialRayX = (fFromX - float(nCurX)) * fSX;
		}
		else {              // ray is going right - get scaled difference between right cell border and start point
			fLengthPartialRayX = (float(nCurX + 1.0f) - fFromX) * fSX;
		}
		if (nGridStepY < 0) { // ray is going up - get scaled difference between start point and top cell border
			fLengthPartialRayY = (fFromY - float(nCurY)) * fSY;
		}
		else {              // ray is going down - get scaled difference between bottom cell border and start point
			fLengthPartialRayY = (float(nCurY + 1.0f) - fFromY) * fSY;
		}

		// did analysis get out of map boundaries?
		bool bOutOfBounds = (nCurX < 0 || nCurX >= nMapX ||
			nCurY < 0 || nCurY >= nMapY);
		// was a hit with a wall cell found?
		bool bHitFound = bOutOfBounds ? false : sMap[nCurY * nMapX + nCurX] != '.';
		// did analysis reach the destination cell?
		bool bDestCellFound = (nCurX == int(fToX) && nCurY == int(fToY));

		fDistIfFound = 0.0f;
		while (!bOutOfBounds && !bHitFound && !bDestCellFound && fDistIfFound < fMaxDistance) {

			// advance to next map cell, depending on length of partial ray's
			if (fLengthPartialRayX < fLengthPartialRayY) {
				// move in x direction
				nCurX += nGridStepX;
				fDistIfFound = fLengthPartialRayX;
				fLengthPartialRayX += fSX;
			}
			else {
				// move in y direction
				nCurY += nGridStepY;
				fDistIfFound = fLengthPartialRayY;
				fLengthPartialRayY += fSY;
			}

			bOutOfBounds = (nCurX < 0 || nCurX >= nMapX ||
				nCurY < 0 || nCurY >= nMapY);
			bHitFound = bOutOfBounds ? false : sMap[nCurY * nMapX + nCurX] != '.';
			bDestCellFound = (nCurX == int(fToX) && nCurY == int(fToY));
		}

		if (bHitFound) {
			// return the correct values - note that fDistIfFound already has the correct value
			fHitX = fFromX + fDistIfFound * fDX;
			fHitY = fFromY + fDistIfFound * fDY;
			nMapCoordX = nCurX;
			nMapCoordY = nCurY;
		}
		else {
			// make sure ref. variables don't suggest meaningful values
			fHitX = -1.0f;
			fHitY = -1.0f;
			nMapCoordX = -1;
			nMapCoordY = -1;
			fDistIfFound = 0.0f;
		}

		return bHitFound;
	}

	

};






