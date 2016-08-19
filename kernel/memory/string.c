/* Pro-Type Kernel v1.3       */
/* String functions           */
/* By LegendMythe             */

#include <string.h>

/* Compare two strings. Should return  2 if		*/
/* str1 < str2, 0 if they are equal or 1 otherwise. 	*/
int strcmp(const char *str1, const char *str2)
{
	int i = 0;
	while(str2[i] != '\0')
	{
		if(str1[i] != str2[i])
		{
			return 1;
		}
		i++;
	}
	return 0;
}
int strncmp(const char *str1, const char *str2, uint32_t n)
{
	uint32_t i = 0;
	while(i<n)
    {
		if(str1[i] != str2[i])
		{
			return 1;
		}
		i++;
	}
	return 0;
}
/* Copy the NULL-terminated string src into dest, and return dest. */
char *strcpy(char *dest, const char *src)
{
    int i = 0;
    while (src[i]);
    {
      dest[i] = src[i];
 	i++;
    }
	return dest;
}

/* Counts a NULL-terminated string. */
int strlen(const char *str)
{
const char *start = str;
while (*str)
{
str++;
}
return str - start;
}

/* Copy len bytes from src to dest. */
void *memcpy(void *dest, const void *src, uint32_t count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

/* Write len copies of val into dest. */
void memset(uint8_t *dest, uint8_t val, uint32_t len)
{
    for ( ; len != 0; len--) dest[len - 1] = val;
}

/* Write len copies of val into dest. */
void memsetw(uint16_t *dest, uint16_t val, uint32_t len)
{
    for ( ; len != 0; len--) dest[len - 1] = val;
}
/* Write len copies of val into dest. */
void memsetdw(uint32_t *dest, uint32_t val, uint32_t len)
{
	for (; len != 0; len--) dest[len - 1] = val;
}
/* Write len copies of val into dest. */
void memsetqw(uint64_t *dest, uint64_t val, uint32_t len)
{
	for (; len != 0; len--) dest[len - 1] = val;
}
