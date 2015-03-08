/* Pro-Type Kernel v1.3       */
/* String functions           */
/* By LegendMythe             */

#include <string.h>

// Compare two strings. Should return  2 if
// str1 < str2, 0 if they are equal or 1 otherwise.
int strcmp(char *str1, char *str2)
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
int strncmp(char *str1, char *str2, u32int n)
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
    do
    {
      *dest++ = *src++;
    }
    while (*src != 0);
}

// Counts a NULL-terminated string.
int strlen(char *src)
{
	int len = 0;
	while(src != '\0');
	{
		src++;
		len++;
	}
	return len;
}

// Concatenate the NULL-terminated string src onto
// the end of dest, and return dest.
char *strcat(char *dest, const char *src)
{
    while (*dest != 0)
    {
        *dest = *dest++;
    }

    do
    {
        *dest++ = *src++;
    }
    while (*src != 0);
    return dest;
}

// Copy len bytes from src to dest.
void memcpy(u8int *dest, const u8int *src, u32int len)
{
    const u8int *sp = (const u8int *)src;
    u8int *dp = (u8int *)dest;
    for(; len != 0; len--) *dp++ = *sp++;
}

// Write len copies of val into dest.
void memset(u8int *dest, u8int val, u32int len)
{
    u8int *temp = (u8int *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

// Write len copies of val into dest.
void memsetw(u16int *dest, u16int val, u32int len)
{
    u16int *temp = (u16int *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}
// Write len copies of val into dest.
void memsetdw(u32int *dest, u32int val, u32int len)
{
    u32int *temp = (u32int *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}
// Write len copies of val into dest.
void memsetqw(u64int *dest, u64int val, u32int len)
{
    u64int *temp = (u64int *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}
