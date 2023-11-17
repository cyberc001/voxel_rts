#ifndef CONTROLS_H
#define CONTROLS_H

#include "htable_oa.h"
#include "more_math.h"
#include "render/base.h"
#include "utf.h"

typedef enum {
	KEY_INVALID = 0,

	KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_MINUS, KEY_EQUALS,
	KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,

	KEY_SLASH, KEY_ENTER, KEY_BACKSPACE, KEY_DELETE, KEY_TAB, KEY_TILDA, KEY_ESCAPE, KEY_SPACE,

	KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
	KEY_LEFT, KEY_UP, KEY_RIGHT, KEY_DOWN,
	KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_HOME, KEY_END, KEY_INSERT,

	MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE, MOUSE_WHEEL_UP, MOUSE_WHEEL_DOWN,
	
	KEY_CODE_MAX // maximum value of key_code type
} key_code;
#define key_code_is_mouse(kc) ((kc) == MOUSE_LEFT || (kc) == (MOUSE_RIGHT) || (kc) == (MOUSE_MIDDLE))

DEF_HTABLE_OA(controls_map, key_code, char**) // maps a key code to one or more binds, array of binds ends with a NULL string
extern controls_map map_controls;

const char** get_key_binds(key_code kc);
int check_key_bind(key_code kc, const char* bind);
int check_held_key_bind(const char* bind);
vec2f get_mouse_coords();

/* Initializes control scheme from a .lon config */
void controls_init(const char* cfg_fname);
void controls_loop();

/* Adds callback to a list of function pointers */
typedef void (*controls_key_down_cb_t)(key_code key, int mods);
typedef void (*controls_key_up_cb_t)(key_code key, int mods);
typedef void (*controls_key_char_cb_t)(utf_char c);
typedef void (*controls_mouse_move_cb_t)(float x, float y);

void* controls_add_key_down(controls_key_down_cb_t cb);
void* controls_add_key_up(controls_key_up_cb_t cb);
void* controls_add_key_char(controls_key_char_cb_t cb);
void* controls_add_mouse_move(controls_mouse_move_cb_t cb);

/* Misc info */
unsigned char controls_is_key_held(key_code key); // returns 0 if key isn't held, otherwise returns a value > 0 corresponding to the order in which key has been pressed relatively to other keys currently pressed
key_code key_name_to_code(const char* key_name);

#endif
