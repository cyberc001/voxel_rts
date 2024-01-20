#include "controls.h"
#include <GL/glut.h>
#include <stdio.h>
#include "log.h"
#include "formats/lon.h"
#include "linked_list.h"

static size_t str_hash(size_t table_sz, const char** key)
{
	const char* i= *key;
	size_t h = 0;
	for(; *i; ++i)
		h = ((h + *i) % table_sz * 256) % table_sz;
	return h;
}
static int str_cmp(const char** s1, const char** s2)
{
	return strcmp(*s1, *s2);
}
static size_t int_hash(size_t table_sz, const int* key)
{
	return (*key) % table_sz;
}

controls_map map_controls;

const char** get_key_binds(key_code kc)
{
	char*** binds = controls_map_find(&map_controls, kc);
	return binds ? (const char**)*binds : NULL;
}
int check_key_bind(key_code kc, const char* bind)
{
	const char** binds = get_key_binds(kc);
	if(!binds)
		return 0;
	for(; *binds; ++binds)
		if(!strcmp(*binds, bind))
			return 1;
	return 0;
}

// mapping GLFW keycodes to engine's keycodes
DEF_PHTABLE_OA(glfw_kc_map, int, key_code)
glfw_kc_map map_glfw_kc;

// memorizing held keys and buttons
static unsigned char held_map[KEY_CODE_MAX];
static unsigned held_amt = 0;
static vec2f mouse_coords;

// memorizing held binds
DEF_HTABLE_OA(held_bind_map, const char*, unsigned char)
held_bind_map map_held_bind;

int check_held_key_bind(const char* bind)
{
	return !!(held_bind_map_find(&map_held_bind, bind));
}
vec2f get_mouse_coords()
{
	return mouse_coords;
}

unsigned char controls_is_key_held(key_code key)
{
	return held_map[key];
}

// translating between key names and key codes
DEF_HTABLE_OA(key_name_map, char*, key_code)
key_name_map map_key_name;

key_code key_name_to_code(const char* key_name)
{
	key_code* kc = key_name_map_find(&map_key_name, key_name);
	return kc ? *kc : KEY_INVALID;
}

/* CALLBACKS */
DEF_LINKED_LIST(controls_key_down_cb_list_t, controls_key_down_cb_t)
controls_key_down_cb_list_t controls_key_down_list;
DEF_LINKED_LIST(controls_key_up_cb_list_t, controls_key_up_cb_t)
controls_key_up_cb_list_t controls_key_up_list;
DEF_LINKED_LIST(controls_key_char_cb_list_t, controls_key_char_cb_t)
controls_key_char_cb_list_t controls_key_char_list;
DEF_LINKED_LIST(controls_mouse_move_cb_list_t, controls_mouse_move_cb_t)
controls_mouse_move_cb_list_t controls_mouse_move_list;

static void controls_cb_kb(GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
	key_code* _kc = glfw_kc_map_find(&map_glfw_kc, key);
	if(!_kc) return;
	key_code kc = *_kc;

	if(action == GLFW_PRESS){
		held_map[kc] = ++held_amt;
		const char** binds = get_key_binds(kc);
		for(; binds && *binds; ++binds)
			if(!held_bind_map_find(&map_held_bind, *binds))
				held_bind_map_insert(&map_held_bind, *binds, 1);

		controls_key_down_cb_list_t_node* n = controls_key_down_list.head.next;
		while(n != &controls_key_down_list.tail){
			n->data(kc, mods);
			n = n->next;
		}
	}
	else if(action == GLFW_RELEASE){
		held_map[kc] = 0; --held_amt;
		const char** binds = get_key_binds(kc);
		for(; binds && *binds; ++binds)
			held_bind_map_delete(&map_held_bind, *binds);

		controls_key_up_cb_list_t_node* n = controls_key_up_list.head.next;
		while(n != &controls_key_up_list.tail){
			n->data(kc, mods);
			n = n->next;
		}
	}
}
static void controls_cb_char(GLFWwindow* wnd, unsigned int codepoint)
{
	controls_key_char_cb_list_t_node* n = controls_key_char_list.head.next;
	while(n != &controls_key_char_list.tail){
		n->data(codepoint);
		n = n->next;
	}
}
#define GLFW_BUTTON_TO_KC(bt) (bt == GLFW_MOUSE_BUTTON_1 ? MOUSE_LEFT\
				: bt == GLFW_MOUSE_BUTTON_2 ? MOUSE_RIGHT\
				: bt == GLFW_MOUSE_BUTTON_3 ? MOUSE_MIDDLE\
				: KEY_INVALID)
static void controls_cb_mouse_button(GLFWwindow* wnd, int button, int action, int mods)
{
	key_code butn = GLFW_BUTTON_TO_KC(button);
	if(action == GLFW_PRESS){
		held_map[butn] = ++held_amt;
		const char** binds = get_key_binds(butn);
		for(; binds && *binds; ++binds)
			if(!held_bind_map_find(&map_held_bind, *binds))
				held_bind_map_insert(&map_held_bind, *binds, 1);

		controls_key_down_cb_list_t_node* n = controls_key_down_list.head.next;
		while(n != &controls_key_down_list.tail){
			n->data(butn, mods);
			n = n->next;
		}
	}
	else if(action == GLFW_RELEASE){
		held_map[butn] = 0; --held_amt;
		const char** binds = get_key_binds(butn);
		for(; binds && *binds; ++binds)
			held_bind_map_delete(&map_held_bind, *binds);

		controls_key_up_cb_list_t_node* n = controls_key_up_list.head.next;
		while(n != &controls_key_up_list.tail){
			n->data(butn, mods);
			n = n->next;
		}
	}
}
static void controls_cb_mouse_move(GLFWwindow* wnd, double x, double y)
{
	int w, h; glfwGetWindowSize(main_wnd, &w, &h);
	x /= w; y /= h;
	y = 1 - y;
	mouse_coords.x = x; mouse_coords.y = y;
	controls_mouse_move_cb_list_t_node* n = controls_mouse_move_list.head.next;
	while(n != &controls_mouse_move_list.tail){
		n->data(x, y);
		n = n->next;
	}
}
void controls_cb_scroll(GLFWwindow* window, double dx, double dy)
{
	key_code butn = dy > 0 ? MOUSE_WHEEL_UP : dy < 0 ? MOUSE_WHEEL_DOWN : KEY_INVALID;
	{
		controls_key_down_cb_list_t_node* n = controls_key_down_list.head.next;
		while(n != &controls_key_down_list.tail){
			n->data(butn, 0);
			n = n->next;
		}
	}
	{
		controls_key_up_cb_list_t_node* n = controls_key_up_list.head.next;
		while(n != &controls_key_up_list.tail){
			n->data(butn, 0);
			n = n->next;
		}
	}
}

void* controls_add_key_down(controls_key_down_cb_t cb)
{
	return controls_key_down_cb_list_t_insert_end(&controls_key_down_list, cb);
}
void* controls_add_key_up(controls_key_up_cb_t cb)
{
	return controls_key_up_cb_list_t_insert_end(&controls_key_up_list, cb);
}
void* controls_add_key_char(controls_key_char_cb_t cb)
{
	return controls_key_char_cb_list_t_insert_end(&controls_key_char_list, cb);
}
void* controls_add_mouse_move(controls_mouse_move_cb_t cb)
{
	return controls_mouse_move_cb_list_t_insert_end(&controls_mouse_move_list, cb);
}

/* INITIALIZATION */
void controls_init(const char* cfg_fname)
{
	memset(held_map, 0, sizeof(held_map));

	// initialize callback lists
	controls_key_down_cb_list_t_create(&controls_key_down_list);
	controls_key_up_cb_list_t_create(&controls_key_up_list);
	controls_mouse_move_cb_list_t_create(&controls_mouse_move_list);
	controls_key_char_cb_list_t_create(&controls_key_char_list);

	// setting up all key mappings
	glfw_kc_map_create(&map_glfw_kc, 512, int_hash);

	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_UNKNOWN, KEY_INVALID);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_0, KEY_0);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_1, KEY_1);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_2, KEY_2);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_3, KEY_3);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_4, KEY_4);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_5, KEY_5);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_6, KEY_6);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_7, KEY_7);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_8, KEY_8);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_9, KEY_9);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_MINUS, KEY_MINUS);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_EQUAL, KEY_EQUALS);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_A, KEY_A);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_B, KEY_B);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_C, KEY_C);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_D, KEY_D);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_E, KEY_E);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_F, KEY_F);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_G, KEY_G);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_H, KEY_H);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_I, KEY_I);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_J, KEY_J);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_K, KEY_K);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_L, KEY_L);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_M, KEY_M);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_N, KEY_N);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_O, KEY_O);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_P, KEY_P);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_Q, KEY_Q);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_R, KEY_R);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_S, KEY_S);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_T, KEY_T);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_U, KEY_U);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_V, KEY_V);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_W, KEY_W);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_X, KEY_X);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_Y, KEY_Y);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_Z, KEY_Z);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_SLASH, KEY_SLASH);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_ENTER, KEY_ENTER);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_BACKSPACE, KEY_BACKSPACE);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_DELETE, KEY_DELETE);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_TAB, KEY_TAB);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_GRAVE_ACCENT, KEY_TILDA);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_ESCAPE, KEY_ESCAPE);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_SPACE, KEY_SPACE);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_F1, KEY_F1);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_F2, KEY_F2);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_F3, KEY_F3);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_F4, KEY_F4);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_F5, KEY_F5);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_F6, KEY_F6);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_F7, KEY_F7);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_F8, KEY_F8);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_F9, KEY_F9);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_F10, KEY_F10);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_F11, KEY_F11);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_F12, KEY_F12);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_LEFT, KEY_LEFT);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_RIGHT, KEY_RIGHT);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_UP, KEY_UP);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_DOWN, KEY_DOWN);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_PAGE_UP, KEY_PAGE_UP);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_PAGE_DOWN, KEY_PAGE_DOWN);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_HOME, KEY_HOME);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_END, KEY_END);
	glfw_kc_map_insert(&map_glfw_kc, GLFW_KEY_DOWN, KEY_DOWN);

	// init config key names mappings
	key_name_map_create(&map_key_name, 96, str_hash, str_cmp);
	key_name_map_insert(&map_key_name, "0", KEY_0);
	key_name_map_insert(&map_key_name, "1", KEY_1);
	key_name_map_insert(&map_key_name, "2", KEY_2);
	key_name_map_insert(&map_key_name, "3", KEY_3);
	key_name_map_insert(&map_key_name, "4", KEY_4);
	key_name_map_insert(&map_key_name, "5", KEY_5);
	key_name_map_insert(&map_key_name, "6", KEY_6);
	key_name_map_insert(&map_key_name, "7", KEY_7);
	key_name_map_insert(&map_key_name, "8", KEY_8);
	key_name_map_insert(&map_key_name, "9", KEY_9);

	key_name_map_insert(&map_key_name, "a", KEY_A);
	key_name_map_insert(&map_key_name, "b", KEY_B);
	key_name_map_insert(&map_key_name, "c", KEY_C);
	key_name_map_insert(&map_key_name, "d", KEY_D);
	key_name_map_insert(&map_key_name, "e", KEY_E);
	key_name_map_insert(&map_key_name, "f", KEY_F);
	key_name_map_insert(&map_key_name, "g", KEY_G);
	key_name_map_insert(&map_key_name, "h", KEY_H);
	key_name_map_insert(&map_key_name, "i", KEY_I);
	key_name_map_insert(&map_key_name, "j", KEY_J);
	key_name_map_insert(&map_key_name, "k", KEY_K);
	key_name_map_insert(&map_key_name, "l", KEY_L);
	key_name_map_insert(&map_key_name, "m", KEY_M);
	key_name_map_insert(&map_key_name, "n", KEY_N);
	key_name_map_insert(&map_key_name, "o", KEY_O);
	key_name_map_insert(&map_key_name, "p", KEY_P);
	key_name_map_insert(&map_key_name, "q", KEY_Q);
	key_name_map_insert(&map_key_name, "r", KEY_R);
	key_name_map_insert(&map_key_name, "s", KEY_S);
	key_name_map_insert(&map_key_name, "t", KEY_T);
	key_name_map_insert(&map_key_name, "u", KEY_U);
	key_name_map_insert(&map_key_name, "v", KEY_V);
	key_name_map_insert(&map_key_name, "w", KEY_W);
	key_name_map_insert(&map_key_name, "x", KEY_X);
	key_name_map_insert(&map_key_name, "y", KEY_Y);
	key_name_map_insert(&map_key_name, "z", KEY_Z);

	key_name_map_insert(&map_key_name, "slash", KEY_SLASH);
	key_name_map_insert(&map_key_name, "enter", KEY_ENTER);
	key_name_map_insert(&map_key_name, "backspace", KEY_BACKSPACE); key_name_map_insert(&map_key_name, "bspace", KEY_BACKSPACE);
	key_name_map_insert(&map_key_name, "delete", KEY_DELETE); key_name_map_insert(&map_key_name, "del", KEY_DELETE);
	key_name_map_insert(&map_key_name, "tab", KEY_TAB);
	key_name_map_insert(&map_key_name, "~", KEY_TILDA);
	key_name_map_insert(&map_key_name, "escape", KEY_ESCAPE); key_name_map_insert(&map_key_name, "esc", KEY_ESCAPE);
	key_name_map_insert(&map_key_name, "space", KEY_SPACE);

	key_name_map_insert(&map_key_name, "f1", KEY_F1);
	key_name_map_insert(&map_key_name, "f2", KEY_F2);
	key_name_map_insert(&map_key_name, "f3", KEY_F3);
	key_name_map_insert(&map_key_name, "f4", KEY_F4);
	key_name_map_insert(&map_key_name, "f5", KEY_F5);
	key_name_map_insert(&map_key_name, "f6", KEY_F6);
	key_name_map_insert(&map_key_name, "f7", KEY_F7);
	key_name_map_insert(&map_key_name, "f8", KEY_F8);
	key_name_map_insert(&map_key_name, "f9", KEY_F9);
	key_name_map_insert(&map_key_name, "f10", KEY_F10);
	key_name_map_insert(&map_key_name, "f11", KEY_F11);
	key_name_map_insert(&map_key_name, "f12", KEY_F12);

	key_name_map_insert(&map_key_name, "left", KEY_LEFT);
	key_name_map_insert(&map_key_name, "up", KEY_UP);
	key_name_map_insert(&map_key_name, "right", KEY_RIGHT);
	key_name_map_insert(&map_key_name, "down", KEY_DOWN);

	key_name_map_insert(&map_key_name, "pageup", KEY_PAGE_UP); key_name_map_insert(&map_key_name, "page_up", KEY_PAGE_UP);
	key_name_map_insert(&map_key_name, "pagedown", KEY_PAGE_DOWN);  key_name_map_insert(&map_key_name, "page_down", KEY_PAGE_DOWN);
	key_name_map_insert(&map_key_name, "home", KEY_HOME); 
	key_name_map_insert(&map_key_name, "end", KEY_END);
	key_name_map_insert(&map_key_name, "insert", KEY_INSERT); key_name_map_insert(&map_key_name, "ins", KEY_INSERT);

	key_name_map_insert(&map_key_name, "mouse1", MOUSE_LEFT); key_name_map_insert(&map_key_name, "mouse_left", MOUSE_LEFT);
	key_name_map_insert(&map_key_name, "mouse2", MOUSE_RIGHT); key_name_map_insert(&map_key_name, "mouse_right", MOUSE_RIGHT);
	key_name_map_insert(&map_key_name, "mouse3", MOUSE_MIDDLE); key_name_map_insert(&map_key_name, "mouse_middle", MOUSE_MIDDLE);
	key_name_map_insert(&map_key_name, "mouse_up", MOUSE_WHEEL_UP); key_name_map_insert(&map_key_name, "mouse_wheel_up", MOUSE_WHEEL_UP);
	key_name_map_insert(&map_key_name, "mouse_down", MOUSE_WHEEL_DOWN); key_name_map_insert(&map_key_name, "mouse_wheel_down", MOUSE_WHEEL_DOWN);

	held_bind_map_create(&map_held_bind, 32, str_hash, str_cmp);

	// reading config file
	controls_map_create(&map_controls, 32, (size_t(*)(size_t, const key_code*)) int_hash);
	FILE* fd = fopen(cfg_fname, "r");
	if(!fd){
		LOG_ERROR("Cannot open control config file \"%s\" for reading", cfg_fname);
		return;
	}
	lon_array* arr = read_lon(fd);
	fclose(fd);
	if(arr->data[0]->type != LON_OBJECT_TYPE_DICT)
		LOG_ERROR("Control configuration is not a dictionary");
	else{
		lon_dict* cfg = (lon_dict*)arr->data[0];
		for(size_t i = 0; i < cfg->data.size; ++i)
			if(lon_htable_is_allocated(&cfg->data, i)){
				char* bind_name = cfg->data.keys[i];
				lon_object* _key = cfg->data.data[i];
				if(_key->type != LON_OBJECT_TYPE_STRING)
					LOG_ERROR("Key name for bind \%s is not a string", bind_name);
				else{
					char* key = ((lon_string*)_key)->value;
					key_code kc = key_name_to_code(key);
					if(kc == KEY_INVALID)
						LOG_ERROR("Key name \"%s\" is not recognized", key);
					else{
						char*** bind_arr = controls_map_find(&map_controls, kc);
						if(bind_arr){
							size_t bind_arr_sz = 0;
							for(; (*bind_arr)[bind_arr_sz]; ++bind_arr_sz)
								;
							bind_arr_sz += 2;
							*bind_arr = realloc(*bind_arr, sizeof(char*) * bind_arr_sz);
							(*bind_arr)[bind_arr_sz - 2] = strdup(bind_name);
							(*bind_arr)[bind_arr_sz - 1] = NULL;
						}
						else{
							char** bind_arr = malloc(sizeof(char*) * 2);
							bind_arr[0] = strdup(bind_name);
							bind_arr[1] = NULL;
							controls_map_insert(&map_controls, kc, bind_arr);
						}
					}
				}
			}
	}
	lon_object_free((lon_object*)arr);

	// setting up glfw callbacks
	glfwSetKeyCallback(main_wnd, controls_cb_kb);
	glfwSetCharCallback(main_wnd, controls_cb_char);
	glfwSetMouseButtonCallback(main_wnd, controls_cb_mouse_button);
	glfwSetCursorPosCallback(main_wnd, controls_cb_mouse_move);
	glfwSetScrollCallback(main_wnd, controls_cb_scroll);
}

static pthread_t event_thr;
static void __controls_loop()
{
	while(!glfwWindowShouldClose(main_wnd))
		glfwPollEvents();
}
void controls_loop()
{
	pthread_create(&event_thr, NULL, __controls_loop, NULL);
}
