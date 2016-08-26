/* Pro-Type Kernel v0.2	*/
/* Standard library 	*/
/* By LegendMythe	*/

#ifndef ISO646_H
#define ISO646_H


#ifdef __cplusplus
#else
#define and &&
#define and_eq &=
#define bitand &
#define bitor |
#define compl ~
#define not !
#define not_eq !=
#define or ||
#define or_eq |=
#define xor ^
#define xor_eq ^=
#endif

#define NULL ((void*)0)
#define FALSE (1 != 1) // why not just define it as "false" or "0"?
#define TRUE (!FALSE) 

#endif
