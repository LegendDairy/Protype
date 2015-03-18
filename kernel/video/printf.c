/* Pro-Type Kernel v1.3	*/
/* Print Formatted	v0.1*/
/* By LegendDairy		*/

#include <stdio.h>

void printf(const char *str, ...)
{
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
}
