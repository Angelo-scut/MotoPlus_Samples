/**** funclib.c ****/
/* Copyright 2009 YASKAWA ELECTRIC All Rights reserved. */

#include "motoPlus.h"

// for API INTERFACE

// for API & FUNCTIONS

extern long mpPutVarData(MP_VAR_DATA *, long);
extern long mpPutPosVarData(MP_POSVAR_DATA *, long);

extern long mpGetVarData(MP_VAR_INFO *sData, LONG* rData, LONG num);
extern long mpReadIO(MP_IO_INFO *sData, USHORT* rData, LONG num);
extern long mpWriteIO(MP_IO_DATA *sData, LONG num);


// for LOCAL DEFINITIONS

/**************************************
**	useful function for application ***
***************************************/


STATUS GetBVar(UINT16 index, long *value)
{
  MP_VAR_INFO info;

  info.usType = MP_RESTYPE_VAR_B;
  info.usIndex = index;

  return mpGetVarData(&info, value, 1);
}


STATUS GetMultiBVar(UINT16 startIdx, long *value, long nbr)
{
  int i;
  MP_VAR_INFO info[20];

  for(i=0; i<nbr; i++)
  {
    info[i].usType = MP_RESTYPE_VAR_B;
    info[i].usIndex = startIdx + i;
  }

  return mpGetVarData(info, value, nbr);
}


STATUS SetBVar(UINT16 index, long value)
{
  MP_VAR_DATA info;

  info.usType = MP_RESTYPE_VAR_B;
  info.usIndex = index;
  info.ulValue = value;

  return mpPutVarData(&info, 1);
}


STATUS GetIVar(UINT16 index, long *value)
{
  MP_VAR_INFO info;

  info.usType = MP_RESTYPE_VAR_I;
  info.usIndex = index;

  return mpGetVarData(&info, value, 1);
}


STATUS SetIVar(UINT16 index, long value)
{
  MP_VAR_DATA info;

  info.usType = MP_RESTYPE_VAR_I;
  info.usIndex = index;
  info.ulValue = value;

  return mpPutVarData(&info, 1);
}


STATUS GetSVar(UINT16 index, char *value)
{
  MP_VAR_INFO info;

  info.usType = 10/*MP_RESTYPE_VAR_S*/;
  info.usIndex = index;

  return mpGetVarData(&info, (long *)value, 1);
}


STATUS SetDVar(UINT16 index, long value)
{
  MP_VAR_DATA info;

  info.usType = MP_RESTYPE_VAR_D;
  info.usIndex = index;
  info.ulValue = value;

  return mpPutVarData(&info, 1);
}


STATUS SetMultiDVar(UINT16 startIdx, long *values, long nbr)
{
  int i;
  MP_VAR_DATA info[64];

  nbr = (nbr > 64) ? 64 : nbr;

  for(i=0; i<nbr; i++) 
  {
    info[i].usType = MP_RESTYPE_VAR_D;
    info[i].usIndex = startIdx + i;
    info[i].ulValue = values[i];
  }

  return mpPutVarData(info, nbr);
}


STATUS mpGetMultiDVar(UINT16 startIdx, long *values, long nbr)
{
  int i;
  MP_VAR_INFO info[64];

  nbr = (nbr > 64) ? 64 : nbr;

  for(i=0; i<nbr; i++) 
  {
    info[i].usType = MP_RESTYPE_VAR_D;
    info[i].usIndex = startIdx + i;
  }

  return mpGetVarData(info, values, nbr);
}


STATUS GetXVar(UINT16 type, UINT16 index, char *value)
{
  MP_VAR_INFO info;

  info.usType = type;
  info.usIndex = index;

  return mpGetVarData(&info, (long *)value, 1);
}


STATUS GetIo(UINT32 ioAddr, USHORT *signal)
{
  MP_IO_INFO nfo;

  nfo.ulAddr = ioAddr;
  return mpReadIO(&nfo, signal, 1);
}

STATUS SetIo(UINT32 ioAddr, USHORT signal)
{
  MP_IO_DATA dta;
  dta.ulAddr = ioAddr;
  dta.ulValue = signal;
  return mpWriteIO(&dta, 1);
}

STATUS SetMultiIo(UINT32 ioStartAddr, USHORT *signal, long nbr)
{
  MP_IO_DATA dta[20];
  int i;

  if( nbr > 20 )
    return ERROR;

  for(i=0; i<nbr; i++)
  {
    dta[i].ulAddr = ioStartAddr+i;
    dta[i].ulValue = signal[i];
  }
  return mpWriteIO(dta, nbr);
}

STATUS WriteIO(UINT32 addr, UINT16 value)
{
  MP_IO_DATA sData;
  sData.ulAddr = addr;
  sData.ulValue = value;

  return mpWriteIO(&sData, 1);
}


STATUS ReadIO(UINT32 addr, UINT16* value)
{
  MP_IO_INFO sData;
  sData.ulAddr = addr;

  return mpReadIO (&sData, value, 1);
}
