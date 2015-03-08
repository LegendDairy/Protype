/* Pro-Type Kernel v1.3       */
/* String functions port	  */
/* By LegendMythe             */

#ifndef STRING_H
#define STRING_H

#include <common.h>
#include <stdint.h>

/** Compare two strings. Should return -1 if str1 < str2. **/
int strcmp(char *str1, char *str2);
/** Copy the NULL-terminated string src into dest, and return dest. **/
char *strcpy(char *dest, const char *src);
/** Concatenate the NULL-terminated string src onto the end of dest, and return dest.**/
char *strcat(char *dest, const char *src);


/*** Simpel Memory functions to copy. ***/
/*** bytes and set certain value's in ***/
/*** pointers.						  ***/

/** Copy len bytes from src to dest. **/
void memcpy(u8int *dest, const u8int *src, u32int len);

/** Write len copies of a u8int val into dest. **/
void memset(u8int *dest, u8int val, u32int len);

/** Write len copies of a u16int val into dest. **/
void memsetw(u16int *dest, u16int val, u32int len);

/** Write len copies of a u32int val into dest. **/
void memsetdw(u32int *dest, u32int val, u32int len);

/** Write len copies of a u32int val into dest. **/
void memsetqw(u64int *dest, u64int val, u32int len);

#endif
