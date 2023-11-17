#ifndef LON_H
#define LON_H

#include <stdio.h>
#include "htable_oa.h"

enum lon_object_type{
	LON_OBJECT_TYPE_STRING		= 1,

	LON_OBJECT_TYPE_INTEGER,
	LON_OBJECT_TYPE_FLOAT,

	LON_OBJECT_TYPE_ARRAY,
	LON_OBJECT_TYPE_DICT
};

typedef struct lon_object lon_object;
struct lon_object {
	enum lon_object_type type;
};
const char* lon_type_to_string(lon_object* obj);
void lon_object_free(lon_object* obj);
void lon_object_print(lon_object* obj);

typedef struct {
	lon_object;
	char* value;
} lon_string;

typedef struct {
	lon_object;
	long value;
} lon_integer;
typedef struct {
	lon_object;
	double value;
} lon_float;

typedef struct {
	lon_object;
	lon_object** data;
	size_t size;
} lon_array;

DEF_HTABLE_OA(lon_htable, char*, lon_object*)
typedef struct {
	lon_object;
	lon_htable data;
} lon_dict;
#define lon_dict_find(dict, key) (lon_htable_find(&((dict)->data), (key)))

/* API */
lon_array* read_lon(FILE* fd);

#endif
