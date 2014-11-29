;***************************************************************************
;IBM Corp 1994. D.W.Kenner
;Copyright IBM Corp.
;***************************************************************************
;
;The following procedure allows a process to determine if it is running on a
;a OS/2 system. This will work for a real or a protect mode application.
;
;If the test is successful the version of OS/2 that is running will be returned
;in BX.
;    BX = 1400h  = OS/2 2.0
;    BX = 140Ah  = OS/2 2.1
;    BX = 140Bh  = OS/2 2.11
;
;Return the value in AX  if successful else return 0 for error
;
;
;***************************************************************************
.286
VddMiscData    segment  word public  'DATA'
     OS220          equ   20
     OS221          equ   21
     OS2211         equ   211
VddMiscData    ends
;
;External declarations that are assumed to be in our main module
;
     extrn      _VDD_NAME      :WORD
     extrn      _VDDProcAdr    :DWORD
     PUBLIC     _OS2TEST
     PUBLIC     _GetVddApiEntry

CSEG     SEGMENT   WORD  PUBLIC  'CODE'
         ASSUME    CS:CSEG, SS:nothing, ES:nothing, DS:nothing

_OS2TEST  Proc far
;assume   CS:VddMiscCode,DS:VddMiscData
;    Save original register state
     push      BP
     mov       BP,SP
     mov       AX,4010H
     int       2FH
     cmp       AX,0
;    if a valid return then
;    bx now holds which version of OS/2 we are running
;    else if we got a error get out of here
     jne       Error
     mov       AX,BX
     jmp       Done
Error:
     mov       AX,0
Done:
     leave
     ret
_OS2TEST  Endp

;***************************************************************************
;GetVddApiEntry
;This routine will generate int 2F call to see  get the entry point for
;the associated vdd by the name VDD_NAME. This routine may be called in either
;real mode or protect mode. Depending on which mode this routine is being called
;from will determine which entry point will be returned to the calling function.
;If no handler is installed for the mode that is making the call the return value
;will be in AX.
;
;
;***************************************************************************
_GETVDDAPIENTRY     Proc far
;    Save our initial register state
     push      DS
     push      ES
     push      DI
     push      SI
     push      AX
     mov       AX,4011h
;Load DS:SI with the vdd name to find
;
     lea       si,_VDD_NAME
     int       2fh
     mov       AX,ES
     or        AX,DI
;    Did we get a good return if not adios
     jz        ApiDone
;    store our return value
     mov       word ptr _VDDProcAdr,di
     mov       _VDDProcAdr,ES

ApiDone:
;    Restore our register state
     pop       DS
     pop       SI
     pop       DI
     pop       ES
     pop       DS
     ret
_GETVDDAPIENTRY   Endp
CSEG      ENDS
          END
;***************************************************************************
;IBM Corp 1994. D.W.Kenner
;Copyright IBM Corp.
;End of this file
;***************************************************************************




