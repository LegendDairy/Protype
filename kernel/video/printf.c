/* Pro-Type Kernel v1.3	*/
/* Print Formatted v0.1	*/
/* By LegendDairy	*/

#include <stdio.h>
#include <mutex.h>

extern void acquireLock(uint64_t);
extern void releaseLock(uint64_t);

uint64_t volatile text_lock = 0;

void printf(const char *str, ...)
{
	acquireLock(&text_lock);
	va_list args;
	va_start(args, str);

	while(*str)
	{
		if ('%' == *str)
		{
			str++;

			switch (*str)
			{
			case 'x':
				DebugPutHex(va_arg(args, uint64_t));
				break;
			case 'd':
				DebugPutDec(va_arg(args, uint64_t));
				break;
			case 's':
				DebugPuts(va_arg(args, char *));
				break;
			}
		}
		else
			putch(*str);
		str++;
	}

	va_end(args);
	releaseLock(&text_lock);
	text_lock = 0;
}
