/*
 *  AVR 8-bit code generator for TCC
 * 
 *  Copyright (c) 2017 Mohamed Anwar <mohamed_anwar@opmbx.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef TARGET_DEFS_ONLY

/* number of available registers */
#define NB_REGS            12

/* a register can belong to several classes. The classes must be
   sorted from more general to more precise (see gv2() code which does
   assumptions on it). */
#define RC_INT     0x0001	/* generic integer register */
#define RC_FLOAT   0x0002	/* generic float register */
//#define RC_EAX     0x0004
//#define RC_ST0     0x0008
//#define RC_ECX     0x0010
//#define RC_EDX     0x0020
#define RC_IRET    RC_INT	/* function return: integer register */
#define RC_LRET    RC_INT	/* function return: second integer register */
#define RC_FRET    RC_INT	/* function return: float register */

/* pretty names for the registers */
enum {
    //TREG_R0 = 0,
    //TREG_R1,
    //TREG_R2,
    //TREG_R3,
    //TREG_R4,
    //TREG_R5,
    //TREG_R6,
    //TREG_R7,
    //TREG_R8,
    //TREG_R9,
    //TREG_R10,
    //TREG_R11,
    //TREG_R12,
    //TREG_R13,
    //TREG_R14,
    //TREG_R15,
    //TREG_R16,
    //TREG_R17,
    TREG_R18 = 0,
    TREG_R19,
    TREG_R20,
    TREG_R21,
    TREG_R22,
    TREG_R23,
    TREG_R24,
    TREG_R25,
    TREG_R26,
    TREG_R27,
    //TREG_R28,
    //TREG_R29,
    TREG_R30,
    TREG_R31,
};

/* return registers for function */
#define REG_IRET TREG_R24	/* single word int return register */
#define REG_LRET TREG_R25	/* second word return register (for long long) */
#define REG_FRET TREG_R22	/* float return register */

/* defined if function parameters must be evaluated in reverse order */
#define INVERT_FUNC_PARAMS

/* defined if structures are passed as pointers. Otherwise structures
   are directly pushed on stack. */
#define FUNC_STRUCT_PARAM_AS_PTR

/* pointer size, in bytes */
#define PTR_SIZE 2

/* long double size and alignment, in bytes */
#define LDOUBLE_SIZE  12
#define LDOUBLE_ALIGN 4
/* maximum alignment (for aligned attribute support) */
#define MAX_ALIGN     2

/******************************************************/
/* ELF defines */

#define EM_TCC_TARGET EM_C60

/* relocation type for 32 bit data relocation */
#define R_DATA_32   R_C60_32
#define R_DATA_PTR  R_C60_32
#define R_JMP_SLOT  R_C60_JMP_SLOT
#define R_COPY      R_C60_COPY

#define ELF_START_ADDR 0x00000400
#define ELF_PAGE_SIZE  0x1000

/******************************************************/
#else /* ! TARGET_DEFS_ONLY */
/******************************************************/
#include "tcc.h"

ST_DATA const int reg_classes[NB_REGS] = {
#if 0
    /* eax */ RC_INT | RC_FLOAT | RC_EAX, 
    // only allow even regs for floats (allow for doubles)
    /* ecx */ RC_INT | RC_ECX,
    /* edx */ RC_INT | RC_INT_BSIDE | RC_FLOAT | RC_EDX,
    // only allow even regs for floats (allow for doubles)
    /* st0 */ RC_INT | RC_INT_BSIDE | RC_ST0,
    /* A4  */ RC_C67_A4,
    /* A5  */ RC_C67_A5,
    /* B4  */ RC_C67_B4,
    /* B5  */ RC_C67_B5,
    /* A6  */ RC_C67_A6,
    /* A7  */ RC_C67_A7,
    /* B6  */ RC_C67_B6,
    /* B7  */ RC_C67_B7,
    /* A8  */ RC_C67_A8,
    /* A9  */ RC_C67_A9,
    /* B8  */ RC_C67_B8,
    /* B9  */ RC_C67_B9,
    /* A10  */ RC_C67_A10,
    /* A11  */ RC_C67_A11,
    /* B10  */ RC_C67_B10,
    /* B11  */ RC_C67_B11,
    /* A12  */ RC_C67_A10,
    /* A13  */ RC_C67_A11,
    /* B12  */ RC_C67_B10,
    /* B13  */ RC_C67_B11
#endif
};

/******************************************************/

/* output a symbol and patch all calls to it */
void gsym_addr(int t, int a)
{
    printf("gsym_addr(t=%d, a=%d)\n", t, a);
}

void gsym(int t)
{
    printf("gsym(t=%d)\n", t);
}

/* load 'r' from value 'sv' */
void load(int r, SValue * sv)
{
    printf("load(r=%d, sv=%p)\n", r, sv);
}


/* store register 'r' in lvalue 'v' */
void store(int r, SValue * v)
{
    printf("store(r=%d, v=%p)\n", r, v);
}

/* 'is_jmp' is '1' if it is a jump */
static void gcall_or_jmp(int is_jmp)
{
    printf("gcall_or_jmp(is_jmp=%d)\n", is_jmp);
}

/* generate function call with address in (vtop->t, vtop->c) and free function
   context. Stack entry is popped */
void gfunc_call(int nb_args)
{
    printf("gfunc_call(nb_args=%d)\n", nb_args);
}

/* generate function prolog of type 't' */
void gfunc_prolog(CType * func_type)
{
    printf("gfunc_prolog(func_type = %p)\n", func_type);
}

/* generate function epilog */
void gfunc_epilog(void)
{
    printf("gfun_epilog()\n");
}

/* generate a jump to a label */
int gjmp(int t)
{
    printf("gjmp(t=%d)\n", t);
}

/* generate a jump to a fixed address */
void gjmp_addr(int a)
{
    printf("gjmp_addr(a=%d)\n", a);
}

/* generate a test. set 'inv' to invert test. Stack entry is popped */
int gtst(int inv, int t)
{
    printf("gstat(inv=%d, t=%d)\n", inv, t);
}

/* generate an integer binary operation */
void gen_opi(int op)
{
    printf("gen_opi(op=%d)\n", op);
}

/* generate a floating point operation 'v = t1 op t2' instruction. The
   two operands are guaranted to have the same floating point type */
void gen_opf(int op)
{
    printf("gen_opf(op=%d)\n", op);
}


/* convert integers to fp 't' type. Must handle 'int', 'unsigned int'
   and 'long long' cases. */
void gen_cvt_itof(int t)
{
    printf("gen_cvt_itof(t=%d)\n", t);
}

/* convert fp to int 't' type */
void gen_cvt_ftoi(int t)
{
    printf("gen_cvt_ftoi(t=%d)\n", t);
}

/* convert from one floating point type to another */
void gen_cvt_ftof(int t)
{
    printf("gen_cvt_ftof(t=%d)\n", t);
}

/* computed goto support */
void ggoto(void)
{
    printf("ggoto()\n");
}

/* end of AVR code generator */
/*************************************************************/
#endif
/*************************************************************/
