#ifndef OBJECT_H
#define OBJECT_H

#include "htable_oa.h"
DEF_HTABLE_OA(vtable, char*, void*)

typedef struct object object;
struct object {
	vtable* vt;
};

vtable vt_create(vtable* parent_vt_ptr);
// Put vt_init macro in object constructor; it lazy creates vtable of a class, allows you to run code for inserting virtual functions, and assigns the vtable to the object being created (it SHOULD be named "o")
#define vt_init(parent_vt_ptr, ...)\
{\
	static vtable vt = {.size = 0};\
	if(!vt.size){\
		vt = vt_create(parent_vt_ptr);\
		__VA_ARGS__;\
	}\
	o->vt = &vt;\
}

#define vt_insert(vt_ptr, fname, func)\
{\
	vtable_delete(vt_ptr, #fname);\
	vtable_insert(vt_ptr, #fname, func);\
}
#define vt_call(obj, fname, ...) ( ((fname)(*vtable_find((obj).vt, #fname)))(&(obj), ##__VA_ARGS__) )

#endif
