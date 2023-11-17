#include <stdio.h>
#include "render/base.h"
#include "game/terrain.h"
#include "more_math.h"
#include "ticker.h"
#include "controls.h"
#include "audio.h"

// TEST
#include "ui/label.h"
#include "ui/textbox.h"
#include "ui/button.h"
#include "ui/scrollbar.h"
#include "ui/listbox.h"

const float cam_scroll_speed_keys = 6; // per second
const float cam_scroll_speed_mouse = 10; // per second
const float cam_zoom_speed = 0.2; // per wheel scroll
const float cam_rotate_speed = 25; // per screen delta (-1; 1)
static vec3f cam_pos_change = {0, 0, 0}; // x - forward/backward, z - left/right, y - zoom in/out

static void camera_kb_down(key_code key, int mods)
{
	if(check_key_bind(key, "camera_up"))
		cam_pos_change.x += cam_scroll_speed_keys;
	else if(check_key_bind(key, "camera_down"))
		cam_pos_change.x -= cam_scroll_speed_keys;
	else if(check_key_bind(key, "camera_left"))
		cam_pos_change.z += cam_scroll_speed_keys;
	else if(check_key_bind(key, "camera_right"))
		cam_pos_change.z -= cam_scroll_speed_keys;
	else if(check_key_bind(key, "camera_zoom_in"))
		render_cam_pos.y += cam_zoom_speed;
	else if(check_key_bind(key, "camera_zoom_out"))
		render_cam_pos.y -= cam_zoom_speed;

}
static void camera_kb_up(key_code key, int mods)
{
	if(check_key_bind(key, "camera_up"))
		cam_pos_change.x -= cam_scroll_speed_keys;
	else if(check_key_bind(key, "camera_down"))
		cam_pos_change.x += cam_scroll_speed_keys;
	else if(check_key_bind(key, "camera_left"))
		cam_pos_change.z -= cam_scroll_speed_keys;
	else if(check_key_bind(key, "camera_right"))
		cam_pos_change.z += cam_scroll_speed_keys;
}

static void camera_mouse_move(float x, float y)
{
	static float prev_x = INFINITY, prev_y = INFINITY;
	if(isinf(prev_x)){
		prev_x = x; prev_y = y;
	}

	if(check_held_key_bind("camera_rotate")){
		render_cam_rot.x -= (y - prev_y) * cam_rotate_speed;
		render_cam_rot.y += (x - prev_x) * cam_rotate_speed;
	}

	prev_x = x; prev_y = y;
}

void camera_ui_tick(unsigned ms)
{
	//printf("%f %f %f\n", render_cam_pos.x, render_cam_pos.y, render_cam_pos.z);
	//printf("%f %f %f\n", render_cam_rot.x, render_cam_rot.y, render_cam_rot.z);

	float s_frac = ms / 1000.; // fractions of a second
	vec3f ang = {render_cam_rot.x, render_cam_rot.y, render_cam_rot.z};

	render_cam_pos.x += cam_pos_change.x * s_frac * cos(ang_to_rad(ang.y + 90));
	render_cam_pos.z += cam_pos_change.x * s_frac * sin(ang_to_rad(ang.y + 90));
	
	render_cam_pos.x += cam_pos_change.z * s_frac * cos(ang_to_rad(ang.y));
	render_cam_pos.z += cam_pos_change.z * s_frac * sin(ang_to_rad(ang.y));
}

int main()
{
	audio_init();
	resources_init();
	render_init();
	tickers_init();

	controls_init("ctl_cfg.lon");
	ui_init();

	controls_add_key_down(camera_kb_down);
	controls_add_key_up(camera_kb_up);
	controls_add_mouse_move(camera_mouse_move);

	resources_scan();
	
	button bt; button_create(&bt, (vec2f){0, 1}, (vec2f){0.4, 0.06}, (vec2f){0.05, 0.05},
					atlas_texture_find("button_l"), atlas_texture_find("button_r"), atlas_texture_find("button_m"),
					atlas_texture_find("button_l_pressed"), atlas_texture_find("button_r_pressed"), atlas_texture_find("button_m_pressed"),
					font_find("courbd"), "нажми меня");
	textbox tb; textbox_create(&tb, (vec2f){0, 1 - 0.07}, (vec2f){0.4, 0.06}, (vec2f){0.05, 0.05},
					atlas_texture_find("textbox_l"), atlas_texture_find("textbox_r"), atlas_texture_find("textbox_m"),
					font_find("courbd"));
	scrollbar sb; scrollbar_create(&sb, (vec2f){0.0, 0.0}, (vec2f){0.05, 1.0},
					atlas_texture_find("scrollbar_u"), atlas_texture_find("scrollbar_d"), atlas_texture_find("scrollbar_m"), atlas_texture_find("scrollbar_s"),
					0, 10);

	label l; label_create(&l, (vec2f){0, 0}, (vec2f){0.05, 0.05}, font_find("courbd"), "Совсем непонятно, что делать. Кому-то хочется плакать, кому-то хочется стучать... К ВАМ. ЗАЧЕМ вы славите ДЪЯВОЛА? Каждая травинка, и листочек, и каждое животное... оно хвалит БОГА. Молодёжь наша, юноши и девушки. ЗАЧЕМ вы славите ДЪЯВОЛА? Это ЕГО музыка. Это ОНА звучит в АДУ. Именно под эту музыку... истязают грешников.");
	label_set_color(&l, (vec3f){0.5, 0.5, 0.5});
	label_set_wrap(&l, WRAP_WORD, 0.65);

	listbox lb; listbox_create(&lb, (vec2f){0.1, 0.1}, (vec2f){0.8, 0.8}, (vec2f){0.05, 0.05},
					atlas_texture_find("listbox_lt"), atlas_texture_find("listbox_rt"), atlas_texture_find("listbox_lb"), atlas_texture_find("listbox_rb"),
					atlas_texture_find("listbox_u"), atlas_texture_find("listbox_d"), atlas_texture_find("listbox_l"), atlas_texture_find("listbox_r"),
					atlas_texture_find("listbox_m"));
	listbox_set_scrollbar(&lb, &sb);
	label_set_in_listbox(&l, 1);
	ui_element_add_child((ui_element*)&lb, (ui_element*)&l);
	ui_element_add_child((ui_element*)&lb, (ui_element*)&bt);
	ui_element_add_child((ui_element*)&lb, (ui_element*)&tb);
	ui_add_element((ui_element*)&lb);
	ui_add_element((ui_element*)&sb);

	audio_play_sound(sound_find("its a jhonny"));

	ui_ticker_add(camera_ui_tick);

	terrain_init();
	controls_loop();
	render_loop();
}
