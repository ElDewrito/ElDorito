/* XDE v1.02 eXtended length disassembler engine, based on LDE/ADE engines */

/* NOTE: opcodes marked with C_SPECIAL(==0) are also analyzed in the XDE.C */

unsigned long xde_table[ TBL_max ] =
{
          // add modrm
/* 00 */  C_MODRM+C_DST_FL+C_SRC_REG+C_MOD_RM+C_OPSZ8,
/* 01 */  C_MODRM+C_DST_FL+C_SRC_REG+C_MOD_RM,
/* 02 */  C_MODRM+C_DST_FL+C_MOD_REG+C_SRC_RM+C_OPSZ8,
/* 03 */  C_MODRM+C_DST_FL+C_MOD_REG+C_SRC_RM,
          // add al, c8
/* 04 */  C_DATA1+C_DST_FL+C_MOD_ACC+C_OPSZ8,
          // add ax/eax, c16/32
/* 05 */  C_DATA66+C_DST_FL+C_MOD_ACC,
          // push es
/* 06 */  C_BAD+C_PUSH+C_SPECIAL,
          // pop es
/* 07 */  C_BAD+C_POP+C_SPECIAL,
          // or modrm
/* 08 */  C_MODRM+C_DST_FL+C_SRC_REG+C_MOD_RM+C_OPSZ8,
/* 09 */  C_MODRM+C_DST_FL+C_SRC_REG+C_MOD_RM,
/* 0A */  C_MODRM+C_DST_FL+C_MOD_REG+C_SRC_RM+C_OPSZ8,
/* 0B */  C_MODRM+C_DST_FL+C_MOD_REG+C_SRC_RM,
          // or al, c8
/* 0C */  C_DATA1+C_DST_FL+C_MOD_ACC+C_OPSZ8,
          // or ax/eax, c16/32
/* 0D */  C_DATA66+C_DST_FL+C_MOD_ACC,
          // push cs
/* 0E */  C_BAD+C_PUSH+C_SPECIAL,
          // 0F-prefix
/* 0F */  C_SPECIAL,
          // adc modrm
/* 10 */  C_MODRM+C_BAD+C_MOD_FL+C_SRC_REG+C_MOD_RM+C_OPSZ8,
/* 11 */  C_MODRM+      C_MOD_FL+C_SRC_REG+C_MOD_RM,
/* 12 */  C_MODRM+C_BAD+C_MOD_FL+C_MOD_REG+C_SRC_RM+C_OPSZ8,
/* 13 */  C_MODRM+      C_MOD_FL+C_MOD_REG+C_SRC_RM,
          // adc al, c8
/* 14 */  C_DATA1+C_BAD+C_MOD_FL+C_MOD_ACC+C_OPSZ8,
          // adc ax/eax, c16/32
/* 15 */  C_DATA66+C_BAD+C_MOD_FL+C_MOD_ACC,
          // push ss
/* 16 */  C_BAD+C_PUSH+C_SPECIAL,
          // pop ss
/* 17 */  C_BAD+C_POP+C_SPECIAL,
          // sbb modrm
/* 18 */  C_MODRM+C_BAD+C_MOD_FL+C_SRC_REG+C_MOD_RM+C_OPSZ8,
/* 19 */  C_MODRM+      C_MOD_FL+C_SRC_REG+C_MOD_RM,
/* 1A */  C_MODRM+C_BAD+C_MOD_FL+C_MOD_REG+C_SRC_RM+C_OPSZ8,
/* 1B */  C_MODRM+      C_MOD_FL+C_MOD_REG+C_SRC_RM,
          // sbb al, c8
/* 1C */  C_DATA1+C_BAD+C_MOD_FL+C_MOD_ACC+C_OPSZ8,
          // sbb ax/eax, c16/32
/* 1D */  C_DATA66+C_BAD+C_MOD_FL+C_MOD_ACC,
          // push ds
/* 1E */  C_BAD+C_PUSH+C_SPECIAL,
          // pop ds
/* 1F */  C_BAD+C_POP+C_SPECIAL,
          // and modrm
/* 20 */  C_MODRM+C_DST_FL+C_SRC_REG+C_MOD_RM+C_OPSZ8,
/* 21 */  C_MODRM+C_DST_FL+C_SRC_REG+C_MOD_RM,
/* 22 */  C_MODRM+C_DST_FL+C_MOD_REG+C_SRC_RM+C_OPSZ8,
/* 23 */  C_MODRM+C_DST_FL+C_MOD_REG+C_SRC_RM,
          // and al, c8
/* 24 */  C_DATA1+C_DST_FL+C_MOD_ACC+C_OPSZ8,
          // and ax/eax, c16/32
/* 25 */  C_DATA66+C_DST_FL+C_MOD_ACC,
          // es:
/* 26 */  C_BAD+C_SPECIAL,
          // daa
/* 27 */  C_BAD+C_MOD_FL+C_OPSZ8+C_MOD_ACC,
          // sub modrm
/* 28 */  C_MODRM+C_DST_FL+C_SRC_REG+C_MOD_RM+C_OPSZ8,
/* 29 */  C_MODRM+C_DST_FL+C_SRC_REG+C_MOD_RM,
/* 2A */  C_MODRM+C_DST_FL+C_MOD_REG+C_SRC_RM+C_OPSZ8,
/* 2B */  C_MODRM+C_DST_FL+C_MOD_REG+C_SRC_RM,
          // sub al, c8
/* 2C */  C_DATA1+C_DST_FL+C_MOD_ACC+C_OPSZ8,
          // sub ax/eax, c16/32
/* 2D */  C_DATA66+C_DST_FL+C_MOD_ACC,
          // cs:
/* 2E */  C_BAD+C_SPECIAL,
          // das
/* 2F */  C_BAD+C_MOD_FL+C_OPSZ8+C_MOD_ACC,
          // xor modrm
/* 30 */  C_MODRM+C_DST_FL+C_SRC_REG+C_MOD_RM+C_OPSZ8,
/* 31 */  C_MODRM+C_DST_FL+C_SRC_REG+C_MOD_RM,
/* 32 */  C_MODRM+C_DST_FL+C_MOD_REG+C_SRC_RM+C_OPSZ8,
/* 33 */  C_MODRM+C_DST_FL+C_MOD_REG+C_SRC_RM,
          // xor al, c8
/* 34 */  C_DATA1+C_DST_FL+C_MOD_ACC+C_OPSZ8,
          // xor ax/eax, c16/32
/* 35 */  C_DATA66+C_DST_FL+C_MOD_ACC,
          // ss:
/* 36 */  C_BAD+C_SPECIAL,
          // aaa
/* 37 */  C_BAD+C_MOD_FL+C_OPSZ8+C_MOD_ACC+C_SPECIAL,
          // cmp modrm
/* 38 */  C_MODRM+C_DST_FL+C_SRC_REG+C_SRC_RM+C_OPSZ8,
/* 39 */  C_MODRM+C_DST_FL+C_SRC_REG+C_SRC_RM,
/* 3A */  C_MODRM+C_DST_FL+C_SRC_REG+C_SRC_RM+C_OPSZ8,
/* 3B */  C_MODRM+C_DST_FL+C_SRC_REG+C_SRC_RM,
          // cmp al, c8
/* 3C */  C_DATA1+C_DST_FL+C_SRC_ACC+C_OPSZ8,
          // cmp ax, c16/32
/* 3D */  C_DATA66+C_DST_FL+C_SRC_ACC,
          // ds:
/* 3E */  C_BAD+C_SPECIAL,
          // aas
/* 3F */  C_BAD+C_MOD_FL+C_OPSZ8+C_MOD_ACC+C_SPECIAL,
          // inc ax/eax
/* 40 */  0+C_MOD_FL+C_MOD_R0,
/* 41 */  0+C_MOD_FL+C_MOD_R0,
/* 42 */  0+C_MOD_FL+C_MOD_R0,
/* 43 */  0+C_MOD_FL+C_MOD_R0,
/* 44 */  C_BAD+C_MOD_FL+C_MOD_R0,
/* 45 */  0+C_MOD_FL+C_MOD_R0,
/* 46 */  0+C_MOD_FL+C_MOD_R0,
          // inc di/edi
/* 47 */  0+C_MOD_FL+C_MOD_R0,
          // dec ax/eax
/* 48 */  0+C_MOD_FL+C_MOD_R0,
/* 49 */  0+C_MOD_FL+C_MOD_R0,
/* 4A */  0+C_MOD_FL+C_MOD_R0,
/* 4B */  0+C_MOD_FL+C_MOD_R0,
/* 4C */  C_BAD+C_MOD_FL+C_MOD_R0,
/* 4D */  0+C_MOD_FL+C_MOD_R0,
/* 4E */  0+C_MOD_FL+C_MOD_R0,
          // dec di/edi
/* 4F */  0+C_MOD_FL+C_MOD_R0,
          // push ax/eax
/* 50 */  0+C_PUSH+C_SRC_R0,
/* 51 */  0+C_PUSH+C_SRC_R0,
/* 52 */  0+C_PUSH+C_SRC_R0,
/* 53 */  0+C_PUSH+C_SRC_R0,
/* 54 */  0+C_PUSH+C_SRC_R0,
/* 55 */  0+C_PUSH+C_SRC_R0,
/* 56 */  0+C_PUSH+C_SRC_R0,
          // push di/edi
/* 57 */  0+C_PUSH+C_SRC_R0,
          // pop ax/eax
/* 58 */  0+C_POP+C_DST_R0,
/* 59 */  0+C_POP+C_DST_R0,
/* 5A */  0+C_POP+C_DST_R0,
/* 5B */  0+C_POP+C_DST_R0,
/* 5C */  C_BAD+C_POP+C_DST_R0,
/* 5D */  0+C_POP+C_DST_R0,
/* 5E */  0+C_POP+C_DST_R0,
          // pop di/edi
/* 5F */  0+C_POP+C_DST_R0,
          // pusha/pushad
/* 60 */  C_BAD+C_PUSH+C_SPECIAL,
          // popa/popad
/* 61 */  C_BAD+C_POP+C_SPECIAL,
          // bound
/* 62 */  C_MODRM+C_BAD+C_UNDEF,
          // arpl
/* 63 */  C_MODRM+C_BAD+C_UNDEF,
          // fs:
/* 64 */  C_SPECIAL,
          // gs:
/* 65 */  C_BAD+C_SPECIAL,
          // 66-prefix
/* 66 */  C_SPECIAL,
          // 67-prefix
/* 67 */  C_SPECIAL,
          // push c16/32
/* 68 */  C_DATA66+C_PUSH,
          // imul r,r,c16/32
/* 69 */  C_MODRM+C_DATA66+C_DST_FL+C_MOD_REG+C_SRC_RM,
          // push c8
/* 6A */  C_DATA1+C_PUSH,
          // imul r,r,c8
/* 6B */  C_MODRM+C_DATA1+C_DST_FL+C_MOD_REG+C_SRC_RM,
          // insb
/* 6C */  C_BAD+C_SRC_FL+C_DST_RM+C_SPECIAL,
          // insd
/* 6D */  C_BAD+C_SRC_FL+C_DST_RM+C_SPECIAL,
          // outsb
/* 6E */  C_BAD+C_SRC_FL+C_SRC_RM+C_SPECIAL,
          // outsd
/* 6F */  C_BAD+C_SRC_FL+C_SRC_RM+C_SPECIAL,
          // jxx short
/* 70 */  C_DATA1+C_REL+C_BAD+C_UNDEF,
/* 71 */  C_DATA1+C_REL+C_BAD+C_UNDEF,
/* 72 */  C_DATA1+C_REL+C_UNDEF,
/* 73 */  C_DATA1+C_REL+C_UNDEF,
/* 74 */  C_DATA1+C_REL+C_UNDEF,
/* 75 */  C_DATA1+C_REL+C_UNDEF,
/* 76 */  C_DATA1+C_REL+C_UNDEF,
/* 77 */  C_DATA1+C_REL+C_UNDEF,
/* 78 */  C_DATA1+C_REL+C_UNDEF,
/* 79 */  C_DATA1+C_REL+C_UNDEF,
/* 7A */  C_DATA1+C_REL+C_BAD+C_UNDEF,
/* 7B */  C_DATA1+C_REL+C_BAD+C_UNDEF,
/* 7C */  C_DATA1+C_REL+C_UNDEF,
/* 7D */  C_DATA1+C_REL+C_UNDEF,
/* 7E */  C_DATA1+C_REL+C_UNDEF,
/* 7F */  C_DATA1+C_REL+C_UNDEF,
          // ttt: 0=add 1=or 2=adc 3=sbb 4=and 5=sub 6=xor 7=cmp
          // ttt [r/m]:8, imm8
/* 80 */  C_MODRM+C_DATA1+C_SPECIAL,
          // ttt [r/m]:16/32, imm16/32
/* 81 */  C_MODRM+C_DATA66+C_SPECIAL,
          // ttt [r/m]:8, imm8
/* 82 */  C_MODRM+C_DATA1+C_BAD+C_SPECIAL,
          // ttt [r/m]:16/32, imm8
/* 83 */  C_MODRM+C_DATA1+C_SPECIAL,
          // test [r/m]:8, r8
/* 84 */  C_MODRM+C_DST_FL+C_SRC_REG+C_SRC_RM+C_OPSZ8,
          // test [r/m]:16/32, r16/32
/* 85 */  C_MODRM+C_DST_FL+C_SRC_REG+C_SRC_RM,
          // xchg [r/m]:8, r8
/* 86 */  C_MODRM+C_MOD_REG+C_MOD_RM+C_OPSZ8,
          // xchg [r/m]:16/32, r16/32
/* 87 */  C_MODRM+C_MOD_REG+C_MOD_RM,
          // mov [r/m]:8, r8
/* 88 */  C_MODRM+C_DST_RM+C_SRC_REG+C_OPSZ8,
          // mov [r/m]:16/32, r16/32
/* 89 */  C_MODRM+C_DST_RM+C_SRC_REG,
          // mov r8, [r/m]:8
/* 8A */  C_MODRM+C_DST_REG+C_SRC_RM+C_OPSZ8,
          // mov r16/32, [r/m]:16/32
/* 8B */  C_MODRM+C_DST_REG+C_SRC_RM,
          // mov [r/m]:16, sreg
/* 8C */  C_MODRM+C_BAD+C_DST_RM+C_SPECIAL,
          // lea r16/32, [r/m]
/* 8D */  C_MODRM+C_DST_REG+C_SRC_RM+C_SPECIAL,
          // mov sreg, [r/m]:16
/* 8E */  C_MODRM+C_BAD+C_DST_RM+C_SPECIAL,
          // pop [r/m]:16/32
/* 8F */  C_MODRM+C_POP+C_DST_RM+C_SPECIAL,
          // nop
/* 90 */  0,
          // xchg ecx, eax
/* 91 */  0+C_MOD_R0+C_MOD_ACC,
/* 92 */  0+C_MOD_R0+C_MOD_ACC,
/* 93 */  C_BAD+C_MOD_R0+C_MOD_ACC,
/* 94 */  C_BAD+C_MOD_R0+C_MOD_ACC,
/* 95 */  C_BAD+C_MOD_R0+C_MOD_ACC,
/* 96 */  C_BAD+C_MOD_R0+C_MOD_ACC,
          // xchg edi, eax
/* 97 */  C_BAD+C_MOD_R0+C_MOD_ACC,
          // cwde
/* 98 */  C_BAD+C_SPECIAL,
          // cdq
/* 99 */  0+C_SPECIAL,
          // call far
/* 9A */  C_DATA66+C_DATA2+C_BAD+C_UNDEF+C_CMD_CALL,
          // fwait
/* 9B */  0+C_UNDEF,                                      /* fpu */
          // pushf
/* 9C */  C_BAD+C_PUSH+C_SRC_FL,
          // popf
/* 9D */  C_BAD+C_POP+C_DST_FL,
          // sahf
/* 9E */  C_BAD+C_DST_FL+C_SPECIAL,
          // lahf
/* 9F */  C_BAD+C_SRC_FL+C_SPECIAL,
          // mov al, [imm8]
/* A0 */  C_ADDR67+C_DST_ACC+C_SRC_RM+C_OPSZ8,
          // mov ax/eax, [imm16/32]
/* A1 */  C_ADDR67+C_DST_ACC+C_SRC_RM,
          // mov [imm8], al
/* A2 */  C_ADDR67+C_SRC_ACC+C_DST_RM+C_OPSZ8,
          // mov [imm16/32], ax/eax
/* A3 */  C_ADDR67+C_SRC_ACC+C_DST_RM,
          // movsb
/* A4 */  0+C_SRC_FL+C_MOD_RM+C_SPECIAL,
          // movsd
/* A5 */  0+C_SRC_FL+C_MOD_RM+C_SPECIAL,
          // cmpsb
/* A6 */  0+C_MOD_FL+C_SRC_RM+C_SPECIAL,
          // cmpsd
/* A7 */  0+C_MOD_FL+C_SRC_RM+C_SPECIAL,
          // test al, c8
/* A8 */  C_DATA1+C_DST_FL+C_SRC_ACC+C_OPSZ8,
          // test ax/eax, c16/32
/* A9 */  C_DATA66+C_DST_FL+C_SRC_ACC,
          // stosb
/* AA */  0+C_SRC_FL+C_DST_RM+C_OPSZ8+C_SRC_ACC+C_SPECIAL,
          // stosd
/* AB */  0+C_SRC_FL+C_DST_RM+C_SRC_ACC+C_SPECIAL,
          // lodsb
/* AC */  0+C_SRC_FL+C_SRC_RM+C_OPSZ8+C_DST_ACC+C_SPECIAL,
          // lodsd
/* AD */  C_BAD+C_SRC_FL+C_SRC_RM+C_DST_ACC+C_SPECIAL,
          // scasb
/* AE */  0+C_SRC_FL+C_DST_FL+C_SRC_RM+C_OPSZ8+C_SRC_ACC+C_SPECIAL,
          // scasd
/* AF */  C_BAD+C_SRC_FL+C_DST_FL+C_SRC_RM+C_SRC_ACC+C_SPECIAL,
          // mov al, c8
/* B0 */  C_DATA1+C_DST_R0+C_OPSZ8,
/* B1 */  C_DATA1+C_DST_R0+C_OPSZ8,
/* B2 */  C_DATA1+C_DST_R0+C_OPSZ8,
/* B3 */  C_DATA1+C_DST_R0+C_OPSZ8,
/* B4 */  C_DATA1+C_DST_R0+C_OPSZ8,
/* B5 */  C_DATA1+C_DST_R0+C_OPSZ8,
/* B6 */  C_DATA1+C_BAD+C_DST_R0+C_OPSZ8,
          // mov bh, c8
/* B7 */  C_DATA1+C_BAD+C_DST_R0+C_OPSZ8,
          // mov ax/eax, c16/32
/* B8 */  C_DATA66+C_DST_R0,
/* B9 */  C_DATA66+C_DST_R0,
/* BA */  C_DATA66+C_DST_R0,
/* BB */  C_DATA66+C_DST_R0,
/* BC */  C_DATA66+C_BAD+C_DST_R0,
/* BD */  C_DATA66+C_DST_R0,
/* BE */  C_DATA66+C_DST_R0,
          // mov di/edi, c16/32
/* BF */  C_DATA66+C_DST_R0,
          // ttt: 0=rol 1=ror 2=rcl 3=rcr 4=shl 5=shr 6=sal 7=sar
          // shift-ttt [r/m]:8, imm8
/* C0 */  C_MODRM+C_DATA1+C_DST_FL+C_DST_RM+C_OPSZ8+C_SPECIAL,
          // shift-ttt [r/m]:16/32, imm8
/* C1 */  C_MODRM+C_DATA1+C_DST_FL+C_DST_RM+C_SPECIAL,
          // retn c16
/* C2 */  C_DATA2+C_STOP+C_UNDEF,
          // retn
/* C3 */  C_STOP+C_UNDEF,
          // les
/* C4 */  C_MODRM+C_BAD+C_DST_REG+C_SRC_RM+C_SPECIAL,
          // lds
/* C5 */  C_MODRM+C_BAD+C_DST_REG+C_SRC_RM+C_SPECIAL,
          // ttt=000, other illegal
          // mov [r/m], imm8
/* C6 */  C_MODRM+C_DST_RM+C_DATA1+C_SPECIAL,
          // mov [r/m], imm16/32
/* C7 */  C_MODRM+C_DST_RM+C_DATA66+C_SPECIAL,
          // enter
/* C8 */  C_DATA2+C_DATA1+C_MOD_RM+C_SPECIAL,
          // leave
/* C9 */  0+C_SRC_RM+C_SPECIAL,
          // retf c16
/* CA */  C_DATA2+C_STOP+C_BAD+C_UNDEF,
          // retf
/* CB */  C_STOP+C_BAD+C_UNDEF,
          // int3
/* CC */  C_BAD,
          // int n
/* CD */  C_DATA1+C_UNDEF,
          // into
/* CE */  C_BAD+C_UNDEF,
          // iret
/* CF */  C_STOP+C_BAD+C_UNDEF,
          // ttt: 0=rol 1=ror 2=rcl 3=rcr 4=shl 5=shr 6=sal 7=sar
          // shift-ttt [r/m]:8, 1
/* D0 */  C_MODRM+C_DST_FL+C_MOD_RM+C_OPSZ8+C_SPECIAL,
          // shift-ttt [r/m]:16/32, 1
/* D1 */  C_MODRM+C_DST_FL+C_MOD_RM+C_SPECIAL,
          // shift-ttt [r/m]:8, cl
/* D2 */  C_MODRM+C_DST_FL+C_MOD_RM+C_OPSZ8+C_SPECIAL,
          // shift-ttt [r/m]:16/32, cl
/* D3 */  C_MODRM+C_DST_FL+C_MOD_RM+C_SPECIAL,
          // aam nn
/* D4 */  C_DATA1+C_BAD+C_DST_FL+C_SPECIAL,
          // aad nn
/* D5 */  C_DATA1+C_BAD+C_DST_FL+C_SPECIAL,
          // setalc
/* D6 */  C_BAD+C_SRC_FL+C_OPSZ8+C_DST_ACC,
          // xlat
/* D7 */  C_BAD+C_OPSZ8+C_MOD_ACC+C_SRC_RM+C_SPECIAL,
/* D8 */  C_MODRM+C_UNDEF,                   /* fpu */
/* D9 */  C_MODRM+C_UNDEF,                   /* fpu */
/* DA */  C_MODRM+C_UNDEF,                   /* fpu */
/* DB */  C_MODRM+C_UNDEF,                   /* fpu */
/* DC */  C_MODRM+C_UNDEF,                   /* fpu */
/* DD */  C_MODRM+C_UNDEF,                   /* fpu */
/* DE */  C_MODRM+C_UNDEF,                   /* fpu */
/* DF */  C_MODRM+C_UNDEF,                   /* fpu */
          // loopne
/* E0 */  C_DATA1+C_REL+C_BAD+C_UNDEF,
          // loope
/* E1 */  C_DATA1+C_REL+C_BAD+C_UNDEF,
          // loop
/* E2 */  C_DATA1+C_REL+C_UNDEF,
          // jecxz
/* E3 */  C_DATA1+C_REL+C_UNDEF,
          // in al, nn
/* E4 */  C_DATA1+C_BAD+C_OPSZ8+C_DST_ACC+C_SPECIAL,
          // in ax/eax, nn
/* E5 */  C_DATA1+C_BAD+C_DST_ACC+C_SPECIAL,
          // out nn, al
/* E6 */  C_DATA1+C_BAD+C_OPSZ8+C_SRC_ACC+C_SPECIAL,
          // out nn, ax/eax
/* E7 */  C_DATA1+C_BAD+C_SRC_ACC+C_SPECIAL,
          // call near
/* E8 */  C_DATA66+C_REL+C_UNDEF+C_CMD_CALL,
          // jmp near
/* E9 */  C_DATA66+C_REL+C_STOP+C_UNDEF,
          // jmp far
/* EA */  C_DATA66+C_DATA2+C_BAD+C_STOP+C_UNDEF,
          // jmp short
/* EB */  C_DATA1+C_REL+C_STOP+C_UNDEF,
          // in al, dx
/* EC */  C_BAD+C_OPSZ8+C_DST_ACC+C_SPECIAL,
          // in ax/eax, dx
/* ED */  C_BAD+C_DST_ACC+C_SPECIAL,
          // out dx, al
/* EE */  C_BAD+C_OPSZ8+C_SRC_ACC+C_SPECIAL,
          // out dx, ax/eax
/* EF */  C_BAD+C_SRC_ACC+C_SPECIAL,
          // lock prefix
/* F0 */  C_BAD+C_SPECIAL,
          //
/* F1 */  C_BAD+C_UNDEF,
          // repne
/* F2 */  C_SPECIAL,
          // repe
/* F3 */  C_SPECIAL,
          // hlt
/* F4 */  C_BAD,
          // cmc
/* F5 */  C_BAD+C_MOD_FL,
          // ttt: 0=test 1=??? 2=not 3=neg 4=mul 5=imul 6=div 7=idiv
/* F6 */  C_MODRM+C_SPECIAL,
/* F7 */  C_MODRM+C_SPECIAL,
          // clc
/* F8 */  0+C_DST_FL,
          // stc
/* F9 */  0+C_DST_FL,
          // cli
/* FA */  C_BAD+C_DST_FL,
          // sti
/* FB */  C_BAD+C_DST_FL,
          // cld
/* FC */  0+C_DST_FL,
          // std
/* FD */  0+C_DST_FL,
          // ttt: 0=inc 1=dec 2=??? 3=??? 4=??? 5=??? 6=??? 7=???
/* FE */  C_MODRM+C_SPECIAL,
          // ttt: 0=inc 1=dec 2=callnear 3=callfar 4=jmpnear 5=jmpfar 6=push 7=???
/* FF */  C_MODRM+C_SPECIAL,

/* 2nd half of the table, 0F-prefixed opcodes */

             // /0=SLDT /1=STR /2=LLDT /3=LTR /4=VERR /5=VERW /6=??? /7=???
/* 0F 00 */  C_MODRM+C_UNDEF,
             // /0=SGDT /1=SIDT /2=LGDT /3=LIDT /4=SMSW /5=??? /6=LMSW /7=INVPLG
/* 0F 01 */  C_MODRM+C_UNDEF,
             // LAR r16/32, [r/m]:16/32
/* 0F 02 */  C_MODRM+C_UNDEF,
             // LSL r16/32, [r/m]:16/32
/* 0F 03 */  C_MODRM+C_UNDEF,
/* 0F 04 */  C_ERROR,
/* 0F 05 */  C_ERROR,
             // CLTS
/* 0F 06 */  C_UNDEF,
/* 0F 07 */  C_ERROR,
             // INVD
/* 0F 08 */  C_UNDEF,
             // WBINVD
/* 0F 09 */  C_UNDEF,
             // ???
/* 0F 0A */  C_UNDEF,
             // UD2
/* 0F 0B */  C_UNDEF,
/* 0F 0C */  C_ERROR,
/* 0F 0D */  C_ERROR,
/* 0F 0E */  C_ERROR,
/* 0F 0F */  C_ERROR,
             //
/* 0F 10 */  C_ERROR,
/* 0F 11 */  C_ERROR,
/* 0F 12 */  C_ERROR,
/* 0F 13 */  C_ERROR,
/* 0F 14 */  C_ERROR,
/* 0F 15 */  C_ERROR,
/* 0F 16 */  C_ERROR,
/* 0F 17 */  C_ERROR,
/* 0F 18 */  C_ERROR,
/* 0F 19 */  C_ERROR,
/* 0F 1A */  C_ERROR,
/* 0F 1B */  C_ERROR,
/* 0F 1C */  C_ERROR,
/* 0F 1D */  C_ERROR,
/* 0F 1E */  C_ERROR,
/* 0F 1F */  C_ERROR,
/* 0F 20 */  C_ERROR,
/* 0F 21 */  C_ERROR,
/* 0F 22 */  C_ERROR,
/* 0F 23 */  C_ERROR,
/* 0F 24 */  C_ERROR,
/* 0F 25 */  C_ERROR,
/* 0F 26 */  C_ERROR,
/* 0F 27 */  C_ERROR,
/* 0F 28 */  C_ERROR,
/* 0F 29 */  C_ERROR,
/* 0F 2A */  C_ERROR,
/* 0F 2B */  C_ERROR,
/* 0F 2C */  C_ERROR,
/* 0F 2D */  C_ERROR,
/* 0F 2E */  C_ERROR,
/* 0F 2F */  C_ERROR,
             // WRMSR
/* 0F 30 */  C_UNDEF,
/* 0F 31 */  C_ERROR,
/* 0F 32 */  C_ERROR,
/* 0F 33 */  C_ERROR,
/* 0F 34 */  C_ERROR,
/* 0F 35 */  C_ERROR,
/* 0F 36 */  C_ERROR,
/* 0F 37 */  C_ERROR,
/* 0F 38 */  C_ERROR,
/* 0F 39 */  C_ERROR,
/* 0F 3A */  C_ERROR,
/* 0F 3B */  C_ERROR,
/* 0F 3C */  C_ERROR,
/* 0F 3D */  C_ERROR,
/* 0F 3E */  C_ERROR,
/* 0F 3F */  C_ERROR,
/* 0F 40 */  C_ERROR,
/* 0F 41 */  C_ERROR,
/* 0F 42 */  C_ERROR,
/* 0F 43 */  C_ERROR,
/* 0F 44 */  C_ERROR,
/* 0F 45 */  C_ERROR,
/* 0F 46 */  C_ERROR,
/* 0F 47 */  C_ERROR,
/* 0F 48 */  C_ERROR,
/* 0F 49 */  C_ERROR,
/* 0F 4A */  C_ERROR,
/* 0F 4B */  C_ERROR,
/* 0F 4C */  C_ERROR,
/* 0F 4D */  C_ERROR,
/* 0F 4E */  C_ERROR,
/* 0F 4F */  C_ERROR,
/* 0F 50 */  C_ERROR,
/* 0F 51 */  C_ERROR,
/* 0F 52 */  C_ERROR,
/* 0F 53 */  C_ERROR,
/* 0F 54 */  C_ERROR,
/* 0F 55 */  C_ERROR,
/* 0F 56 */  C_ERROR,
/* 0F 57 */  C_ERROR,
/* 0F 58 */  C_ERROR,
/* 0F 59 */  C_ERROR,
/* 0F 5A */  C_ERROR,
/* 0F 5B */  C_ERROR,
/* 0F 5C */  C_ERROR,
/* 0F 5D */  C_ERROR,
/* 0F 5E */  C_ERROR,
/* 0F 5F */  C_ERROR,
/* 0F 60 */  C_ERROR,
/* 0F 61 */  C_ERROR,
/* 0F 62 */  C_ERROR,
/* 0F 63 */  C_ERROR,
/* 0F 64 */  C_ERROR,
/* 0F 65 */  C_ERROR,
/* 0F 66 */  C_ERROR,
/* 0F 67 */  C_ERROR,
/* 0F 68 */  C_ERROR,
/* 0F 69 */  C_ERROR,
/* 0F 6A */  C_ERROR,
/* 0F 6B */  C_ERROR,
/* 0F 6C */  C_ERROR,
/* 0F 6D */  C_ERROR,
/* 0F 6E */  C_ERROR,
/* 0F 6F */  C_ERROR,
/* 0F 70 */  C_ERROR,
/* 0F 71 */  C_ERROR,
/* 0F 72 */  C_ERROR,
/* 0F 73 */  C_ERROR,
/* 0F 74 */  C_ERROR,
/* 0F 75 */  C_ERROR,
/* 0F 76 */  C_ERROR,
/* 0F 77 */  C_ERROR,
/* 0F 78 */  C_ERROR,
/* 0F 79 */  C_ERROR,
/* 0F 7A */  C_ERROR,
/* 0F 7B */  C_ERROR,
/* 0F 7C */  C_ERROR,
/* 0F 7D */  C_ERROR,
/* 0F 7E */  C_ERROR,
/* 0F 7F */  C_ERROR,
             // jxx near
/* 0F 80 */  C_DATA66+C_REL+C_UNDEF,
/* 0F 81 */  C_DATA66+C_REL+C_UNDEF,
/* 0F 82 */  C_DATA66+C_REL+C_UNDEF,
/* 0F 83 */  C_DATA66+C_REL+C_UNDEF,
/* 0F 84 */  C_DATA66+C_REL+C_UNDEF,
/* 0F 85 */  C_DATA66+C_REL+C_UNDEF,
/* 0F 86 */  C_DATA66+C_REL+C_UNDEF,
/* 0F 87 */  C_DATA66+C_REL+C_UNDEF,
/* 0F 88 */  C_DATA66+C_REL+C_UNDEF,
/* 0F 89 */  C_DATA66+C_REL+C_UNDEF,
/* 0F 8A */  C_DATA66+C_REL+C_UNDEF,
/* 0F 8B */  C_DATA66+C_REL+C_UNDEF,
/* 0F 8C */  C_DATA66+C_REL+C_UNDEF,
/* 0F 8D */  C_DATA66+C_REL+C_UNDEF,
/* 0F 8E */  C_DATA66+C_REL+C_UNDEF,
/* 0F 8F */  C_DATA66+C_REL+C_UNDEF,
             /* setxx */
/* 0F 90 */  C_MODRM+C_UNDEF,
/* 0F 91 */  C_MODRM+C_UNDEF,
/* 0F 92 */  C_MODRM+C_UNDEF,
/* 0F 93 */  C_MODRM+C_UNDEF,
/* 0F 94 */  C_MODRM+C_UNDEF,
/* 0F 95 */  C_MODRM+C_UNDEF,
/* 0F 96 */  C_MODRM+C_UNDEF,
/* 0F 97 */  C_MODRM+C_UNDEF,
/* 0F 98 */  C_MODRM+C_UNDEF,
/* 0F 99 */  C_MODRM+C_UNDEF,
/* 0F 9A */  C_MODRM+C_UNDEF,
/* 0F 9B */  C_MODRM+C_UNDEF,
/* 0F 9C */  C_MODRM+C_UNDEF,
/* 0F 9D */  C_MODRM+C_UNDEF,
/* 0F 9E */  C_MODRM+C_UNDEF,
/* 0F 9F */  C_MODRM+C_UNDEF,
             // push fs
/* 0F A0 */  C_PUSH+C_SPECIAL,
             // pop fs
/* 0F A1 */  C_POP+C_SPECIAL,
             // cpuid
/* 0F A2 */  C_SPECIAL,
             // bt [r/m]:16/32, r16/32
/* 0F A3 */  C_MODRM+C_DST_FL+C_SRC_REG+C_SRC_RM,
             // shld [r/m]:16/32, r16/32, imm8
/* 0F A4 */  C_MODRM+C_DST_FL+C_DATA1+C_MOD_RM+C_SRC_REG,
             // shld [r/m]:16/32, r16/32, CL
/* 0F A5 */  C_MODRM+C_DST_FL+C_MOD_RM+C_SRC_REG+C_SPECIAL,
/* 0F A6 */  C_ERROR,
/* 0F A7 */  C_ERROR,
             // push gs
/* 0F A8 */  C_PUSH+C_SPECIAL,
             // pop gs
/* 0F A9 */  C_POP+C_SPECIAL,
             // RSM
/* 0F AA */  C_UNDEF,
             // bts [r/m]:16/32, r16/32
/* 0F AB */  C_MODRM+C_DST_FL+C_MOD_RM+C_SRC_REG,
             // shrd [r/m]:16/32, r16/32, imm8
/* 0F AC */  C_MODRM+C_DST_FL+C_DATA1+C_MOD_RM+C_SRC_REG,
             // shrd [r/m]:16/32, r16/32, CL
/* 0F AD */  C_MODRM+C_DST_FL+C_MOD_RM+C_SRC_REG+C_SPECIAL,
/* 0F AE */  C_ERROR,
             // imul r16/32, [r/m]:16/32
/* 0F AF */  C_MODRM+C_DST_FL+C_MOD_REG+C_SRC_RM,
             // cmpxchg [r/m]:8, r8
/* 0F B0 */  C_MODRM+C_DST_FL+C_MOD_RM+C_SRC_REG+C_MOD_ACC+C_OPSZ8,
             // cmpxchg [r/m]:16/32, r16/32
/* 0F B1 */  C_MODRM+C_DST_FL+C_MOD_RM+C_SRC_REG+C_MOD_ACC,
             // lss reg, r/m
/* 0F B2 */  C_MODRM+C_BAD+C_DST_REG+C_SRC_RM+C_SPECIAL,
             // btr [r/m]:16/32, r16/32
/* 0F B3 */  C_MODRM+C_DST_FL+C_MOD_RM+C_SRC_REG,
             // lfs reg, r/m
/* 0F B4 */  C_MODRM+C_BAD+C_DST_REG+C_SRC_RM+C_SPECIAL,
             // lgs reg, r/m
/* 0F B5 */  C_MODRM+C_BAD+C_DST_REG+C_SRC_RM+C_SPECIAL,
             // movzx r16/32, [r/m]:8
/* 0F B6 */  C_MODRM+C_DST_REG+C_SRC_RM,
             // movzx 32, [r/m]:16
/* 0F B7 */  C_MODRM+C_DST_REG+C_SRC_RM,
/* 0F B8 */  C_ERROR,
/* 0F B9 */  C_ERROR,
             // ttt: 1=??? 2=??? 3=??? 4=bt 5=bts 6=btr 7=btc
             // ttt [r/m], imm8
/* 0F BA */  C_MODRM+C_DATA1+C_DST_FL+C_SRC_RM+C_SPECIAL,
             // btc [r/m]:16/32, r16/32
/* 0F BB */  C_MODRM+C_DST_FL+C_MOD_RM+C_SRC_REG,
             // bsf r16/32, [r/m]:16/32
/* 0F BC */  C_MODRM+C_DST_FL+C_DST_REG+C_SRC_RM,
             // bsr r16/32, [r/m]:16/32
/* 0F BD */  C_MODRM+C_DST_FL+C_DST_REG+C_SRC_RM,
             // movsx r16/32, [r/m]:8
/* 0F BE */  C_MODRM+C_DST_REG+C_SRC_RM,
             // movsx r32, [r/m]:16
/* 0F BF */  C_MODRM+C_DST_REG+C_SRC_RM,
             // xadd [r/m]:8, r8
/* 0F C0 */  C_MODRM+C_DST_FL+C_MOD_REG+C_MOD_RM+C_OPSZ8,
             // xadd [r/m]:16/32, r16/32
/* 0F C1 */  C_MODRM+C_DST_FL+C_MOD_REG+C_MOD_RM,
/* 0F C2 */  C_ERROR,
/* 0F C3 */  C_ERROR,
/* 0F C4 */  C_ERROR,
/* 0F C5 */  C_ERROR,
/* 0F C6 */  C_ERROR,
/* 0F C7 */  C_ERROR,
             // BSWAP r32
/* 0F C8 */  C_MOD_R0,
/* 0F C9 */  C_MOD_R0,
/* 0F CA */  C_MOD_R0,
/* 0F CB */  C_MOD_R0,
/* 0F CC */  C_MOD_R0,
/* 0F CD */  C_MOD_R0,
/* 0F CE */  C_MOD_R0,
/* 0F CF */  C_MOD_R0,
/* 0F D0 */  C_ERROR,
/* 0F D1 */  C_ERROR,
/* 0F D2 */  C_ERROR,
/* 0F D3 */  C_ERROR,
/* 0F D4 */  C_ERROR,
/* 0F D5 */  C_ERROR,
/* 0F D6 */  C_ERROR,
/* 0F D7 */  C_ERROR,
/* 0F D8 */  C_ERROR,
/* 0F D9 */  C_ERROR,
/* 0F DA */  C_ERROR,
/* 0F DB */  C_ERROR,
/* 0F DC */  C_ERROR,
/* 0F DD */  C_ERROR,
/* 0F DE */  C_ERROR,
/* 0F DF */  C_ERROR,
/* 0F E0 */  C_ERROR,
/* 0F E1 */  C_ERROR,
/* 0F E2 */  C_ERROR,
/* 0F E3 */  C_ERROR,
/* 0F E4 */  C_ERROR,
/* 0F E5 */  C_ERROR,
/* 0F E6 */  C_ERROR,
/* 0F E7 */  C_ERROR,
/* 0F E8 */  C_ERROR,
/* 0F E9 */  C_ERROR,
/* 0F EA */  C_ERROR,
/* 0F EB */  C_ERROR,
/* 0F EC */  C_ERROR,
/* 0F ED */  C_ERROR,
/* 0F EE */  C_ERROR,
/* 0F EF */  C_ERROR,
/* 0F F0 */  C_ERROR,
/* 0F F1 */  C_ERROR,
/* 0F F2 */  C_ERROR,
/* 0F F3 */  C_ERROR,
/* 0F F4 */  C_ERROR,
/* 0F F5 */  C_ERROR,
/* 0F F6 */  C_ERROR,
/* 0F F7 */  C_ERROR,
/* 0F F8 */  C_ERROR,
/* 0F F9 */  C_ERROR,
/* 0F FA */  C_ERROR,
/* 0F FB */  C_ERROR,
/* 0F FC */  C_ERROR,
/* 0F FD */  C_ERROR,
/* 0F FE */  C_ERROR,
/* 0F FF */  C_ERROR,

/* additional tables, added in XDE */

                /* ttt: 0=add 1=or 2=adc 3=sbb 4=and 5=sub 6=xor 7=cmp */
                /* x=0..3 */
/* 8x /0 */  C_DST_FL+C_MOD_RM,
/* 8x /1 */  C_DST_FL+C_MOD_RM,
/* 8x /2 */  C_MOD_FL+C_MOD_RM,
/* 8x /3 */  C_MOD_FL+C_MOD_RM,
/* 8x /4 */  C_DST_FL+C_MOD_RM,
/* 8x /5 */  C_DST_FL+C_MOD_RM,
/* 8x /6 */  C_DST_FL+C_MOD_RM,
/* 8x /7 */  C_DST_FL+C_SRC_RM,
                /* ttt: 0=test 1=??? 2=not 3=neg 4=mul 5=imul 6=div 7=idiv */
/* F6 /0 */  C_DATA1+C_DST_FL+C_SRC_RM,
/* F6 /1 */  C_BAD,
/* F6 /2 */  C_DST_FL+C_MOD_RM,
/* F6 /3 */  C_DST_FL+C_MOD_RM,
/* F6 /4 */  C_DST_FL+C_SRC_RM+C_SPECIAL,
/* F6 /5 */  C_DST_FL+C_SRC_RM+C_SPECIAL,
/* F6 /6 */  C_DST_FL+C_SRC_RM+C_SPECIAL,
/* F6 /7 */  C_DST_FL+C_SRC_RM+C_SPECIAL,
                /* ttt: 0=test 1=??? 2=not 3=neg 4=mul 5=imul 6=div 7=idiv */
/* F7 /0 */  C_DATA66+C_DST_FL+C_SRC_RM,
/* F7 /1 */  C_BAD,
/* F7 /2 */  C_DST_FL+C_MOD_RM,
/* F7 /3 */  C_DST_FL+C_MOD_RM,
/* F7 /4 */  C_DST_FL+C_SRC_RM+C_SPECIAL,
/* F7 /5 */  C_DST_FL+C_SRC_RM+C_SPECIAL,
/* F7 /6 */  C_DST_FL+C_SRC_RM+C_SPECIAL,
/* F7 /7 */  C_DST_FL+C_SRC_RM+C_SPECIAL,
                /* ttt: 0=inc 1=dec 2=??? 3=??? 4=??? 5=??? 6=??? 7=??? */
/* FE /0 */  C_MOD_FL+C_MOD_RM,
/* FE /1 */  C_MOD_FL+C_MOD_RM,
/* FE /2 */  C_BAD,
/* FE /3 */  C_BAD,
/* FE /4 */  C_BAD,
/* FE /5 */  C_BAD,
/* FE /6 */  C_BAD,
/* FE /7 */  C_BAD,
             /* ttt: 0=inc 1=dec 2=callnear 3=callfar 4=jmpnear 5=jmpfar 6=push 7=??? */
/* FF /0 */  C_MOD_FL+C_MOD_RM,
/* FF /1 */  C_MOD_FL+C_MOD_RM,
/* FF /2 */  C_UNDEF+C_CMD_CALL,
/* FF /3 */  C_UNDEF+C_CMD_CALL,
/* FF /4 */  C_STOP+C_UNDEF,
/* FF /5 */  C_STOP+C_UNDEF,
/* FF /6 */  C_PUSH+C_SRC_RM,
/* FF /7 */  C_BAD

}; /* unsigned long xde_table[512] */

/* EOF */
