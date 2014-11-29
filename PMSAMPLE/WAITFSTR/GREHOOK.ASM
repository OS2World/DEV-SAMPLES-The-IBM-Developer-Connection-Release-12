	TITLE	H:\GETTEXT\GREHOOK.C
	.386
	.387
	INCLUDELIB OS2386.LIB
	INCLUDELIB dde4sbs.lib
	INCLUDELIB dde4sbm.lib
CODE32	SEGMENT DWORD USE32 PUBLIC 'CODE'
CODE32	ENDS
DATA32	SEGMENT DWORD USE32 PUBLIC 'DATA'
DATA32	ENDS
CONST32	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST32	ENDS
BSS32	SEGMENT DWORD USE32 PUBLIC 'BSS'
BSS32	ENDS
GLOBDATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
GLOBDATA	ENDS
DGROUP	GROUP CONST32, BSS32, DATA32
	ASSUME	CS:FLAT, DS:FLAT, SS:FLAT, ES:FLAT
	PUBLIC	OS2_PM_DRV_ENABLE_LEVELS
	PUBLIC	Waiting
	PUBLIC	SpiedPid
	PUBLIC	GreEntries
	PUBLIC	WaitString
	EXTRN	_printfieee:PROC
	EXTRN	DosQueryMem:PROC
	EXTRN	memcpy:PROC
	EXTRN	DosSleep:PROC
	EXTRN	DosGetInfoBlocks:PROC
	EXTRN	DosAllocMem:PROC
	EXTRN	strstr:PROC
	EXTRN	DosOpenEventSem:PROC
	EXTRN	DosPostEventSem:PROC
	EXTRN	DosCloseEventSem:PROC
	EXTRN	DosFreeMem:PROC
	EXTRN	strncpy:PROC
	EXTRN	DosCreateEventSem:PROC
	EXTRN	DosWaitEventSem:PROC
	EXTRN	_DosFlatToSel:PROC
	EXTRN	_DosSelToFlat:PROC
	EXTRN	_dllentry:PROC
GLOBDATA	SEGMENT
Waiting	DD 0H
SpiedPid	DD 0H
GreEntries	DB 0H
	DB 0400H DUP (00H)
WaitString	DB 0H
	DB 0100H DUP (00H)
GLOBDATA	ENDS
DATA32	SEGMENT
OS2_PM_DRV_ENABLE_LEVELS	DB 03H
	DB 03H
	DB 03H
	DB 03H
	DB 03H
	DB 03H
	DB 03H
	DB 03H
	DB 03H
	DB 03H
	DB 03H
	DB 03H
	DB 03H
	DB 0H
@STAT1	DB "GREHOOK Graphic engine h"
DB "ook sample",0aH,0H
@STAT2	DB "PM_OS2_DRV_ENABLE called"
DB " for function %X (Instal"
DB "l Monitor)",0aH,0H
@STAT3	DB "Param2 %p rc %d , Size %"
DB "d ,Attr %8.8X ",0aH,0H
@STAT4	DB "Table %p rc %d , Size %d"
DB " ,Attr %8.8X ",0aH,0H
	ALIGN 04H
@STAT5	DB "Graphic table Entries Si"
DB "ze=%d",0aH,0H
	ALIGN 04H
@STAT6	DB "At index %X Address was "
DB "%p",0aH,0H
@STAT7	DB "At index %X Saved Addres"
DB "s is  %p",0aH,0H
	ALIGN 04H
@STAT8	DB "At index %X Address is  "
DB "%p",0aH,0H
@STAT9	DB "function %X pointer Is n"
DB "ot 32 bits (%p)",0aH,0H
	ALIGN 04H
@STATa	DB "At index %X Address was "
DB "%p",0aH,0H
@STATb	DB "At index %X Saved Addres"
DB "s is  %p",0aH,0H
	ALIGN 04H
@STATc	DB "At index %X Address is  "
DB "%p",0aH,0H
@STATd	DB "function %X pointer Is n"
DB "ot 32 bits (%p)",0aH,0H
	ALIGN 04H
@STATe	DB "At index %X Address was "
DB "%p",0aH,0H
@STATf	DB "At index %X Saved Addres"
DB "s is  %p",0aH,0H
	ALIGN 04H
@STAT10	DB "At index %X Address is  "
DB "%p",0aH,0H
@STAT11	DB "function %X pointer Is n"
DB "ot 32 bits (%p)",0aH,0H
	ALIGN 04H
@STAT12	DB "At index %X Address was "
DB "%p",0aH,0H
@STAT13	DB "At index %X Saved Addres"
DB "s is  %p",0aH,0H
	ALIGN 04H
@STAT14	DB "At index %X Address is  "
DB "%p",0aH,0H
@STAT15	DB "function %X pointer Is n"
DB "ot 32 bits (%p)",0aH,0H
	ALIGN 04H
@STAT16	DB "At index %X Address was "
DB "%p",0aH,0H
@STAT17	DB "At index %X Saved Addres"
DB "s is  %p",0aH,0H
	ALIGN 04H
@STAT18	DB "At index %X Address is  "
DB "%p",0aH,0H
@STAT19	DB "function %X pointer Is n"
DB "ot 32 bits (%p)",0aH,0H
	ALIGN 04H
@STAT1a	DB "Received Size %d should "
DB "be TABLES %d",0aH,0H
	ALIGN 04H
@STAT1b	DB "\sem32\waitstr",0H
	ALIGN 04H
@STAT1c	DB "\sem32\waitstr",0H
	ALIGN 04H
@STAT1d	DB "\sem32\waitstr",0H
	DD	_dllentry
DATA32	ENDS
BSS32	SEGMENT
	ALIGN 04H
COMM	hWait:DWORD
	ALIGN 04H
COMM	HookedPid:DWORD
BSS32	ENDS
CONST32	SEGMENT
CONST32	ENDS
CODE32	SEGMENT

;***** 95 LONG APIENTRY OS2_PM_DRV_ENABLE(ULONG Subfunc,
	ALIGN 04H

	PUBLIC OS2_PM_DRV_ENABLE
OS2_PM_DRV_ENABLE	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,020H

;***** 106     if (Subfunc==0x0C) {
	CMP	DWORD PTR [EBP+08H],0cH;	Subfunc
	JNE	FELB30

;***** 107 #ifdef DEBUG
;***** 108        printf("GREHOOK Graphic engine hook sample\n");
	MOV	EAX,OFFSET FLAT: @STAT1
	CALL	_printfieee

;***** 109        printf("PM_OS2_DRV_ENABLE called for function %X (Install Monitor)\n",Subfunc);
	PUSH	DWORD PTR [EBP+08H];	Subfunc
	MOV	EAX,OFFSET FLAT: @STAT2
	SUB	ESP,04H
	CALL	_printfieee

;***** 110 #endif
;***** 111        Size=256*4;
	MOV	DWORD PTR [EBP-04H],0400H;	Size

;***** 112        rc=DosQueryMem(Param2,&Size,&Attr);
	LEA	EAX,[EBP-0cH];	Attr
	PUSH	EAX
	LEA	EAX,[EBP-04H];	Size
	PUSH	EAX
	PUSH	DWORD PTR [EBP+010H];	Param2
	MOV	AL,03H
	CALL	DosQueryMem
	MOV	[EBP-08H],EAX;	rc

;***** 113 #ifdef DEBUG
;***** 114        printf("Param2 %p rc %d , Size %d ,Attr %8.8X \n",Param2,rc,Size,Attr);
	PUSH	DWORD PTR [EBP-0cH];	Attr
	PUSH	DWORD PTR [EBP-04H];	Size
	PUSH	DWORD PTR [EBP-08H];	rc
	PUSH	DWORD PTR [EBP+010H];	Param2
	MOV	EAX,OFFSET FLAT: @STAT3
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,028H

;***** 115 #endif
;***** 116        /*---------------------------------------------------*/
;***** 117        /* Make sure there is enough room in the parameter 2 */
;***** 118        if ((rc==0)&&(Size>=sizeof(TABLES))) {
	CMP	DWORD PTR [EBP-08H],0H;	rc
	JNE	FELB31
	CMP	DWORD PTR [EBP-04H],08H;	Size
	JB	FELB31

;***** 119           Size=0x1000;
	MOV	DWORD PTR [EBP-04H],01000H;	Size

;***** 120           rc=DosQueryMem(Param2->GreEntryTable,&Size,&Attr);
	LEA	EAX,[EBP-0cH];	Attr
	PUSH	EAX
	LEA	EAX,[EBP-04H];	Size
	PUSH	EAX
	MOV	EAX,[EBP+010H];	Param2
	PUSH	DWORD PTR [EAX+04H]
	MOV	AL,03H
	CALL	DosQueryMem
	MOV	[EBP-08H],EAX;	rc

;***** 121 #ifdef DEBUG
;***** 122           printf("Table %p rc %d , Size %d ,Attr %8.8X \n",Param2->GreEntryTable,rc,Size,Attr);
	PUSH	DWORD PTR [EBP-0cH];	Attr
	PUSH	DWORD PTR [EBP-04H];	Size
	PUSH	DWORD PTR [EBP-08H];	rc
	MOV	EAX,[EBP+010H];	Param2
	PUSH	DWORD PTR [EAX+04H]
	MOV	EAX,OFFSET FLAT: @STAT4
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,020H

;***** 123 #endif
;***** 124           Size=min(Size,sizeof(GreEntries));
	CMP	DWORD PTR [EBP-04H],0400H;	Size
	JAE	FELB32
	MOV	EAX,[EBP-04H];	Size
	MOV	[EBP-010H],EAX;	#460
	JMP	FELB33
	ALIGN 04H
FELB32:
	MOV	DWORD PTR [EBP-010H],0400H;	#460
FELB33:
	MOV	EAX,[EBP-010H];	#460
	MOV	[EBP-04H],EAX;	Size

;***** 125 #ifdef DEBUG
;***** 126           printf("Graphic table Entries Size=%d\n",Size);
	PUSH	DWORD PTR [EBP-04H];	Size
	MOV	EAX,OFFSET FLAT: @STAT5
	SUB	ESP,04H
	CALL	_printfieee

;***** 127 #endif
;***** 128           /* Save the original copy of the Table */
;***** 129           memcpy(GreEntries,Param2->GreEntryTable,Size);
	MOV	ECX,[EBP-04H];	Size
	MOV	EAX,[EBP+010H];	Param2
	MOV	EDX,[EAX+04H]
	MOV	EAX,OFFSET FLAT: GreEntries
	CALL	memcpy

;***** 130           Size=4;
	MOV	DWORD PTR [EBP-04H],04H;	Size

;***** 131           /*----------------------------------------------------*/
;***** 132           /*- Set GreCharStringPos Interception ----------------*/
;***** 133           Index=(USHORT)(NGreCharStringPos&0x000000FFL);
	MOV	WORD PTR [EBP-012H],036H;	Index

;***** 134           rc=DosQueryMem((PVOID)Param2->GreEntryTable[Index],&Size,&Attr);
	LEA	EAX,[EBP-0cH];	Attr
	PUSH	EAX
	LEA	EAX,[EBP-04H];	Size
	PUSH	EAX
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	MOV	AL,03H
	CALL	DosQueryMem
	ADD	ESP,014H
	MOV	[EBP-08H],EAX;	rc

;***** 135           if (rc==0) {
	CMP	DWORD PTR [EBP-08H],0H;	rc
	JNE	FELB34

;***** 136 #ifdef DEBUG
;***** 137              printf("At index %X Address was %p\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT6
	SUB	ESP,04H
	CALL	_printfieee

;***** 138              printf("At index %X Saved Address is  %p\n",Index,GreEntries[Index]);
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX*04H+GreEntries]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT7
	SUB	ESP,04H
	CALL	_printfieee

;***** 139 #endif
;***** 140              Param2->GreEntryTable[Index]=HookedGre32Entry10; /* 10 Parms (pmddim.h */
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	MOV	DWORD PTR [EAX+ECX*04H],OFFSET FLAT: HookedGre32Entry10

;***** 141 #ifdef DEBUG
;***** 142              printf("At index %X Address is  %p\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT8
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,024H
	JMP	FELB35
	ALIGN 04H
FELB34:

;***** 143 #endif
;***** 144           } else {
;***** 145              printf("function %X pointer Is not 32 bits (%p)\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT9
	SUB	ESP,04H
	CALL	_printfieee

;***** 146              DosSleep(5000L);
	PUSH	01388H
	MOV	AL,01H
	CALL	DosSleep
	ADD	ESP,010H
FELB35:

;***** 147           } /* endif */
;***** 148           /*----------------------------------------------------*/
;***** 149           /*- Set GreCharString Interception -------------------*/
;***** 150           Index=(USHORT)(NGreCharString&0x000000FFL);
	MOV	WORD PTR [EBP-012H],035H;	Index

;***** 151           rc=DosQueryMem((PVOID)Param2->GreEntryTable[Index],&Size,&Attr);
	LEA	EAX,[EBP-0cH];	Attr
	PUSH	EAX
	LEA	EAX,[EBP-04H];	Size
	PUSH	EAX
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	MOV	AL,03H
	CALL	DosQueryMem
	ADD	ESP,0cH
	MOV	[EBP-08H],EAX;	rc

;***** 152           if (rc==0) {
	CMP	DWORD PTR [EBP-08H],0H;	rc
	JNE	FELB36

;***** 153 #ifdef DEBUG
;***** 154              printf("At index %X Address was %p\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STATa
	SUB	ESP,04H
	CALL	_printfieee

;***** 155              printf("At index %X Saved Address is  %p\n",Index,GreEntries[Index]);
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX*04H+GreEntries]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STATb
	SUB	ESP,04H
	CALL	_printfieee

;***** 156 #endif
;***** 157              Param2->GreEntryTable[Index]=HookedGre32Entry5;
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	MOV	DWORD PTR [EAX+ECX*04H],OFFSET FLAT: HookedGre32Entry5

;***** 158 #ifdef DEBUG
;***** 159              printf("At index %X Address is  %p\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STATc
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,024H
	JMP	FELB37
	ALIGN 04H
FELB36:

;***** 160 #endif
;***** 161           } else {
;***** 162              printf("function %X pointer Is not 32 bits (%p)\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STATd
	SUB	ESP,04H
	CALL	_printfieee

;***** 163              DosSleep(5000L);
	PUSH	01388H
	MOV	AL,01H
	CALL	DosSleep
	ADD	ESP,010H
FELB37:

;***** 164           } /* endif */
;***** 165           /*----------------------------------------------------*/
;***** 166           /*- Set GreCharStr Interception  (VIO graphic)-------*/
;***** 167           Index=(USHORT)(NGreCharStr&0x000000FFL);
	MOV	WORD PTR [EBP-012H],03cH;	Index

;***** 168           rc=DosQueryMem((PVOID)Param2->GreEntryTable[Index],&Size,&Attr);
	LEA	EAX,[EBP-0cH];	Attr
	PUSH	EAX
	LEA	EAX,[EBP-04H];	Size
	PUSH	EAX
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	MOV	AL,03H
	CALL	DosQueryMem
	ADD	ESP,0cH
	MOV	[EBP-08H],EAX;	rc

;***** 169           if (rc==0) {
	CMP	DWORD PTR [EBP-08H],0H;	rc
	JNE	FELB38

;***** 170 #ifdef DEBUG
;***** 171              printf("At index %X Address was %p\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STATe
	SUB	ESP,04H
	CALL	_printfieee

;***** 172              printf("At index %X Saved Address is  %p\n",Index,GreEntries[Index]);
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX*04H+GreEntries]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STATf
	SUB	ESP,04H
	CALL	_printfieee

;***** 173 #endif
;***** 174              Param2->GreEntryTable[Index]=HookedGre32Entry5;
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	MOV	DWORD PTR [EAX+ECX*04H],OFFSET FLAT: HookedGre32Entry5

;***** 175 #ifdef DEBUG
;***** 176              printf("At index %X Address is  %p\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT10
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,024H
	JMP	FELB39
	ALIGN 04H
FELB38:

;***** 177 #endif
;***** 178           } else {
;***** 179              printf("function %X pointer Is not 32 bits (%p)\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT11
	SUB	ESP,04H
	CALL	_printfieee

;***** 180              DosSleep(5000L);
	PUSH	01388H
	MOV	AL,01H
	CALL	DosSleep
	ADD	ESP,010H
FELB39:

;***** 181           } /* endif */
;***** 182           /*----------------------------------------------------*/
;***** 183           /*- Set GreCharRect Interception  (VIO graphic) ------*/
;***** 184           Index=(USHORT)(NGreCharRect&0x000000FFL);
	MOV	WORD PTR [EBP-012H],03bH;	Index

;***** 185           rc=DosQueryMem((PVOID)Param2->GreEntryTable[Index],&Size,&Attr);
	LEA	EAX,[EBP-0cH];	Attr
	PUSH	EAX
	LEA	EAX,[EBP-04H];	Size
	PUSH	EAX
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	MOV	AL,03H
	CALL	DosQueryMem
	ADD	ESP,0cH
	MOV	[EBP-08H],EAX;	rc

;***** 186           if (rc==0) {
	CMP	DWORD PTR [EBP-08H],0H;	rc
	JNE	FELB40

;***** 187 #ifdef DEBUG
;***** 188              printf("At index %X Address was %p\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT12
	SUB	ESP,04H
	CALL	_printfieee

;***** 189              printf("At index %X Saved Address is  %p\n",Index,GreEntries[Index]);
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX*04H+GreEntries]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT13
	SUB	ESP,04H
	CALL	_printfieee

;***** 190 #endif
;***** 191              Param2->GreEntryTable[Index]=HookedGre32Entry5;
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	MOV	DWORD PTR [EAX+ECX*04H],OFFSET FLAT: HookedGre32Entry5

;***** 192 #ifdef DEBUG
;***** 193              printf("At index %X Address i
;***** 193 s  %p\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT14
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,024H
	JMP	FELB41
	ALIGN 04H
FELB40:

;***** 194 #endif
;***** 195           } else {
;***** 196              printf("function %X pointer Is not 32 bits (%p)\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT15
	SUB	ESP,04H
	CALL	_printfieee

;***** 197              DosSleep(5000L);
	PUSH	01388H
	MOV	AL,01H
	CALL	DosSleep
	ADD	ESP,010H
FELB41:

;***** 198           } /* endif */
;***** 199           /*----------------------------------------------------*/
;***** 200           /*- Set GreSetupDC  Interception  (Init graphic) -----*/
;***** 201           Index=(USHORT)(NGreSetupDC&0x000000FFL);
	MOV	WORD PTR [EBP-012H],081H;	Index

;***** 202           rc=DosQueryMem((PVOID)Param2->GreEntryTable[Index],&Size,&Attr);
	LEA	EAX,[EBP-0cH];	Attr
	PUSH	EAX
	LEA	EAX,[EBP-04H];	Size
	PUSH	EAX
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	MOV	AL,03H
	CALL	DosQueryMem
	ADD	ESP,0cH
	MOV	[EBP-08H],EAX;	rc

;***** 203           if (rc==0) {
	CMP	DWORD PTR [EBP-08H],0H;	rc
	JNE	FELB42

;***** 204 #ifdef DEBUG
;***** 205              printf("At index %X Address was %p\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT16
	SUB	ESP,04H
	CALL	_printfieee

;***** 206              printf("At index %X Saved Address is  %p\n",Index,GreEntries[Index]);
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX*04H+GreEntries]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT17
	SUB	ESP,04H
	CALL	_printfieee

;***** 207 #endif
;***** 208              Param2->GreEntryTable[Index]=HookedGre32Entry8;
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	MOV	DWORD PTR [EAX+ECX*04H],OFFSET FLAT: HookedGre32Entry8

;***** 209 #ifdef DEBUG
;***** 210              printf("At index %X Address is  %p\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT18
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,024H
	JMP	FELB43
	ALIGN 04H
FELB42:

;***** 211 #endif
;***** 212           } else {
;***** 213              printf("function %X pointer Is not 32 bits (%p)\n",Index,Param2->GreEntryTable[Index]);
	MOV	EAX,[EBP+010H];	Param2
	MOV	EAX,[EAX+04H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	Index
	PUSH	DWORD PTR [EAX+ECX*04H]
	XOR	EAX,EAX
	MOV	AX,[EBP-012H];	Index
	PUSH	EAX
	MOV	EAX,OFFSET FLAT: @STAT19
	SUB	ESP,04H
	CALL	_printfieee

;***** 214              DosSleep(5000L);
	PUSH	01388H
	MOV	AL,01H
	CALL	DosSleep
	ADD	ESP,010H
FELB43:
	JMP	FELB44
	ALIGN 04H
FELB31:

;***** 215           } /* endif */
;***** 216 
;***** 217        } else {
;***** 218           printf("Received Size %d should be TABLES %d\n",Size,sizeof(TABLES));
	PUSH	08H
	PUSH	DWORD PTR [EBP-04H];	Size
	MOV	EAX,OFFSET FLAT: @STAT1a
	SUB	ESP,04H
	CALL	_printfieee

;***** 219           DosSleep(5000L);
	PUSH	01388H
	MOV	AL,01H
	CALL	DosSleep
	ADD	ESP,010H
FELB44:

;***** 220        } /* endif */
;***** 221 #ifdef DEBUG
;***** 222        DosSleep(5000L);   /* Allow some time to read messages at boot time */
	PUSH	01388H
	MOV	AL,01H
	CALL	DosSleep
	ADD	ESP,04H

;***** 223 #endif
;***** 224        return 0;
	MOV	EAX,0H
	JMP	FELB45
	ALIGN 04H
FELB30:

;***** 225     } else {
;***** 226        return -1;
	MOV	EAX,0ffffffffH
	JMP	FELB45
	ALIGN 04H
FELB46:

;***** 227     } /* endif function 0x0C */
;***** 228 }
FELB45:
	ADD	ESP,0cH
	LEAVE	
	RET	
OS2_PM_DRV_ENABLE	ENDP

;***** 274 BOOL  CheckSpying() {
	ALIGN 04H

	PUBLIC CheckSpying
CheckSpying	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,0cH

;***** 280     if (Waiting==FALSE) return FALSE;
	CMP	DWORD PTR  Waiting,0H
	JNE	FELB47
	MOV	EAX,0H
	JMP	FELB48
	ALIGN 04H
FELB47:

;***** 281     /*---------------------------------------------------------*/
;***** 282     /*--If Pid is 0 wait for string anywhere      -------------*/
;***** 283     if (SpiedPid==0) return TRUE;
	CMP	DWORD PTR  SpiedPid,0H
	JNE	FELB49
	MOV	EAX,01H
	JMP	FELB48
	ALIGN 04H
FELB49:

;***** 284     /*---------------------------------------------------------*/
;***** 285     /*--Now compare Pid     wanted and current   --------------*/
;***** 286     /*--comparing Tid too does not work for Comm Mgr AVio------*/
;***** 287     rc=DosGetInfoBlocks(&ptib,&ppib);
	LEA	EAX,[EBP-0cH];	ppib
	PUSH	EAX
	LEA	EAX,[EBP-08H];	ptib
	PUSH	EAX
	MOV	AL,02H
	CALL	DosGetInfoBlocks
	ADD	ESP,08H
	MOV	[EBP-04H],EAX;	rc

;***** 288     if (rc==0) {
	CMP	DWORD PTR [EBP-04H],0H;	rc
	JNE	FELB50

;***** 289        if (ppib->pib_ulpid==SpiedPid) {
	MOV	EAX,[EBP-0cH];	ppib
	MOV	ECX,DWORD PTR  SpiedPid
	CMP	[EAX],ECX
	JNE	FELB51

;***** 290           return TRUE;
	MOV	EAX,01H
	JMP	FELB48
	ALIGN 04H
FELB51:

;***** 291        } else {
;***** 292           return FALSE;
	MOV	EAX,0H
	JMP	FELB48
	ALIGN 04H
FELB52:
	JMP	FELB53
	ALIGN 04H
FELB50:

;***** 293        }
;***** 294     } else {
;***** 295        return FALSE;
	MOV	EAX,0H
	JMP	FELB48
	ALIGN 04H
FELB53:

;***** 296     } /* endif */
;***** 297 }
FELB48:
	LEAVE	
	RET	
CheckSpying	ENDP

;***** 298 VOID PmGpiMatch(PUCHAR  InData,ULONG Size) {
	ALIGN 04H

	PUBLIC PmGpiMatch
PmGpiMatch	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,014H
	MOV	[EBP+08H],EAX;	InData
	MOV	[EBP+0cH],EDX;	Size

;***** 301    rc=DosAllocMem((PVOID)&Data,Size+1,fALLOC);
	PUSH	057H
	MOV	EAX,[EBP+0cH];	Size
	INC	EAX
	PUSH	EAX
	LEA	EAX,[EBP-08H];	Data
	PUSH	EAX
	MOV	AL,03H
	CALL	DosAllocMem
	ADD	ESP,0cH
	MOV	[EBP-04H],EAX;	rc

;***** 302    if (rc==0) {
	CMP	DWORD PTR [EBP-04H],0H;	rc
	JNE	FELB54

;***** 303       Data[Size]=0x00; /* NUll terminate string for strstr */
	MOV	EAX,[EBP-08H];	Data
	MOV	ECX,[EBP+0cH];	Size
	MOV	BYTE PTR [EAX+ECX],0H

;***** 304       memcpy(Data,InData,Size);
	MOV	ECX,[EBP+0cH];	Size
	MOV	EDX,[EBP+08H];	InData
	MOV	EAX,[EBP-08H];	Data
	CALL	memcpy

;***** 305       if (strstr(Data,WaitString)) {
	MOV	EDX,OFFSET FLAT: WaitString
	MOV	EAX,[EBP-08H];	Data
	CALL	strstr
	OR	EAX,EAX
	JE	FELB55

;***** 306          Waiting   =FALSE;
	MOV	DWORD PTR  Waiting,0H

;***** 307          hWait=0;
	MOV	DWORD PTR  hWait,0H

;***** 308          DosOpenEventSem(WAITSEM,&hWait);
	PUSH	OFFSET FLAT: hWait
	PUSH	OFFSET FLAT: @STAT1b
	MOV	AL,02H
	CALL	DosOpenEventSem

;***** 309          DosPostEventSem(hWait);
	PUSH	DWORD PTR  hWait
	MOV	AL,01H
	CALL	DosPostEventSem

;***** 310          DosCloseEventSem(hWait);
	PUSH	DWORD PTR  hWait
	MOV	AL,01H
	CALL	DosCloseEventSem
	ADD	ESP,010H

;***** 311          hWait=0;
	MOV	DWORD PTR  hWait,0H
FELB55:

;***** 312       } /* endif */
;***** 313       DosFreeMem(Data);
	PUSH	DWORD PTR [EBP-08H];	Data
	MOV	AL,01H
	CALL	DosFreeMem
	ADD	ESP,04H
FELB54:

;***** 314    } /* endif */
;***** 315 }
FELB56:
	ADD	ESP,0cH
	LEAVE	
	RET	
PmGpiMatch	ENDP

;***** 316 VOID VioMatch(VioPs *pVio) {
	ALIGN 04H

	PUBLIC VioMatch
VioMatch	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,020H
	MOV	[EBP+08H],EAX;	pVio

;***** 321    Size=0x2000;
	MOV	DWORD PTR [EBP-04H],02000H;	Size

;***** 322    rc=DosQueryMem((PVOID)pVio->LVB,&Size,&Attr);
	LEA	EAX,[EBP-0cH];	Attr
	PUSH	EAX
	LEA	EAX,[EBP-04H];	Size
	PUSH	EAX
	MOV	EAX,[EBP+08H];	pVio
	PUSH	DWORD PTR [EAX+014H]
	MOV	AL,03H
	CALL	DosQueryMem
	ADD	ESP,0cH
	MOV	[EBP-08H],EAX;	rc

;***** 323    if (rc==0) {
	CMP	DWORD PTR [EBP-08H],0H;	rc
	JNE	FELB57

;***** 324       PUCHAR Data;
;***** 325       Size=pVio->Rows*pVio->Cols;
	MOV	EAX,[EBP+08H];	pVio
	XOR	ECX,ECX
	MOV	CX,[EAX+024H]
	MOV	EDX,[EBP+08H];	pVio
	XOR	EAX,EAX
	MOV	AX,[EDX+022H]
	IMUL	EAX,ECX
	MOV	[EBP-04H],EAX;	Size

;***** 326       rc=DosAllocMem((PVOID)&Data,Size+1,fALLOC);
	PUSH	057H
	MOV	EAX,[EBP-04H];	Size
	INC	EAX
	PUSH	EAX
	LEA	EAX,[EBP-010H];	Data
	PUSH	EAX
	MOV	AL,03H
	CALL	DosAllocMem
	ADD	ESP,0cH
	MOV	[EBP-08H],EAX;	rc

;***** 327       if (rc==0) {
	CMP	DWORD PTR [EBP-08H],0H;	rc
	JNE	FELB58

;***** 328          USHORT Cell,CellSize,Offset;
;***** 329          CellSize= pVio->CellSize;
	MOV	EAX,[EBP+08H];	pVio
	MOV	AX,[EAX+020H]
	MOV	[EBP-012H],AX;	CellSize

;***** 330          Offset=0L;
	MOV	WORD PTR [EBP-014H],0H;	Offset

;***** 331          for (Cell=0;Cell<CellSize*pVio->Rows*pVio->Cols;Cell+=CellSize) {
	MOV	WORD PTR [EBP-016H],0H;	Cell
	ALIGN 04H
FELB59:
	MOV	ECX,[EBP+08H];	pVio
	XOR	EAX,EAX
	MOV	AX,[ECX+022H]
	XOR	ECX,ECX
	MOV	CX,[EBP-012H];	CellSize
	IMUL	ECX,EAX
	MOV	EDX,[EBP+08H];	pVio
	XOR	EAX,EAX
	MOV	AX,[EDX+024H]
	IMUL	EAX,ECX
	XOR	ECX,ECX
	MOV	CX,[EBP-016H];	Cell
	CMP	EAX,ECX
	JLE	FELB60

;***** 332             Data[Offset]=pVio->LVB[Cell];
	MOV	EAX,[EBP+08H];	pVio
	MOV	EAX,[EAX+014H]
	XOR	ECX,ECX
	MOV	CX,[EBP-016H];	Cell
	MOV	DL,BYTE PTR [EAX+ECX]
	MOV	EAX,[EBP-010H];	Data
	XOR	ECX,ECX
	MOV	CX,[EBP-014H];	Offset
	MOV	BYTE PTR [EAX+ECX],DL

;***** 333             Offset++;
	MOV	AX,[EBP-014H];	Offset
	INC	AX
	MOV	[EBP-014H],AX;	Offset
	MOV	AX,[EBP-012H];	CellSize
	ADD	AX,[EBP-016H];	Cell
	MOV	[EBP-016H],AX;	Cell
	JMP	FELB59
	ALIGN 04H
FELB60:

;***** 334          } /* endfor */
;***** 335          Data[Size]=0x00; /* NUll terminate string for strstr */
	MOV	EAX,[EBP-010H];	Data
	MOV	ECX,[EBP-04H];	Size
	MOV	BYTE PTR [EAX+ECX],0H

;***** 336          if (strstr(Data,WaitString)) {
	MOV	EDX,OFFSET FLAT: WaitString
	MOV	EAX,[EBP-010H];	Data
	CALL	strstr
	OR	EAX,EAX
	JE	FELB61

;***** 337             Waiting   =FALSE;
	MOV	DWORD PTR  Waiting,0H

;***** 338             hWait=0;
	MOV	DWORD PTR  hWait,0H

;***** 339             DosOpenEventSem(WAITSEM,&hWait);
	PUSH	OFFSET FLAT: hWait
	PUSH	OFFSET FLAT: @STAT1c
	MOV	AL,02H
	CALL	DosOpenEventSem

;***** 340             DosPostEventSem(hWait);
	PUSH	DWORD PTR  hWait
	MOV	AL,01H
	CALL	DosPostEventSem

;***** 341             DosCloseEventSem(hWait);
	PUSH	DWORD PTR  hWait
	MOV	AL,01H
	CALL	DosCloseEventSem
	ADD	ESP,010H

;***** 342             hWait=0;
	MOV	DWORD PTR  hWait,0H
FELB61:

;***** 343          } /* endif */
;***** 344          DosFreeMem(Data);
	PUSH	DWORD PTR [EBP-010H];	Data
	MOV	AL,01H
	CALL	DosFreeMem
	ADD	ESP,04H
FELB58:
FELB57:

;***** 345       } /* endif */
;***** 346    } /* endif */
;***** 347 }
FELB62:
	ADD	ESP,08H
	LEAVE	
	RET	
VioMatch	ENDP

;***** 350 ULONG APIENTRY HookedGre32Entry5(HDC hDC,...) {
	ALIGN 04H

	PUBLIC HookedGre32Entry5
HookedGre32Entry5	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,024H

;***** 364    va_start(args,hDC);
	LEA	EAX,[EBP+0cH];	hDC
	MOV	[EBP-04H],EAX;	args

;***** 365    b  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-08H],EAX;	b

;***** 366    c  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-0cH],EAX;	c

;***** 367    d  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-010H],EAX;	d

;***** 368    Fct=va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-014H],EAX;	Fct

;***** 369    va_end(args);
	MOV	DWORD PTR [EBP-04H],0H;	args

;***** 370    /*-----------------------------------------------------------------*/
;***** 371    /* Switch depending on Function -----------------------------------*/
;***** 372    switch (Fct&0x0000FFFFL) {
	MOV	EAX,[EBP-014H];	Fct
	AND	EAX,0ffffH
	JMP	BLBL68
	ALIGN 04H
BLBL69:

;***** 373       /*-----------------------------------------------------------------*/
;***** 374       /* PM Gpi Char String operation -----------------------------------*/
;***** 375       case NGreCharString:
;***** 376          if (CheckSpying()) {
	CALL	CheckSpying
	OR	EAX,EAX
	JE	FELB63

;***** 377              PmGpiMatch((UCHAR *)c,b);
	MOV	EDX,[EBP-08H];	b
	MOV	EAX,[EBP-0cH];	c
	CALL	PmGpiMatch
FELB63:

;***** 378          } /* endif */
;***** 379          break;
	JMP	BLBL67
	ALIGN 04H
BLBL70:

;***** 380       /*-----------------------------------------------------------------*/
;***** 381       /* VIO Char String operation    -----------------------------------*/
;***** 382       case NGreCharStr:
;***** 383          if (CheckSpying()) { /* If we are spying for that process/thread */
	CALL	CheckSpying
	OR	EAX,EAX
	JE	FELB64

;***** 384              VioMatch((VioPs *)b);
	MOV	EAX,[EBP-08H];	b
	CALL	VioMatch
FELB64:

;***** 385          } /* endif */
;***** 386          break;
	JMP	BLBL67
	ALIGN 04H
BLBL71:

;***** 387       /*-----------------------------------------------------------------*/
;***** 388       /* VIO Char Rect operation      -----------------------------------*/
;***** 389       case NGreCharRect:
;***** 390          if (CheckSpying()) { /* If we are spying for that process/thread */
	CALL	CheckSpying
	OR	EAX,EAX
	JE	FELB65

;***** 391              VioMatch((VioPs *)b);
	MOV	EAX,[EBP-08H];	b
	CALL	VioMatch
FELB65:

;***** 392          } /* endif */
;***** 393          break;
	JMP	BLBL67
	ALIGN 04H
BLBL72:

;***** 394       default:
;***** 395          break;
	JMP	BLBL67
	ALIGN 04H
	JMP	BLBL67
	ALIGN 04H
BLBL68:
	CMP	EAX,05035H
	JE	BLBL69
	CMP	EAX,0403cH
	JE	BLBL70
	CMP	EAX,0403bH
	JE	BLBL71
	JMP	BLBL72
	ALIGN 04H
BLBL67:

;***** 396    } /* endswitch */
;***** 397    /*-----------------------------------------------------------------*/
;***** 398    /* Get Function index and call default handling -------------------*/
;***** 399    Index=(USHORT)(Fct&0x000000FFL);
	MOV	EAX,[EBP-014H];	Fct
	AND	EAX,0ffH
	MOV	[EBP-016H],AX;	Index

;***** 400    return (*GreEntries[Index])(hDC,b,c,d,Fct);
	XOR	EAX,EAX
	MOV	AX,[EBP-016H];	Index
	MOV	EAX,DWORD PTR [EAX*04H+GreEntries]
	MOV	[EBP-01cH],EAX;	#469
	PUSH	DWORD PTR [EBP-014H];	Fct
	PUSH	DWORD PTR [EBP-010H];	d
	PUSH	DWORD PTR [EBP-0cH];	c
	PUSH	DWORD PTR [EBP-08H];	b
	PUSH	DWORD PTR [EBP+08H];	hDC
	MOV	AL,05H
	CALL	DWORD PTR [EBP-01cH];	#469
	ADD	ESP,014H
	JMP	FELB66
	ALIGN 04H

;***** 401 }
FELB66:
	ADD	ESP,08H
	LEAVE	
	RET	
HookedGre32Entry5	ENDP

;***** 404 ULONG APIENTRY HookedGre32Entry8(HDC hDC,...) {
	ALIGN 04H

	PUBLIC HookedGre32Entry8
HookedGre32Entry8	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,028H

;***** 411    va_start(args,hDC);
	LEA	EAX,[EBP+0cH];	hDC
	MOV	[EBP-04H],EAX;	args

;***** 412    b  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-08H],EAX;	b

;***** 413    c  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-0cH],EAX;	c

;***** 414    d  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-010H],EAX;	d

;***** 415    e  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-014H],EAX;	e

;***** 416    f  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-018H],EAX;	f

;***** 417    g  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-01cH],EAX;	g

;***** 418    Fct=va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-020H],EAX;	Fct

;***** 419    va_end(args);
	MOV	DWORD PTR [EBP-04H],0H;	args

;***** 420    /*-----------------------------------------------------------------*/
;***** 421    /* Switch depending on Function -----------------------------------*/
;***** 422    /* Would need SetupDC to correlate window with HDC clip rectangle--*/
;***** 423    /* To be sure we are getting the right text                      --*/
;***** 424 // if ((Fct&0x0000FFFF)==NGreSetupDC) {
;***** 425 //    if (CheckSpying()) { /* If we are spying for that process/thread */
;***** 426 //    } /* endif */
;***** 427 // } /* endif */
;***** 428    /*-----------------------------------------------------------------*/
;***** 429    /* Get Function index and call default handling -------------------*/
;***** 430    Index=(USHORT)(Fct&0x000000FFL);
	MOV	EAX,[EBP-020H];	Fct
	AND	EAX,0ffH
	MOV	[EBP-022H],AX;	Index

;***** 431    return (*GreEntries[Index])(hDC,b,c,d,e,f,g,Fct);
	XOR	EAX,EAX
	MOV	AX,[EBP-022H];	Index
	MOV	EAX,DWORD PTR [EAX*04H+GreEntries]
	MOV	[EBP-028H],EAX;	#478
	PUSH	DWORD PTR [EBP-020H];	Fct
	PUSH	DWORD PTR [EBP-01cH];	g
	PUSH	DWORD PTR [EBP-018H];	f
	PUSH	DWORD PTR [EBP-014H];	e
	PUSH	DWORD PTR [EBP-010H];	d
	PUSH	DWORD PTR [EBP-0cH];	c
	PUSH	DWORD PTR [EBP-08H];	b
	PUSH	DWORD PTR [EBP+08H];	hDC
	MOV	AL,08H
	CALL	DWORD PTR [EBP-028H];	#478
	ADD	ESP,020H
	JMP	FELB73
	ALIGN 04H
FELB73:
	LEAVE	
	RET	
HookedGre32Entry8	ENDP

;***** 435 ULONG APIENTRY HookedGre32Entry10(HDC hDC,...) {
	ALIGN 04H

	PUBLIC HookedGre32Entry10
HookedGre32Entry10	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,038H

;***** 442    va_start(args,hDC);
	LEA	EAX,[EBP+0cH];	hDC
	MOV	[EBP-04H],EAX;	args

;***** 443    b  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-08H],EAX;	b

;***** 444    c  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-0cH],EAX;	c

;***** 445    d  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-010H],EAX;	d

;***** 446    e  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-014H],EAX;	e

;***** 447    f  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-018H],EAX;	f

;***** 448    g  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-01cH],EAX;	g

;***** 449    h  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-020H],EAX;	h

;***** 450    i  =va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-024H],EAX;	i

;***** 451    Fct=va_arg(args,ULONG);
	MOV	EAX,[EBP-04H];	args
	ADD	EAX,04H
	MOV	[EBP-04H],EAX;	args
	MOV	EAX,[EBP-04H];	args
	MOV	EAX,[EAX-04H]
	MOV	[EBP-028H],EAX;	Fct

;***** 452    va_end(args);
	MOV	DWORD PTR [EBP-04H],0H;	args

;***** 453    /*-----------------------------------------------------------------*/
;***** 454    /* Switch depending on Function -----------------------------------*/
;***** 455    if ((Fct&0x0000FFFF)==NGreCharStringPos) {
	MOV	EAX,[EBP-028H];	Fct
	AND	EAX,0ffffH
	CMP	EAX,07036H
	JNE	FELB74

;***** 456       if (CheckSpying()) { /* If we are spying for that process/thread */
	CALL	CheckSpying
	OR	EAX,EAX
	JE	FELB75

;***** 457          PmGpiMatch((UCHAR *)f,e);
	MOV	EDX,[EBP-014H];	e
	MOV	EAX,[EBP-018H];	f
	CALL	PmGpiMatch
FELB75:
FELB74:

;***** 458       } /* endif */
;***** 459    } /* endif */
;***** 460    /*-----------------------------------------------------------------*/
;***** 461    /* Get Function index and call default handling -------------------*/
;***** 462    Index=(USHORT)(Fct&0x000000FFL);
	MOV	EAX,[EBP-028H];	Fct
	AND	EAX,0ffH
	MOV	[EBP-02aH],AX;	Index

;***** 463    return (*GreEntries[Index])(hDC,b,c,d,e,f,g,h,i,Fct);
	XOR	EAX,EAX
	MOV	AX,[EBP-02aH];	Index
	MOV	EAX,DWORD PTR [EAX*04H+GreEntries]
	MOV	[EBP-030H],EAX;	#490
	PUSH	DWORD PTR [EBP-028H];	Fct
	PUSH	DWORD PTR [EBP-024H];	i
	PUSH	DWORD PTR [EBP-020H];	h
	PUSH	DWORD PTR [EBP-01cH];	g
	PUSH	DWORD PTR [EBP-018H];	f
	PUSH	DWORD PTR [EBP-014H];	e
	PUSH	DWORD PTR [EBP-010H];	d
	PUSH	DWORD PTR [EBP-0cH];	c
	PUSH	DWORD PTR [EBP-08H];	b
	PUSH	DWORD PTR [EBP+08H];	hDC
	MOV	AL,0aH
	CALL	DWORD PTR [EBP-030H];	#490
	ADD	ESP,028H
	JMP	FELB76
	ALIGN 04H

;***** 464 }
FELB76:
	ADD	ESP,08H
	LEAVE	
	RET	
HookedGre32Entry10	ENDP

;***** 465 APIRET APIENTRY WaitForString(PID WinPid,PUCHAR String) {
	ALIGN 04H

	PUBLIC WaitForString
WaitForString	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,010H

;***** 467    hWait=0;
	MOV	DWORD PTR  hWait,0H

;***** 468    strncpy(WaitString,String,STRINGSIZE);
	MOV	ECX,0ffH
	MOV	EDX,[EBP+0cH];	String
	MOV	EAX,OFFSET FLAT: WaitString
	CALL	strncpy

;***** 469    WaitString[STRINGSIZE]=0x00;
	MOV	BYTE PTR  WaitString+0ffH,0H

;***** 470    rc=DosCreateEventSem(WAITSEM,&hWait,0,FALSE);
	PUSH	0H
	PUSH	0H
	PUSH	OFFSET FLAT: hWait
	PUSH	OFFSET FLAT: @STAT1d
	MOV	AL,04H
	CALL	DosCreateEventSem
	ADD	ESP,010H
	MOV	[EBP-04H],EAX;	rc

;***** 471    if (rc!=NO_ERROR) {
	CMP	DWORD PTR [EBP-04H],0H;	rc
	JE	FELB77

;***** 472       return rc;
	MOV	EAX,[EBP-04H];	rc
	JMP	FELB78
	ALIGN 04H
FELB77:

;***** 473    } /* endif */
;***** 474    Waiting   =TRUE;
	MOV	DWORD PTR  Waiting,01H

;***** 475    SpiedPid  = WinPid;
	MOV	EAX,[EBP+08H];	WinPid
	MOV	DWORD PTR  SpiedPid,EAX

;***** 476    /* Now wait until the string appears */
;***** 477    rc=DosWaitEventSem(hWait,SEM_INDEFINITE_WAIT);
	PUSH	0ffffffffH
	PUSH	DWORD PTR  hWait
	MOV	AL,02H
	CALL	DosWaitEventSem
	MOV	[EBP-04H],EAX;	rc

;***** 478    Waiting   =FALSE;
	MOV	DWORD PTR  Waiting,0H

;***** 479    DosCloseEventSem(hWait);
	PUSH	DWORD PTR  hWait
	MOV	AL,01H
	CALL	DosCloseEventSem
	ADD	ESP,0cH

;***** 480    hWait=0;
	MOV	DWORD PTR  hWait,0H

;***** 481    return rc;
	MOV	EAX,[EBP-04H];	rc
	JMP	FELB78
	ALIGN 04H

;***** 482 }
FELB78:
	ADD	ESP,0cH
	LEAVE	
	RET	
WaitForString	ENDP
CODE32	ENDS
END
