#include <stdio.h>
#include "render/base.h"
#include "game/terrain.h"
#include "more_math.h"
#include "ticker.h"
#include "controls.h"
#include "audio.h"

#include "ui/elements/selection.h"

int main()
{
	audio_init();
	resources_init();
	render_init();
	tickers_init();

	controls_init("ctl_cfg.lon");
	resources_scan();
	ui_init();

	selection sel;
	selection_create(&sel, 
		(struct ui_full_box_textures){
		atlas_texture_find("selbox_lt"), atlas_texture_find("selbox_rt"), atlas_texture_find("selbox_lb"), atlas_texture_find("selbox_rb"), atlas_texture_find("selbox_u"), atlas_texture_find("selbox_d"), atlas_texture_find("selbox_l"), atlas_texture_find("selbox_r"), atlas_texture_find("selbox_m")
		});
	ui_add_element((ui_element*)&sel);

	game_logic_init();
	//audio_play_sound(sound_find("its a jhonny"));

	terrain_init();
	controls_loop();
	render_loop();
}
