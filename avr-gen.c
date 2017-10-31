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

#define AVR_DEBUG(...) fprintf(stderr, __VA_ARGS__)

#ifdef TARGET_DEFS_ONLY

/* number of available registers */
#define NB_REGS            32

/* a register can belong to several classes. The classes must be
   sorted from more general to more precise (see gv2() code which does
   assumptions on it). */
#define RC_BYTE    0x0001   /* generic byte register */
#define RC_INT     0x0002	/* generic integer register */
#define RC_FLOAT   0x0004	/* generic float register */
#define RC_LONG    RC_FLOAT
#define RC_LLONG   0x0008
/* Return registers */
#define RC_R24     0x0010
#define RC_R25     0x0020
#define RC_R23     0x0040
#define RC_R22     0x0080
#define RC_R21     0x0100
#define RC_R20     0x0200
#define RC_R19     0x0400
#define RC_R18     0x0800

#define RC_ADIW    0x0010   /* can be used in adiw instruction */

#define RC_BRET    RC_R24	/* function return: byte register */
#define RC_IRET    RC_R25	/* function return: second integer register */
#define RC_LRET    RC_R22	/* function return: second integer register */
#define RC_LLRET   RC_R18	/* function return: second integer register */
#define RC_FRET    RC_LRET	/* function return: float register */

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

static int reg_idx[] =  {
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    26,
    27,
    28,
    29,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    30,
    31,
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
#define REG_BRET TREG_R24	/* single word byte return register */
#define REG_IRET TREG_R25	/* single word int return register */
#define REG_LRET TREG_R26	/* second word return register (for long long) */
#define REG_FRET TREG_R24	/* float return register */

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
#define R_DATA_32   R_AVR_32
#define R_DATA_PTR  R_AVR_32
#define R_JMP_SLOT  R_C60_JMP_SLOT
#define R_COPY      R_C60_COPY

#define ELF_START_ADDR 0x00000400
#define ELF_PAGE_SIZE  0x1000

/******************************************************/
#else /* ! TARGET_DEFS_ONLY */
/******************************************************/
#include "tcc.h"

ST_DATA const int reg_classes[NB_REGS] = {
    /* R18 */ RC_BYTE | RC_R18,
    /* R19 */ RC_BYTE | RC_R19,
    /* R20 */ RC_BYTE | RC_R20,
    /* R21 */ RC_BYTE | RC_R21,
    /* R22 */ RC_BYTE | RC_R22,
    /* R23 */ RC_BYTE | RC_R23,
    /* R24 */ RC_BYTE | RC_R24,
    /* R25 */ RC_BYTE | RC_R25,
    /* R26 */ RC_BYTE,
    /* R27 */ RC_BYTE,
    /* R28 */ RC_BYTE,
    /* R29 */ RC_BYTE,
    /* R2  */ RC_BYTE,
    /* R3  */ RC_BYTE,
    /* R4  */ RC_BYTE,
    /* R5  */ RC_BYTE,
    /* R6  */ RC_BYTE,
    /* R7  */ RC_BYTE,
    /* R8  */ RC_BYTE,
    /* R9  */ RC_BYTE,
    /* R10 */ RC_BYTE,
    /* R11 */ RC_BYTE,
    /* R12 */ RC_BYTE,
    /* R13 */ RC_BYTE,
    /* R14 */ RC_BYTE,
    /* R15 */ RC_BYTE,
    /* R16 */ RC_BYTE,
    /* R17 */ RC_BYTE,
    /* R30 */ RC_BYTE,
    /* R31 */ RC_BYTE,
};

/******************************************************/

void g(int c)
{
    int ind1;
    ind1 = ind + 1;
    if (ind1 > cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    cur_text_section->data[ind] = c;
    ind = ind1;
}

void o(unsigned int c)
{
    while (c) {
        g(c);
        c = c >> 8;
    }
}

void o4(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4)
{
    g((c3 << 4) | c4);
    g((c1 << 4) | c2);
}

/*****************************************************/

/* output a symbol and patch all calls to it */
ST_FUNC void gsym_addr(int t, int a)
{
    AVR_DEBUG("# gsym_addr(t=%d, a=%d)\n", t, a);
    int n, *ptr;
    while (t) {
        ptr = (int *)(cur_text_section->data + t);
        n = *ptr; /* next value */
        *ptr = a - t - 4;
        t = n;
    }
}

ST_FUNC void gsym(int t)
{
    AVR_DEBUG("# gsym(t=%d)\n", t);
    gsym_addr(t, ind);
}

/* load 'r' from value 'sv' */
ST_FUNC void load(int r, SValue *sv)
{
    AVR_DEBUG("# load(r=%d, sv=%p)\n", r, sv);

    int v, t, ft, fc, fr;
    SValue v1;

    fr = sv->r;
    ft = sv->type.t;
    fc = sv->c.ul;

    v = fr & VT_VALMASK;
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
            AVR_DEBUG("ldd %s, Y%+d\n", reg_names[r], fc);
        }
        //gen_modrm(r, fr, sv->sym, fc);
    } else {
        if (v == VT_CONST) {
            AVR_DEBUG("ldi %s, %d\n", reg_names[r], fc);
            o4(0xE, (fc >> 4) & 0xF, reg_idx[r] - 0x10, fc & 0xF);
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
            AVR_DEBUG("mov %s, %s\n", reg_names[r], reg_names[v]);
            r = reg_idx[r];
            v = reg_idx[v];
            o4(0x2, 0xC | ((r >> 3) & 0x2) | ((v >> 4) & 1), r & 0xF, v & 0xF);
        }
    }
}


/* store register 'r' in lvalue 'v' */
ST_FUNC void store(int r, SValue * v)
{
    AVR_DEBUG("# store(r=%d, v=%p)\n", r, v);

    int fr, bt, ft, fc;

    ft = v->type.t;
    fc = v->c.ul;
    fr = v->r & VT_VALMASK;
    bt = ft & VT_BTYPE;
    printf("ft = %x, fc = %x, fr = %x, bt = %x\n", ft, fc, v->r, bt);

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

    if (fr == VT_CONST) {   /* Constant memory reference */
        AVR_DEBUG("ldi r30, %x\n", fc & 0xFF);
        o(0xE000 | (((fc >> 4) & 0xF) << 8) | ((30 - 0x10) << 4) | (fc & 0xF));

        fc >>= 8;
        AVR_DEBUG("ldi r31, %x\n", fc & 0xFF);
        o(0xE000 | (((fc >> 4) & 0xF) << 8) | ((31 - 0x10) << 4) | (fc & 0xF));

        AVR_DEBUG("st Z, %s\n", reg_names[r]);
        r = reg_idx[r];
        o4(0x8, 0x2 | ((r >> 4) & 1), r & 0xF, 0);
    } else if (v->r & VT_LVAL) {
        if (v->r & VT_LVAL_BYTE) {
            fc = -fc;

            AVR_DEBUG("ldd r31, Y%+d\n", fc);
            o4(0x8 | ((fc >> 4) & 2), ((fc >> 3) & 0xC) | ((31 >> 4) & 1), 31 & 0xF, 0x8 | (fc & 0x7));

            --fc;
            AVR_DEBUG("ldd r30, Y%+d\n", fc);
            o4(0x8 | ((fc >> 4) & 2), ((fc >> 3) & 0xC) | ((30 >> 4) & 1), 30 & 0xF, 0x8 | (fc & 0x7));

            AVR_DEBUG("st Z, %s\n", reg_names[r]);
            r = reg_idx[r];
            o4(0x8, 0x2 | ((r >> 4) & 1), r & 0xF, 0);
        } else {
            fc = -fc;
            AVR_DEBUG("std Y%+d, %s\n", fc, reg_names[r]);
            r = reg_idx[r];
            o4(0x8 | ((fc >> 5) & 1), 0x2 | ((fc >> 3) & 0x3) | ((r >> 4) & 1), r & 0xF, 0x8 | fc & 0x7);
        }
    } else if (fr == VT_LOCAL) {    /* Offset on stack */
        fc = -fc;
        AVR_DEBUG("std Y%+d, %s\n", fc, reg_names[r]);
        r = reg_idx[r];
        o(((0x8 | ((fc >> 5) & 1)) << 12) | ((0x2 | ((fc >> 3) & 0x3) | ((r >> 4) & 1)) << 8) | ((r & 0xF) << 4) | 0x8 | fc & 0x7);
    } else if (fr != r) {
        AVR_DEBUG("mov %s, %s\n", reg_names[fr], reg_names[r]);
        fr = reg_idx[fr];
        r = reg_idx[r];
        o4(0x2, 0xC | ((fr >> 3) & 0x2) | ((r >> 4) & 1), fr & 0xF, r & 0xF);
    }
}

/* 'is_jmp' is '1' if it is a jump */
ST_FUNC void gcall_or_jmp(int is_jmp)
{
    AVR_DEBUG("# gcall_or_jmp(is_jmp=%d)\n", is_jmp);
}

/* generate function call with address in (vtop->t, vtop->c) and free function
   context. Stack entry is popped */
ST_FUNC void gfunc_call(int nb_args)
{
    AVR_DEBUG("# gfunc_call(nb_args=%d)\n", nb_args);
}

static int arg_regs[] = {
    TREG_R25,
    TREG_R24,
    TREG_R23,
    TREG_R22,
    TREG_R21,
    TREG_R20,
    TREG_R19,
    TREG_R18,
    TREG_R17,
    TREG_R16,
    TREG_R15,
    TREG_R14,
    TREG_R13,
    TREG_R12,
    TREG_R11,
    TREG_R10,
    TREG_R9,
    TREG_R8,
};

/* generate function prolog of type 't' */
ST_FUNC void gfunc_prolog(CType *func_type)
{
    AVR_DEBUG("//------------------------------------//\n");
    AVR_DEBUG("# gfunc_prolog(func_type=%p)\n", func_type);

    Sym *sym;
    int n, addr, size, align;

    sym = func_type->ref;
    func_vt = sym->type;

    int reg_index = 0;

    addr = 1;
    int i = 0, r, off;

    while (sym = sym->next) {
        CType *type;
        type = &sym->type;
        size = type_size(type, &align);

        if (reg_index < 10) {   /* Arguments passed by registers */
            switch (size) {
            case 8:
                tcc_error("64-bit argument size is not yet supported");
            case 4:
                tcc_error("32-bit argument size is not yet supported");
            case 2: /* Choose the odd valued register */
                AVR_DEBUG("std Y%+d, %s\n", reg_index + 2, reg_names[arg_regs[reg_index]]);
                r = reg_idx[arg_regs[reg_index]];
                off = reg_index + 2;
                o4(0x8 | ((off >> 4) & 0x2), 0x2 | ((off >> 3) & 0x3) | ((r >> 4) & 1), (r & 0xF), 0x8 | off & 0x7);
            case 1: /* Choose the even-valued register */
                AVR_DEBUG("std Y%+d, %s\n", reg_index + 1, reg_names[arg_regs[reg_index + 1]]);
                r = reg_idx[arg_regs[reg_index + 1]];
                off = reg_index + 1;
                o4(0x8 | ((off >> 4) & 0x2), 0x2 | ((off >> 3) & 0x3) | ((r >> 4) & 1), (r & 0xF), 0x8 | off & 0x7);
            }
            reg_index += size;
        } else {
            tcc_error("arguments passed by stack is not yet supported");
        }

        sym_push(sym->v & ~SYM_FIELD, type, VT_LOCAL | lvalue_type(type->t), addr);

        AVR_DEBUG("# gfun_prolog: arg[%d] at stack ptr %i [%d bytes]\n", i++, addr, size);
        addr += size;
    }
}

/* generate function epilog */
ST_FUNC void gfunc_epilog(void)
{
    AVR_DEBUG("# gfun_epilog()\n");
    AVR_DEBUG("ret\n");
    o(0x9508);
    AVR_DEBUG("//------------------------------------//\n");
}

/* generate a jump to a label */
ST_FUNC int gjmp(int t)
{
    AVR_DEBUG("# gjmp(t=%d)\n", t);
}

/* generate a jump to a fixed address */
ST_FUNC void gjmp_addr(int a)
{
    AVR_DEBUG("# gjmp_addr(a=%d)\n", a);
}

/* generate a test. set 'inv' to invert test. Stack entry is popped */
ST_FUNC int gtst(int inv, int t)
{
    AVR_DEBUG("# gstat(inv=%d, t=%d)\n", inv, t);
}

/* generate an integer binary operation */
ST_FUNC void gen_opi(int op)
{
    AVR_DEBUG("# gen_opi(op=%d)\n", op);

    int r, _op;
    unsigned c, _c;

    switch (op) {
    case '+':
    case TOK_ADDC1: /* add with carry */
        _op = 0; /* Add */
    gen_op:
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
            /* Immediate Operand */
            int r2;
            vswap();
            r = gv(RC_INT);
            r2 = vtop->r2;
            vswap();
            c = vtop->c.ui;

            if (reg_idx[r] >= 24 && 
                reg_idx[r2] == reg_idx[r] + 1 &&
                c >> 6 < 2) { /* Could be done in utmost 2 ADIW operations */
                while (c & 0x3F) {
                    _c = c & 0x3F;
                    AVR_DEBUG("adiw %s, %d\n", reg_names[r], c & 0x3F);
                    c >>= 6;
                }
            } else if (reg_idx[r] >= 16) {   /* Can use subi and sbci */
                c = -c;
                AVR_DEBUG("subi %s, %d\n", reg_names[r], c & 0xFF);
                o4(0x5, (c >> 4) & 0xF, reg_idx[r], c & 0xF);
                c >>= 16;
                AVR_DEBUG("sbci %s, %d\n", reg_names[r2], (c >> 8) & 0xFF);
                o4(0x4, (c >> 4) & 0xF, reg_idx[r2], c & 0xF);
            } else {
                tcc_error("XXX: Operation on register unsupported");
            }
        } else {
            int r12, r21, r22;
            gv2(RC_INT, RC_INT);
            r = vtop[0].r;
            r12 = vtop[0].r2;
            r21 = vtop[-1].r;
            r22 = vtop[-1].r2;
            AVR_DEBUG("%s %s, %s\n", _op? "sub" : "add", reg_names[r], reg_names[r21]);
            AVR_DEBUG("%s %s, %s\n", _op? "sbc" : "adc", reg_names[r12], reg_names[r22]);
        }

        vtop--;
        if (op >= TOK_ULT && op <= TOK_GT) {
            vtop->r = VT_CMP;
            vtop->c.i = op;
        }
        break;
    case '-':
    case TOK_SUBC1: /* sub with carry generation */
        _op = 1;
        goto gen_op;
    }
}

/* generate a floating point operation 'v = t1 op t2' instruction. The
   two operands are guaranted to have the same floating point type */
ST_FUNC void gen_opf(int op)
{
    AVR_DEBUG("# gen_opf(op=%d)\n", op);
}


/* convert integers to fp 't' type. Must handle 'int', 'unsigned int'
   and 'long long' cases. */
ST_FUNC void gen_cvt_itof(int t)
{
    AVR_DEBUG("# gen_cvt_itof(t=%d)\n", t);
}

/* convert fp to int 't' type */
ST_FUNC void gen_cvt_ftoi(int t)
{
    AVR_DEBUG("# gen_cvt_ftoi(t=%d)\n", t);
}

/* convert from one floating point type to another */
ST_FUNC void gen_cvt_ftof(int t)
{
    AVR_DEBUG("# gen_cvt_ftof(t=%d)\n", t);
}

/* computed goto support */
ST_FUNC void ggoto(void)
{
    AVR_DEBUG("# ggoto()\n");
}

/* end of AVR code generator */
/*************************************************************/
#endif
/*************************************************************/
