/* Pro-Type Kernel v0.2		*/
/* Debug Text Driver		*/
/* From the Internet		*/

#include <text.h>
#include <mutex.h>

/* These define our textpointer, our background and foreground
*  colors (attributes), and x and y cursor coordinates */
uint16_t *textmemptr = (uint16_t *)0xB8000;
uint16_t attrib = 0x0F;
uint32_t csr_x = 0, csr_y = 0;

void outb(uint16_t port, uint8_t byte)
{
    asm volatile("outb %1, %0":: "dN" (port), "a" (byte));
}

/* Scrolls the screen */
void scroll(void)
{
    unsigned blank, temp;

    /* A blank is defined as a space... we need to give it
    *  backcolor too */
    blank = 0x20 | (attrib << 8);

    /* Row 25 is the end, this means we need to scroll up */
    if(csr_y >= 25)
    {
        /* Move the current text chunk that makes up the screen
        *  back in the buffer by a line */
        temp = csr_y - 25 + 1;
        memcpy (textmemptr, textmemptr + temp * 80, (25 - temp) * 80 * 2);

        /* Finally, we set the chunk of memory that occupies
        *  the last line of text to our 'blank' character */
        memsetw (textmemptr + (25 - temp) * 80, blank, 80);
        csr_y = 25 - 1;
    }
}

/* Updates the hardware cursor: the little blinking line
*  on the screen under the last character pressed! */
void move_csr(void)
{
    unsigned temp;

    temp = csr_y * 80 + csr_x;

    outb(0x3D4, 14);
    outb(0x3D5, temp >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, temp);
}

/* Clears the screen */
void DebugClearScreen()
{
    unsigned blank;
    int i;

    /* Again, we need the 'short' that will be used to
    *  represent a space with color */
    blank = 0x20 | (attrib << 8);

    /* Sets the entire screen to spaces in our current
    *  color */
    for(i = 0; i < 25; i++)
        memsetw (textmemptr + i * 80, blank, 80);

    /* Update out virtual cursor, and then move the
    *  hardware cursor */
    csr_x = 0;
    csr_y = 0;
    move_csr();
}

/* Puts a single character on the screen */
void putch(char c)
{
    unsigned short *where;
    unsigned att = attrib << 8;

    /* Handle a backspace, by moving the cursor back one space */
    if(c == 0x08)
    {
        if(csr_x != 0) csr_x--;
    }
    /* Handles a tab by incrementing the cursor's x, but only
    *  to a point that will make it divisible by 8 */
    else if(c == 0x09)
    {
        csr_x = (csr_x + 8) & ~(8 - 1);
    }
    /* Handles a 'Carriage Return', which simply brings the
    *  cursor back to the margin */
    else if(c == '\r')
    {
        csr_x = 0;
    }
    /* We handle our newlines the way DOS and the BIOS do: we
    *  treat it as if a 'CR' was also there, so we bring the
    *  cursor to the margin and we increment the 'y' value */
    else if(c == '\n')
    {
        csr_x = 0;
        csr_y++;
    }
    /* Any character greater than and including a space, is a
    *  printable character. The equation for finding the index
    *  in a linear chunk of memory can be represented by:
    *  Index = [(y * width) + x] */
    else if(c >= ' ')
    {
        where = textmemptr + (csr_y * 80 + csr_x);
        *where = c | att;	/* Character AND attributes: color */
        csr_x++;
    }

    /* If the cursor has reached the edge of the screen's width, we
    *  insert a new line in there */
    if(csr_x >= 80)
    {
        csr_x = 0;
        csr_y++;
    }

    /* Scroll the screen if needed, and finally move the cursor */
    scroll();
    move_csr();
}

/* Uses the above routine to output a string... */
void DebugPuts(const char *text)
{
    int i;

    for (i = 0; i < strlen(text); i++)
    {
        putch(text[i]);
    }
}

/* Sets the forecolor and backcolor that we will use */
void DebugSetTextColour(uint8_t foreground, uint8_t background)
{
    /* Top 4 bytes are the background, bottom 4 bytes
    *  are the foreground color */
    attrib = (background << 4) | (foreground & 0x0F);
}

/** Prints a hexadecimal on the screen. **/
void DebugPutHex(uint64_t n)
{
	int64_t tmp;

	DebugPuts("0x");

	char noZeroes = 1;

	int i;
	for (i = 56; i > 0; i -= 4)
	{
		tmp = (n >> i) & 0xF;
		if (tmp == 0 && noZeroes != 0)
		{
			continue;
		}

		if (tmp >= 0xA)
		{
			noZeroes = 0;
			putch(tmp - 0xA + 'a');
		}
		else
		{
			noZeroes = 0;
			putch(tmp + '0');
		}
	}

	tmp = n & 0xF;
	if (tmp >= 0xA)
	{
		putch(tmp - 0xA + 'a');
	}
	else
	{
		putch(tmp + '0');
	}

}

/** Prints a decimal number on the screen. **/
void DebugPutDec(uint32_t n)
{

	if (n == 0)
	{
		putch('0');
		return;
	}

	int32_t acc = n;
	char c[32];
	int i = 0;
	while (acc > 0)
	{
		c[i] = '0' + acc % 10;
		acc /= 10;
		i++;
	}
	c[i] = 0;

	char c2[32];
	c2[i--] = 0;
	int j = 0;
	while (i >= 0)
	{
		c2[i--] = c[j++];
	}
	DebugPuts(c2);

}

/** Prints an integer as a binairy on the screen. **/
void DebugPutBin(uint32_t n)
{

	if (n == 0)
	{
		putch('0');
		return;
	}

	int32_t acc = n;
	char c[32];
	int i = 0;
	while (acc > 0)
	{
		c[i] = '0' + acc % 2;
		acc /= 2;
		i++;
	}
	c[i] = 0;

	char c2[32];
	c2[i--] = 0;
	int j = 0;
	while (i >= 0)
	{
		c2[i--] = c[j++];
	}
	DebugPuts(c2);

}
