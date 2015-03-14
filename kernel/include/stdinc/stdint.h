/* Pro-Type Kernel v1.3       */
/* x86 Typedefines            */
/* By LegendMythe             */

#ifndef STDINT_H
#define STDINT_H


// Some nice typedefs, to standardise sizes across platforms.
// These typedefs are written for 32-bit X86.
typedef unsigned long int u64int;
typedef          long int s64int;
typedef unsigned int	  u32int;
typedef          int	  s32int;
typedef unsigned short	  u16int;
typedef          short	  s16int;
typedef unsigned char	  u8int;
typedef          char     s8int;

typedef unsigned char	  byte;
typedef unsigned short	  word;
typedef unsigned int	  dword;
typedef unsigned long int qword;

typedef unsigned long int   uint64_t;
typedef unsigned int   uint32_t;
typedef          int   int32_t;
typedef unsigned short uint16_t;
typedef          short int16_t;
typedef unsigned char  uint8_t;
typedef          char  int8_t;

#endif
