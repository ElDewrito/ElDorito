
void xde_sprintfl(/*OUT*/char* output, /*IN*/unsigned long fl)
{
  output[0] = 0;

//  if (fl & C_BAD ) strcat(output,"C_BAD|");
  if (fl & C_BAD ) strcat(output,"C_BAD");

//  if (output[0])
//  if (output[strlen(output)-1]=='|')
//    output[strlen(output)-1]=0;
}

void xde_sprintset(/*OUT*/char* output, /*IN*/unsigned long set)
{
  output[0] = 0;

  if (set==XSET_UNDEF)
  {
    strcat(output,"???|");
  }
  else
  {

    if ((set&XSET_EAX)==XSET_EAX) strcat(output,"EAX|"); else
    if ((set&XSET_EAX)==XSET_AX)  strcat(output,"AX|" ); else
    {
      if (set&XSET_AL) strcat(output,"AL|");
      if (set&XSET_AH) strcat(output,"AH|");
    }

    if ((set&XSET_ECX)==XSET_ECX) strcat(output,"ECX|"); else
    if ((set&XSET_ECX)==XSET_CX)  strcat(output,"CX|" ); else
    {
      if (set&XSET_CL) strcat(output,"CL|");
      if (set&XSET_CH) strcat(output,"CH|");
    }

    if ((set&XSET_EDX)==XSET_EDX) strcat(output,"EDX|"); else
    if ((set&XSET_EDX)==XSET_DX)  strcat(output,"DX|" ); else
    {
      if (set&XSET_DL) strcat(output,"DL|");
      if (set&XSET_DH) strcat(output,"DH|");
    }

    if ((set&XSET_EBX)==XSET_EBX) strcat(output,"EBX|"); else
    if ((set&XSET_EBX)==XSET_BX ) strcat(output,"BX|" ); else
    {
      if (set&XSET_BL) strcat(output,"BL|");
      if (set&XSET_BH) strcat(output,"BH|");
    }

    if ((set&XSET_ESP)==XSET_ESP) strcat(output,"ESP|"); else
    if ((set&XSET_ESP)==XSET_SP ) strcat(output,"SP|");

    if ((set&XSET_EBP)==XSET_EBP) strcat(output,"EBP|"); else
    if ((set&XSET_EBP)==XSET_BP ) strcat(output,"BP|");

    if ((set&XSET_ESI)==XSET_ESI) strcat(output,"ESI|"); else
    if ((set&XSET_ESI)==XSET_SI ) strcat(output,"SI|");

    if ((set&XSET_EDI)==XSET_EDI) strcat(output,"EDI|"); else
    if ((set&XSET_EDI)==XSET_DI ) strcat(output,"DI|");

    if (set&XSET_FL) strcat(output,"F|");
    if (set&XSET_MEM) strcat(output,"M|");

    if (set&XSET_OTHER) strcat(output,"other|");
    if (set&XSET_DEV) strcat(output,"dev|");

  }

  if (output[0])
  if (output[strlen(output)-1]=='|')
    output[strlen(output)-1]=0;
} /* strcat_set */
