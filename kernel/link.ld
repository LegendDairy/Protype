/* Pro-Type Kernel v1.3       */
/* Linkerscript               */
/* From www.wiki.osdev.org    */

ENTRY(main)
SECTIONS
{
	. = 1M;
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

}

