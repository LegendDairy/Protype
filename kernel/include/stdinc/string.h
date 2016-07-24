/* Pro-Type Kernel v1.3       */
/* String functions port	  */
/* By LegendMythe             */

#ifndef STRING_H
#define STRING_H

#include <common.h>
#include <stdint.h>

/** Compares two NULL-terminated strings. Should return -1 if str1 < str2, 0 if they are equal. **/
int strcmp(const char *str1, const char *str2);
/** Compares two NULL-terminated strings, returns 0 if they are equal **/
int strncmp(const char *str1, const char *str2, uint32_t n);
/** Copy the NULL-terminated string src into dest, and return dest. **/
char *strcpy(char *dest, const char *src);
/** Concatenate the NULL-terminated string src onto the end of dest, and return dest.**/
char *strcat(char *dest, const char *src);
/** Returns the length of a given NULL-terminated string (str). */
int strlen(const char *str);

/** Copy len bytes from src to dest. **/
void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);
/** Write len copies of a u8int val into dest. **/
void memset(uint8_t *dest, uint8_t val, uint32_t len);
/** Write len copies of a u16int val into dest. **/
void memsetw(uint16_t *dest, uint16_t val, uint32_t len);
/** Write len copies of a u32int val into dest. **/
void memsetdw(uint32_t *dest, uint32_t val, uint32_t len);
/** Write len copies of a u32int val into dest. **/
void memsetqw(uint64_t *dest, uint64_t val, uint32_t len);

#endif
