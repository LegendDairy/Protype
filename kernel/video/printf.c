/* Pro-Type Kernel v0.2		*/
/* Print Formatted v0.1		*/
/* By LegendDairy		*/

#include <stdio.h>
#include <mutex.h>

uint32_t volatile text_lock = 0;

void printf(const char *str, ...)
{
	acquireLock((uint32_t*)&text_lock);
	__sync_synchronize();
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
	releaseLock((uint32_t*)&text_lock);
	__sync_synchronize();
}
