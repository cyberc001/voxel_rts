#include "object.h"
#include "resources.h"

vtable vt_create(vtable* parent_vt_ptr)
{
	vtable vt;
	vtable_create(&vt, 16, str_hash, str_cmp);
	if(parent_vt_ptr)
		for(size_t i = 0; i < parent_vt_ptr->size; ++i)
			if(vtable_is_allocated(parent_vt_ptr, i))
				vtable_insert(&vt, parent_vt_ptr->keys[i], parent_vt_ptr->data[i]);
	return vt;
}
