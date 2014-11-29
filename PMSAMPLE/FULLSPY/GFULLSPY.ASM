;	Static Name Aliases
;
;	$S2655_DriveLetter	EQU	DriveLetter
	TITLE   gviospy.c
	.286p
	.287
GVIOSPY_TEXT	SEGMENT  WORD PUBLIC 'CODE'
GVIOSPY_TEXT	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME  CS: GVIOSPY_TEXT, DS: DGROUP, SS: DGROUP
PUBLIC  _ThreadStarted
EXTRN	DOSEXIT:FAR
EXTRN	DOSSEMCLEAR:FAR
EXTRN	DOSSEMSET:FAR
EXTRN	DOSSEMWAIT:FAR
EXTRN	DOSCREATETHREAD:FAR
EXTRN	DOSQCURDISK:FAR
EXTRN	DOSALLOCSEG:FAR
EXTRN	DOSQCURDIR:FAR
EXTRN	DOSCREATESEM:FAR
EXTRN	DOSOPENSEM:FAR
EXTRN	DOSENTERCRITSEC:FAR
EXTRN	DOSEXITCRITSEC:FAR
EXTRN	DOSALLOCSHRSEG:FAR
EXTRN	DOSGETPID:FAR
EXTRN	VIOWRTCHARSTR:FAR
EXTRN	DOSGETSHRSEG:FAR
EXTRN	DOSBEEP:FAR
_BSS      SEGMENT
COMM NEAR	_StackPtr:	BYTE:	 4
COMM NEAR	_SelStack:	BYTE:	 2
COMM NEAR	_ThreadID:	BYTE:	 2
_BSS      ENDS
_DATA      SEGMENT
	ORG	$+2
$SG2656	DB	'Thread Started   ',  00H
$SG2657	DB	'\SEM\FULLSPY.SEM1',  00H
$SG2659	DB	'\SEM\FULLSPY.SEM1',  00H
$SG2663	DB	'\SEM\FULLSPY.SEM2',  00H
$SG2665	DB	'\SEM\FULLSPY.SEM2',  00H
$SG2666	DB	'\SHAREMEM\FULLSPY.DAT',  00H
$SG2668	DB	'\SHAREMEM\FULLSPY.DAT',  00H
$SG2672	DB	'Waiting            ',  00H
$SG2674	DB	'Good PID           ',  00H
$SG2677	DB	'Bad  PID           ',  00H
$SG2678	DB	'Thread Ended       ',  00H
	ORG	$-216
_ThreadStarted	DW	00H
	ORG	$+214
$S2655_DriveLetter	DB	'?ABCDEFGHIJKLMNOPQRSTUVWXYZ',  00H
_DATA      ENDS
GVIOSPY_TEXT      SEGMENT
	ASSUME	CS: GVIOSPY_TEXT
; Line 18
; Line 4
; Line 17
; Line 21
; Line 30
; Line 31
; Line 32
; Line 25
; Line 41
; Line 2700
; Line 42
; Line 43
; Line 45
; Line 5
; Line 6
; Line 7
; Line 8
; Line 37
	PUBLIC	_VIOSERVICE
_VIOSERVICE	PROC FAR
	pusha	
	push	ds
	push	es
	mov	bp,sp
	sub	sp,6
;	temp2 = 24
;	temp1 = 26
;	index = 28
;	AxDxOnEntry = -4
;	PAxDxFunction = -6
; Line 40
	mov	WORD PTR [bp-6],OFFSET _AxDxFunction	;PAxDxFunction
; Line 42
	call	WORD PTR [bp-6]	;PAxDxFunction
	mov	WORD PTR [bp-4],ax	;AxDxOnEntry
	mov	WORD PTR [bp-2],dx
; Line 44
	call	_ReadScreen
; Line 46
	mov	ax,WORD PTR [bp-4]	;AxDxOnEntry
	mov	WORD PTR [bp+18],ax
	mov	sp,bp
	pop	es
	pop	ds
	ASSUME DS: DGROUP
	popa	
	ret	
	nop	

_VIOSERVICE	ENDP
; Line 50
	PUBLIC	_AxDxFunction
_AxDxFunction	PROC NEAR
	ret	
	nop	

_AxDxFunction	ENDP
; Line 51
GVIOSPY_TEXT      ENDS
CONST      SEGMENT
$T20000	DW SEG _SelStack 
$T20001	DW SEG _StackPtr 
CONST      ENDS
GVIOSPY_TEXT      SEGMENT
	ASSUME	CS: GVIOSPY_TEXT
	PUBLIC	_ReadScreen
_ReadScreen	PROC NEAR
	enter	WORD PTR 2,0
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
	ASSUME DS: DGROUP
; Line 53
	call	FAR PTR DOSENTERCRITSEC
; Line 54
	cmp	WORD PTR _ThreadStarted,0
	je	$I2634
; Line 55
	call	FAR PTR DOSEXITCRITSEC
; Line 56
	pop	ds
	ASSUME DS: DGROUP
	leave	
	ret	
$I2634:
;	rc = -2
; Line 58
	mov	WORD PTR _ThreadStarted,1
; Line 59
	call	FAR PTR DOSEXITCRITSEC
; Line 60
	push	1000
	mov	ax,OFFSET _SelStack
	mov	dx,SEG _SelStack
	push	dx
	push	ax
	push	1
	call	FAR PTR DOSALLOCSEG
; Line 61
	mov	es,$T20000
	mov	dx,WORD PTR es:_SelStack
	mov	ax,998
	mov	es,$T20001
	mov	WORD PTR es:_StackPtr,ax
	mov	WORD PTR es:_StackPtr+2,dx
; Line 62
	push	SEG _SpyThread
	push	OFFSET _SpyThread
	mov	ax,OFFSET _ThreadID
	mov	dx,SEG _ThreadID
	push	dx
	push	ax
	push	WORD PTR es:_StackPtr+2
	push	WORD PTR es:_StackPtr
	call	FAR PTR DOSCREATETHREAD
	mov	WORD PTR [bp-2],ax	;rc
; Line 63
	or	ax,ax
	je	$I2640
; Line 64
	push	200
	push	50
	call	FAR PTR DOSBEEP
; Line 65
	cmp	WORD PTR [bp-2],8	;rc
	je	$I2640
; Line 66
	push	250
	push	50
	call	FAR PTR DOSBEEP
; Line 67
	cmp	WORD PTR [bp-2],89	;rc
	je	$I2640
; Line 68
	push	300
	push	50
	call	FAR PTR DOSBEEP
; Line 69
	cmp	WORD PTR [bp-2],212	;rc
	je	$I2640
; Line 70
	push	350
	push	50
	call	FAR PTR DOSBEEP
; Line 71
; Line 72
$I2640:
; Line 73
; Line 74
; Line 75
; Line 76
	pop	ds
	ASSUME DS: DGROUP
	leave	
	ret	

_ReadScreen	ENDP
; Line 83
	PUBLIC	_SpyThread
_SpyThread	PROC FAR
	enter	WORD PTR 30,0
	push	di
	push	si
;	rc = -4
;	Sem1 = -8
;	Sem2 = -12
;	DirInfo = -24
;	DirSel = -2
;	PidInfo = -18
;	Drive = -20
;	Map = -30
;	PathLen = -26
;	DriveLetter = 0
; Line 93
	push	1400
	push	50
	call	FAR PTR DOSBEEP
; Line 94
	push	1500
	push	50
	call	FAR PTR DOSBEEP
; Line 95
	push	1550
	push	50
	call	FAR PTR DOSBEEP
; Line 96
	push	1450
	push	50
	call	FAR PTR DOSBEEP
; Line 97
	push	1350
	push	50
	call	FAR PTR DOSBEEP
; Line 98
	push	ds
	push	OFFSET DGROUP:$SG2656
	push	15
	push	24
	push	1
	push	0
	call	FAR PTR VIOWRTCHARSTR
; Line 100
	push	1
	lea	ax,WORD PTR [bp-8]	;Sem1
	push	ss
	push	ax
	push	ds
	push	OFFSET DGROUP:$SG2657
	call	FAR PTR DOSCREATESEM
	mov	WORD PTR [bp-4],ax	;rc
; Line 101
	cmp	ax,183
	jne	$I2658
; Line 102
	lea	ax,WORD PTR [bp-8]	;Sem1
	push	ss
	push	ax
	push	ds
	push	OFFSET DGROUP:$SG2659
	call	FAR PTR DOSOPENSEM
	mov	WORD PTR [bp-4],ax	;rc
; Line 103
	jmp	SHORT $I2660
$I2658:
; Line 104
	cmp	WORD PTR [bp-4],0	;rc
	je	$JCC342
	jmp	$I2661
$JCC342:
; Line 105
	push	WORD PTR [bp-6]
	push	WORD PTR [bp-8]	;Sem1
	call	FAR PTR DOSSEMSET
; Line 106
; Line 110
$I2660:
; Line 113
	push	1
	lea	ax,WORD PTR [bp-12]	;Sem2
	push	ss
	push	ax
	push	ds
	push	OFFSET DGROUP:$SG2663
	call	FAR PTR DOSCREATESEM
	mov	WORD PTR [bp-4],ax	;rc
; Line 114
	cmp	ax,183
	jne	$I2664
; Line 115
	lea	ax,WORD PTR [bp-12]	;Sem2
	push	ss
	push	ax
	push	ds
	push	OFFSET DGROUP:$SG2665
	call	FAR PTR DOSOPENSEM
	mov	WORD PTR [bp-4],ax	;rc
; Line 116
; Line 118
$I2664:
	push	258
	push	ds
	push	OFFSET DGROUP:$SG2666
	lea	ax,WORD PTR [bp-2]	;DirSel
	push	ss
	push	ax
	call	FAR PTR DOSALLOCSHRSEG
	mov	WORD PTR [bp-4],ax	;rc
; Line 119
	cmp	ax,183
	jne	$I2667
; Line 120
	push	ds
	push	OFFSET DGROUP:$SG2668
	lea	ax,WORD PTR [bp-2]	;DirSel
	push	ss
	push	ax
	call	FAR PTR DOSGETSHRSEG
	mov	WORD PTR [bp-4],ax	;rc
; Line 121
; Line 122
$I2667:
	mov	dx,WORD PTR [bp-2]	;DirSel
	sub	ax,ax
	mov	WORD PTR [bp-24],ax	;DirInfo
	mov	WORD PTR [bp-22],dx
; Line 123
	lea	ax,WORD PTR [bp-18]	;PidInfo
	push	ss
	push	ax
	call	FAR PTR DOSGETPID
; Line 125
$FC2670:
; Line 126
	push	ds
	push	OFFSET DGROUP:$SG2672
	push	15
	push	24
	push	1
	push	0
	call	FAR PTR VIOWRTCHARSTR
; Line 127
	push	WORD PTR [bp-6]
	push	WORD PTR [bp-8]	;Sem1
	push	-1
	push	-1
	call	FAR PTR DOSSEMWAIT
; Line 128
	les	bx,DWORD PTR [bp-24]	;DirInfo
	mov	ax,WORD PTR [bp-18]	;PidInfo
	cmp	WORD PTR es:[bx],ax
	je	$JCC503
	jmp	$I2673
$JCC503:
; Line 129
	push	ds
	push	OFFSET DGROUP:$SG2674
	push	15
	push	24
	push	1
	push	0
	call	FAR PTR VIOWRTCHARSTR
; Line 130
	push	1000
	push	20
	call	FAR PTR DOSBEEP
; Line 131
	push	900
	push	20
	call	FAR PTR DOSBEEP
; Line 132
	push	1000
	push	20
	call	FAR PTR DOSBEEP
; Line 133
	push	WORD PTR [bp-6]
	push	WORD PTR [bp-8]	;Sem1
	call	FAR PTR DOSSEMSET
; Line 134
	les	bx,DWORD PTR [bp-24]	;DirInfo
	mov	cx,256
	lea	di,WORD PTR [bx+2]
	sub	ax,ax
	rep	stosb
	mov	dx,es
; Line 135
	lea	ax,WORD PTR [bp-20]	;Drive
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-30]	;Map
	push	ss
	push	ax
	call	FAR PTR DOSQCURDISK
; Line 136
	mov	WORD PTR [bp-26],200	;PathLen
; Line 137
	push	WORD PTR [bp-20]	;Drive
	mov	ax,WORD PTR [bp-24]	;DirInfo
	mov	dx,WORD PTR [bp-22]
	add	ax,5
	push	dx
	push	ax
	lea	ax,WORD PTR [bp-26]	;PathLen
	push	ss
	push	ax
	call	FAR PTR DOSQCURDIR
; Line 138
	cmp	WORD PTR [bp-20],26	;Drive
	jbe	$I2675
	mov	WORD PTR [bp-20],0	;Drive
; Line 139
$I2675:
	les	bx,DWORD PTR [bp-24]	;DirInfo
	mov	si,WORD PTR [bp-20]	;Drive
	mov	al,BYTE PTR $S2655_DriveLetter[si]
	mov	BYTE PTR es:[bx+2],al
; Line 140
	mov	BYTE PTR es:[bx+3],58
; Line 141
	mov	BYTE PTR es:[bx+4],92
; Line 142
	mov	si,WORD PTR [bp-26]	;PathLen
	mov	BYTE PTR es:[bx+5][si],0
; Line 143
	push	WORD PTR [bp-10]
	push	WORD PTR [bp-12]	;Sem2
	call	FAR PTR DOSSEMCLEAR
; Line 144
	jmp	$FC2670
$I2661:
; Line 106
; Line 107
	push	0
	push	0
	call	FAR PTR DOSEXIT
; Line 108
	pop	si
	pop	di
	leave	
	ret	
	nop	
$I2673:
; Line 144
; Line 145
	push	ds
	push	OFFSET DGROUP:$SG2677
	push	15
	push	24
	push	1
	push	0
	call	FAR PTR VIOWRTCHARSTR
; Line 146
	push	WORD PTR [bp-10]
	push	WORD PTR [bp-12]	;Sem2
	push	-1
	push	-1
	call	FAR PTR DOSSEMWAIT
; Line 147
; Line 148
	jmp	$FC2670
	nop	

_SpyThread	ENDP
GVIOSPY_TEXT	ENDS
END
