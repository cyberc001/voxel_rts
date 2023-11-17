#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>

#define DEF_LINKED_LIST_STRUCT(tname, T)\
typedef struct tname ## _node tname ## _node;\
struct tname ## _node {\
	struct tname ## _node *prev, *next;\
	T data;\
};\
typedef struct {\
	tname ## _node head;\
	tname ## _node tail;\
} tname;

#define DEF_LINKED_LIST_CONSTRUCTOR(tname, T)\
static void tname ## _create(tname *self)\
{\
	self->head.prev = NULL; self->head.next = &self->tail;\
	self->tail.prev = &self->head; self->tail.next = NULL;\
}

#define DEF_LINKED_LIST_DESTRUCTOR(tname, T)\
static void tname ## _destroy(tname *self)\
{\
	tname ## _node *n = self->head.next;\
	while(n != &self->tail){\
		tname ## _node *next = n->next;\
		free(n);\
		n = next;\
	}\
}

#define DEF_LINKED_LIST_INSERT_END(tname, T)\
static tname ## _node * tname ## _insert_end(tname *self, T el)\
{\
	tname ## _node* n = malloc(sizeof(tname ## _node));\
	n->data = el;\
	n->next = &self->tail;\
	n->prev = self->tail.prev;\
	self->tail.prev->next = n;\
	self->tail.prev = n;\
	return n;\
}

#define DEF_LINKED_LIST_INSERT_BEGIN(tname, T)\
static tname ## _node * tname ## _insert_begin(tname *self, T el)\
{\
	tname ## _node* n = malloc(sizeof(tname ## _node));\
	n->data = el;\
	n->next = self->head.next;\
	n->prev = &self->tail;\
	self->head.next->prev = n;\
	self->head.next = n;\
	return n;\
}

#define DEF_LINKED_LIST_DELETE(tname, T)\
static void tname ## _delete(tname *self, tname ## _node *n)\
{\
	n->prev->next = n->next;\
	n->next->prev = n->prev;\
	free(n);\
}

#define DEF_LINKED_LIST_ITERATE(tname, T)\
static void tname ## _iterate(tname *self, void (*it_func)(T* el))\
{\
	tname ## _node *n = self->head.next;\
	while(n != &self->tail){\
		it_func(&n->data);\
		n = n->next;\
	}\
}
#define DEF_LINKED_LIST_NEXT(tname, T)\
static tname ## _node * tname ## _next(tname *self, tname ## _node *n)\
{\
	if(!n)\
		return self->head.next == &self->tail ? NULL : self->head.next;\
	return n == self->tail.prev ? NULL : n->next;\
}

#define DEF_LINKED_LIST(tname, T)\
	DEF_LINKED_LIST_STRUCT(tname, T)\
	DEF_LINKED_LIST_CONSTRUCTOR(tname, T)\
	DEF_LINKED_LIST_DESTRUCTOR(tname, T)\
	DEF_LINKED_LIST_INSERT_END(tname, T)\
	DEF_LINKED_LIST_INSERT_BEGIN(tname, T)\
	DEF_LINKED_LIST_DELETE(tname, T)\
	DEF_LINKED_LIST_NEXT(tname, T)

#endif
