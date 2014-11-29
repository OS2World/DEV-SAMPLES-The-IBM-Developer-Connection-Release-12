	TITLE	VDFT.C
	.386
	.387
	INCLUDELIB os2386.lib
	INCLUDELIB dde4nbs.lib
CODE32	SEGMENT DWORD USE32 PUBLIC 'CODE'
CODE32	ENDS
DATA32	SEGMENT DWORD USE32 PUBLIC 'DATA'
DATA32	ENDS
CONST32	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST32	ENDS
BSS32	SEGMENT DWORD USE32 PUBLIC 'BSS'
BSS32	ENDS
CSWAP_TEXT	SEGMENT DWORD USE32 PUBLIC 'CODE'
CSWAP_TEXT	ENDS
CINIT_TEXT	SEGMENT DWORD USE32 PUBLIC 'CODE'
CINIT_TEXT	ENDS
CSWAP_DATA	SEGMENT PARA USE32 PUBLIC 'DATA'
CSWAP_DATA	ENDS
SWAPINSTDATA	SEGMENT PARA USE32 PUBLIC 'DATA'
SWAPINSTDATA	ENDS
CINIT_DATA	SEGMENT PARA USE32 PUBLIC 'DATA'
CINIT_DATA	ENDS
DGROUP	GROUP CONST32, BSS32, DATA32
	ASSUME	CS:FLAT, DS:FLAT, SS:FLAT, ES:FLAT
	PUBLIC	szPDDName
	PUBLIC	HVDM_Current
	PUBLIC	PDB_Current
	EXTRN	VDHREGISTERVDD:PROC
	EXTRN	VDHINSTALLUSERHOOK:PROC
	EXTRN	VDHWAITEVENTSEM:PROC
	EXTRN	VDHRESETEVENTSEM:PROC
	EXTRN	VDHALLOCMEM:PROC
	EXTRN	VDHCREATESEM:PROC
	EXTRN	VDHPOSTEVENTSEM:PROC
	EXTRN	VDHDESTROYSEM:PROC
	EXTRN	memcpy:PROC
	EXTRN	VDHREGISTERAPI:PROC
CINIT_DATA	SEGMENT
szPDDName	DB "PDD.SYS",0H
CINIT_DATA	ENDS
SWAPINSTDATA	SEGMENT
	ALIGN 04H
HVDM_Current	DB 0H
	DB 04H DUP (00H)
PDB_Current	DB 0H
	DB 02H DUP (00H)
SWAPINSTDATA	ENDS
DATA32	SEGMENT
@STAT1	DB "VDD.SYS",0H
@STAT2	DB "VDD.SYS",0H
DATA32	ENDS
BSS32	SEGMENT
	ALIGN 04H
COMM	hirq_DFT:DWORD
	ALIGN 04H
COMM	HVDM_Owner:DWORD
	ALIGN 04H
COMM	pMessage:DWORD
	ALIGN 04H
COMM	SemOS2Handle:DWORD
	ALIGN 04H
COMM	SemWinHandle:DWORD
	ALIGN 04H
COMM	Status_byte:DWORD
BSS32	ENDS
CONST32	SEGMENT
CONST32	ENDS
CODE32	SEGMENT

;***** 116 BOOL EXPENTRY Start_Here(PSZ psz)
	ALIGN 04H
CODE32	ENDS
CINIT_TEXT	SEGMENT

	PUBLIC START_HERE
START_HERE	PROC
	PUSH	EBP
	MOV	EBP,ESP

;***** 121      _interrupt(3);
	INT	3	

;***** 122 #endif
;***** 123      if(!VDHRegisterVDD(VDDNAME,OS2Service,NULL))
	PUSH	OFFSET FLAT:@STAT1
	PUSH	OFFSET FLAT: OS2SERVICE
	PUSH	0H
	CALL	VDHREGISTERVDD
	OR	EAX,EAX
	JNE	BLBL1

;***** 124      {
;***** 125        return(FALSE);
	XOR	EAX,EAX
	JMP	BLBL2
	ALIGN 04H
BLBL1:

;***** 126      }
;***** 127 
;***** 128     /*---------------------------------------------------------------*/
;***** 129     /*- Register a VDM termination handler entry point.             -*/
;***** 130     /*---------------------------------------------------------------*/
;***** 131 
;***** 132     if ((VDHInstallUserHook(VDM_TERMINATE, (PUSERHOOK) VDFTTerminate)) == 0)
	PUSH	01H
	PUSH	OFFSET FLAT: VDFTTERMINATE
	CALL	VDHINSTALLUSERHOOK
	OR	EAX,EAX
	JNE	BLBL3

;***** 133         return FALSE;             /* return FALSE if VDH call failed */
	XOR	EAX,EAX
	JMP	BLBL2
	ALIGN 04H
BLBL3:

;***** 134 
;***** 135     /*---------------------------------------------------------------*/
;***** 136     /*- Register a VDM creation handler entry point.                -*/
;***** 137     /*---------------------------------------------------------------*/
;***** 138 
;***** 139     if ((VDHInstallUserHook(VDM_CREATE, (PUSERHOOK) VDFTCreate)) == 0)
	PUSH	0H
	PUSH	OFFSET FLAT: VDFTCREATE
	CALL	VDHINSTALLUSERHOOK
	OR	EAX,EAX
	JNE	BLBL4

;***** 140         return FALSE;             /* return FALSE if VDH call failed */
	XOR	EAX,EAX
	JMP	BLBL2
	ALIGN 04H
BLBL4:

;***** 141 
;***** 142      return TRUE;
	MOV	EAX,01H
	JMP	BLBL2
	ALIGN 04H

;***** 143 }
BLBL2:
	LEAVE	
	RET	04H
START_HERE	ENDP

;***** 169 LONG EXPENTRY OS2Service(SGID sgid,ULONG ulCommand,ULONG ulCountIn,
	ALIGN 04H
CINIT_TEXT	ENDS
CODE32	SEGMENT

	PUBLIC OS2SERVICE
OS2SERVICE	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,010H

;***** 178      _interrupt(3);
	INT	3	
	ALIGN 04H
BLBL5:

;***** 179 #endif
;***** 180      do
;***** 181      {
;***** 182           switch(ulCommand)
	MOV	EAX,[EBP+018H];	ulCommand
	JMP	BLBL9
	ALIGN 04H
BLBL10:

;***** 183           {
;***** 184           case INIT_COMMAND:
;***** 185                apiRet= OS2InitSetup();
	CALL	OS2InitSetup
	MOV	[EBP-04H],EAX;	apiRet

;***** 186                break;
	JMP	BLBL8
	ALIGN 04H
BLBL11:

;***** 187           case POST_MESSAGE:
;***** 188                apiRet=OS2PostMessage(sgid,ulCountIn,pvDataIn);
	MOV	ECX,[EBP+010H];	pvDataIn
	MOV	EDX,[EBP+014H];	ulCountIn
	MOV	AX,[EBP+01cH];	sgid
	CALL	OS2PostMessage
	MOV	[EBP-04H],EAX;	apiRet

;***** 189                break;
	JMP	BLBL8
	ALIGN 04H
BLBL12:

;***** 190 
;***** 191           case READ_QUEUE:
;***** 192                apiRet= OS2ReadQueue(sgid,ulCountout,pvDataOut);
	MOV	ECX,[EBP+08H];	pvDataOut
	MOV	EDX,[EBP+0cH];	ulCountout
	MOV	AX,[EBP+01cH];	sgid
	CALL	OS2ReadQueue
	MOV	[EBP-04H],EAX;	apiRet

;***** 193                break;
	JMP	BLBL8
	ALIGN 04H
BLBL13:

;***** 194           cas
;***** 194 e TERMINATE_COMMAND:
;***** 195                OS2TerminateService();
	CALL	OS2TerminateService

;***** 196                break;
	JMP	BLBL8
	ALIGN 04H
BLBL14:

;***** 197           default:
;***** 198                apiRet = TRUE;
	MOV	DWORD PTR [EBP-04H],01H;	apiRet

;***** 199                break;
	JMP	BLBL8
	ALIGN 04H
	JMP	BLBL8
	ALIGN 04H
BLBL9:
	OR	EAX,EAX
	JE	BLBL10
	CMP	EAX,01H
	JE	BLBL11
	CMP	EAX,02H
	JE	BLBL12
	CMP	EAX,03H
	JE	BLBL13
	JMP	BLBL14
	ALIGN 04H
BLBL8:

;***** 200           }
;***** 201 
;***** 202 
;***** 203      }ONCE;
BLBL6:

;***** 204      return(apiRet);
	MOV	EAX,[EBP-04H];	apiRet
	JMP	BLBL7
	ALIGN 04H

;***** 205 }
BLBL7:
	ADD	ESP,0cH
	LEAVE	
	RET	018H
OS2SERVICE	ENDP

;***** 231 LONG OS2ReadQueue(SGID sgid,ULONG ulCountout,
	ALIGN 04H

	PUBLIC OS2ReadQueue
OS2ReadQueue	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,0cH
	MOV	[EBP+08H],AX;	sgid
	MOV	[EBP+0cH],EDX;	ulCountout
	MOV	[EBP+010H],ECX;	pvDataOut

;***** 236      _interrupt(3);
	INT	3	
	ALIGN 04H
BLBL15:

;***** 237 #endif
;***** 238      do
;***** 239      {
;***** 240 
;***** 241           /*
;***** 242            *wait until we get cleared
;***** 243            *and if so assume we have a new message waiting for us
;***** 244            */
;***** 245           if(!VDHWaitEventSem(SemOS2Handle,
	PUSH	DWORD PTR  SemOS2Handle
	PUSH	0ffffffffH
	CALL	VDHWAITEVENTSEM
	OR	EAX,EAX
	JNE	BLBL16

;***** 246                           SEM_INDEFINITE_WAIT) )
;***** 247           {
;***** 248                return(TRUE);
	MOV	EAX,01H
	JMP	BLBL17
	ALIGN 04H
BLBL16:

;***** 249           }
;***** 250           /*
;***** 251            *reset the semaphore for the next time
;***** 252            *through
;***** 253            */
;***** 254           VDHResetEventSem(SemOS2Handle);
	PUSH	DWORD PTR  SemOS2Handle
	CALL	VDHRESETEVENTSEM

;***** 255           /*
;***** 256            *retrieve the message data
;***** 257            *and store it in pvDataOut
;***** 258            */
;***** 259           if(OS2ReadMessageData(sgid,ulCountout,pvDataOut) )
	MOV	ECX,[EBP+010H];	pvDataOut
	MOV	EDX,[EBP+0cH];	ulCountout
	MOV	AX,[EBP+08H];	sgid
	CALL	OS2ReadMessageData
	OR	EAX,EAX
	JE	BLBL18

;***** 260           {
;***** 261                return(TRUE);
	MOV	EAX,01H
	JMP	BLBL17
	ALIGN 04H
BLBL18:

;***** 262           }
;***** 263 
;***** 264      }ONCE;
BLBL19:

;***** 265      return(FALSE);
	XOR	EAX,EAX
	JMP	BLBL17
	ALIGN 04H

;***** 266 
;***** 267 }
BLBL17:
	ADD	ESP,0cH
	LEAVE	
	RET	
OS2ReadQueue	ENDP

;***** 294 BOOL OS2InitSetup(VOID)
	ALIGN 04H

	PUBLIC OS2InitSetup
OS2InitSetup	PROC
	PUSH	EBP
	MOV	EBP,ESP

;***** 298      _interrupt(3);
	INT	3	

;***** 299 #endif
;***** 300 
;***** 301      /*
;***** 302       *if we have not created our structure yet
;***** 303       */
;***** 304      if(!pMessage)
	CMP	DWORD PTR  pMessage,0H
	JNE	BLBL20

;***** 305      {
;***** 306 
;***** 307          /*
;***** 308           *allocate our control
;***** 309           *structure for our messagelist
;***** 310           */
;***** 311           if( ( pMessage = VDHAllocMem(sizeof(MESSAGE),
	PUSH	018H
	PUSH	01H
	CALL	VDHALLOCMEM
	MOV	DWORD PTR  pMessage,EAX
	CMP	DWORD PTR  pMessage,0H
	JNE	BLBL21

;***** 312                                         VDHAM_SWAPPABLE) ) == NULL)
;***** 313           {
;***** 314                return(TRUE);
	MOV	EAX,01H
	JMP	BLBL22
	ALIGN 04H
BLBL21:
BLBL20:

;***** 315           }
;***** 316      }
;***** 317      pMessage->SrcSessionIdentifier = -1;
	MOV	EAX,DWORD PTR  pMessage
	MOV	WORD PTR [EAX],0ffffH

;***** 318      pMessage->DstSessionIdentifier = -1;
	MOV	EAX,DWORD PTR  pMessage
	MOV	WORD PTR [EAX+02H],0ffffH

;***** 319      pMessage->ulMessageType        = 0;
	MOV	EAX,DWORD PTR  pMessage
	MOV	DWORD PTR [EAX+04H],0H

;***** 320      pMessage->ulMessageSubCommand  = 0;
	MOV	EAX,DWORD PTR  pMessage
	MOV	DWORD PTR [EAX+08H],0H

;***** 321      pMessage->ulBufferSize         = 0;
	MOV	EAX,DWORD PTR  pMessage
	MOV	DWORD PTR [EAX+0cH],0H

;***** 322      pMessage->pvMessageData    = NULL;
	MOV	EAX,DWORD PTR  pMessage
	MOV	DWORD PTR [EAX+010H],0H

;***** 323      pMessage->pvNextMessage    = NULL;
	MOV	EAX,DWORD PTR  pMessage
	MOV	DWORD PTR [EAX+014H],0H

;***** 324      /*
;***** 325       *create our event semaphore for the
;***** 326       *read thread from the OS/2 process
;***** 327       */
;***** 328 
;***** 329      if(!VDHCreateSem(&SemOS2Handle,
	PUSH	OFFSET FLAT:SemOS2Handle
	PUSH	0H
	CALL	VDHCREATESEM
	OR	EAX,EAX
	JNE	BLBL23

;***** 330                      VDH_EVENTSEM) )
;***** 331      {
;***** 332           return(TRUE);
	MOV	EAX,01H
	JMP	BLBL22
	ALIGN 04H
BLBL23:

;***** 333      }
;***** 334 
;***** 335      VDHResetEventSem(SemOS2Handle);
	PUSH	DWORD PTR  SemOS2Handle
	CALL	VDHRESETEVENTSEM

;***** 336      return(FALSE);
	XOR	EAX,EAX
	JMP	BLBL22
	ALIGN 04H

;***** 337 
;***** 338 }
BLBL22:
	LEAVE	
	RET	
OS2InitSetup	ENDP

;***** 366 BOOL OS2TerminateService(VOID)
	ALIGN 04H

	PUBLIC OS2TerminateService
OS2TerminateService	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,04H

;***** 368      BOOL fError = FALSE;
	MOV	DWORD PTR [EBP-04H],0H;	fError

;***** 369 
;***** 370 #ifdef DEBUG
;***** 371      _interrupt(3);
	INT	3	
	ALIGN 04H
BLBL24:

;***** 372 #endif
;***** 373 
;***** 374      do
;***** 375      {
;***** 376 
;***** 377           /*
;***** 378            *post our semaphore
;***** 379            */
;***** 380           VDHPostEventSem(SemOS2Handle);
	PUSH	DWORD PTR  SemOS2Handle
	CALL	VDHPOSTEVENTSEM

;***** 381           /*
;***** 382            *and get rid of it
;***** 383            */
;***** 384           VDHDestroySem(SemOS2Handle);
	PUSH	DWORD PTR  SemOS2Handle
	CALL	VDHDESTROYSEM

;***** 385 
;***** 386      }ONCE;
BLBL25:

;***** 387      return(fError);
	MOV	EAX,[EBP-04H];	fError
	JMP	BLBL26
	ALIGN 04H

;***** 388 }
BLBL26:
	LEAVE	
	RET	
OS2TerminateService	ENDP

;***** 415 BOOL OS2PostMessage(SGID SessionId,ULONG ulCountIn,PVOID pvDataIn)
	ALIGN 04H

	PUBLIC OS2PostMessage
OS2PostMessage	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,014H
	MOV	[EBP+08H],AX;	SessionId
	MOV	[EBP+0cH],EDX;	ulCountIn
	MOV	[EBP+010H],ECX;	pvDataIn

;***** 417      BOOL fError = FALSE;
	MOV	DWORD PTR [EBP-04H],0H;	fError
	ALIGN 04H
BLBL27:

;***** 418      PMESSAGE pMessage;
;***** 419 
;***** 420 
;***** 421      do
;***** 422      {
;***** 423           /*
;***** 424            *get the next link in our list
;***** 425            */
;***** 426            pMessage = GetNextLink();
	CALL	GetNextLink
	MOV	[EBP-08H],EAX;	pMessage

;***** 427 
;***** 428 
;***** 429           /*
;***** 430            *allocate some local storage
;***** 431            */
;***** 432            if( (pMessage = VDHAllocMem(ulCountIn,VDHAM_SWAPPABLE))== NULL)
	PUSH	DWORD PTR [EBP+0cH];	ulCountIn
	PUSH	01H
	CALL	VDHALLOCMEM
	MOV	[EBP-08H],EAX;	pMessage
	CMP	DWORD PTR [EBP-08H],0H;	pMessage
	JNE	BLBL28

;***** 433            {
;***** 434                return(TRUE);
	MOV	EAX,01H
	JMP	BLBL29
	ALIGN 04H
BLBL28:

;***** 435            }
;***** 436 
;***** 437           /*
;***** 438            *copy our message data in
;***** 439            *and hold onto our session identifier
;***** 440            */
;***** 441            pMessage->SrcSessionIdentifier = SessionId;
	MOV	EAX,[EBP-08H];	pMessage
	MOV	CX,[EBP+08H];	SessionId
	MOV	[EAX],CX

;***** 442            pMessage->ulBufferSize         = ulCountIn;
	MOV	EAX,[EBP-08H];	pMessage
	MOV	ECX,[EBP+0cH];	ulCountIn
	MOV	[EAX+0cH],ECX

;***** 443            memcpy(pMessage->pvMessageData,pvDataIn,
	MOV	ECX,[EBP+0cH];	ulCountIn
	MOV	EDX,[EBP+010H];	pvDataIn
	MOV	EAX,[EBP-08H];	pMessage
	MOV	EAX,[EAX+010H]
	CALL	memcpy
	TEST	EAX,0540000H

;***** 444                   ulCountIn);
;***** 445 
;***** 446 
;***** 447           /*
;***** 448            *clear the semaphore on the Windows side
;***** 449            */
;***** 450           VDHPostEventSem(SemWinHandle);
	PUSH	DWORD PTR  SemWinHandle
	CALL	VDHPOSTEVENTSEM

;***** 451 
;***** 452 
;***** 453      }ONCE;
BLBL30:

;***** 454      return(fError);
	MOV	EAX,[EBP-04H];	fError
	JMP	BLBL29
	ALIGN 04H

;***** 455 }
BLBL29:
	ADD	ESP,0cH
	LEAVE	
	RET	
OS2PostMessage	ENDP

;***** 481 BOOL OS2ReadMessageData(SGID sgid,ULONG ulCountout,PVOID pvDataOut)
	ALIGN 04H

	PUBLIC OS2ReadMessageData
OS2ReadMessageData	PROC
	PUSH	EBP
	MOV	EBP,ESP
	MOV	[EBP+08H],AX;	sgid
	MOV	[EBP+0cH],EDX;	ulCountout
	MOV	[EBP+010H],ECX;	pvDataOut

;***** 488      return(FALSE);
	XOR	EAX,EAX
	JMP	BLBL31
	ALIGN 04H

;***** 489 }
BLBL31:
	LEAVE	
	RET	
OS2ReadMessageData	ENDP

;***** 515 PMESSAGE GetNextLink(VOID)
	ALIGN 04H

	PUBLIC GetNextLink
GetNextLink	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,04H
	ALIGN 04H
BLBL32:

;***** 526           for(pMessageNext = (PMESSAGE)pMessage->pvNextMessage;
	MOV	EAX,DWORD PTR  pMessage
	MOV	EAX,[EAX+014H]
	MOV	[EBP-04H],EAX;	pMessageNext
	ALIGN 04H
BLBL33:
	CMP	DWORD PTR [EBP-04H],0H;	pMessageNext
	JNE	BLBL34
	MOV	EAX,[EBP-04H];	pMessageNext
	MOV	EAX,[EAX+014H]
	MOV	[EBP-04H],EAX;	pMessageNext
	JMP	BLBL33
	ALIGN 04H
BLBL34:

;***** 527                          !pMessageNext;
;***** 528                          pMessageNext = pMessageNext->pvNextMessage)
;***** 529           {
;***** 530                ;
;***** 531           }
;***** 532 
;***** 533 
;***** 534      }ONCE;
BLBL35:

;***** 535      return(pMessageNext);
	MOV	EAX,[EBP-04H];	pMessageNext
	JMP	BLBL36
	ALIGN 04H

;***** 536 }
BLBL36:
	LEAVE	
	RET	
GetNextLink	ENDP

;***** 562 BOOL VddDpmiEntry(ULONG ulMessageID,PVOID pvDataIn,PVOID pvDataOut)
	ALIGN 04H

	PUBLIC VDDDPMIENTRY
VDDDPMIENTRY	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,04H

;***** 564      BOOL fError = FALSE;
	MOV	DWORD PTR [EBP-04H],0H;	fError

;***** 565 
;***** 566           switch(ulMessageID)
	MOV	EAX,[EBP+010H];	ulMessageID
	JMP	BLBL39
	ALIGN 04H
BLBL40:

;***** 567           {
;***** 568           case WINOS2_INIT:
;***** 569                break;
	JMP	BLBL38
	ALIGN 04H
BLBL41:

;***** 570           case WINOS2_REGISTERCALLBACK:
;***** 571                break;
	JMP	BLBL38
	ALIGN 04H
BLBL42:

;***** 572           case WINOS2_TERMINATE:
;***** 573                break;
	JMP	BLBL38
	ALIGN 04H
BLBL43:

;***** 574           case WINOS2_POSTMESSAGE:
;***** 575                break;
	JMP	BLBL38
	ALIGN 04H
BLBL44:

;***** 576           default:
;***** 577                fError = TRUE;
	MOV	DWORD PTR [EBP-04H],01H;	fError

;***** 578                break;
	JMP	BLBL38
	ALIGN 04H
	JMP	BLBL38
	ALIGN 04H
BLBL39:
	OR	EAX,EAX
	JE	BLBL40
	CMP	EAX,01H
	JE	BLBL41
	CMP	EAX,02H
	JE	BLBL42
	CMP	EAX,03H
	JE	BLBL43
	JMP	BLBL44
	ALIGN 04H
BLBL38:

;***** 579           }
;***** 580 
;***** 581      return(fError);
	MOV	EAX,[EBP-04H];	fError
	JMP	BLBL37
	ALIGN 04H

;***** 582 }
BLBL37:
	LEAVE	
	RET	0cH
VDDDPMIENTRY	ENDP

;***** 614 BOOL HOOKENTRY VDFTCreate(HVDM hvdm)
	ALIGN 04H

	PUBLIC VDFTCREATE
VDFTCREATE	PROC
	PUSH	EBP
	MOV	EBP,ESP

;***** 617            _interrupt(3);
	INT	3	

;***** 618 #endif
;***** 619 
;***** 620     /*---------------------------------------------------------------*/
;***** 621     /* Set up per VDM instance data.                                 */
;***** 622     /*---------------------------------------------------------------*/
;***** 623      HVDM_Current = hvdm;
	MOV	EAX,[EBP+08H];	hvdm
	MOV	DWORD PTR  HVDM_Current,EAX

;***** 624 
;***** 625      if(!VDHRegisterAPI(VDDNAME,(PFNHOOK)0,(PFNHOOK)VddDpmiEntry))
	PUSH	OFFSET FLAT:@STAT2
	PUSH	0H
	PUSH	OFFSET FLAT: VDDDPMIENTRY
	CALL	VDHREGISTERAPI
	OR	EAX,EAX
	JNE	BLBL45

;***** 626      {
;***** 627           return(FALSE);
	XOR	EAX,EAX
	JMP	BLBL46
	ALIGN 04H
BLBL45:

;***** 628      }
;***** 629      return  TRUE;
	MOV	EAX,01H
	JMP	BLBL46
	ALIGN 04H

;***** 630 }
BLBL46:
	LEAVE	
	RET	04H
VDFTCREATE	ENDP

;***** 662 BOOL HOOKENTRY VDFTTerminate(HVDM hvdm)
	ALIGN 04H

	PUBLIC VDFTTERMINATE
VDFTTERMINATE	PROC
	PUSH	EBP
	MOV	EBP,ESP

;***** 665            _interrupt(3);
	INT	3	

;***** 666 #endif
;***** 667 
;***** 668         return TRUE;
	MOV	EAX,01H
	JMP	BLBL47
	ALIGN 04H

;***** 669 
;***** 670 }
BLBL47:
	LEAVE	
	RET	04H
VDFTTERMINATE	ENDP
CODE32	ENDS
END	START_HERE
