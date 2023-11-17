#ifndef HTABLE_OA_H
#define HTABLE_OA_H

#include <stdlib.h>
#include <string.h>

#define HTABLE_OA_RATIO 2 / 3
#define HTABLE_OA_GROWTH 2

#define DEF_HTABLE_OA_STRUCT(tname, K, V)\
typedef struct{\
	size_t size;\
	size_t occupied;\
	V * data;\
	K * keys;\
	size_t free_bitmap_sz;\
	size_t* free_bitmap;\
	size_t (*hash_func)(size_t table_sz, const K* key);\
	int (*cmp_func) (const K* key1, const K* key2);\
} tname;

#define DEF_HTABLE_OA_CONSTRUCTOR(tname, K, V)\
static int tname ## _create(tname * self, size_t size,\
				size_t (*hash_func)(size_t table_sz, const K* key),\
				int (*cmp_func)(const K* key1, const K* key2))\
{\
	self->size = size;\
	self->occupied = 0;\
	self->data = malloc(sizeof(V) * size);\
	if(!self->data) return 0;\
	self->keys = malloc(sizeof(K) * size);\
	if(!self->keys) { free(self->data); return 0; }\
	size_t epbb = sizeof(size_t) * 8;\
	self->free_bitmap_sz = size / epbb + !!(size % epbb);\
	self->free_bitmap = malloc(self->free_bitmap_sz * sizeof(size_t));\
	if(!self->free_bitmap) { free(self->data); free(self->keys); return 0; }\
	memset(self->free_bitmap, 0, self->free_bitmap_sz * sizeof(size_t));\
	self->hash_func = hash_func;\
	self->cmp_func = cmp_func;\
	return 1;\
}

#define DEF_HTABLE_OA_DESTRUCTOR(tname, K, V)\
static void tname ## _destroy(tname * self)\
{\
	free(self->data);\
	free(self->keys);\
	free(self->free_bitmap);\
}

#define DEF_HTABLE_OA_IS_ALLOCATED(tname, K, V)\
static int tname ## _is_allocated(tname * self, size_t i)\
{\
	size_t epbb = sizeof(size_t) * 8;\
	return !!(self->free_bitmap[i / epbb] & ( ((size_t)1) << (epbb - 1 - i % epbb)));\
}
#define DEF_HTABLE_OA_SET_ALLOCATED(tname, K, V)\
static void tname ## _set_allocated(tname * self, size_t i, int state)\
{\
	size_t epbb = sizeof(size_t) * 8;\
	if(state)\
		self->free_bitmap[i / epbb] |= ( ((size_t)1) << (epbb - 1 - i % epbb));\
	else\
		self->free_bitmap[i / epbb] &= ~( ((size_t)1) << (epbb - 1 - i % epbb));\
}

#define DEF_HTABLE_OA_INSERT(tname, K, V)\
static int tname ## _rebuild(tname * self, size_t new_sz); /* rebuild and insert depend on each other, so forward declare*/\
static V * tname ## _insert(tname * self, K key, V el)\
{\
	if(self->occupied >= self->size * HTABLE_OA_RATIO)\
		tname ## _rebuild(self, self->size * HTABLE_OA_GROWTH);\
	size_t h = self->hash_func(self->size, (const K*)&key);\
	size_t i;\
	for(i = h; i < self->size && tname ## _is_allocated(self, i); ++i) {}\
	if(i < self->size){\
		self->data[i] = el;\
		self->keys[i] = key;\
		tname ## _set_allocated(self, i, 1);\
		++self->occupied;\
		return &self->data[i];\
	}\
	for(i = 0; i < h && tname ## _is_allocated(self, i); ++i) {}\
	if(i < h){\
		self->data[i] = el;\
		self->keys[i] = key;\
		tname ## _set_allocated(self, i, 1);\
		++self->occupied;\
		return &self->data[i];\
	}\
	return NULL;\
}

#define DEF_HTABLE_OA_FIND(tname, K, V)\
static V * tname ## _find(tname * self, const K key)\
{\
	size_t h = self->hash_func(self->size, (const K*)&key);\
	size_t i;\
	if(self->cmp_func)\
		for(i = h; i < self->size\
			&& tname ## _is_allocated(self, i)\
			&& self->cmp_func((const K*)&self->keys[i], (const K*)&key);\
			 ++i) {}\
	else\
		for(i = h; i < self->size\
			&& tname ## _is_allocated(self, i)\
			&& self->keys[i] != key;\
			 ++i) {}\
	\
	if(i < self->size && tname ## _is_allocated(self, i))\
		return (V*)&self->data[i];\
	if(i < self->size && !tname ## _is_allocated(self, i))\
		return NULL;\
	\
	if(self->cmp_func)\
		for(i = 0; i < h\
			&& tname ## _is_allocated(self, i)\
			&& self->cmp_func((const K*)&self->keys[i], (const K*)&key); ++i) {}\
	else\
		for(i = 0; i < h\
			&& tname ## _is_allocated(self, i)\
			&& self->keys[i] != key; ++i) {}\
	if(i < h && tname ## _is_allocated(self, i)){\
		return (V*)&self->data[i];\
	}\
	return NULL;\
}

#define DEF_HTABLE_OA_DELETE(tname, K, V)\
static void tname ## _delete(tname * self, K key)\
{\
	size_t h = self->hash_func(self->size, (const K*)&key);\
	size_t i;\
	if(self->cmp_func)\
		for(i = h; i < self->size\
			&&  tname ## _is_allocated(self, i)\
			&& self->cmp_func((const K*)&self->keys[i], (const K*)&key);\
			 ++i) {}\
	else\
		for(i = h; i < self->size\
			&& tname ## _is_allocated(self, i)\
			&& self->keys[i] != key;\
			 ++i) {}\
	\
	if(i < self->size && tname ## _is_allocated(self, i)){\
		size_t j;\
		for(j = i; j < self->size\
			&& tname ## _is_allocated(self, j)\
			&& self->hash_func(self->size, (const K*)&self->keys[j]) == h;\
			++j) {}\
		--j;\
		if(i == j)\
			tname ## _set_allocated(self, i, 0);\
		else{\
			for(size_t k = i; k < j; ++k)\
				tname ## _set_allocated(self, k, 0);\
			for(size_t k = i; k < j - 1; ++k)\
				tname ## _set_allocated(self, k, 1);\
		}\
		memmove(&self->keys[i], &self->keys[i+1], (j - i) * sizeof(K));\
		memmove(&self->data[i], &self->data[i+1], (j - i) * sizeof(V));\
		--self->occupied;\
	}\
	if(!tname ## _is_allocated(self, i))\
		return;\
	\
	if(self->cmp_func)\
		for(i = 0; i < h\
			&& tname ## _is_allocated(self, i)\
			&& self->cmp_func((const K*)&self->keys[i], (const K*)&key); ++i) {}\
	else\
		for(i = 0; i < h\
			&& tname ## _is_allocated(self, i)\
			&& self->keys[i] != key; ++i) {}\
	\
	if(i < h && tname ## _is_allocated(self, i)){\
		size_t j;\
		for(j = i; j < self->size\
			&& tname ## _is_allocated(self, j)\
			&& self->hash_func(self->size, (const K*)&self->keys[j]) == h;\
			++j) {}\
		--j;\
		if(i == j)\
			tname ## _set_allocated(self, i, 0);\
		else{\
			for(size_t k = i; k < j; ++k)\
				tname ## _set_allocated(self, k, 0);\
			for(size_t k = i; k < j - 1; ++k)\
				tname ## _set_allocated(self, k, 1);\
		}\
		memmove(&self->keys[i], &self->keys[i+1], (j - i) * sizeof(K));\
		memmove(&self->data[i], &self->data[i+1], (j - i) * sizeof(V));\
		--self->occupied;\
	}\
}

#define DEF_HTABLE_OA_REBUILD(tname, K, V)\
static int tname ## _rebuild(tname * self, size_t new_sz)\
{\
	size_t old_size = self->size;\
	self->size = new_sz;\
	\
	V * old_data = self->data;\
	K * old_keys = self->keys;\
	self->data = malloc(sizeof(V) * self->size);\
	if(!self->data) return 0;\
	self->keys = malloc(sizeof(K) * self->size);\
	if(!self->keys) { free(self->data); return 0; }\
	\
	size_t* old_free_bitmap = self->free_bitmap;\
	size_t epbb = sizeof(size_t) * 8;\
	self->free_bitmap_sz = self->size / epbb + !!(self->size % epbb);\
	self->free_bitmap = malloc(self->free_bitmap_sz * sizeof(size_t));\
	if(!self->free_bitmap) { free(self->data); free(self->keys); return 0; }\
	memset(self->free_bitmap, 0, self->free_bitmap_sz * sizeof(size_t));\
	\
	for(size_t i = 0; i < old_size; ++i){\
		if(!!(old_free_bitmap[i / epbb] & ( ((size_t)1) << (epbb - 1 - i % epbb))))\
			tname ## _insert(self, old_keys[i], old_data[i]);\
	}\
	free(old_data);\
	free(old_keys);\
	free(old_free_bitmap);\
	return 1;\
}

#define DEF_HTABLE_OA(tname, K, V)\
DEF_HTABLE_OA_STRUCT(tname, K, V)\
DEF_HTABLE_OA_IS_ALLOCATED(tname, K, V)\
DEF_HTABLE_OA_SET_ALLOCATED(tname, K, V)\
DEF_HTABLE_OA_CONSTRUCTOR(tname, K, V)\
DEF_HTABLE_OA_DESTRUCTOR(tname, K, V)\
DEF_HTABLE_OA_INSERT(tname, K, V)\
DEF_HTABLE_OA_FIND(tname, K, V)\
DEF_HTABLE_OA_DELETE(tname, K, V)\
DEF_HTABLE_OA_REBUILD(tname, K, V)

#endif
