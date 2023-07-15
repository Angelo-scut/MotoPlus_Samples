// for extern Functions definition
STATUS GetBVar(UINT16 index, long *value);
STATUS GetMultiBVar(UINT16 startIdx, long *value, long nbr);
STATUS SetBVar(UINT16 index, long value);
STATUS GetIVar(UINT16 index, long *value);
STATUS SetIVar(UINT16 index, long value);
STATUS GetSVar(UINT16 index, char *value);
STATUS SetDVar(UINT16 index, long value);
STATUS SetMultiDVar(UINT16 startIdx, long *values, long nbr);
STATUS mpGetMultiDVar(UINT16 startIdx, long *values, long nbr);
STATUS GetXVar(UINT16 type, UINT16 index, char *value);
STATUS GetIo(UINT32 ioAddr, USHORT *signal);
STATUS SetIo(UINT32 ioAddr, USHORT signal);
STATUS SetMultiIo(UINT32 ioStartAddr, USHORT *signal, long nbr);
STATUS WriteIO(UINT32 addr, UINT16 value);
STATUS ReadIO(UINT32 addr, UINT16* value);
