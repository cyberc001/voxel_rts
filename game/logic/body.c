#include "game/logic/body.h"
#include "dyn_array.h"
#include "render/list.h"

/* Helper functions */

void* lua_create_body(lua_State* L, int type, size_t size)
{
	body* body_ptr = lua_newuserdata(L, size);
	body_init(body_ptr, type, size);
	luaL_setmetatable(L, "body");
	return body_ptr;
}
void* lua_push_body(lua_State* L, const body* b)
{
	body* body_ptr = lua_newuserdata(L, b->size);
	*body_ptr = *b;
	luaL_setmetatable(L, "body");
	return body_ptr;
}

void* lua_get_body(lua_State* L, int index)
{
	return luaL_checkudata(L, index, "body");
}

body_box* lua_push_body_box(lua_State* L, vec3f _size)
{
	body_box* b = lua_create_body(L, LUA_BODY_TYPE_BOX, sizeof(body_box));
	b->_size = _size;
	return b;
}
body_hexahedron* lua_push_body_hexahedron(lua_State* L, const hexahedron* h)
{
	body_hexahedron* b = lua_create_body(L, LUA_BODY_TYPE_HEXAHEDRON, sizeof(body_hexahedron));
	b->geom = *h;
	return b;
}

/* body API */

static int lua_new_cuboid(lua_State* L)
{
	vec3f _size;
	if(lua_type(L, 2) == LUA_TNUMBER){
		_size = (vec3f){
			luaL_checknumber(L, 1),
			luaL_checknumber(L, 2),
			luaL_checknumber(L, 3)
		};
	} else {
		float side = luaL_checknumber(L, 1);
		_size = (vec3f){side, side, side};
	}
	lua_push_body_box(L, _size);
	return 1;
}

/* body methods */

static int lua_body_index(lua_State* L)
{
	body* b = lua_get_body(L, 1);
	if(lua_type(L, 2) == LUA_TSTRING){
		const char* arg = lua_tostring(L, 2);

		if(!strcmp(arg, "pos")){
			lua_push_vec3(L, b->transform.pos);
			return 1;
		} else if(!strcmp(arg, "rot")){
			lua_push_vec4(L, b->transform.rot);
			return 1;
		} else if(!strcmp(arg, "scale")){
			lua_push_vec3(L, b->transform.scale);
			return 1;
		}

		luaL_getmetatable(L, "body");
		lua_getfield(L, -1, lua_tostring(L, 2));
		return 1;
	} else
		return luaL_error(L, "String expected, got %s", lua_typename(L, lua_type(L, 2)));
}
static int lua_body_newindex(lua_State* L)
{	
	body* b = lua_get_body(L, 1);
	if(lua_type(L, 2) == LUA_TSTRING){
		const char* arg = lua_tostring(L, 2);

		if(!strcmp(arg, "pos")){
			b->transform.pos = lua_get_vec3(L, 3);
			b->transform.dirty = 1;
			return 1;
		} else if(!strcmp(arg, "rot")){
			b->transform.rot = lua_get_vec4(L, 3);
			b->transform.dirty = 1;
			return 1;
		} else if(!strcmp(arg, "scale")){
			b->transform.scale = lua_get_vec3(L, 3);
			b->transform.dirty = 1;
			return 1;
		}
		return luaL_error(L, "Cannot assign to unknown field '%s'", arg);
	} else
		return luaL_error(L, "String expected, got %s", lua_typename(L, lua_type(L, 2)));
}

static int lua_body_get_center(lua_State* L)
{
	body* b = lua_get_body(L, 1);
	lua_push_vec3(L, body_get_center(b));
	return 1;
}
static int lua_body_get_bbox(lua_State* L)
{
	body* b = lua_get_body(L, 1);
	lua_push_bbox(L, body_get_bbox(b));
	return 1;
}

static int lua_body_check_collision(lua_State* L)
{
	body* b1 = lua_get_body(L, 1);
	body* b2 = lua_get_body(L, 2);

	vec3f resolution;
	int collided = sat_check_collision(b1, b2, &resolution);
	lua_pushboolean(L, collided);
	if(collided){
		lua_push_vec3(L, resolution);
		return 2;
	}
	return 1;
}
static int lua_body_check_terrain_collision(lua_State* L)
{
	body* b = lua_get_body(L, 1);
	vec3f resolution; 
	vec3f forward = (vec3f){1, 0, 0};
	if(lua_istable(L, 2))
		forward = lua_get_vec3(L, 2);
	vec4f new_rot = {0, 0, 0, 1};

	int collided = sat_check_terrain_collision(b, &resolution, forward, &new_rot);
	lua_pushboolean(L, collided);
	if(collided){
		lua_push_vec3(L, resolution);
		lua_push_vec4(L, new_rot);
		return 3;
	}
	return 1;
}

static int lua_body_is_selected(lua_State* L)
{
	body* b = lua_get_body(L, 1);
	lua_pushboolean(L, body_is_selected(b));
	return 1;
}

static int lua_body_draw(lua_State* L)
{
	body* b = lua_get_body(L, 1);

	if(!b->robj.buf && !(b->robj.flags & RENDER_OBJ_FLAG_NOTINIT))
		b->robj = body_render(b);

	vec3f wf_clr = {-1, -1, -1};
	if(!lua_isnoneornil(L, 2)){
		luaL_checktype(L, 2, LUA_TTABLE);
		wf_clr = lua_get_vec3(L, 2);
	}
	b->robj.colorize = wf_clr;

	render_info_draw inf = {&b->robj, b->transform.pos, b->transform.rot, b->transform.scale};
	render_list_add_draw(&inf);
	return 0;
}


static const struct luaL_Reg cfuncs[] = {
	{"new_cuboid", lua_new_cuboid},

	{NULL, NULL}
};

void game_logic_init_body(lua_State* _s)
{
	lua_newtable(_s);
	luaL_setfuncs(_s, cfuncs, 0);
	lua_setglobal(_s, "body");

	luaL_newmetatable(_s, "body");
	lua_pushcfunction(_s, lua_body_index); lua_setfield(_s, -2, "__index");
	lua_pushcfunction(_s, lua_body_newindex); lua_setfield(_s, -2, "__newindex");

	lua_pushcfunction(_s, lua_body_get_center); lua_setfield(_s, -2, "get_center");
	lua_pushcfunction(_s, lua_body_get_bbox); lua_setfield(_s, -2, "get_bbox");

	lua_pushcfunction(_s, lua_body_check_collision); lua_setfield(_s, -2, "check_collision");
	lua_pushcfunction(_s, lua_body_check_terrain_collision); lua_setfield(_s, -2, "check_terrain_collision");

	lua_pushcfunction(_s, lua_body_is_selected); lua_setfield(_s, -2, "is_selected");

	lua_pushcfunction(_s, lua_body_draw); lua_setfield(_s, -2, "draw");

	lua_pop(_s, 1);

}
