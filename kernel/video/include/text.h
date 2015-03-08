/* Pro-Type Kernel v1.3	*/
/* Debug Text Driver	*/
/* From the Internet	*/

#ifndef TEXT_H
#define TEXT_H

#include <common.h>
#include <string.h>
#include <stdint.h>

/** Clears the screen. **/
void DebugClearScreen(void);
/** Changes the foreground and background colour **/
void DebugSetTextColour(u8int foreground, u8int background);
/** Displays a null-terminated string on the screen. **/
void DebugPuts(u8int *str);
/** Prints a hexadecimal on the screen. **/
void DebugPutHex(u32int n);
/** Prints a decimal number on the screen. **/
void DebugPutDec(u32int n);
/** Prints an integer as a binairy number on the screen. **/
void DebugPutBin(u32int n);
/** Puts a single char on the screen **/
void putch(char c);

#endif
