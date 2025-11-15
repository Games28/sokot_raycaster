#include "Game.h"

static Game* game_ptr = nullptr;

static void init_cb() { game_ptr->init(); }
static void cleanup_cb() { game_ptr->cleanup(); }
static void frame_cb() { game_ptr->frame(); }
static void input_cb(const sapp_event* e) { game_ptr->input(e); }

sapp_desc sokol_main(int argc, char* argv[])
{
	
	static Game game;
	game_ptr = &game;

	sapp_desc app_desc; zeroMem(app_desc);
	app_desc.init_cb = init_cb;
	app_desc.cleanup_cb = cleanup_cb;
	app_desc.frame_cb = frame_cb;
	app_desc.event_cb = input_cb;
	app_desc.width = WINDOW_WDITH;
	app_desc.height = WINDOW_HEIGHT;
	app_desc.icon.sokol_default = true;
	app_desc.sample_count = 4;

	return app_desc;

}