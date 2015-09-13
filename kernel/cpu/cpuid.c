/* Pro-Type Kernel v1.3		*/
/* CPUID Inline functions	*/
/* By LegendMythe			*/

#include <cpuid.h>

static inline void cpuid(int code, uint32_t *a, uint32_t *d)
{
	asm volatile inline("cpuid": "= a"(*a), " = d"(*d) : "a"(code) : "ecx", "ebx");
}