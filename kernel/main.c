/* Pro-Type Kernel v1.3		*/
/* Kernel Entry Point		*/
/* By LegendMythe		*/

#include <text.h>
#include <idt.h>
#include <apic.h>
#include <stdarg.h>

extern uint64_t end;

void printf( const char * fmt, ... )
{
    va_list args;  /* Used as a pointer to the next variable argument. */

    va_start( args, fmt );  /* Initialize the pointer to arguments. */

    while (*fmt)
    {
        if ('%' == *fmt)
        {
            fmt ++;

            switch (*fmt)
            {
                 case 'x':  /* Format string says 'd'. */
                            /* ASSUME there is an integer at the args pointer. */
                     DebugPutHex(va_arg( args, uint64_t));
                     break;
		 case 'd':  /* Format string says 'd'. */
                            /* ASSUME there is an integer at the args pointer. */
                     DebugPutDec(va_arg( args, uint64_t));
                     break;
		case 's':  /* Format string says ''. */
                            /* ASSUME there is an integer at the args pointer. */
                     DebugPuts(va_arg( args, char *));
                     break;
             }
        }
        else 
        putch(*fmt);
        fmt++;
    }

    va_end( args );
}

int main(uint64_t *memorymap, uint64_t map_entries)
{

	DebugClearScreen();
	DebugSetTextColour(0x2, 0);
	DebugPuts("Protype v1.3\n");
	DebugSetTextColour(0xF, 0);
	DebugPutHex((uint64_t)&end);
	DebugPuts("\n");
	DebugPutHex((uint64_t)memorymap);
	DebugPuts("\n");
	DebugPutHex(map_entries);
        DebugPuts("\n");
	printf("%s %d %x","Hello World of Formatted Strings!\n", (uint64_t)5, 6);
	init_idt();
	setup_apic();
	
	//asm volatile("sti");
	
	for (;;);
	while(1);
	return 0;
}
