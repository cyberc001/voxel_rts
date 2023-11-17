#include "formats/lon.h"
#include <ctype.h>

#include "log.h"

#define IS_SPECIAL_CHAR(c) ((c) == ',' || (c) == '[' || (c) == ']' || (c) == '{' || (c) == '}' || (c) == ':')

const char* lon_type_to_string(lon_object* obj)
{
	switch(obj->type)
	{
		case LON_OBJECT_TYPE_STRING: return "STRING";
		case LON_OBJECT_TYPE_INTEGER: return "INTEGER";
		case LON_OBJECT_TYPE_FLOAT: return "FLOAT";
		case LON_OBJECT_TYPE_ARRAY: return "ARRAY";
		case LON_OBJECT_TYPE_DICT: return "DICT";
	}
	return "INVALID";
}

static int skip_ws(char** fptr, unsigned* line)
{
	for(; **fptr && isspace(**fptr); ++(*fptr))
		if(**fptr == '\n')
			++(*line);
	return **fptr != '\0';
}

static lon_object* lon_read_object(char** fptr, unsigned* line);

lon_array* read_lon(FILE* fd)
{
	fseek(fd, 0, SEEK_END);
	long fsz = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	char* fstr = malloc(fsz + 1);
	fread(fstr, 1, fsz, fd);
	fstr[fsz] = '\0';

	char* fptr = fstr;
	unsigned line = 1;

	lon_array* arr = malloc(sizeof(lon_array));
	arr->type = LON_OBJECT_TYPE_ARRAY;
	arr->size = 8;
	arr->data = malloc(sizeof(void*) * arr->size);

	size_t arr_i = 0;
	lon_object* obj;
	while((obj = lon_read_object(&fptr, &line))){
		if(arr_i >= arr->size){
			arr->size *= 2;
			arr->data = realloc(arr->data, sizeof(void*) * arr->size);
		}
		arr->data[arr_i++] = obj;
	}
	arr->size = arr_i;
	arr->data = realloc(arr->data, sizeof(void*) * arr_i);
	free(fstr);
	return arr;
}

static lon_integer* lon_read_integer(char** fptr, unsigned* line)
{
	char* endptr;
	long value = strtol(*fptr, &endptr, 0);
	if(!(isspace(*endptr) || IS_SPECIAL_CHAR(*endptr))){
		LOG_ERROR("Invalid characters \"%.3s...\"in integer \"%.*s...\"", endptr, (int)(endptr - *fptr + 3), *fptr);
		*fptr = endptr;
		return NULL;
	}
	*fptr = endptr;

	lon_integer* obj = malloc(sizeof(lon_integer));
	obj->type = LON_OBJECT_TYPE_INTEGER;
	obj->value = value;
	return obj;
}
static lon_float* lon_read_float(char** fptr, unsigned* line)
{
	char* endptr;
	double value = strtod(*fptr, &endptr);
	if(!(isspace(*endptr) || IS_SPECIAL_CHAR(*endptr) || *endptr == 'f')){
		LOG_ERROR("Invalid characters \"%.3s...\"in floating-point number \"%.*s...\"", endptr, (int)(endptr - *fptr + 3), *fptr);
		*fptr = endptr;
		return NULL;
	}
	if(*endptr == 'f')
		++endptr;
	*fptr = endptr;

	lon_float* obj = malloc(sizeof(lon_float));
	obj->type = LON_OBJECT_TYPE_FLOAT;
	obj->value = value;
	return obj;
}
static lon_object* lon_read_number(char** fptr, unsigned* line)
{
	int dot = 0;
	char* ptr = *fptr;
	for(; *ptr && !isspace(*ptr) && !IS_SPECIAL_CHAR(*ptr); ++ptr){
		if(*ptr == '\n')
			++(*line);
		else if(*ptr == '.')
			dot = 1;
	}
	if(ptr[-1] == 'f' || dot)
		return (lon_object*)lon_read_float(fptr, line);
	else
		return (lon_object*)lon_read_integer(fptr, line);
}

static lon_string* lon_read_string(char** fptr, unsigned* line)
{
	size_t str_sz = 32, str_i = 0;
	char* str = malloc(str_sz);
	int quotes = 0;
	for(; **fptr && (!isspace(**fptr) || quotes) && (quotes || !IS_SPECIAL_CHAR(**fptr)); ++(*fptr)){
		if(**fptr == '\n')
			++(*line);
		if(**fptr == '"')
			quotes = !quotes;
		else{
			if(str_i + 1 >= str_sz){
				str_sz *= 2;
				str = realloc(str, str_sz);
			}
			str[str_i++] = **fptr;
		}
	}
	if(quotes && !**fptr)
		LOG_WARN("EOF without closing string quotes");
	str = realloc(str, str_i + 1);
	str[str_i] = '\0';

	lon_string* obj = malloc(sizeof(lon_string));
	obj->type = LON_OBJECT_TYPE_STRING;
	obj->value = str;
	return obj;
}

static void lon_array_free(lon_array* arr)
{
	for(size_t i = 0; i < arr->size; ++i)
		lon_object_free(arr->data[i]);
	free(arr->data);
	free(arr);
}
static lon_array* lon_read_array(char** fptr, unsigned* line)
{
	lon_array* arr = malloc(sizeof(lon_array));
	arr->type = LON_OBJECT_TYPE_ARRAY;
	arr->size = 8;
	arr->data = malloc(arr->size * sizeof(void*));
	size_t arr_i = 0;

	++(*fptr);
	if(!skip_ws(fptr, line)){
		LOG_ERROR("Unexpected EOF before ending an array");
		arr->size = arr_i;
		lon_array_free(arr);
		return NULL;
	}
	if(**fptr == ']'){
		++(*fptr);
		free(arr->data);
		arr->data = NULL;
		arr->size = 0;
		return arr;
	}

	for(;;){
		if(!skip_ws(fptr, line)){
			LOG_ERROR("Unexpected EOF before ending an array");
			lon_array_free(arr);
			return NULL;
		}

		lon_object* obj = lon_read_object(fptr, line);
		if(obj){
			if(arr_i >= arr->size){
				arr->size *= 2;
				arr->data = realloc(arr->data, arr->size * sizeof(void*));
			}
			arr->data[arr_i++] = obj;
		}

		if(!skip_ws(fptr, line)){
			LOG_ERROR("Unexpected EOF before ending an array");
			arr->size = arr_i;
			lon_array_free(arr);
			return NULL;
		}
		if(**fptr == ']'){
			++(*fptr);
			arr->data = realloc(arr->data, arr_i * sizeof(void*));
			arr->size = arr_i;
			return arr;
		}
		else if(**fptr != ','){
			LOG_ERROR("Expected a comma after array element");
			arr->size = arr_i;
			lon_array_free(arr);
			return NULL;
		}
		++(*fptr);
	}
}

static size_t str_hash(size_t table_sz, const char** key)
{
	const char* i = *key;
	size_t h = 0;
	for(; *i; ++i)
		h = ((h + *i) % table_sz) * 256 % table_sz;
	return h;
}
static int str_cmp(const char** key1, const char** key2)
{
	return strcmp(*key1, *key2);
}
static void lon_dict_free(lon_dict* dict)
{
	for(size_t i = 0; i < dict->data.size; ++i)
		if(lon_htable_is_allocated(&dict->data, i)){
			free(dict->data.keys[i]);
			lon_object_free(dict->data.data[i]);
		}
	lon_htable_destroy(&dict->data);
	free(dict);
}
static lon_dict* lon_read_dict(char** fptr, unsigned* line)
{
	lon_dict* dict = malloc(sizeof(lon_dict));
	dict->type = LON_OBJECT_TYPE_DICT;
	lon_htable_create(&dict->data, 8, str_hash, str_cmp);

	++(*fptr);
	if(!skip_ws(fptr, line)){
		LOG_ERROR("Unexpected EOF before ending a dictionary");
		lon_dict_free(dict);
		return NULL;
	}
	if(**fptr == '}'){
		++(*fptr);
		return dict;
	}

	for(;;){
		if(!skip_ws(fptr, line)){
			LOG_ERROR("Unexpected EOF before ending a dictionary");
			lon_dict_free(dict);
			return NULL;
		}

		lon_object* key = lon_read_object(fptr, line);
		if(!key)
			continue;
		if(key->type != LON_OBJECT_TYPE_STRING){
			LOG_ERROR("Expected STRING type for dictionary key, got %s", lon_type_to_string(key));
			continue;
		}

		if(!skip_ws(fptr, line)){
			LOG_ERROR("Unexpected EOF before ending a dictionary key");
			lon_dict_free(dict);
			return NULL;
		}

		if(**fptr != ':'){
			LOG_ERROR("Expected ':' after a dictionary key");
			lon_dict_free(dict);
			return NULL;
		}
		++(*fptr);

		if(!skip_ws(fptr, line)){
			LOG_ERROR("Unexpected EOF before ending a dictionary value pair");
			lon_dict_free(dict);
			return NULL;
		}

		lon_object* value = lon_read_object(fptr, line);
		if(!value)
			continue;

		char* str_key = ((lon_string*)key)->value;
		free(key);
		lon_htable_insert(&dict->data, str_key, value);

		if(!skip_ws(fptr, line)){
			LOG_ERROR("Unexpected EOF before ending a dictionary");
			lon_dict_free(dict);
			return NULL;
		}

		if(**fptr == '}'){
			++(*fptr);
			return dict;
		}
		else if(**fptr != ','){
			lon_object_print(value); puts("");
			LOG_ERROR("Expected a comma after a dictionary key-value pair, got '%c'", **fptr);
			lon_dict_free(dict);
			return NULL;
		}
		++(*fptr);
	}

}

static lon_object* lon_read_object(char** fptr, unsigned* line)
{
	if(!skip_ws(fptr, line))
		return NULL;

	if(**fptr == '[')
		return (lon_object*)lon_read_array(fptr, line);
	else if(**fptr == '{')
		return (lon_object*)lon_read_dict(fptr, line);
	else if(isdigit(**fptr) || **fptr == '-' || **fptr == '+')
		return lon_read_number(fptr, line);
	else
		return (lon_object*)lon_read_string(fptr, line);
}

void lon_object_free(lon_object* obj)
{
	switch(obj->type){
		case LON_OBJECT_TYPE_ARRAY:
			lon_array_free((lon_array*)obj);
			break;
		case LON_OBJECT_TYPE_DICT:
			lon_dict_free((lon_dict*)obj);
			break;

		case LON_OBJECT_TYPE_STRING:
			free(((lon_string*)obj)->value);
			free(obj);
			break;

		default:
			free(obj);
			break;
	}
}
void lon_object_print(lon_object* obj)
{
	switch(obj->type)
	{
		case LON_OBJECT_TYPE_STRING:
			printf("%s", ((lon_string*)obj)->value);
			break;
		case LON_OBJECT_TYPE_INTEGER:
			printf("%ld", ((lon_integer*)obj)->value);
			break;
		case LON_OBJECT_TYPE_FLOAT:
			printf("%f", ((lon_float*)obj)->value);
			break;

		case LON_OBJECT_TYPE_ARRAY: {
			lon_array* arr = (lon_array*)obj;
			printf("[ ");
			for(size_t i = 0; i < arr->size; ++i){
				lon_object_print(arr->data[i]);
				if(i < arr->size - 1)
					printf(", ");
			}
			printf(" ]");
		} break;
		case LON_OBJECT_TYPE_DICT: {
			lon_dict* dict = (lon_dict*)obj;
			printf("{ ");
			int first_comma = 1;
			for(size_t i = 0; i < dict->data.size; ++i)
				if(lon_htable_is_allocated(&dict->data, i)){
					if(!first_comma)
						printf(", ");
					first_comma = 0;
					printf("%s : ", dict->data.keys[i]);
					lon_object_print(dict->data.data[i]);
				}
			printf(" }");
		} break;
	}
}
