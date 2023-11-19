#ifndef DYN_ARRAY_H
#define DYN_ARRAY_H

#include <stdlib.h>

#define DYN_ARRAY_GROWTH 4

#define DEF_DYN_ARRAY_STRUCT(tname, T)\
typedef struct {\
	T* data;\
	size_t busy, ln;\
} tname;

#define DEF_DYN_ARRAY_CONSTRUCTOR(tname, T)\
static void tname ## _create(tname *self)\
{\
	self->data = NULL;\
	self->busy = self->ln = 0;\
}

#define DEF_DYN_ARRAY_DESTRUCTOR(tname, T)\
static void tname ## _destroy(tname *self)\
{\
	free(self->data);\
}

#define DEF_DYN_ARRAY_PUSH(tname, T)\
static size_t tname ## _push(tname *self, T el)\
{\
	if(self->busy >= self->ln){\
		self->ln += DYN_ARRAY_GROWTH;\
		self->data = realloc(self->data, sizeof(T) * self->ln);\
	}\
	self->data[self->busy++] = el;\
	return self->busy - 1;\
}

#define DEF_DYN_ARRAY_DELETE_LAST(tname, T)\
static void tname ## _delete_last(tname* self)\
{\
	--self->busy;\
}
#define DEF_DYN_ARRAY_DELETE(tname, T)\
static void tname ## _delete(tname *self, size_t i)\
{\
	if(i == self->busy - 1)\
		tname ## _delete_last(self);\
	else{\
		memmove(self->data + i, self->data + i + 1, (self->busy - i - 1) * sizeof(T));\
		--self->busy;\
	}\
}

#define DEF_DYN_ARRAY(tname, T)\
	DEF_DYN_ARRAY_STRUCT(tname, T)\
	DEF_DYN_ARRAY_CONSTRUCTOR(tname, T)\
	DEF_DYN_ARRAY_DESTRUCTOR(tname, T)\
	DEF_DYN_ARRAY_PUSH(tname, T)\
	DEF_DYN_ARRAY_DELETE_LAST(tname, T)\
	DEF_DYN_ARRAY_DELETE(tname, T)

#endif
