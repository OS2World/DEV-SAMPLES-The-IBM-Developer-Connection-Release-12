;       SCCSID = @(#)dllstub.asm	6.1 91/10/08
;       SCCSID = @(#)dllstub.asm	6.2 91/08/24
;  SCCSID = @(#)dllstub.asm	6.2 91/08/24
;************************************************************************
;*                                                                      *
;*                                                                      *
;*               Copyright (c) IBM Corporation  1987, 1990              *
;*               Copyright (c) Microsoft Corp.  1987, 1990              *
;*                         All Rights Reserved                          *
;*                                                                      *
;************************************************************************
;
; DLL Initialization module for 32-bit PMWP.DLL - this was ripped word for
; word from the application design guide, so if it doesn't work, shoot the
; information developers !                                      - ChrisA
;

.386
.MODEL SMALL

EXTRN  _DLLInit : NEAR
EXTRN  _DLLUninit : NEAR

.CODE

ASSUME CS: FLAT, DS: FLAT, SS: FLAT, ES: FLAT

ENTRYPOINT PROC
;int 3
    push ebp                      ; create a stack frame
    mov  ebp,esp

    cmp DWORD PTR [ebp+8], 1
    je  Uninit

    mov  eax,0                    ; argc == 0
    push eax


    call _DLLInit                 ; Jump to our init routine...
    jmp Done

Uninit:
    call _DLLUninit               ; Jump to our uninit routine...

Done:
    mov  esp,ebp
    pop  ebp                      ; recover bp
;   mov  eax,1                    ; Successful
    ret                           ; return to loader

ENTRYPOINT ENDP

END ENTRYPOINT
