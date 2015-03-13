/* Pro-Type Kernel v1.3	*/
/* Debug Text Driver	*/
/* From the Internet	*/

#include <text.h>
#include <stdint.h>

u16int *video_memory = 0xB8000;
unsigned char x_csr = 0;
unsigned char y_csr = 0;
unsigned char attribute = 0x0F;

void outb(u16int port, u8int byte)
{
    asm volatile("outb %1, %0":: "dN" (port), "a" (byte));
}


/** Updates the hardware cursor. **/
void move_cursor(void)
{
	u16int tmp = (y_csr * 80) + x_csr;

	outb(0x3D4, 14);
	outb(0x3D5, tmp >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, tmp);

}
/** Clears the screen. **/
void DebugClearScreen(void)
{
	int i;
	for(i=0;i<80*25;i++)
	{
		video_memory[i]= 0 | (attribute << 8);
	}
	x_csr = 0;
	y_csr = 0;
	move_cursor();
}
/** Scrolls the text if y > 24. **/
void scroll (void)
{
    if (y_csr>=25)
    {
        u16int temp;
        temp = y_csr - 25 + 1;// Protection against y>25
        // Write line 2-23 to line 1-22, copies in bytes -> *2
        memcpy(video_memory, video_memory+80*temp,(25-temp)*80*2);
        // Make a blank line at y = 24
        memsetw(video_memory+80*(25-temp), (0 | (attribute << 8) ), 80);
        y_csr = 24;           // Get y back at 24.
	x_csr = 0;
    }
}
/** Changes the foreground and background colour **/
void DebugSetTextColour(u8int foreground,u8int background)
{
	attribute = (foreground | (background << 4) );
}
/** Puts a single char on the screen **/
void putch(char c)
{
u16int attword = attribute << 8;
    // Handle newline by moving cursor back to left and increasing the row
    if(c=='\n')
    {
       y_csr++;
       x_csr=0;
    }
    // Handle carriage return
    else if(c=='\r')
    {
        x_csr=0;
    }
    // Handle a backspace, by moving the cursor back one space
    else if (c == 0x08 && x_csr)
    {
       video_memory[y_csr*80+x_csr] = (0 | (attribute << 8) );
       x_csr--;
    }
    // Handle a tab by increasing the cursor's X, but only to a point
    // where it is divisible by 8.
    else if (c == 0x09)
    {
       x_csr = (x_csr+8) & ~(8-1);
    }
    // Handle any other printable character.
    else if(c >= ' ')
    {
        u16int val = c | attword;
        video_memory[x_csr+y_csr*80] = val;
        x_csr++;
    }
    if (x_csr>=80)
    {
        y_csr++;
        x_csr=0;
    }
    scroll();
    move_cursor();
}

void DebugPuts(u8int *str)
{
	int i;
	for (i = 0; i<strlen(str); i++)
	{
		putch(str[i]);
	}
}

/** Prints a hexadecimal on the screen. **/
void DebugPutHex(u32int n)
{
	s32int tmp;

	DebugPuts("0x");

	char noZeroes = 1;

	int i;
	for (i = 28; i > 0; i -= 4)
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
void DebugPutDec(u32int n)
{

	if (n == 0)
	{
		putch('0');
		return;
	}

	s32int acc = n;
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
void DebugPutBin(u32int n)
{

	if (n == 0)
	{
		putch('0');
		return;
	}

	s32int acc = n;
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
