#include <stdio.h>
#include "render/base.h"
#include "game/terrain.h"
#include "more_math.h"
#include "ticker.h"
#include "controls.h"
#include "audio.h"

int main()
{
	audio_init();
	resources_init();
	render_init();
	tickers_init();

	controls_init("ctl_cfg.lon");
	ui_init();

	resources_scan();
	game_logic_init();

	//audio_play_sound(sound_find("its a jhonny"));

	terrain_init();
	controls_loop();
	render_loop();
}
