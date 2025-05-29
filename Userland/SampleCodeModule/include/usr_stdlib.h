#ifndef _USRLIB_H_
#define _USRLIB_H_

#include <stdint.h>

uint64_t puts(const char *string);
uint64_t putChar(const char *string);
uint64_t newLine();
uint64_t strlen(const char *string);

void unsigned_num_to_str(uint32_t num, uint32_t start, char *buff);
void signed_num_to_str(int32_t num, char *buff);
uint32_t unsigned_str_to_num(uint64_t *it, uint64_t buff_length, char *buff);
void unsigned_num_to_hex_str(uint32_t num, char *buff);
int32_t signed_str_to_num(uint64_t *it, uint64_t buff_length, char *buff);




#endif