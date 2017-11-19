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
    TREG_R24 = 0,
    TREG_R25,
    TREG_R18,
    TREG_R19,
    TREG_R20,
    TREG_R21,
    TREG_R22,
    TREG_R23,
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
    24,
    25,
    18,
    19,
    20,
    21,
    22,
    23,
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
    "r24",
    "r25",
    "r18",
    "r19",
    "r20",
    "r21",
    "r22",
    "r23",
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
#define MAX_ALIGN     1

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
    /* R24 */ RC_BYTE | RC_R24,
    /* R25 */ RC_BYTE | RC_R25,
    /* R18 */ RC_BYTE | RC_R18,
    /* R19 */ RC_BYTE | RC_R19,
    /* R20 */ RC_BYTE | RC_R20,
    /* R21 */ RC_BYTE | RC_R21,
    /* R22 */ RC_BYTE | RC_R22,
    /* R23 */ RC_BYTE | RC_R23,
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

/* AVR instruction emitters */
/*
 *  Arithmetic and Logic instructions
 */
/* Add with Carry */
#define _ADC(d, r) o4(0x1, 0xC | (((r) >> 3) & 0x2) | ((d) >> 4), (d) & 0xF, (r) & 0xF)
/* Add without Carry */ 
#define _ADD(d, r) o4(0x0, 0xC | (((r) >> 3) & 0x2) | ((d) >> 4), (d) & 0xF, (r) & 0xF)
/* Add Immediate to Word */
#define _ADIW(d, k) o4(0x9, 0x6, (((k) >> 4) & 0xC) | ((d) & 0x3), (k) & 0xF)
/* Logical AND */
#define _AND(d, r) o4(0x2, (((r) >> 3) & 0x2) | ((d) >> 4), (d) & 0xF, (r) & 0xF)
/* Subtract with Carry */
#define _SBC(d, r) o4(0x0, 0x8 | (((r) >> 3) & 0x2) | ((d) >> 4), (d) & 0xF, (r) & 0xF)
/* Subtract without Carry */
#define _SUB(d, r) o4(0x1, 0x8 | (((r) >> 3) & 0x2) | ((d) >> 4), (d) & 0xF, (r) & 0xF)
/* Subtract Immediate with Carry */
#define _SBCI(d, k) o4(0x4, ((k) >> 4) & 0xF, (d), (k) & 0xF)
/* Subtract Immediate */
#define _SUBI(d, k) o4(0x5, ((k) >> 4) & 0xF, (d), (k) & 0xF)

/*
 *  Branch instructions
 */
/* Branch if Bit in SREG is Set */
#define _BRBS(s, k) o4(0xF, ((k) >> 5) & 0x3, ((k) >> 1) & 0xF, (((k) << 3) & 0x8) | ((s) & 0x7))
/* Branch if Equal */
#define _BREQ(k) _BRBS(0x1, k)
/* Branch if Less Than */
#define _BRLT(k) _BRBS(0x4, k)
/* Compare with Immediate */
#define _CPI(r, k) o4(0x3, (k) >> 4, (r), (k) & 0xFF);
/* Relative Call to Subroutine */
#define _RCALL(k) o((0xD << 12) | ((k) & 0xFFF))
/* Relative Jump */
#define _RJMP(k) o((0xC << 12) | ((k) & 0xFFF))
/* Return from Subroutine*/
#define _RET() o(0x9508)

/*
 *  Data Transfer instructions
 */
/* Load Immediate */
#define _LDI(d, k) o4(0xE, ((k) >> 4) & 0xF, (d) - 0x10, (k) & 0xF);
/* Load Indirect from Data Space to Register using Index Y */
#define _LDDYq(d, q) o4(0x8 | (((q) >> 4) & 0x2), (0xC & ((q) >> 1)) | ((d) >> 4), (d) & 0xF, 0x8 | ((q) & 0x7))
/* Copy Register */
#define _MOV(d, r) o4(0x2, 0xC | (((d) >> 3) & 0x2) | (((r) >> 4) & 1), (d) & 0xF, (r) & 0xF)
/* Store Indirect From Register to Data Space using Index Y */
#define _STDYq(r, q) o4(0x8 | (((q) >> 4) & 0x2), 0x2 | (((q) >> 1) & 0xC) | (((r) >> 4) & 1), (r) & 0xF, 0x8 | (q) & 0x7)
/* Store Indirect From Register to Data Space using Index Z */
#define _STZ(r) o4(0x8, 0x2 | (((r) >> 4) & 1), (r) & 0xF, 0)

/*****************************************************/

/* output a symbol and patch all calls to it */
ST_FUNC void gsym_addr(int t, int a)
{
    AVR_DEBUG("# gsym_addr(t=%d, a=%d)\n", t, a);
    unsigned short n, *ptr;
    while (t) {
        ptr = (unsigned short *)(cur_text_section->data + t);
        n = (*ptr >> 3) & 0x7F; /* next value */
        AVR_DEBUG("n = %X\n", n);
        *ptr |= ((a - t - 2) >> 1) << 3;    /* XXX?!! */
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
    AVR_DEBUG("fr = %X, ft=%X, fc=%d\n", fr, ft, fc);

    v = fr & VT_VALMASK;
    AVR_DEBUG("v = %X\n", v);

    if ((fr & VT_LVAL) && (v == VT_LOCAL)) {
        /* Load lvalue from stack */
        fc = fc < 0? -fc : fc; /* XXX */
        AVR_DEBUG("ldd %s, Y%+d\n", reg_names[r], fc);
        _LDDYq(reg_idx[r], fc);
    } else if (v == VT_CONST) {
        /* Load immediate */
        if (reg_idx[r] >= 16) {
            AVR_DEBUG("ldi %s, %d\n", reg_names[r], fc);
            _LDI(reg_idx[r], fc);
        } else {
            tcc_error("Unsupported\n");
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
        _MOV(reg_idx[r], reg_idx[v]);
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

    if (fr == VT_CONST) {   /* Constant memory reference */
        if (v->r & VT_SYM) {
            AVR_DEBUG("Relocation\n");
        }
        AVR_DEBUG("ldi r30, %x\n", fc & 0xFF);
        _LDI(30, fc & 0xFF);

        fc >>= 8;
        AVR_DEBUG("ldi r31, %x\n", fc & 0xFF);
        _LDI(31, fc & 0xFF);

        AVR_DEBUG("st Z, %s\n", reg_names[r]);
        _STZ(reg_idx[r]);
    } else if (fr == VT_LOCAL) {    /* Offset on stack */
        fc = fc < 0? -fc : fc; /* XXX */
        AVR_DEBUG("std Y%+d, %s\n", fc, reg_names[r]);
        _STDYq(reg_idx[r], fc);
    } else if (v->r & VT_LVAL) {
        if (v->r & VT_LVAL_BYTE) {
            fc = -fc;

            AVR_DEBUG("mov r31, %s\n", reg_names[fr & VT_VALMASK]);
            _MOV(31, reg_idx[fr & VT_VALMASK]);

            AVR_DEBUG("mov r30, %s\n", reg_names[v->r2]);
            _MOV(30, reg_idx[v->r2]);

            AVR_DEBUG("st Z, %s\n", reg_names[r]);
            _STZ(reg_idx[r]);
        } else {
            if ((fr & VT_VALMASK) == VT_LOCAL) {
                AVR_DEBUG("std Y%+d, %s\n", -fc, reg_names[r]);
                _STDYq(reg_idx[r], -fc);
            }
        }
    } else if (fr != r) {
        AVR_DEBUG("mov %s, %s\n", reg_names[fr], reg_names[r]);
        _MOV(reg_idx[fr], reg_idx[r]);
    }
}

/* 'is_jmp' is '1' if it is a jump */
ST_FUNC void gcall_or_jmp(int is_jmp)
{
    AVR_DEBUG("# gcall_or_jmp(is_jmp=%d)\n", is_jmp);
    int r;
    if ((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
        /* constant case */
        if (vtop->r & VT_SYM) {
            /* relocation case */
            greloc(cur_text_section, vtop->sym,
                   ind, R_AVR_13_PCREL);
        } else {
            AVR_DEBUG("reolcation PC\n");
            /* put an empty PC32 relocation */
            /*put_elf_reloc(symtab_section, cur_text_section,
                          ind + 1, R_X86_64_PC32, 0);*/
        }
        //oad(0xe8 + is_jmp, vtop->c.ul - 4); /* call/jmp im */
        if (is_jmp) {

        } else {
            AVR_DEBUG("rcall .%s\n", get_tok_str(vtop->sym->v, NULL));
            _RCALL(vtop->c.ul);
        }
    } else {
        AVR_DEBUG("indirect\n");
        /* otherwise, indirect call */
        //r = TREG_R11;
        //load(r, vtop);
        //o(0x41); /* REX */
        //o(0xff); /* call/jmp *r */
        //o(0xd0 + REG_VALUE(r) + (is_jmp << 4));
    }
}

#define NB_ARG_REGS 18
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

static int arg_regs_class[] = {
    RC_R25,
    RC_R24,
    RC_R23,
    RC_R22,
    RC_R21,
    RC_R20,
    RC_R19,
    RC_R18,
    //TREG_R17,
    //TREG_R16,
    //TREG_R15,
    //TREG_R14,
    //TREG_R13,
    //TREG_R12,
    //TREG_R11,
    //TREG_R10,
    //TREG_R9,
    //TREG_R8,
};

/* generate function call with address in (vtop->t, vtop->c) and free function
   context. Stack entry is popped */
ST_FUNC void gfunc_call(int nb_args)
{
    AVR_DEBUG("# gfunc_call(nb_args=%d)\n", nb_args);
    int size, type, align, args_size, i;
    int reg_index = 0;

    for (i = 0; i < nb_args; i++) {
        type = vtop->type.t & VT_BTYPE;
        if (type == VT_STRUCT ||
            type == VT_LDOUBLE) {
            tcc_error("Struct and long double arguments are not yet supported");
        //} else if (is_sse_float(vtop->type.t)) {
            //int j = --sse_reg;
            //if (j < 8) {
            //    gv(RC_FLOAT); /* only one float register */
            //    /* movaps %xmm0, %xmmN */
            //    o(0x280f);
            //    o(0xc0 + (sse_reg << 3));
            //}
        } else {
            if (reg_index < NB_ARG_REGS) {
                size = type_size(vtop, &align);
                switch (size) {
                case 8:
                    tcc_error("64-bit argument size is not yet supported");
                case 4:
                    tcc_error("32-bit argument size is not yet supported");
                case 2: /* Choose the odd valued register */
                    gv(arg_regs_class[reg_index]);
                case 1: /* Choose the even-valued register */
                    gv(arg_regs_class[reg_index+1]);
                }
                reg_index += size;
            } else {
                tcc_error("arguments passed by stack is not yet supported");
            }
        }
        vtop--;
    }

    gcall_or_jmp(0);
    vtop--;
}

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
    _RET();
    AVR_DEBUG("//------------------------------------//\n");
}

/* generate a jump to a label */
ST_FUNC int gjmp(int t)
{
    AVR_DEBUG("# gjmp(t=%d)\n", t);
    int r = ind;
    AVR_DEBUG("rjmp .%+d\n", t);
    _RJMP(t);
    return r;
}

/* generate a jump to a fixed address */
ST_FUNC void gjmp_addr(int a)
{
    AVR_DEBUG("# gjmp_addr(a=%d)\n", a);
}

/* generate a test. set 'inv' to invert test. Stack entry is popped */
ST_FUNC int gtst(int inv, int t)
{
    AVR_DEBUG("# gtst(inv=%d, t=%d)\n", inv, t);

    int v, *p;

    v = vtop->r & VT_VALMASK;
    AVR_DEBUG("v = %X\n", v);
    if (v == VT_CMP) {
        /* fast case : can jump directly since flags are set */
        switch ((vtop->c.ui)) {
        case TOK_GT:
            v = ind;
            AVR_DEBUG("brlt .%+d\n", t);
            _BRLT(t);
            t = v;
        }
        AVR_DEBUG("c %X\n", vtop->c.i);
        //g(0x0f);
        //t = psym((vtop->c.i - 16) ^ inv, t);
    } else if (v == VT_JMP || v == VT_JMPI) {
        /* && or || optimization */
        //if ((v & 1) == inv) {
        //    /* insert vtop->c jump list in t */
        //    p = &vtop->c.i;
        //    while (*p != 0)
        //        p = (int *)(cur_text_section->data + *p);
        //    *p = t;
        //    t = vtop->c.i;
        //} else {
        //    t = gjmp(t);
        //    gsym(vtop->c.i);
        //}
    } else {
        //if (is_float(vtop->type.t) || 
        //    (vtop->type.t & VT_BTYPE) == VT_LLONG) {
        //    vpushi(0);
        //    gen_op(TOK_NE);
        //}
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
            /* constant jmp optimization */
            if ((vtop->c.i != 0) != inv) 
                t = gjmp(t);

            /* Otherwise fall through */
        } else {
            int type = vtop->type.t;
            if (type == VT_BYTE) {
                v = gv(RC_BYTE);
                AVR_DEBUG("and %s, %s\n", reg_names[v], reg_names[v]);
                _AND(reg_idx[v], reg_idx[v]);
                int r = ind;
                AVR_DEBUG("breq .%+d\n", t);
                _BREQ(t);
                t = r;
            } else if (type == VT_INT) {

            } else {
                /* XXX */
            }
        //    v = gv(RC_INT);
        //    o(0x85);
        //    o(0xc0 + v * 9);
        //    g(0x0f);
        //    t = psym(0x85 ^ inv, t);
        }
    }

    --vtop;
    return t;
}

/* generate an integer binary operation */
ST_FUNC void gen_opi(int op)
{
    AVR_DEBUG("# gen_opi(op=%d)\n", op);

    int r, _op, t;
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
            t = vtop->type.t & VT_BTYPE;

            if (t == VT_BYTE) {
                r = gv(RC_BYTE);
            } else {
                r = gv(RC_INT);
                r2 = vtop->r2;
            }

            vswap();
            c = vtop->c.ui;

            if (t == VT_BYTE) {
                switch (_op) {
                case TOK_GT:
                    AVR_DEBUG("cpi %s, %d\n", reg_names[r], c+1);
                    _CPI(reg_idx[r], c+1);
                    break;
                default:
                    if (reg_idx[r] >= 16) {   /* Can use subi */
                        c = _op? -c : c;
                        AVR_DEBUG("subi %s, %d\n", reg_names[r], c & 0xFF);
                        _SUBI(reg_idx[r], c);
                    } else {
                        tcc_error("XXX: Operation on register unsupported");
                    }
                }
            } else {
                c = _op? -c : c;
                AVR_DEBUG("r = %d, r2 = %d, c = %d\n", reg_idx[r], reg_idx[r2], c);
                if (reg_idx[r] >= 24 && 
                    reg_idx[r2] == reg_idx[r] + 1 &&
                    c >> 6 < 2) { /* Could be done in utmost 2 ADIW operations */
                    while (c & 0x3F) {
                        _c = c & 0x3F;
                        AVR_DEBUG("adiw %s, %d\n", reg_names[r], c & 0x3F);
                        _ADIW(reg_idx[r], c);
                        c >>= 6;
                    }
                } else if (reg_idx[r] >= 16) {   /* Can use subi and sbci */
                    AVR_DEBUG("subi %s, %d\n", reg_names[r], c & 0xFF);
                    _SUBI(reg_idx[r], c & 0xFF);
                    c >>= 16;
                    AVR_DEBUG("sbci %s, %d\n", reg_names[r2], c & 0xFF);
                    _SBCI(reg_idx[r2], c & 0xFF);
                } else {
                    tcc_error("XXX: Operation on register unsupported");
                }
            }
        } else {
            if (vtop->type.t == VT_BYTE) {
                int r2;
                gv2(RC_BYTE, RC_BYTE);
                r = vtop[0].r;
                r2 = vtop[-1].r;
                if (!_op) {  /* Add */
                    AVR_DEBUG("add %s, %s\n", reg_names[r2], reg_names[r]);
                    _ADD(reg_idx[r2], reg_idx[r]);
                } else { /* Sub */
                    AVR_DEBUG("sub %s, %s\n", reg_names[r2], reg_names[r]);
                    _SUB(reg_idx[r2], reg_idx[r]);
                }
            } else {
                int r12, r21, r22;
                gv2(RC_INT, RC_INT);
                r = vtop[0].r;
                r12 = vtop[0].r2;
                r21 = vtop[-1].r;
                r22 = vtop[-1].r2;
                if (_op) {
                    AVR_DEBUG("%s %s, %s\n", "sub", reg_names[r], reg_names[r21]);
                    _SUB(reg_idx[r], reg_idx[r21]);
                    AVR_DEBUG("%s %s, %s\n", "sbc", reg_names[r12], reg_names[r22]);
                    _SBC(reg_idx[r12], reg_idx[r22]);
                } else {
                    AVR_DEBUG("%s %s, %s\n", "add", reg_names[r], reg_names[r21]);
                    _ADD(reg_idx[r], reg_idx[r21]);
                    AVR_DEBUG("%s %s, %s\n", "adc", reg_names[r12], reg_names[r22]);
                    _ADC(reg_idx[r12], reg_idx[r22]);
                }
            }
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
    case TOK_GT:
        _op = TOK_GT;
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
