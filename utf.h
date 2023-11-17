#ifndef UTF_H
#define UTF_H

#include <stddef.h>

typedef unsigned utf_char;

utf_char utf_next_char(const char** str);
void utf_encode_char(utf_char c, char* out); // out should have a size of 9+

// misc utils
char* str_append(char* to, size_t* to_sz, const char* from); // appends to a buffer "to" than doesn't necessary have the same size as strlen(to). LEAVES 2 EXTRA CHARACTERS, for '\0' and for anything else

#endif
