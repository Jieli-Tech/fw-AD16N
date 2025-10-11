#ifndef midi_asm_h__
#define midi_asm_h__

//for pi32v2
#define MUL(macc,Rm,Rn)				asm volatile("%0 = %1 * %2(s)	\n\t" : "=&r"(macc) : "r"(Rm),"r"(Rn),"0"(macc) : )
#define MAC(macc,Rm,Rn)				asm volatile("%0 += %1 * %2(s)	\n\t" : "=&r"(macc) : "r"(Rm),"r"(Rn),"0"(macc) : )
#define MAS(macc,Rm,Rn)				asm volatile("%0 -= %1 * %2(s)	\n\t" : "=&r"(macc) : "r"(Rm),"r"(Rn),"0"(macc) : )
#define MACDIV(r,macc,d)			asm volatile("%0 = %0 / %2(s) \n\t" "%1 = %0.l \n\t" :"=&r"(macc),"=&r"(r):"r"(d),"0"(macc),"1"(d))
#define MRSI(Rm,macc,Rn)			asm volatile("%0 = %1 >> %2(s)	\n\t" : "=&r"(Rm) : "r"(macc),"r"(Rn),"0"(Rm) : )
#define MRSR(Rm,macc,Rn)			asm volatile("%0 = %1 >> %2(s)	\n\t" : "=&r"(Rm) : "r"(macc),"r"(Rn),"0"(Rm) : )
#define MULSI(Ro,macc,Rm,Rn,Rp)		{MUL(macc,Rm, Rn); MRSI(Ro, macc,Rp);}
#define MULRSR(r,macc,a,b,shift)	{MUL(macc,a,b);MRSR(r,macc,shift);}
#define MULRS(r,macc,a,b,shift)		{MUL(macc,a,b);MRSI(r,macc,shift);}
#define MACSR(macc,Rn)				asm volatile ("%0 >>= %1 \n\t":"=&r"(macc):"r"(Rn),"0"(macc):)
#define MACSL(macc,Rn)				asm volatile ("%0 <<= %1(sat) \n\t":"=&r"(macc):"r"(Rn),"0"(macc):)
#define MACGET(h,l,macc)			asm volatile ("%0 = %2.h \n\t" "%1 = %2.l \n\t": "=&r" (h),"=&r" (l) : "r"(macc),"0"(h),"1"(l) :)
#define VSHL(r,a,b)					r = b > 0 ? (a << b) : a >> (-b)

// 4byte多出来的指令
#define GET_UBYTE_VALUE(value,addr,shift)  asm volatile("%0 = b[%1 + %2](u) \n\t":"=&r"(value):"r"(addr),"r"(shift),"0"(value):)
#define GET_USHORT_VALUE(value,addr,shift) asm volatile("%0 = h[%1 + %2 << 1](u) \n\t":"=&r"(value):"r"(addr),"r"(shift),"0"(value):)
#endif
