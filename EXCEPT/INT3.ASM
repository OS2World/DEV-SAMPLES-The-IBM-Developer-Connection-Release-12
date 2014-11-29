        title   sample.c
        .386
        .387
CODE32  segment dword use32 public 'CODE'
CODE32  ends
DATA32  segment dword use32 public 'DATA'
DATA32  ends
CONST32_RO      segment dword use32 public 'CONST'
CONST32_RO      ends
BSS32   segment dword use32 public 'BSS'
BSS32   ends
DGROUP  group BSS32, DATA32
        assume  cs:FLAT, ds:FLAT, ss:FLAT, es:FLAT
CODE32  segment
        public int3
int3   proc
        int 3
        ret
int3   endp

CODE32  ends
end
