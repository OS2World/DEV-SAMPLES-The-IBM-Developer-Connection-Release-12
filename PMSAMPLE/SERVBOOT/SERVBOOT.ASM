;*  IDSS_Asm_Opts: -ML -N -P                                          *
;*                                                                    *
;*--------------------------------------------------------------------*
;*  First lines must be MASM Assembler options.                       *
;*--------------------------------------------------------------------*
;*  There is no Option forced by ASMOS2 IDSS process.                 *
;*                                                                    *
;*  IBM MASM/2 default options are:                                   *
;*                                                                    *
;*     -B32  Buffer size is 32K bytes                                 *
;*     -MU   Lowercase are converted to Uppercase                     *
;*     -S    Leave the source segments in their specified order       *
;*                                                                    *
;*--------------------------------------------------------------------*
;*           G set printer to condensed mode, Double printing       *
;*                                                                    *
             .XLIST
             SUBTTL Description - Source file requirements
             TITLE  SERVBOOT.SYS 1.0 - OS/2 Character Device Driver
             .XCREF
             PAGE    63,132
             .LIST
             PAGE
COMMENT      |
*******************************************************************************
*            AUTHOR:        P.Guillon - D/0768 CER IBM La Gaude               *
*                                                                             *
*            ENVIRONMENT:   PCAT, PS/2   OS/2 1.1 or higher                   *
*                                                                             *
*            FUNCTION:      Provides Support to Reboot OS/2                   *
*                                                                             *
*      This Bimodal Character Device Driver accepts the following commands:   *
*                                                                             *
*          - INIT      Initialize Device                                      *
*          - OPEN      Open Device                                            *
*          - CLOSE     Close Device                                           *
*          - DEINSTALL Deinstall Device                                       *
*                                                                             *
*          - READ, WRITE, IOCTL  Category X'80' Function X'40'                *
*                                                                             *
*      The READ, WRITE and IOCTL functions call Reboot Vector in Kernel       *
*      and do not usually return.                                             *
*                                                                             *
*      This companion program of WATCHDOG can be invoked by any appli-        *
*      cation running in DOS or OS/2 mode:                                    *
*                                                                             *
*          - WATCHDOG use DosOpen to make sure that SERVBOOT.SYS  is          *
*            installed, then DosDevIOCtl to reboot.                           *
*                                                                             *
*          - The command "echo > sreboot$" will reboot OS/2.                  *
*                                                                             *
*      The design of this little Device Driver was done with in mind the      *
*      IDSS PS/2 La Gaude OS/2 servers.                                       *
*-----------------------------------------------------------------------------*
*      What you need to Assemble this Program:                                *
*                                                                             *
*           Required Files:                                                   *
*                                                                             *
*                SERVBOOT.ASM  - Source code for this C Program.              *
*                SERVBOOT.DEF  - definition file used during LINK.            *
*                SERVBOOT.MAK  - Make file                                    *
*                                                                             *
*           Required toolkit includes:                                        *
*                                                                             *
*                SYSMAC.INC - This is a master include file which includes    *
*                             the two components of the Macro Assembler       *
*                             function call macro interface, DOSCALLS.INC     *
*                             and SUBCALLS.INC.                               *
*                                                                             *
*              DOSCALLS.INC - This include file contains a macro for each     *
*                             DOS function call.                              *
*                                                                             *
*              SUBCALLS.INC - This include file contains a macro for each     *
*                             Kbd, Mou, and Vio function call.                *
*                                                                             *
*                 ERROR.INC - This include file contains EQUates associating  *
*                             error return codes from the system function     *
*                             calls with their symbolic names.                *
*******************************************************************************
             |
             Page$   <History, ASM directives, Includes and Macros>
COMMENT      |
*******************************************************************************
*            Change History:                                                  *
*                                                                             *
*                - 09/28/90 Initial Release Version 1.00                      *
*                                                                             *
*******************************************************************************
             |
             .286c                     ;  Recognize and assemble 80286
                                          ;/ instructions
             IF1
             INCLUDE SYSMAC.INC
             INCLUDE ERROR.INC
             ENDIF
;
;----------- MISCELLANEOUS MACROS
;
Ver$         MACRO
             DB      "1.00"            ;  Current Version
             ENDM

Page$        MACRO   subtitle
             ;; Macro to insert subttl and page eject directives
             ;; into the listings.
             SUBTTL  subtitle
             PAGE
             ENDM

DevName$     MACRO
             ;; Device name for DOSOPEN (must be 8 characters)
             DB      "SREBOOT$"        ;  Device name for DOSOPEN
             ENDM

DeviceHelp$  MACRO   function
             ;; Macro to call the DevHlp router.
             ;; USES: DL
             MOV     DL,function
             CALL    Dev_Hlp_Addr      ;  Call DevHlp router
                                          ;/ Carry is set if DevHlp failed
             ENDM

GetVector$   MACRO   varnum,erraddr
             ;; Macro to obtain kernel vectors via the GetDOSVar DevHlp
             ;; and set ES:BX to point to this vector.
             ;; USES: AX, BX, ES, DL
                                       ;  AL = Index of desired vector
             MOV     AL,varnum
             DeviceHelp$ GetDOSVar     ;  Call DevHlp router
                                          ;  AX:BX = Far POINTER to vector
                                          ;  Carry set = error, jump to
                                          ;/ |/ caller supplied address:
             JC      erraddr
                                       ;  Else (no error),
             MOV     ES,AX                ;/ ES = segment of entry point
             ENDM

             Page$   <Equates>

;*---------------------------------*
;            DevHlp commands used  *
;*---------------------------------*
;
GetDOSVar    EQU     24h               ;  36 - Get ptr to DOS variable

;*---------------------------------*
;            GetDOSVar indices     *
;*---------------------------------*
;
VectorReboot EQU     5                 ;   5 - Jump to, to reboot

;*---------------------------------------------------*
;*           Device Driver request packet constants. *
;*---------------------------------------------------*
;
ReqPktReg    EQU     ES:[BX]           ;  Request Packet Registers
PktMax       EQU     18                ;  Somewhat arbitrary maximum
                                          ;/ size of request packet
;
;----------- Request packet status ...
;
Done            EQU  0100h
Error           EQU  8000h
General_Failure EQU  000Ch
Unknown_Command EQU  0003h

;
;----------- SERVBOOT Device Driver specific
;
ServBoot_Cat    EQU  80h               ;  SERVBOOT Device Driver category
ServBoot_Func   EQU  40h               ;  SERVBOOT IOCTL function


;*----------------------------------*
;*           Miscellaneous Equates  *
;*----------------------------------*
;
CRLF         EQU     0D0Ah             ;  For DosWrites during Init
StdOut       EQU     1                    ;/
YES          EQU     1                 ;  For error msg processing
NO           EQU     0                    ;/

             Page$   <Data Structures>
;*----------------------------------------------------------*
;*           Structure of a device driver request packet    *
;*----------------------------------------------------------*
Packet       STRUC
PktLen       DB      ?                 ;  Length in bytes of packet
PktUnit      DB      ?                 ;  Subunit number of block device
PktCmd       DB      ?                 ;  Command code
PktStatus    DW      ?                 ;  Status word
PktDOSLink   DD      ?                 ;  Reserved
PktDevLink   DD      ?                 ;  Device multiple-request link
PktData      DB      PktMax DUP(?)     ;  Device specific data area.
Packet       ENDS

;*----------------------------------------------------------*
;*           Equates for initialization packet data         *
;*----------------------------------------------------------*
InitPkt      STRUC
ip_basepkt   DB      PktData DUP (?)   ;  Request header (IGNORE)
ip_data1     DB      ?                 ;  IN  = Nothing
                                       ;  OUT = Number of block devices
                                       ;        or units (UNUSED)
ip_pointer1  DD      ?                 ;  IN  = Pointer to DevHlp entry
                                       ;        point
                                       ;  OUT = (+0) Offset to end of
                                       ;        code segment,
                                       ;        (+2) offset to end of
                                       ;        data segment
ip_pointer2  DD      ?                 ;  IN  = Points to Init arguments
                                       ;        (currently UNUSED)
                                       ;  OUT = Points to BPB array for
                                       ;        block devices (UNUSED)
ip_data2     DB      ?                 ;  IN  = Drive number of first
                                       ;        block device unit
                                       ;        (UNUSED)
                                       ;  OUT = Nothing
InitPkt      ENDS

;*----------------------------------------------------------*
;*           Request Packet for Generic IOCtls.             *
;*----------------------------------------------------------*
IOCtlPkt     STRUC                     ;  Generic IOCtl request packet
             DB      PktData DUP (?)   ;  Request header (IGNORE)
giocategory  DB      ?                 ;  Function category
giofunction  DB      ?                 ;  Function code
gioparapack  DD      ?                 ;  Parameter Buffer address  (UNUSED)
giodatapack  DD      ?                 ;  Data Buffer address (UNUSED)
giosfn       DW      ?                 ;  System File Number  (UNUSED)
IOCtlPkt     ENDS
             Page$   <Data declarations>
COMMENT      |
*-*-*-*-*-*-*-*-*-*-*-*-*- B E G I N G   D A T A -*-*-*-*-*-*-*-*-*-*-*-*-*
*                                                                         *
*            SERVBOOT Data Segment    (Must be declared public)           *
*                                                                         *
              |
DEVDDATA      SEGMENT WORD PUBLIC 'DATA'

;*--------------------------------------------------------------------*
;*           Device header for the SREBOOT$ Device Driver             *
;*                                                                    *
;*           NOTE: This must be first in dd's first data segment.     *
;*--------------------------------------------------------------------*
;
             IFDEF   DEBUG
             PUBLIC  Dev_Header
             ENDIF

Dev_Header   EQU     $
             DD      -1                      ;Ptr to next device hdr  = NONE
             DW      1001100010000000B       ;Device attributes:
             ;       ||||||||||||||||
             ;       |||||||||||||||+-- Bit  0 = Std IN                =  NO
             ;       ||||||||||||||+--- Bit  1 = Std OUT               =  NO
             ;       |||||||||||||+---- Bit  2 = Null Device           =  NO
             ;       ||||||||||||+----- Bit  3 = Clock Device          =  NO
             ;       |||||||||||+------ Bit  4 = Reserved              =   0
             ;       ||||||||||+------- Bit  5 = Reserved              =   0
             ;       |||||||||+-------- Bit  6 = Reserved              =   0
             ;       ||||||+++--------- Bits 7:9 = Func. level= OS/2 DD= 001
             ;       |||||+------------ Bit 10 = Reserved              =   0
             ;       ||||+------------- Bit 11 = Open/Close/Rem. Media = YES
             ;       |||+-------------- Bit 12 = Support shared device
             ;       |||                         access checking       = YES
             ;       ||+--------------- Bit 13 = Output until busy     =  NO
             ;       |+---------------- Bit 14 = Uses IDC              =  NO
             ;       +----------------- Bit 15 = DD type         = CHARACTER
             ;
             DW      DEVDCODE:Strategy       ;  Strategy routine offset
             DW      -1                      ;  Interrupt entry offset (UNUSED)
             DevName$                        ;  Device name (see prologue)
             DW      4 DUP (0)               ;  Reserved words
                                             ;
                                             ;  END device header

;*-------------------------------------*
;*           Misc. variables...        *
;*-------------------------------------*

             IFDEF   DEBUG
             PUBLIC  Dev_Hlp_Addr
             ENDIF

Dev_Hlp_Addr DD      ?                 ;  DevHlp router address

GenFailFlag  DW      NO                ;  For IOCTl subroutine error indication

             Page$   <Data declarations - Request packet command call table>

;*----------------------------------------------------------------------------*
;*           Cmd_Table is a call table used to call a particular subroutine   *
;*           depending on the command code field within the request packet.   *
;*                                                                            *
;*           The table of offsets must be stored in the data segment.         *
;*----------------------------------------------------------------------------*

             IFDEF   DEBUG
             PUBLIC  Cmd_Table
             ENDIF

Cmd_Table    EQU     $                       ;Call table for dd command codes
                                             ;  (Command numbers in decimal)
             DW      Init                    ;   0 - Initialization
             DW      UnkCmd                  ;   1 - Media Check
             DW      UnkCmd                  ;   2 - Build BPB
             DW      UnkCmd                  ;   3 - RESERVED
             DW      Read                    ;   4 - Read
             DW      UnkCmd                  ;   5 - Nondestructive Read
             DW      UnkCmd                  ;   6 - Input Status
             DW      UnkCmd                  ;   7 - Input Flush
             DW      Write                   ;   8 - Write
             DW      UnkCmd                  ;   9 - Write with verify
             DW      UnkCmd                  ;  10 - Output Status
             DW      UnkCmd                  ;  11 - Output Flush
             DW      UnkCmd                  ;  12 - RESERVED
             DW      Open                    ;  13 - Device Open
             DW      Close                   ;  14 - Device Close
             DW      UnkCmd                  ;  15 - Port access
             DW      GenIOCtl                ;  16 - Generic IOCtl command
             DW      UnkCmd                  ;  17 - Reset media
             DW      UnkCmd                  ;  18 - Get Logical Drive Map!
             DW      UnkCmd                  ;  19 - Set Logical Drive Map!
             DW      DeInstall               ;  20 - DeInstall request
                                             ;
Cmd_Max      EQU     ($-Cmd_Table)/2 - 1     ;  Number of entries in CmdTable
                                             ;
                                             ;  Values above 20 get UnkCmd :
                                             ;  21 - Port Access
                                             ;  22 - Partitionable Fixed Disks!
                                             ;  23 - Get fixed disk/unit map!
                                             ;  24:27 - RESERVED

End_Of_Data  EQU     $                 ; Used during initialization.

; HEY !!!!!!
; HEY !!!!!! Any data to be retained past initialization MUST be placed before
; HEY !!!!!! End_Of_Data above...!
; HEY !!!!!!

             Page$   <Data declarations - Init. data>

;*------------------------------------------------------------*
;*           These data will go away after initialization     *
;*------------------------------------------------------------*

Install_Msg  DW      CRLF
             DB      "ServBoot Device Driver: SERVBOOT.SYS, "
             DB      "Version "
             Ver$
             DW      CRLF
             DB      "Unclassified      Author: Pete Guillon"
             DW      CRLF
             DB      "Initialization Complete"
             DW      CRLF
             DW      CRLF

Inst_Msg_Len EQU     $ - Install_Msg

Bwrite       DW      0                 ;  Required for DosWrite


DEVDDATA     ENDS

COMMENT      |
*                                                                         *
*            SERVBOOT Data Segment ends                                   *
*                                                                         *
*-*-*-*-*-*-*-*-*-*-*-*-*- E N D I N G   D A T A -*-*-*-*-*-*-*-*-*-*-*-*-*
             |
             Page$   <Strategy Routine>

COMMENT      |
*-*-*-*-*-*-*-*-*-*-*-*-*- B E G I N G   C O D E -*-*-*-*-*-*-*-*-*-*-*-*-*
*                                                                         *
*            Begin SERVBOOT Code Segment    (Must be declared public)     *
*                                                                         *
             |
DEVDCODE     SEGMENT WORD PUBLIC 'CODE'

             ASSUME CS:DEVDCODE,DS:DEVDDATA
COMMENT      |
*-------------------------------------------------------------------------*
*            Far Procedure STRATEGY gets the request  packet  and based   *
*            on its command code branches to the appropriate subroutine.  *
*            Upon return from the subroutine, the request is  signalled   *
*            serviced with or without error.                              *
*                                                                         *
*            Uses: AX, DI                                                 *
*-------------------------------------------------------------------------*
             |
             IFDEF   DEBUG
             PUBLIC  Strategy
             ENDIF

Strategy     PROC    FAR

             MOV     AL,ReqPktReg.PktCmd    ;  AL = Req. packet command code
             CMP     AL,Cmd_Max             ;  Is command beyond table?
             JA      Unsupported            ;  Yes, go indicate bad command

                                            ;  Else (command within table)
             PUSH    ES                        ;  Save request packet pointers.
             PUSH    BX                        ;  |/
             CBW                               ;  AX = Command
             SHL     AX,1                      ;  AX = Index into CmdTable
             MOV     DI,AX                     ;  DI = CmdTable Index
             MOV     GenFailFlag,NO            ;  Subroutine error indicator=0
             CALL    WORD PTR Cmd_Table[DI]    ;  Call appropriate function
                                               ;  |  (Subroutines may set
                                               ;/ |/ various error statuses)

             POP     BX                     ;  Restore request packet ptrs.
             POP     ES                     ;/

             CMP     GenFailFlag,YES        ;  Subroutine error occurred?
             JNE     Fin                    ;  No, skip General Failure call

                                            ;  Else (Error occurred in subr),
             CALL    GenFail                ;  Request packet status = General
                                            ;/ failure.
             JMP     SHORT Fin

Unsupported: CALL    UnkCmd                 ;  Indicate Unknown command

Fin:         OR      ReqPktReg.PktStatus,Done
             RET

Strategy     ENDP                            ;  End ServBoot strategy routine

             Page$   <Open, Close, Deinstall (non)processing>

COMMENT      |
*------------------------------------------------------------------------*
*            Command was device OPEN, no processing required.            *
*------------------------------------------------------------------------*
             |
             IFDEF   DEBUG
             PUBLIC  Open
             ENDIF

Open         PROC    NEAR

COMMENT      |
*------------------------------------------------------------------------*
*            Command was device CLOSE, no processing required.           *
*------------------------------------------------------------------------*
             |
             IFDEF   DEBUG
             PUBLIC  Close
             ENDIF

Close        LABEL   NEAR

COMMENT      |
*------------------------------------------------------------------------*
*            Command was device DEINSTALL, no processing required.       *
*------------------------------------------------------------------------*
             |
             IFDEF   DEBUG
             PUBLIC  DeInstall
             ENDIF

DeInstall    LABEL   NEAR

             RET                       ;  No processing, just return.

Open         ENDP                      ;  End No processing routines

             Page$   <Generic IOCtl processing>

COMMENT      |
*------------------------------------------------------------------------*
*            Generic IOCtl routine.                                      *
*                                                                        *
*            This routine validates the desired ServBoot IOCtl function, *
*            and calls the appropriate subroutine to handle it:          *
*                                                                        *
*                 IOCTL Category X'80' Funtion X'40' Reboot OS/2         *
*                                                                        *
*            IN:                                                         *
*                                                                        *
*                  GenFailFlag = 0                                       *
*                  ReqPktReg = ES:BX = Request packet address            *
*                                                                        *
*------------------------------------------------------------------------*
             |

             IFDEF   DEBUG
             PUBLIC  GenIOCtl
             ENDIF

GenIOCtl     PROC    NEAR
                                            ;  Category = Our device driver?
             CMP     ReqPktReg.giocategory,ServBoot_Cat
             JNE     GIOunknown             ;  Category != ServBoot Category

                                            ; Test if ServBoot function number
             CMP     ReqPktReg.giofunction,ServBoot_Func
             JNE     GIOunknown             ;/  Function != ServBoot Function

             CALL    Reboot                 ;  Boot OS/2 - Should NOT return
             JMP     SHORT GIOend           ;  Cosmetics: skip error stuff

GIOunknown:  CALL    UnkCmd                 ;  Indicate unknown function
             MOV     GenFailFlag,YES        ;  Turn subroutine error flag on

GIOend:      RET

GenIOCtl     ENDP
             Page$   <Read, Write processing>

COMMENT      |
*------------------------------------------------------------------------*
*            READ/WRITE routine.                                         *
*                                                                        *
*            This routine Reboot OS/2                                    *
*                                                                        *
*            IN:                                                         *
*                                                                        *
*                  GenFailFlag = 0                                       *
*                  ReqPktReg = ES:BX = Request packet address            *
*                                                                        *
*------------------------------------------------------------------------*


*------------------------------------------------------------------------*
*            Command was Read, Reboot OS/2                               *
*------------------------------------------------------------------------*
             |

             IFDEF   DEBUG
             PUBLIC  Read
             ENDIF

Read         PROC    NEAR

COMMENT      |
*------------------------------------------------------------------------*
*            Command was Write, Reboot OS/2                              *
*------------------------------------------------------------------------*
             |

             IFDEF   DEBUG
             PUBLIC  Write
             ENDIF

Write        LABEL   NEAR

             CALL    Reboot                 ;  Boot OS/2 - Should NOT return

             RET                            ;  Cosmetics: return stuff

Read         ENDP


             Page$   <Device driver request packet status routines>

COMMENT      |
*------------------------------------------------------------------------*
*            Mark device driver request packet status with unknown       *
*            command error.                                              *
*                                                                        *
*            IN:                                                         *
*                  ReqPktReg = ES:BX = Request packet address            *
*                                                                        *
*------------------------------------------------------------------------*
             |

             IFDEF   DEBUG
             PUBLIC  UnkCmd
             ENDIF

UnkCmd       PROC    NEAR

             OR      ReqPktReg.PktStatus,Error + Unknown_Command
             RET

UnkCmd       ENDP

COMMENT      |
*------------------------------------------------------------------------*
*            Mark device driver request packet status with General       *
*            Failure error.                                              *
*                                                                        *
*            IN:                                                         *
*                  ReqPktReg = ES:BX = Request packet address            *
*                                                                        *
*------------------------------------------------------------------------*
             |

             IFDEF   DEBUG
             PUBLIC  GenFail
             ENDIF

GenFail      PROC    NEAR

             OR      ReqPktReg.PktStatus,Error + General_Failure
             RET

GenFail      ENDP

             Page$   <IOCtl 40h, Read, Write  - Reboot routine>

COMMENT      |
*------------------------------------------------------------------------*
*            Reboot:  Call saved reboot vector in kernel.                *
*                                                                        *
*            NOTE: The call to the reboot vector should NOT return.      *
*                                                                        *
*------------------------------------------------------------------------*
             |

             IFDEF   DEBUG
             PUBLIC  Reboot
             ENDIF

Reboot       PROC    NEAR

                                            ;  ES:BX = reboot vector:
             GetVector$ VectorReboot,Rfail

             CALL    DWORD PTR ES:[BX]      ;  Call Reboot routine
                                            ;  NOTE: should NOT return

             JMP     SHORT Rend             ;  Cosmetics: skip error stuff

Rfail:       MOV     GenFailFlag,YES        ;  GetVector$ failed,
                                            ;/ Set flag to indicate so

Rend:        RET

Reboot       ENDP

End_Of_Code  EQU     $                      ;  Any code after here is thrown
                                            ;  away after initialization

; HEY !!!!!!
; HEY !!!!!! Any Code to be retained past initialization MUST be placed before
; HEY !!!!!! End_Of_Code above...!
; HEY !!!!!!

             Page$   <End of Code, Init processing>

COMMENT      |
*------------------------------------------------------------------------------*
*            Initialization procedure is placed at the end of the code         *
*            so it can go away once initialization has been done.              *
*                                                                              *
*            ServBoot initialization consists of:                              *
*                                                                              *
*             * Saving the DevHlp address from the request packet              *
*             * Setting EndOfCode and EndOfData offsets in the request packet  *
*                                                                              *
*            IN:                                                               *
*                  ES:BX = Request Packet (Reference InitPkt structure for     *
*                          a description of appropriate fields).               *
*                                                                              *
*------------------------------------------------------------------------------*
             |

Init         PROC    NEAR

             IFDEF   DEBUG
             PUBLIC  InitInt3               ;  Break point at init time

InitInt3:    INT     3
             ENDIF
;
;----------- Get first word of DevHlp entry point and store into our data segment:
;
             MOV     AX,WORD PTR ReqPktReg.ip_pointer1
             MOV     WORD PTR Dev_Hlp_Addr,AX
;
;----------- Get second word of DevHlp entry point and store into our data segment:
;
             MOV     AX,WORD PTR ReqPktReg.ip_pointer1+2
             MOV     WORD PTR Dev_Hlp_Addr+2,AX
;
;----------- Save the pointers to the request packet:
;
             PUSH    ES
             PUSH    BX
;
;----------- Send ServBoot Device Driver installation message to user:
;
             @DosWrite StdOut,Install_Msg,Inst_Msg_Len,Bwrite

             PAGE

;
;----------- Place the addresses of the end of code segment and the
;            end of the data segment into the request header
;
             POP     BX                     ;  Restore request packet addr.
             POP     ES

             LEA     AX,End_Of_Code         ;  AX = end of code segment offset
                                            ;  Store it into request packet
             MOV     WORD PTR ReqPktReg.ip_pointer1,AX

             LEA     AX,End_Of_Data         ;  AX = end of data segment offset
                                            ;  Store it into request packet:
             MOV     WORD PTR ReqPktReg.ip_pointer1+2,AX

             RET

Init         ENDP

DEVDCODE     ENDS

COMMENT      |
*                                                                         *
*            Ending SERVBOOT Code Segment                                 *
*                                                                         *
*-*-*-*-*-*-*-*-*-*-*-*-*- E N D I N G   C O D E -*-*-*-*-*-*-*-*-*-*-*-*-*
             |
             END
