
/* XDE v1.02 eXtended length disassembler engine, based on LDE/ADE engines */

#ifndef __XDE_H__
#define __XDE_H__

/* table indexes */

#define TBL_NORMAL 0             /* table index: normal opcodes             */
#define TBL_0F     256           /* table index: 0F-prefixed opcodes        */
#define TBL_80_83  512           /* table index: 80/81/82/83 /ttt           */
#define TBL_F6     520           /* table index: F6 /ttt                    */
#define TBL_F7     528           /* table index: F7 /ttt                    */
#define TBL_FE     536           /* table index: FE /ttt                    */
#define TBL_FF     544           /* table index: FF /ttt                    */
#define TBL_max    552              /* total entries in the xde_table[]        */
#define TBL_size   (TBL_max*4)   /* total xde_table[] size, in BYTEs        */

/* instruction flags */

#define C_SPECIAL  0             /* NOT A FLAG, must be zero                */
#define C_ADDR1    0x00000001    /* }                                       */
#define C_ADDR2    0x00000002    /* } when OR'ed, we have total addr size   */
#define C_ADDR4    0x00000004    /* }                                       */
#define C_MODRM    0x00000008    /* there is MOD<reg|ttt>R/M                */
#define C_SIB      0x00000010    /* there is SIB                            */
#define C_ADDR67   0x00000020    /* address size = defaddr                  */
#define C_DATA66   0x00000040    /* data size = defdata                     */
#define C_UNDEF    0x00000080    /* register values become UNDEFINED        */
#define C_DATA1    0x00000100    /* }                                       */
#define C_DATA2    0x00000200    /* } when OR'ed, we have total data size   */
#define C_DATA4    0x00000400    /* }                                       */
#define C_BAD      0x00000800    /* "bad", i.e. rarely used instruction     */
#define C_REL      0x00001000    /* it is jxx/call/...                      */
#define C_STOP     0x00002000    /* it is ret/jmp/...                       */
#define C_OPSZ8    0x00004000    /* operand size is 8 bits, otherwise 16/32 */
#define C_SRC_FL   0x00008000    /* put XSET_FL to src_set                  */
#define C_DST_FL   0x00010000    /* put XSET_FL to dst_set                  */
#define C_MOD_FL   (C_SRC_FL+C_DST_FL)
#define C_SRC_REG  0x00020000    /* src_set |= f(REG)                       */
#define C_DST_REG  0x00080000    /* dst_set |= f(REG)                       */
#define C_MOD_REG  (C_SRC_REG+C_DST_REG)
#define C_SRC_RM   0x00040000    /* src_set |= f(R/M)  can be used w/o modrm*/
#define C_DST_RM   0x00100000    /* dst_set |= f(R/M)  can be used w/o modrm*/
#define C_MOD_RM   (C_SRC_RM+C_DST_RM)
#define C_SRC_ACC  0x00200000    /* src_set |= XSET_AL || AX || EAX         */
#define C_DST_ACC  0x00400000    /* dst_set |= XSET_AL || AX || EAX         */
#define C_MOD_ACC  (C_SRC_ACC+C_DST_ACC)
#define C_SRC_R0   0x00800000    /* src_set |= f(opcode & 0x07)             */
#define C_DST_R0   0x01000000    /* dst_set |= f(opcode & 0x07)             */
#define C_MOD_R0   (C_SRC_R0+C_DST_R0)
#define C_PUSH     0x02000000    /* dst_set |= XSET_ESP | XSET_MEM          */
#define C_POP      0x04000000    /* dst_set |= XSET_ESP, src_set |= XSET_MEM*/
#define C_x_shift  27
#define C_x_00001  0x08000000
#define C_x_00010  0x10000000
#define C_x_00100  0x20000000
#define C_x_01000  0x40000000
#define C_x_10000  0x80000000
#define C_x_mask   0xF8000000
#define C_ERROR    0xFFFFFFFF    /* invalid instruction                     */

#define XDE_CMD(fl)  ((fl) & C_x_mask)      /* extract CMD from flags       */

#define C_CMD_other  ( 0 << C_x_shift)      /* 0=other, used if undefined   */
#define C_CMD_CALL   ( 1 << C_x_shift)
#define C_CMD_xxx    ( 2 << C_x_shift)
#define C_CMD_last   (31 << C_x_shift)

/* "object set", stored into src_set && dst_set */

#define XSET_AL    0x00000001
#define XSET_AH    0x00000002
#define XSET_AX    0x00000003
#define XSET_EAX   0x0000000F
#define XSET_CL    0x00000010
#define XSET_CH    0x00000020
#define XSET_CX    0x00000030
#define XSET_ECX   0x000000F0
#define XSET_DL    0x00000100
#define XSET_DH    0x00000200
#define XSET_DX    0x00000300
#define XSET_EDX   0x00000F00
#define XSET_BL    0x00001000
#define XSET_BH    0x00002000
#define XSET_BX    0x00003000
#define XSET_EBX   0x0000F000
#define XSET_SP    0x00010000
#define XSET_ESP   0x00030000
#define XSET_BP    0x00100000
#define XSET_EBP   0x00300000
#define XSET_SI    0x01000000
#define XSET_ESI   0x03000000
#define XSET_DI    0x10000000
#define XSET_EDI   0x30000000
#define XSET_ALL16 0x11113333
#define XSET_ALL32 0x3333FFFF
#define XSET_FL    0x00040000   /* flags                                    */
#define XSET_MEM   0x00080000   /* memory (any address)                     */
#define XSET_OTHER 0x00400000   /* other registers (seg,fpu,cr*,xmm,...)    */
#define XSET_DEV   0x00800000   /* io-ports                                 */
#define XSET_rsrv1 0x04000000
#define XSET_rsrv2 0x08000000
#define XSET_rsrv3 0x40000000
#define XSET_rsrv4 0x80000000
#define XSET_UNDEF 0xFFFFFFFF   /* all values become undefined              */

#pragma pack(push)
#pragma pack(1)

/* instruction is splitted into this structure by xde_disasm()              */
/* then, instruction can be merged from this structure by xde_asm()         */

struct xde_instr
{
  unsigned char  defaddr;        /* 2 or 4, depends on 0x67 prefix          */
  unsigned char  defdata;        /* 2 or 4, depends on 0x66 prefix          */
  unsigned long  len;            /* total instruction length                */
  unsigned long  flag;           /* set of C_xxx flags                      */
  unsigned long  addrsize;       /* size of address (or 0)                  */
  unsigned long  datasize;       /* size of data (or 0)                     */
  unsigned char  p_lock;         /* 0 or F0                                 */
  unsigned char  p_66;           /* 0 or 66                                 */
  unsigned char  p_67;           /* 0 or 67                                 */
  unsigned char  p_rep;          /* 0 or F2/F3                              */
  unsigned char  p_seg;          /* 0 or 26/2E/36/3E/64/65                  */
  unsigned char  opcode;         /* opcode byte (if 0x0F, opcode2 is set)   */
  unsigned char  opcode2;        /* if opcode==0x0F, contains 2nd opcode    */
  unsigned char  modrm;          /* modr/m byte (if C_MODRM)                */
  unsigned char  sib;            /* sib byte (if C_SIB)                     */
//
// example: add eax, ebx
//
// description        READ   WRITE  formula
//
// source regs        +      ?      src_set                      {eax|ebx}
// destination regs   ?      +      dst_set                      {eax|flags}
// modified regs      +      +      dst_set & src_set            {eax}
// read-only regs     +      -      src_set & ~dst_set           {ebx}
// write-only regs    -      +      dst_set & ~src_set           {flags}
//
  unsigned long  src_set;        /* SRC object set (instr. will READ 'em)   */
  unsigned long  dst_set;        /* DST object set (instr. will WRITE 'em)  */
//
  union
  {
  unsigned char  addr_b[8];      /* address bytes, size = addrsize          */
  unsigned short addr_w[4];
  unsigned long  addr_d[2];
  signed char    addr_c[8];
  signed short   addr_s[4];
  signed long    addr_l[2];
  };
  union
  {
  unsigned char  data_b[8];      /* data (imm) bytes, size = datasize       */
  unsigned short data_w[4];
  unsigned long  data_d[2];
  signed char    data_c[8];
  signed short   data_s[4];
  signed long    data_l[2];
  };
}; /* struct xde_instr */

#pragma pack(pop)

int __cdecl xde_disasm(/* IN */ unsigned char *opcode,
                       /* OUT */ struct xde_instr *diza);

int __cdecl xde_asm(/* OUT */ unsigned char* opcode,
                    /* IN */ struct xde_instr* diza);

#endif /* __XDE_H__ */

/* EOF */
