int main(void)
{

volatile unsigned char *videoram = (unsigned char *)0xB8000;
   videoram[0] = 66; /* character 'A' */
   videoram[1] = 0x07; /* light grey (7) on black (0). */

return 0;
}
