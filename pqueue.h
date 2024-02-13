#ifndef PRIO_QUEUE_H
#define PRIO_QUEUE_H

#include <stdlib.h>

#define PQUEUE_GROWTH 4

#define DEF_PQUEUE_STRUCT(tname, T)\
typedef struct {\
	T* data;\
	size_t size, max_size;\
	int (*cmp_func) (const T* el1, const T* el2);\
} tname;

#define DEF_PQUEUE_CONSTRUCTOR(tname, T)\
static void tname ## _create(tname* self,\
				int (*cmp_func) (const T* el1, const T* el2))\
{\
	self->data = NULL;\
	self->cmp_func = cmp_func;\
	self->size = self->max_size = 0;\
}
#define DEF_PQUEUE_DESTRUCTOR(tname, T)\
static void tname ## _destroy(tname* self)\
{\
	free(self->data);\
}

#define DEF_PQUEUE_HEAPIFY_UP(tname, T)\
static void tname ## _heapify_up(tname* self)\
{\
	size_t k = self->size - 1;\
	T el = self->data[k];\
	for(;;){\
		if(self->cmp_func((const T*)&self->data[k/2], (const T*)&el) <= 0)\
			break;\
		self->data[k] = self->data[k/2];\
		k /= 2;\
		if(k == 0) break;\
	}\
	self->data[k] = el;\
}
#define DEF_PQUEUE_HEAPIFY_DOWN(tname, T)\
static void tname ## _heapify_down(tname* self)\
{\
	size_t k = 0;\
	T el = self->data[0];\
	while(k < self->size / 2){\
		if(k*2 + 1 >= self->size) break;\
		size_t _min = k*2 + ( (k*2 + 2 < self->size && self->cmp_func((const T*)&self->data[k*2 + 2], (const T*)&self->data[k*2 + 1])) <= 0 ? 2 : 1);\
		if(self->cmp_func((const T*)&self->data[_min], (const T*)&el) > 0) break;\
		self->data[k] = self->data[_min];\
		k = _min;\
	}\
	self->data[k] = el;\
}
#define DEF_PQUEUE_HEAPIFY(tname, T)\
static void tname ## _heapify(tname* self)\
{\
	for(size_t i = 0; i < self->size; ++i){\
		size_t k = i;\
		T el = self->data[k];\
		for(;;){\
			if(self->cmp_func((const T*)&self->data[k/2], (const T*)&el) <= 0)\
				break;\
			self->data[k] = self->data[k/2];\
			k /= 2;\
			if(k == 0) break;\
		}\
		self->data[k] = el;\
	}\
}

#define DEF_PQUEUE_PUSH(tname, T)\
static void tname ## _push(tname* self, T el)\
{\
	if(self->size == self->max_size){\
		self->max_size += PQUEUE_GROWTH;\
		self->data = realloc(self->data, sizeof(T) * self->max_size);\
	}\
	self->data[self->size++] = el;\
	tname ## _heapify_up(self);\
}
#define DEF_PQUEUE_POP(tname, T)\
static T tname ## _pop(tname* self)\
{\
	T el = self->data[0];\
	self->data[0] = self->data[--self->size];\
	tname ## _heapify_down(self);\
	return el;\
}

#define DEF_PQUEUE_IS_EMPTY(tname, T)\
static int tname ## _is_empty(tname* self) { return !self->size; }

#define DEF_PQUEUE(tname, T)\
DEF_PQUEUE_STRUCT(tname, T)\
DEF_PQUEUE_CONSTRUCTOR(tname, T)\
DEF_PQUEUE_DESTRUCTOR(tname, T)\
DEF_PQUEUE_HEAPIFY_UP(tname, T)\
DEF_PQUEUE_HEAPIFY_DOWN(tname, T)\
DEF_PQUEUE_HEAPIFY(tname, T)\
DEF_PQUEUE_PUSH(tname, T)\
DEF_PQUEUE_POP(tname, T)\
DEF_PQUEUE_IS_EMPTY(tname, T)

#define DEF_PPQUEUE_STRUCT(tname, T)\
typedef struct {\
	T* data;\
	size_t size, max_size;\
} tname;

#define DEF_PPQUEUE_CONSTRUCTOR(tname, T)\
static void tname ## _create(tname* self)\
{\
	self->data = NULL;\
	self->size = self->max_size = 0;\
}
#define DEF_PPQUEUE_DESTRUCTOR(tname, T)\
static void tname ## _destroy(tname* self)\
{\
	free(self->data);\
}

#define DEF_PPQUEUE_HEAPIFY_UP(tname, T)\
static void tname ## _heapify_up(tname* self)\
{\
	size_t k = self->size - 1;\
	T el = self->data[k];\
	for(;;){\
		if(self->data[k/2] <= el)\
			break;\
		self->data[k] = self->data[k/2];\
		k /= 2;\
		if(k == 0) break;\
	}\
	self->data[k] = el;\
}
#define DEF_PPQUEUE_HEAPIFY_DOWN(tname, T)\
static void tname ## _heapify_down(tname* self)\
{\
	size_t k = 0;\
	T el = self->data[0];\
	while(k < self->size / 2){\
		if(k*2 + 1 >= self->size) break;\
		size_t _min = k*2 + ( (k*2 + 2 < self->size && self->cmp_func(&self->data[k*2 + 2], &self->data[k*2 + 1])) <= 0 ? 2 : 1);\
		if(self->cmp_func(&self->data[_min], &el) > 0) break;\
		self->data[k] = self->data[_min];\
		k = _min;\
	}\
	self->data[k] = el;\
}
#define DEF_PPQUEUE_HEAPIFY(tname, T)\
static void tname ## _heapify(tname* self)\
{\
	for(size_t i = 0; i < self->size; ++i){\
		size_t k = i;\
		T el = self->data[k];\
		for(;;){\
			if(self->data[k/2]<= el)\
				break;\
			self->data[k] = self->data[k/2];\
			k /= 2;\
			if(k == 0) break;\
		}\
		self->data[k] = el;\
	}\
}

#define DEF_PPQUEUE_PUSH(tname, T)\
static void tname ## _push(tname* self, T el)\
{\
	if(self->size == self->max_size){\
		self->max_size += PQUEUE_GROWTH;\
		self->data = realloc(self->data, sizeof(T) * self->max_size);\
	}\
	self->data[self->size++] = el;\
	tname ## _heapify_up(self);\
}
#define DEF_PPQUEUE_POP(tname, T)\
static T tname ## _pop(tname* self)\
{\
	T el = self->data[0];\
	self->data[0] = self->data[--self->size];\
	tname ## _heapify_down(self);\
	return el;\
}

#define DEF_PPQUEUE_IS_EMPTY(tname, T)\
static int tname ## _is_empty(tname* self) { return !self->size; }

#define DEF_PPQUEUE(tname, T)\
DEF_PPQUEUE_STRUCT(tname, T)\
DEF_PPQUEUE_CONSTRUCTOR(tname, T)\
DEF_PPQUEUE_DESTRUCTOR(tname, T)\
DEF_PPQUEUE_HEAPIFY_UP(tname, T)\
DEF_PPQUEUE_HEAPIFY_DOWN(tname, T)\
DEF_PPQUEUE_HEAPIFY(tname, T)\
DEF_PPQUEUE_PUSH(tname, T)\
DEF_PPQUEUE_POP(tname, T)\
DEF_PPQUEUE_IS_EMPTY(tname, T)

#endif
