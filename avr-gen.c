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
#define NB_REGS            7

/* a register can belong to several classes. The classes must be
   sorted from more general to more precise (see gv2() code which does
   assumptions on it). */
#define RC_INT     0x0001	/* generic integer register */
#define RC_FLOAT   0x0002	/* generic float register */
#define RC_R24     0x0004
//#define RC_ST0     0x0008
//#define RC_ECX     0x0010
//#define RC_EDX     0x0020
#define RC_IRET    RC_R24	/* function return: integer register */
#define RC_LRET    RC_INT	/* function return: second integer register */
#define RC_FRET    RC_INT	/* function return: float register */

/* pretty names for the registers */
enum {
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
    TREG_R28,
    TREG_R29,
    TREG_R2,
    TREG_R3,
    TREG_R4,
    TREG_R5,
    TREG_R6,
    TREG_R7,
    TREG_R8,
    TREG_R9,
    TREG_R10,
    TREG_R11,
    TREG_R12,
    TREG_R13,
    TREG_R14,
    TREG_R15,
    TREG_R16,
    TREG_R17,
    TREG_R30,
    TREG_R31,
};

static char *reg_names[] =  {
    "r18",
    "r19",
    "r20",
    "r21",
    "r22",
    "r23",
    "r24",
    "r25",
    "r26",
    "r27",
    "r28",
    "r29",
    "r2",
    "r3",
    "r4",
    "r5",
    "r6",
    "r7",
    "r8",
    "r9",
    "r10",
    "r11",
    "r12",
    "r13",
    "r14",
    "r15",
    "r16",
    "r17",
    "r30",
    "r31",
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

#define EM_TCC_TARGET EM_AVR

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
    /* R18 */ RC_INT,
    /* R19 */ RC_INT,
    /* R20 */ RC_INT,
    /* R21 */ RC_INT,
    /* R22 */ RC_INT,
    /* R23 */ RC_INT,
    /* R24 */ RC_INT | RC_IRET,
    /* R25 */ //RC_INT,
    /* R26 */ //RC_INT,
    /* R27 */ //RC_INT,
    /* R28 */ //RC_INT,
    /* R29 */ //RC_INT,
    /* R2  */ //RC_INT,
    /* R3  */ //RC_INT,
    /* R4  */ //RC_INT,
    /* R5  */ //RC_INT,
    /* R6  */ //RC_INT,
    /* R7  */ //RC_INT,
    /* R8  */ //RC_INT,
    /* R9  */ //RC_INT,
    /* R10 */ //RC_INT,
    /* R11 */ //RC_INT,
    /* R12 */ //RC_INT,
    /* R13 */ //RC_INT,
    /* R14 */ //RC_INT,
    /* R15 */ //RC_INT,
    /* R16 */ //RC_INT,
    /* R17 */ //RC_INT,
    /* R30 */ //RC_INT,
    /* R31 */ //RC_INT,
};

/******************************************************/

/* output a symbol and patch all calls to it */
void gsym_addr(int t, int a)
{
    printf("# gsym_addr(t=%d, a=%d)\n", t, a);
}

void gsym(int t)
{
    printf("# gsym(t=%d)\n", t);
}

/* load 'r' from value 'sv' */
ST_FUNC void load(int r, SValue *sv)
{
    printf("# load(r=%d, sv=%p)\n", r, sv);

    int v, t, ft, fc, fr;
    SValue v1;

    fr = sv->r;
    ft = sv->type.t;
    fc = sv->c.ul;

    v = fr & VT_VALMASK;
    //printf("fr=%x, ft=%d, fc=%d, v=%x\n", fr, ft, fc, v);
    if (fr & VT_LVAL) {
        if (v == VT_LLOCAL) {
            v1.type.t = VT_INT;
            v1.r = VT_LOCAL | VT_LVAL;
            v1.c.ul = fc;
            fr = r;
            if (!(reg_classes[fr] & RC_INT))
                fr = get_reg(RC_INT);
            load(fr, &v1);
        }
        if ((ft & VT_BTYPE) == VT_FLOAT) {
            //o(0xd9); /* flds */
            //r = 0;
        } else if ((ft & VT_BTYPE) == VT_DOUBLE) {
            //o(0xdd); /* fldl */
            //r = 0;
        } else if ((ft & VT_BTYPE) == VT_LDOUBLE) {
            //o(0xdb); /* fldt */
            //r = 5;
        } else if ((ft & VT_TYPE) == VT_BYTE) {
            //o(0xbe0f);   /* movsbl */
        } else if ((ft & VT_TYPE) == (VT_BYTE | VT_UNSIGNED)) {
            //o(0xb60f);   /* movzbl */
        } else if ((ft & VT_TYPE) == VT_SHORT) {
            //o(0xbf0f);   /* movswl */
        } else if ((ft & VT_TYPE) == (VT_SHORT | VT_UNSIGNED)) {
            //o(0xb70f);   /* movzwl */
        } else {
            printf("ldd %s, Y%+d\n", reg_names[r], fc);
            //o(0x8b);     /* movl */
        }
        //gen_modrm(r, fr, sv->sym, fc);
    } else {
        if (v == VT_CONST) {
            printf("ldi %s, %d\n", reg_names[r], fc);
            //o(0xb8 + r); /* mov $xx, r */
            //gen_addr32(fr, sv->sym, fc);
        } else if (v == VT_LOCAL) {
            if (fc) {
                //o(0x8d); /* lea xxx(%ebp), r */
                //gen_modrm(r, VT_LOCAL, sv->sym, fc);
            } else {
                //o(0x89);
                //o(0xe8 + r); /* mov %ebp, r */
            }
        } else if (v == VT_CMP) {
            //oad(0xb8 + r, 0); /* mov $0, r */
            //o(0x0f); /* setxx %br */
            //o(fc);
            //o(0xc0 + r);
        } else if (v == VT_JMP || v == VT_JMPI) {
            //t = v & 1;
            //oad(0xb8 + r, t); /* mov $1, r */
            //o(0x05eb); /* jmp after */
            //gsym(fc);
            //oad(0xb8 + r, t ^ 1); /* mov $0, r */
        } else if (v != r) {
            printf("mov %s, %s\n", reg_names[r], reg_names[v]);
            //o(0x89);
            //o(0xc0 + r + v * 8); /* mov v, r */
        }
    }
}


/* store register 'r' in lvalue 'v' */
void store(int r, SValue * v)
{
    printf("# store(r=%d, v=%p)\n", r, v);

    int fr, bt, ft, fc;

    ft = v->type.t;
    fc = v->c.ul;
    fr = v->r & VT_VALMASK;
    bt = ft & VT_BTYPE;

    if (bt == VT_FLOAT) {
        //o(0xd9); /* fsts */
        //r = 2;
    } else if (bt == VT_DOUBLE) {
        //o(0xdd); /* fstpl */
        //r = 2;
    } else if (bt == VT_LDOUBLE) {
        //o(0xc0d9); /* fld %st(0) */
        //o(0xdb); /* fstpt */
        //r = 7;
    } else {
        if (bt == VT_SHORT) {
            //o(0x66);
        } if (bt == VT_BYTE || bt == VT_BOOL) {
            //o(0x88);
        } else {
            //o(0x89);
        }
    }
    if (fr == VT_CONST || fr == VT_LOCAL || (v->r & VT_LVAL)) {
        printf("std Y%+d, %s\n", fc, reg_names[r]);
        //gen_modrm(r, v->r, v->sym, fc);
    } else if (fr != r) {
        printf("mov %s, %s\n", reg_names[fr], reg_names[r]);
        //o(0xc0 + fr + r * 8); /* mov r, fr */
    }
}

/* 'is_jmp' is '1' if it is a jump */
static void gcall_or_jmp(int is_jmp)
{
    printf("# gcall_or_jmp(is_jmp=%d)\n", is_jmp);
}

/* generate function call with address in (vtop->t, vtop->c) and free function
   context. Stack entry is popped */
void gfunc_call(int nb_args)
{
    printf("# gfunc_call(nb_args=%d)\n", nb_args);
}

/* generate function prolog of type 't' */
void gfunc_prolog(CType *func_type)
{
    printf("//------------------------------------//\n");
    printf("# gfunc_prolog(func_type=%p)\n", func_type);

    Sym *sym;
    int n, addr, size, align;

    sym = func_type->ref;
    func_vt = sym->type;

    addr = 0;
    int i = 0;
    while (sym = sym->next) {
        CType *type;
        type = &sym->type;
        size = type_size(type, &align);
        //addr = (addr + align - 1) / align * align;
        sym_push(sym->v & ~SYM_FIELD, type, VT_LOCAL | lvalue_type(type->t), addr);
        printf("# gfun_prolog: arg[%d] at stack ptr %i [%d bytes]\n", i++, addr, size);
        addr += size;
    }
}

/* generate function epilog */
void gfunc_epilog(void)
{
    printf("# gfun_epilog()\n");
    printf("ret\n");
    printf("//------------------------------------//\n");
}

/* generate a jump to a label */
int gjmp(int t)
{
    printf("# gjmp(t=%d)\n", t);
}

/* generate a jump to a fixed address */
void gjmp_addr(int a)
{
    printf("# gjmp_addr(a=%d)\n", a);
}

/* generate a test. set 'inv' to invert test. Stack entry is popped */
int gtst(int inv, int t)
{
    printf("# gstat(inv=%d, t=%d)\n", inv, t);
}

/* generate an integer binary operation */
void gen_opi(int op)
{
    printf("# gen_opi(op=%d)\n", op);

    int r, c;

    switch (op) {
    case '+':
    case TOK_ADDC1: /* add with carry */
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
            /* Immediate Operand */
            vswap();
            r = gv(RC_INT);
            vswap();
            c = vtop->c.i;
            printf("addiw %s, %d\n", reg_names[r], c);
        } else {

        }

        vtop--;
        if (op >= TOK_ULT && op <= TOK_GT) {
            vtop->r = VT_CMP;
            vtop->c.i = op;
        }
        break;
    }
}

/* generate a floating point operation 'v = t1 op t2' instruction. The
   two operands are guaranted to have the same floating point type */
void gen_opf(int op)
{
    printf("# gen_opf(op=%d)\n", op);
}


/* convert integers to fp 't' type. Must handle 'int', 'unsigned int'
   and 'long long' cases. */
void gen_cvt_itof(int t)
{
    printf("# gen_cvt_itof(t=%d)\n", t);
}

/* convert fp to int 't' type */
void gen_cvt_ftoi(int t)
{
    printf("# gen_cvt_ftoi(t=%d)\n", t);
}

/* convert from one floating point type to another */
void gen_cvt_ftof(int t)
{
    printf("# gen_cvt_ftof(t=%d)\n", t);
}

/* computed goto support */
void ggoto(void)
{
    printf("# ggoto()\n");
}

/* end of AVR code generator */
/*************************************************************/
#endif
/*************************************************************/
