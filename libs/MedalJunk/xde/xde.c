
/* XDE v1.02 eXtended length disassembler engine, based on LDE/ADE engines */

#include "xde.h"
#include "xdetbl.c"

#define XDE32_DEFAULT_ADDR      32      /* 16 or 32, changed by 0x67 prefix */
#define XDE32_DEFAULT_DATA      32      /* 16 or 32, changed by 0x66 prefix */

unsigned long /*__inline*/ reg2xset(int reg_sz /*1,2,4*/, unsigned long reg)
{
  if (reg_sz == 1)
    return reg == 0 ? XSET_AL :
           reg == 1 ? XSET_CL :
           reg == 2 ? XSET_DL :
           reg == 3 ? XSET_BL :
           reg == 4 ? XSET_AH :
           reg == 5 ? XSET_CH :
           reg == 6 ? XSET_DH :
                      XSET_BH;
  else
  if (reg_sz == 2)
    return reg == 0 ? (XSET_AX|XSET_AL|XSET_AH) :
           reg == 1 ? (XSET_CX|XSET_CL|XSET_CH) :
           reg == 2 ? (XSET_DX|XSET_DL|XSET_DH) :
           reg == 3 ? (XSET_BX|XSET_BL|XSET_BH) :
           reg == 4 ? XSET_SP :
           reg == 5 ? XSET_BP :
           reg == 6 ? XSET_SI :
                      XSET_DI;
  else /* if (reg_sz == 4) */
    return reg == 0 ? (XSET_EAX|XSET_AX|XSET_AL|XSET_AH) :
           reg == 1 ? (XSET_ECX|XSET_CX|XSET_CL|XSET_CH) :
           reg == 2 ? (XSET_EDX|XSET_DX|XSET_DL|XSET_DH) :
           reg == 3 ? (XSET_EBX|XSET_BX|XSET_BL|XSET_BH) :
           reg == 4 ? (XSET_ESP|XSET_SP) :
           reg == 5 ? (XSET_EBP|XSET_BP) :
           reg == 6 ? (XSET_ESI|XSET_SI) :
                      (XSET_EDI|XSET_DI);
} /* reg2xset() */

/* returns:                                                                 */
/*   0 if error                                                             */
/*   instruction length (== diza.len) if OK                                 */

int __cdecl xde_disasm(/* IN */ unsigned char *opcode,
                       /* OUT */ struct xde_instr *diza)
{
  unsigned char c, *p;
  unsigned long flag, a, d, i, xset;
  unsigned long mod, reg, rm, index, base;

  p = opcode;

  memset(diza, 0x00, sizeof(struct xde_instr));

  diza->defdata = XDE32_DEFAULT_ADDR/8;
  diza->defaddr = XDE32_DEFAULT_DATA/8;

  flag = 0;

  if (*(unsigned short*)p == 0x0000) flag |= C_BAD;
  if (*(unsigned short*)p == 0xFFFF) flag |= C_BAD;

  while(1)
  {

    c = *p++;

    if (c == 0x66)
    {
      if (diza->p_66 != 0) flag |= C_BAD;        /* twice */
      diza->p_66 = 0x66;
      diza->defdata = (XDE32_DEFAULT_DATA^32^16)/8;
      continue;
    }

    if (c == 0x67)
    {
      if (diza->p_67 != 0) flag |= C_BAD;        /* twice */
      diza->p_67 = 0x67;
      diza->defaddr = (XDE32_DEFAULT_ADDR^32^16)/8;
      continue;
    }

    if ((c == 0x26) || (c == 0x2E) || (c == 0x36) || (c == 0x3E) ||
        (c == 0x64) || (c == 0x65))
    {
      if (diza->p_seg != 0) flag |= C_BAD;       /* twice */
      diza->p_seg = c;
      continue;
    }

    if ((c == 0xF2) || (c == 0xF3))
    {
      if (diza->p_rep != 0) flag |= C_BAD;       /* twice */
      diza->p_rep = c;
      continue;
    }

    if (c == 0xF0)
    {
      if (diza->p_lock != 0) flag |= C_BAD;      /* twice */
      diza->p_lock = c;
      continue;
    }

    break;

  } /* do while prefix found */

  /* check after all prefixes, because 66 and F2/F3 can be swapped */
  if (diza->p_rep)
  {
    xset = XSET_FL | (diza->defdata == 2 ? XSET_CX : XSET_ECX);
    diza->src_set |= xset;
    diza->dst_set |= xset;
  }

  /* instruction flags by opcode */
  flag |= xde_table[ TBL_NORMAL + c ];

  if (flag == C_ERROR) return 0;

  diza->opcode = c;

  if (c == 0x0F)
  {
    c = *p++;

    /* 2nd opcode */

    flag |= xde_table[ TBL_0F + c ]; /* from the 2nd half of the table */

    if (flag == C_ERROR) return 0;

    diza->opcode2 = c;

    /* 0F-prefixed "special" opcodes */

    if ( (c == 0xB2) || (c == 0xB4) || (c == 0xB5) || /* lss/lfs/lgs reg,r/m */
         (c == 0xA1) || (c == 0xA9) )                 /* pop fs, gs */
      diza->dst_set |= XSET_OTHER;

    if ((c == 0xA0) || (c == 0xA8))  /* push fs, gs */
      diza->src_set |= XSET_OTHER;

    if (c == 0xA2) /* cpuid */
    {
      diza->src_set |= XSET_EAX;
      diza->dst_set |= XSET_EAX|XSET_EBX|XSET_ECX|XSET_EDX;
    }

    if ((c == 0xA5) || (c == 0xAD))   /* shld/shrd r/m, reg, CL */
      diza->src_set |= XSET_CL;

  }
  else
  {
    /* "special" opcodes */

    if ((c == 0xA4) || (c == 0xA5) || /* movsb, movsd */
        (c == 0xA6) || (c == 0xA7))   /* cmpsb, cmpsd */
    {
      /* 66 or 67 ? */
      xset = diza->defaddr == 2 ? (XSET_SI | XSET_DI) : (XSET_ESI | XSET_EDI);
      diza->src_set |= xset;
      diza->dst_set |= xset;
    }

    if ((c == 0xAC) || (c == 0xAD)) /* lodsb, lodsd */
    {
      xset = diza->defaddr == 2 ? XSET_SI : XSET_ESI;
      diza->src_set |= xset;
      diza->dst_set |= xset;
    }

    if ((c == 0xAA) || (c == 0xAB) || /* stosb, stosd */
        (c == 0xAE) || (c == 0xAF))   /* scasb, scasd */
    {
      xset = diza->defaddr == 2 ? XSET_DI : XSET_EDI;
      diza->src_set |= xset;
      diza->dst_set |= xset;
    }

    if ((c == 0x6C) || (c == 0x6D)) /* insb, insd */
    {
      /* 66/67 ? */
      xset = XSET_DEV | (diza->defaddr == 2 ? XSET_DI : XSET_EDI);
      diza->src_set |= xset | XSET_DX;
      diza->dst_set |= xset;
    }
    if ((c == 0x6E) || (c == 0x6F)) /* outsb, outsd */
    {
      xset = XSET_DEV | (diza->defaddr == 2 ? XSET_SI : XSET_ESI);
      diza->src_set |= xset | XSET_DX;
      diza->dst_set |= xset;
    }

    if (c == 0x9E) /* sahf */
      diza->src_set |= XSET_AH;
    if (c == 0x9F) /* lahf */
      diza->dst_set |= XSET_AH;

    if (c == 0x98) /* cbw, cwde */
    {
      diza->src_set |= diza->defdata == 2 ? XSET_AL : XSET_AX;
      diza->dst_set |= diza->defdata == 2 ? XSET_AX : XSET_EAX;
    }
    if (c == 0x99) /* cwd, cdq */
    {
      diza->src_set |= diza->defdata == 2 ? XSET_AX : XSET_EAX;
      diza->dst_set |= diza->defdata == 2 ? XSET_DX : XSET_EDX;
    }

    if ((c == 0x37) || (c == 0x3F)) /* aaa, aas */
    {
      diza->src_set |= XSET_AH;
      diza->dst_set |= XSET_AH;
    }

    if ((c == 0xD4) || (c == 0xD5)) /* aam xx, aad xx */
    {
      diza->src_set |= c == 0xD4 ? XSET_AL : XSET_AX;
      diza->dst_set |= XSET_AX;
    }

    if (c == 0x60) /* pusha */
      diza->src_set |= diza->defdata == 2 ? XSET_ALL16 : XSET_ALL32;
    if (c == 0x61) /* popa */
      diza->dst_set |= diza->defdata == 2 ? XSET_ALL16 : XSET_ALL32;

    if ((c == 0xE4) || (c == 0xE5) || /* in al|(e)ax, NN */
        (c == 0xE6) || (c == 0xE7))   /* out NN, al|(e)ax */
    {
      /* 66,67 ? */
      diza->src_set |= XSET_DEV;
      diza->dst_set |= XSET_DEV;
    }

    if ((c == 0xEC) || (c == 0xED)) /* in al|(e)ax, dx */
    {
      /* 66,67 ? */
      diza->src_set |= XSET_DEV | XSET_DX;
      diza->dst_set |= XSET_DEV;
    }
    if ((c == 0xEE) || (c == 0xEF)) /* out dx, al|(e)ax */
    {
      /* 66,67 ? */
      diza->src_set |= XSET_DEV;
      diza->dst_set |= XSET_DEV | XSET_DX;
    }

    /* push es, cs, ss, ds */
    if ((c == 0x06) || (c == 0x0E) || (c == 0x16) || (c == 0x1E))
      diza->src_set |= XSET_OTHER;

    if ((c == 0x07) || (c == 0x17) || (c == 0x1F) ||  /* pop es, ss, ds */
        (c == 0xC4) || (c == 0xC5)) /* les/lds reg, r/m */
      diza->dst_set |= XSET_OTHER;

    if (c == 0xD7) /* xlat */
      diza->src_set |= diza->defaddr == 2 ? XSET_BX : XSET_EBX;

    if ((c == 0xC8) || (c == 0xC9)) /* enter xxxx, yy || leave */
    {
      xset = diza->defaddr == 2 ? (XSET_SP | XSET_BP) : (XSET_ESP | XSET_EBP);
      diza->src_set |= xset;
      diza->dst_set |= xset;
    }

    if (c == 0x8C)  /* mov [r/m]:16, sreg */
      diza->src_set |= XSET_OTHER;
    if (c == 0x8E)  /* mov sreg, [r/m]:16 */
      diza->dst_set |= XSET_OTHER;
  }

  if (flag & C_MODRM)
  {
    c = *p++;

    diza->modrm = c;

    mod = c & 0xC0;
    reg = (c & 0x38) >> 3;       /* reg or ttt */
    rm  = c & 0x07;

    c = diza->opcode;

    /* ttt-opcodes (reg==ttt) */

    /* shift */
    if ((c == 0xC0) || (c == 0xC1) || ((c >= 0xD0) && (c <= 0xD3)))
    {
      // ttt: 0=rol 1=ror 2=rcl 3=rcr 4=shl 5=shr 6=sal 7=sar
      if ((reg == 2) || (reg == 3)) /* rcl, rcr -- uses CF */
        diza->src_set |= XSET_FL;
      if ((c == 0xD2) || (c == 0xD3))
        diza->src_set |= XSET_CL;
    }

    if ( (c == 0xC6) || (c == 0xC7) ||  /* mov [r/m], imm8/16/32 */
         (c == 0x8F) )                  /* pop [r/m] */
      // ttt=000, other illegal
      if (reg != 0) flag |= C_BAD;

    if ((c >= 0x80) && (c <= 0x83)) flag |= xde_table[ TBL_80_83  + reg ];

    if (c == 0xBA)
      if (reg >= 5)                 /* bts/btr/btc [r/m], imm8 */
        flag |= C_DST_RM;

    if (c == 0xF6) flag |= xde_table[ TBL_F6 + reg ];
    if (c == 0xF7) flag |= xde_table[ TBL_F7 + reg ];
    if (c == 0xFE) flag |= xde_table[ TBL_FE + reg ];
    if (c == 0xFF) flag |= xde_table[ TBL_FF + reg ];

    if ((c == 0xF6) || (c == 0xF7))
    {
      if ((reg == 4) || (reg == 5)) /* mul/imul r/m */
      {
        if (c == 0xF6)
        {
          diza->src_set |= XSET_AL;
          diza->dst_set |= XSET_AX;
        }
        else
        if (diza->defaddr == 2)
        {
          diza->src_set |= XSET_AX;
          diza->dst_set |= XSET_DX | XSET_AX;
        }
        else
        {
          diza->src_set |= XSET_EAX;
          diza->dst_set |= XSET_EDX | XSET_EAX;
        }
      }
      if ((reg == 6) || (reg == 7)) /* div/idiv r/m */
      {
        if (c == 0xF6)
        {
          diza->src_set |= XSET_AX;
          diza->dst_set |= XSET_AX;
        }
        else
        if (diza->defaddr == 2)
        {
          diza->src_set |= XSET_DX | XSET_AX;
          diza->dst_set |= XSET_DX | XSET_AX;
        }
        else
        {
          diza->src_set |= XSET_EDX | XSET_EAX;
          diza->dst_set |= XSET_EDX | XSET_EAX;
        }
      }
    } /* F6/F7 */

    if (flag == C_ERROR) return 0;

    xset = reg2xset(flag & C_OPSZ8 ? 1 : diza->defdata, reg);
    if (flag & C_SRC_REG) diza->src_set |= xset;
    if (flag & C_DST_REG) diza->dst_set |= xset;

    if (mod == 0xC0)
    {
      xset = reg2xset(flag & C_OPSZ8 ? 1 : diza->defdata, rm);
      /* defaddr (0x67) prefix --> ??? */
      if (flag & C_DST_RM) diza->dst_set |= xset;

      if ( (diza->opcode == 0x0F) &&
           ( (diza->opcode2 == 0xB6) ||  /* movzx, movsx */
             (diza->opcode2 == 0xB7) ||
             (diza->opcode2 == 0xBE) ||
             (diza->opcode2 == 0xBF) ) )
        xset = reg2xset(diza->defdata==4?2:1, rm);

      if (flag & C_SRC_RM) diza->src_set |= xset;
    }
    else
    {
      if (diza->opcode != 0x8D) /* LEA: doesnt access memory contents */
      {
        diza->src_set |= XSET_OTHER;  /* since we have sreg:[xxx] */
        if (flag & C_SRC_RM) diza->src_set |= XSET_MEM;
      }

      if (flag & C_DST_RM) diza->dst_set |= XSET_MEM;

      if (diza->defaddr == 4)
      {
        /* 32-bit MODR/M */

        if (mod == 0x40)
        {
          flag |= C_ADDR1;
        }
        else
        if (mod == 0x80)
        {
          flag |= C_ADDR4;
        }

        if (rm == 4)
        {

          /* SIB */

          flag |= C_SIB;
          c = *p++;
          diza->sib = c;

          //scale = c & 0xC0;
          index = (c & 0x38) >> 3;
          base  = c & 0x07;

          if (base == 5)
          {
            xset = (mod == 0 ? 0 : XSET_EBP) | reg2xset(4, index);
            if (mod == 0) flag |= C_ADDR4;
            if (flag & C_SRC_RM) diza->src_set |= xset;
            if (flag & C_DST_RM) diza->src_set |= xset;
          }
          else
          {
            xset = reg2xset(4, base);
            if (flag & C_SRC_RM) diza->src_set |= xset;
            if (flag & C_DST_RM) diza->src_set |= xset;

            if (index != 0x05)
            {
              xset = reg2xset(4, index);
              if (flag & C_SRC_RM) diza->src_set |= xset;
              if (flag & C_DST_RM) diza->src_set |= xset;
            }
          }

        }
        else
        {
          /* no sib, just modr/m 32 */

          if ((mod == 0x00) && (rm == 0x05))
            flag |= C_ADDR4;
          else
          {
            xset = reg2xset(4, rm);
            if (flag & C_SRC_RM) diza->src_set |= xset;
            if (flag & C_DST_RM) diza->src_set |= xset;
          }

        }
      }
      else
      {
        /* 16-bit MODR/M */

        if (mod == 0x40)
        {
          flag |= C_ADDR1;
        }
        else
        if (mod == 0x80)
        {
          flag |= C_ADDR2;
        }
        else /* mod == 0x00 */
        {
          if (rm == 0x06)
            flag |= C_ADDR2;
        }

        if ((mod != 0x00) || (rm != 0x06))
        {
          xset = rm == 0 ? (XSET_BX | XSET_SI) :
                 rm == 1 ? (XSET_BX | XSET_DI) :
                 rm == 2 ? (XSET_BP | XSET_SI) :
                 rm == 3 ? (XSET_BP | XSET_DI) :
                 rm == 4 ? XSET_SI :
                 rm == 5 ? XSET_DI :
                 rm == 6 ? XSET_BP :
                           XSET_BX;
          if (flag & C_SRC_RM) diza->src_set |= xset;
          if (flag & C_DST_RM) diza->src_set |= xset;
        }

      }
    }
  } /* C_MODRM */
  else
  {
    /* its not modr/m, check for mem ref */

    if (flag & C_SRC_RM) diza->src_set |= XSET_MEM;
    if (flag & C_DST_RM) diza->dst_set |= XSET_MEM;

  }

  if (flag & C_UNDEF)
  {
    diza->src_set = XSET_UNDEF;
    diza->dst_set = XSET_UNDEF;
  }

  xset = reg2xset(diza->defdata, c & 0x07);
  if (flag & C_SRC_R0) diza->src_set |= xset;
  if (flag & C_DST_R0) diza->dst_set |= xset;

  if (flag & C_SRC_FL) diza->src_set |= XSET_FL;
  if (flag & C_DST_FL) diza->dst_set |= XSET_FL;

  xset = diza->defaddr == 2 ? XSET_SP : XSET_ESP; // incorrect, need stk seg sz
  if (flag & C_PUSH)
  {
    diza->src_set |= xset;                              // +
    diza->dst_set |= xset | XSET_MEM;
  }
  if (flag & C_POP)
  {
    diza->src_set |= xset | XSET_MEM;                   // +
    diza->dst_set |= xset;
  }

  xset = flag & C_OPSZ8 ? XSET_AL : diza->defdata == 2 ? XSET_AX : XSET_EAX;
  if (flag & C_SRC_ACC) diza->src_set |= xset;
  if (flag & C_DST_ACC) diza->dst_set |= xset;

  a =  flag & (C_ADDR1 | C_ADDR2 | C_ADDR4);
  d = (flag & (C_DATA1 | C_DATA2 | C_DATA4)) >> 8;

  if (flag & C_ADDR67) a += diza->defaddr;
  if (flag & C_DATA66) d += diza->defdata;

  for(i=0; i<a; i++)
    diza->addr_b[i] = *p++;

  for(i=0; i<d; i++)
    diza->data_b[i] = *p++;

  diza->flag     = flag;
  diza->addrsize = a;
  diza->datasize = d;
  diza->len      = p - opcode;

  return diza->len;

} /* int __cdecl xde_disasm() */

/* returns: diza.len                                                        */

int __cdecl xde_asm(/* OUT */ unsigned char* opcode,
                    /* IN */ struct xde_instr* diza)
{
  unsigned char* p;
  int i;

  p = opcode;

  if (diza->p_seg )               *p++ = diza->p_seg;
  if (diza->p_lock)               *p++ = diza->p_lock;
  if (diza->p_rep )               *p++ = diza->p_rep;
  if (diza->p_67  )               *p++ = diza->p_67;
  if (diza->p_66  )               *p++ = diza->p_66;
  *p++ = diza->opcode;
  if (diza->opcode == 0x0F)       *p++ = diza->opcode2;
  if (diza->flag & C_MODRM)       *p++ = diza->modrm;
  if (diza->flag & C_SIB)         *p++ = diza->sib;
  for(i=0; i<diza->addrsize; i++) *p++ = diza->addr_b[i];
  for(i=0; i<diza->datasize; i++) *p++ = diza->data_b[i];

  return p - opcode;

} /* int __cdecl xde_asm() */

/* EOF */
