#ifndef lint
static char sccsid[] = "@(#)c23.c	1.1 (Berkeley) %G%";
#endif

#include "c2.h"
#define readonly

readonly char revbr[] = {
	JNE, JEQ, JGT, JLT, JGE, JLE,
	JNE, JEQ, JHI, JLO, JHIS, JLOS,
	JBS, JBC, };

/* cursed be the preprocessor, whose impotence and stupidity
/* prevented this table being macro-generated from ../as/instrs
*/

readonly struct optab optab[] = {
"jbr",JBR,0,
"jeql",CBR,JEQ,
"jneq",CBR,JNE,
"jleq",CBR,JLE,
"jgeq",CBR,JGE,
"jlss",CBR,JLT,
"jgtr",CBR,JGT,
"jbc",CBR,JBC,
"jbs",CBR,JBS,
"jlequ",CBR,JLOS,
"jgequ",CBR,JHIS,
"jlssu",CBR,JLO,
"jgtru",CBR,JHI,
"adda",ADDA,LONG,
"addb2",ADD,U(BYTE,OP2),
"addb3",ADD,U(BYTE,OP3),
"addl2",ADD,U(LONG,OP2),
"addl3",ADD,U(LONG,OP3),
"addw2",ADD,U(WORD,OP2),
"addw3",ADD,U(WORD,OP3),
"aobleq",AOBLEQ,0,
"aoblss",AOBLSS,0,
"shal",SHAL,LONG,
"shar",SHAR,LONG,
"shll",SHL,LONG,
"shrl",SHR,LONG,
"shlq",SHL,QUAD,
"shrq",SHR,QUAD,
"bbc",CBR,JBC,
"bbs",CBR,JBS,
"bcc",CBR,JHIS,
"bcs",CBR,JLO,
"beql",CBR,JEQ,
"beqlu",CBR,JEQ,
"bgeq",CBR,JGE,
"bgequ",CBR,JHIS,
"bgtr",CBR,JGT,
"bgtru",CBR,JHI,
"andb2",AND,U(BYTE,OP2),
"andb3",AND,U(BYTE,OP3),
"andl2",AND,U(LONG,OP2),
"andl3",AND,U(LONG,OP3),
"andw2",AND,U(WORD,OP2),
"andw3",AND,U(WORD,OP3),
"orb2",OR,U(BYTE,OP2),
"orb3",OR,U(BYTE,OP3),
"orl2",OR,U(LONG,OP2),
"orl3",OR,U(LONG,OP3),
"orw2",OR,U(WORD,OP2),
"orw3",OR,U(WORD,OP3),
"bitb",BIT,BYTE,
"bitl",BIT,LONG,
"bitw",BIT,WORD,
"bleq",CBR,JLE,
"blequ",CBR,JLOS,
"blss",CBR,JLT,
"blssu",CBR,JLO,
"bneq",CBR,JNE,
"bnequ",CBR,JNE,
"brb",JBR,0,
"brw",JBR,0,
"bvc",CBR,0,
"bvs",CBR,0,
"callf",CALLF,0,
"calls",CALLS,0,
"casel",CASE,LONG,
"clrb",CLR,BYTE,
"clrl",CLR,LONG,
"clrw",CLR,WORD,
"cmpb",CMP,BYTE,
"cmpl",CMP,LONG,
"cmpw",CMP,WORD,
"cvtbl",CVT,U(BYTE,LONG),
"cvtbw",CVT,U(BYTE,WORD),
"cvtlb",CVT,U(LONG,BYTE),
"cvtlw",CVT,U(LONG,WORD),
"cvtwb",CVT,U(WORD,BYTE),
"cvtwl",CVT,U(WORD,LONG),
"decb",DEC,BYTE,
"decl",DEC,LONG,
"decw",DEC,WORD,
"divl2",DIV,U(LONG,OP2),
"divl3",DIV,U(LONG,OP3),
"ediv",EDIV,LONG,
#ifdef EMOD
"emod",EMOD,LONG,
#endif EMOD
"emul",EMUL,LONG,
"incb",INC,BYTE,
"incl",INC,LONG,
"incw",INC,WORD,
"jmp",JMP,0,
"mcomb",COM,BYTE,
"mcoml",COM,LONG,
"mcomw",COM,WORD,
"mnegb",NEG,BYTE,
"mnegl",NEG,LONG,
"mnegw",NEG,WORD,
"movab",MOVA,BYTE,
"moval",MOVA,LONG,
"movaw",MOVA,WORD,
"movb",MOV,BYTE,
"movl",MOV,LONG,
"movw",MOV,WORD,
"movzbl",MOVZ,U(BYTE,LONG),
"movzbw",MOVZ,U(BYTE,WORD),
"movzwl",MOVZ,U(WORD,LONG),
"mull2",MUL,U(LONG,OP2),
"mull3",MUL,U(LONG,OP3),
"pushab",PUSHA,BYTE,
"pushal",PUSHA,LONG,
"pushaw",PUSHA,WORD,
"pushb",PUSH,BYTE,
"pushl",PUSH,LONG,
"pushw",PUSH,WORD,
"ret",JBR,U(RET,2),
"ret#0",JBR,RET,
"ret#1",JBR,U(RET,1),
"ret#2",JBR,U(RET,2),
"suba",SUBA,LONG,
"subb2",SUB,U(BYTE,OP2),
"subb3",SUB,U(BYTE,OP3),
"subl2",SUB,U(LONG,OP2),
"subl3",SUB,U(LONG,OP3),
"subw2",SUB,U(WORD,OP2),
"subw3",SUB,U(WORD,OP3),
"tstb",TST,BYTE,
"tstl",TST,LONG,
"tstw",TST,WORD,
"xorb2",XOR,U(BYTE,OP2),
"xorb3",XOR,U(BYTE,OP3),
"xorl2",XOR,U(LONG,OP2),
"xorl3",XOR,U(LONG,OP3),
"xorw2",XOR,U(WORD,OP2),
"xorw3",XOR,U(WORD,OP3),
"movblk",MOVBLK,0,
"ldf",LDF,FLOAT,
"ldd",LDF,DOUBLE,
"lnf",LNF,FLOAT,
"lnd",LNF,DOUBLE,
"stf",STF,FLOAT,
"std",STF,DOUBLE,
"cmpf",CMPF,FLOAT,
"cmpd",CMPF,DOUBLE,
"cmpf2",CMPF2,FLOAT,
"cmpd2",CMPF2,DOUBLE,
"tstf",TSTF,FLOAT,
"tstd",TSTF,DOUBLE,
"pushd",PUSHD,DOUBLE,
"cvlf",CVLF,U(LONG,FLOAT),
"cvld",CVLF,U(LONG,DOUBLE),
"cvfl",CVFL,U(FLOAT,LONG),
"cvdl",CVFL,U(DOUBLE,LONG),
"ldfd",LDFD,U(FLOAT,DOUBLE),
"cvdf",CVDF,U(DOUBLE,FLOAT),
"negf",NEGF,FLOAT,
"negd",NEGF,DOUBLE,
"addf",ADDF,FLOAT,
"addd",ADDF,DOUBLE,
"subf",SUBF,FLOAT,
"subd",SUBF,DOUBLE,
"mulf",MULF,FLOAT,
"muld",MULF,DOUBLE,
"divf",DIVF,FLOAT,
"divd",DIVF,DOUBLE,
"sinf",SINF,FLOAT,
"cosf",COSF,FLOAT,
"atanf",ATANF,FLOAT,
"logf",LOGF,FLOAT,
"sqrtf",SQRTF,FLOAT,
"expf",EXPF,FLOAT,
".globl",EROU,0,
".text",TEXT,0,
".data",DATA,0,
".bss",BSS,0,
".align",ALIGN,0,
".word",WGEN,0,
".long",LGEN,0,
".set",SET,0,
".lcomm",LCOMM,0,
".comm",COMM,0,
".end",END,0,
"mfpr",MFPR,LONG,
"mtpr",MTPR,LONG,
"probe",PROBE,BYTE,
"adwc",0,0,
"movob",MOVO,BYTE,
"movow",MOVO,WORD,
"bbssi",0,0,
"bicpsw",0,0,
"bispsw",0,0,
"bpt",0,0,
"btcs",0,0,
"cmps2",0,0,
"cmps3",0,0,
"emul",0,0,
"ffc",0,0,
"ffs",0,0,
"halt",0,0,
"insque",0,0,
"kcall",0,0,
"ldpctx",0,0,
"loadr",0,0,
"movpsl",0,0,
"movs2",0,0,
"movs3",0,0,
"nop",0,0,
"rei",0,0,
"remque",0,0,
"sbwc",0,0,
"storer",0,0,
"svpctx",0,0,
"sinf",0,0,
"cosf",0,0,
"atanf",0,0,
"logf",0,0,
"sqrtf",0,0,
"expf",0,0,
0,0,0};
