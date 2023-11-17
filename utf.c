#include "utf.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

utf_char utf_next_char(const char** str)
{
	const char* it = *str;
	if(!*it)
		return 0;

	if((*it) & 0x80){
		// decoding: fill current byte from minor to major bits (right to left), moving the whole byte to the left and starting on another byte if it has filled up
		size_t byte_cnt = 0;
		for(size_t i = 0; ((*it) << i) & 0x80; ++i)
			++byte_cnt;
		
		utf_char c = 0;
		it += byte_cnt - 1;
		size_t bit_cnt = 8; // unfilled major bits
		size_t cur_byte = 0; // from the right
		for(size_t i = 0; i < byte_cnt; --it, ++i){
			if(!bit_cnt){
				bit_cnt = 8;
				++cur_byte;
			}

			size_t bits_to_copy = 6;
			if(bit_cnt < bits_to_copy){ // copy what we can
				c |= (((*it & 0x3F) << (8 - bit_cnt)) & 0xFF) << cur_byte*8;
				bits_to_copy -= bit_cnt;
				bit_cnt = 8;
				++cur_byte;
			}

			c |= (((*it & 0x3F) >> (6 - bits_to_copy) << (8 - bit_cnt)) & 0xFF) << cur_byte*8;
			bit_cnt -= bits_to_copy;
		}

		*str += byte_cnt;
		return c;
	}
	// return plain ASCII character
	++(*str);
	return *it;
}
void utf_encode_char(utf_char c, char* out)
{
	size_t bit_cnt = 31;
	while(!(c & ((utf_char)1 << bit_cnt)))
		--bit_cnt;
	++bit_cnt;

	if(bit_cnt < 8){ // just write the ASCII character
		out[0] = c; out[1] = '\0';
		return;
	}

	size_t byte_cnt = bit_cnt / 6 - !(bit_cnt % 6); // not counting the 1st byte
	out[0] = ((1 << (byte_cnt + 1)) - 1) << (8 - byte_cnt - 1); // writing as much 1s as much bytes the codepoint takes
	size_t cur_bit = bit_cnt;
	out[0] |= (c >> (cur_bit - (bit_cnt % 6))) & ((1 << (8 - byte_cnt - 2)) - 1); // ORing with whatever left in 1st byte, padding with leading zeros if necessary
	cur_bit -= 8 - byte_cnt - 2;
	size_t i = 1;
	while(cur_bit > 0){
		out[i] = 0x80 | ((c >> (cur_bit - 6)) & 0x3F);
		cur_bit -= 6; ++i;
	}
	out[i] = '\0';
}

char* str_append(char* to, size_t* to_sz, const char* from)
{
	if(*to_sz <= strlen(to) + strlen(from) + 2){
		*to_sz = strlen(to) + strlen(from) + 2;
		to = realloc(to, *to_sz);
	}
	strcat(to, from);
	return to;
}
