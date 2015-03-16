/* Pro-Type Kernel v1.3	*/
/* Debug Text Driver	*/
/* From the Internet	*/

#ifndef TEXT_H
#define TEXT_H

#include <common.h>
#include <stdint.h>
#include <string.h>

/** Clears the screen. **/
void DebugClearScreen(void);
/** Changes the foreground and background colour **/
void DebugSetTextColour(uint8_t foreground, uint8_t background);
/** Displays a null-terminated string on the screen. **/
void DebugPuts(char *str);
/** Prints a hexadecimal on the screen. **/
void DebugPutHex(uint32_t n);
/** Prints a decimal number on the screen. **/
void DebugPutDec(uint32_t n);
/** Prints an integer as a binairy number on the screen. **/
void DebugPutBin(uint32_t n);
/** Puts a single char on the screen **/
void putch(char c);
void outb(uint16_t port, uint8_t byte);
#endif
