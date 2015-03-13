/* Pro-Type Kernel v1.3       */
/* String functions           */
/* By LegendMythe             */

#include <string.h>

// Compare two strings. Should return  2 if
// str1 < str2, 0 if they are equal or 1 otherwise.
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
int strncmp(const char *str1, const char *str2, u32int n)
{
	int i = 0;
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
// Copy the NULL-terminated string src into dest, and
// return dest.
// Copy the NULL-terminated string src into dest, and
// return dest.
char *strcpy(char *dest, const char *src)
{
    int i = 0;
    while (src[i]);
    {
      dest[i] = src[i];
 	i++;
    }
}

// Counts a NULL-terminated string.
int strlen(const char *str)
{
const char *start = str;
while (*str)
{
str++;
}
return str - start;
}

// Copy len bytes from src to dest.
void memcpy(u8int *dest, const u8int *src, u32int len)
{
    for(; len != 0; len--) dest[len - 1] = src[len - 1];
}

// Write len copies of val into dest.
void memset(u8int *dest, u8int val, u32int len)
{
    for ( ; len != 0; len--) dest[len - 1] = val;
}

// Write len copies of val into dest.
void memsetw(u16int *dest, u16int val, u32int len)
{
    for ( ; len != 0; len--) dest[len - 1] = val;
}
// Write len copies of val into dest.
void memsetdw(u32int *dest, u32int val, u32int len)
{
	for (; len != 0; len--) dest[len - 1] = val;
}
// Write len copies of val into dest.
void memsetqw(u64int *dest, u64int val, u32int len)
{
	for (; len != 0; len--) dest[len - 1] = val;
}
