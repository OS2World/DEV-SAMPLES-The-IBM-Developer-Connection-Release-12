;	Static Name Aliases
;
;	$S3813_hSpyDll	EQU	hSpyDll
;	$S3818_DriveLetter	EQU	DriveLetter
;	$S3789_PointedDC	EQU	PointedDC
;	$S3790_Nhvio	EQU	Nhvio
;	$S3791_Length	EQU	Length
;	$S3792_PointerPos	EQU	PointerPos
;	$S3793_Pointed	EQU	Pointed
;	$S3794_LastPointed	EQU	LastPointed
;	$S3795_Subclassed	EQU	Subclassed
;	$S3796_OldProc	EQU	OldProc
;	$S3858_hSpyDll	EQU	hSpyDll
	TITLE   whatisd.c
	.286p
	.287
WHATISD_TEXT	SEGMENT  WORD PUBLIC 'CODE'
WHATISD_TEXT	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME  CS: WHATISD_TEXT, DS: DGROUP, SS: DGROUP
PUBLIC  TARGETWINDOW
PUBLIC  QUERYTEXT
PUBLIC  JOURNAL1
PUBLIC  SUCCESS1
PUBLIC  SUCCESS2
PUBLIC  ERR1
PUBLIC  ERR2
PUBLIC  ERR3
EXTRN	WINQUERYWINDOWULONG:FAR
EXTRN	VIOGETCURPOS:FAR
EXTRN	_strlen:FAR
EXTRN	DOSGETPID:FAR
EXTRN	WINENDPAINT:FAR
EXTRN	WININVALIDATERECT:FAR
EXTRN	WINBEGINPAINT:FAR
EXTRN	WINRELEASEHOOK:FAR
EXTRN	DOSDELETE:FAR
EXTRN	DOSBEEP:FAR
EXTRN	WINSUBCLASSWINDOW:FAR
EXTRN	WINSENDMSG:FAR
EXTRN	WINWINDOWFROMPOINT:FAR
EXTRN	WINBROADCASTMSG:FAR
EXTRN	DOSQCURDISK:FAR
EXTRN	GPIQUERYDEVICE:FAR
EXTRN	WINQUERYWINDOWDC:FAR
EXTRN	DOSQCURDIR:FAR
EXTRN	WINQUERYANCHORBLOCK:FAR
EXTRN	WINQUERYPOINTERPOS:FAR
EXTRN	GREENTRY4:FAR
EXTRN	GREENTRY5:FAR
EXTRN	WINGETLASTERROR:FAR
EXTRN	DOSSIZESEG:FAR
EXTRN	_itoa:FAR
EXTRN	_memset:FAR
EXTRN	_strcat:FAR
EXTRN	WINAVAILMEM:FAR
EXTRN	_ltoa:FAR
EXTRN	VIOREADCHARSTR:FAR
EXTRN	DOSGETMODHANDLE:FAR
EXTRN	_strcpy:FAR
_BSS      SEGMENT
COMM NEAR	AVIOTEXT:	 1:	 80
COMM NEAR	XCUR:	BYTE:	 2
COMM NEAR	YCUR:	BYTE:	 2
COMM NEAR	DIRECTORY:	 1:	 256
COMM NEAR	MAXAVAIL:	BYTE:	 2
COMM NEAR	SEGSIZE:	BYTE:	 4
COMM NEAR	USERPTR:	BYTE:	 4
COMM NEAR	HANDLE0:	BYTE:	 4
COMM NEAR	HANDLE1:	BYTE:	 4
COMM NEAR	HANDLE2:	BYTE:	 4
COMM NEAR	HANDLE3:	BYTE:	 4
COMM NEAR	CREATESTRUCT:	BYTE:	 42
COMM NEAR	SHAREDTEXT:	 1:	 80
COMM NEAR	SHAREDTEXT1:	 1:	 80
_BSS      ENDS
CONST      SEGMENT
$SG3832	DB	'Handle NULL ',  00H
	ORG	$+1
$SG3833	DB	'  ,',  00H
$SG3834	DB	' <-DC  ,',  00H
	ORG	$+1
$SG3835	DB	' <-HWND ',  00H
	ORG	$+1
$SG3850	DB	'Handle NULL',  00H
$SG3852	DB	'WHATISD',  00H
	ORG	$+44
$SG3867	DB	'Handle NULL',  00H
$SG3872	DB	'PMTSTHDL',  00H
	ORG	$+3
$SG3893	DB	'D:\JOURNAL1.OUT',  00H
$SG3894	DB	'D:\JOURNAL1.OUT',  00H
$SG3896	DB	'D:\JOURNAL1.OUT',  00H
$SG3897	DB	'D:\JOURNAL1.OUT',  00H
CONST      ENDS
_DATA      SEGMENT
TARGETWINDOW	DD	00H
QUERYTEXT	DW	00H
$S3795_Subclassed	DD	00H
$S3796_OldProc	DD	00H
JOURNAL1	DD	00H
SUCCESS1	DW	00H
SUCCESS2	DW	00H
ERR1	DD	00H
ERR2	DD	00H
ERR3	DD	00H
_DATA      ENDS
CONST      SEGMENT
	ORG	$-116
$S3818_DriveLetter	DB	'?ABCDEFGHIJKLMNOPQRSTUVWXYZ',  00H
CONST      ENDS
_BSS      SEGMENT
	ORG	$+18
$S3789_PointedDC	DW 02H DUP (?)
	ORG	$-6
$S3790_Nhvio	DW 01H DUP (?)
	ORG	$+4
$S3791_Length	DW 01H DUP (?)
	ORG	$-20
$S3792_PointerPos	DW 04H DUP (?)
	ORG	$-12
$S3793_Pointed	DW 02H DUP (?)
	ORG	$+22
$S3858_hSpyDll	DW 01H DUP (?)
	ORG	$-16
$S3794_LastPointed	DW 02H DUP (?)
	ORG	$+8
$S3813_hSpyDll	DW 01H DUP (?)
_BSS      ENDS
CONST      SEGMENT
	ORG	$-44
$T20000	DW SEG DIRECTORY 
$T20001	DW SEG HANDLE0 
$T20002	DW SEG HANDLE1 
$T20003	DW SEG HANDLE2 
$T20004	DW SEG HANDLE3 
$T20006	DW SEG USERPTR 
$T20007	DW SEG SEGSIZE 
$T20008	DW SEG MAXAVAIL 
CONST      ENDS
WHATISD_TEXT      SEGMENT
	ASSUME	CS: WHATISD_TEXT
; Line 21
; Line 16
; Line 20
; Line 29
; Line 30
; Line 31
; Line 24
; Line 44
; Line 769
; Line 1527
; Line 2703
; Line 2807
; Line 2854
; Line 2944
; Line 45
; Line 46
; Line 48
; Line 22
; Line 23
; Line 24
; Line 25
; Line 26
; Line 762
; Line 65
	PUBLIC	INPUTHOOKPROC
INPUTHOOKPROC	PROC FAR
	enter	WORD PTR 38,0
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
	ASSUME DS: DGROUP
;	habSpy = 12
;	pQmsg = 8
;	bRemove = 6
;	HHeap = -4
;	Drive = -6
;	Map = -12
;	PathLen = -8
;	DriveLetter = 0
; Line 72
	mov	ax,WORD PTR TARGETWINDOW
	or	ax,WORD PTR TARGETWINDOW+2
	jne	$JCC17
	jmp	$I3819
$JCC17:
; Line 73
	les	bx,DWORD PTR [bp+8]	;pQmsg
	cmp	WORD PTR es:[bx+4],-16384
	jb	$JCC31
	jmp	$I3820
$JCC31:
; Line 74
	mov	ax,WORD PTR es:[bx+4]
	cmp	ax,112
	jne	$JCC43
	jmp	$SC3828
$JCC43:
	cmp	ax,116
	je	$JCC51
	jmp	$I3851
$JCC51:
; Line 136
; Line 76
	push	WORD PTR TARGETWINDOW+2
	push	WORD PTR TARGETWINDOW
	add	ah,16
	push	ax
	push	WORD PTR es:[bx+8]
	push	WORD PTR es:[bx+6]
	push	WORD PTR es:[bx+12]
	push	WORD PTR es:[bx+10]
	call	FAR PTR WINSENDMSG
; Line 77
	mov	ax,WORD PTR $S3795_Subclassed
	or	ax,WORD PTR $S3795_Subclassed+2
	je	$JCC96
	jmp	$I3851
$JCC96:
; Line 78
	push	0
	push	1
	push	ds
	push	OFFSET DGROUP:$S3792_PointerPos
	call	FAR PTR WINQUERYPOINTERPOS
; Line 79
	push	0
	push	1
	push	ds
	push	OFFSET DGROUP:$S3792_PointerPos
	push	1
	push	0
	call	FAR PTR WINWINDOWFROMPOINT
	mov	WORD PTR $S3793_Pointed,ax
	mov	WORD PTR $S3793_Pointed+2,dx
	mov	WORD PTR $S3795_Subclassed,ax
	mov	WORD PTR $S3795_Subclassed+2,dx
; Line 80
	push	200
	push	50
	call	FAR PTR DOSBEEP
; Line 81
	push	WORD PTR $S3795_Subclassed+2
	push	WORD PTR $S3795_Subclassed
	push	SEG TEXTWINDOWPROC
	push	OFFSET TEXTWINDOWPROC
	call	FAR PTR WINSUBCLASSWINDOW
	mov	WORD PTR $S3796_OldProc,ax
	mov	WORD PTR $S3796_OldProc+2,dx
; Line 82
	or	ax,dx
	jne	$I3827
	push	1200
	push	50
	call	FAR PTR DOSBEEP
; Line 83
$I3827:
	push	WORD PTR $S3795_Subclassed+2
	push	WORD PTR $S3795_Subclassed
	push	0
	push	0
	push	1
	call	FAR PTR WININVALIDATERECT
; Line 85
; Line 138
	jmp	$I3851
	nop	
$SC3828:
; Line 88
	push	256
	push	0
	mov	ax,OFFSET DIRECTORY
	mov	dx,SEG DIRECTORY
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,8
; Line 89
	lea	ax,WORD PTR [bp-6]	;Drive
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-12]	;Map
	push	ss
	push	ax
	call	FAR PTR DOSQCURDISK
; Line 90
	mov	WORD PTR [bp-8],200	;PathLen
; Line 91
	push	WORD PTR [bp-6]	;Drive
	mov	ax,OFFSET DIRECTORY+3
	mov	dx,SEG DIRECTORY
	push	dx
	push	ax
	lea	ax,WORD PTR [bp-8]	;PathLen
	push	ss
	push	ax
	call	FAR PTR DOSQCURDIR
; Line 92
	cmp	WORD PTR [bp-6],26	;Drive
	jbe	$I3829
	mov	WORD PTR [bp-6],0	;Drive
; Line 93
$I3829:
	mov	es,$T20000
	mov	bx,WORD PTR [bp-6]	;Drive
	mov	al,BYTE PTR $S3818_DriveLetter[bx]
	mov	BYTE PTR es:DIRECTORY,al
; Line 94
	mov	BYTE PTR es:DIRECTORY+1,58
; Line 95
	mov	BYTE PTR es:DIRECTORY+2,92
; Line 96
	mov	bx,WORD PTR [bp-8]	;PathLen
	mov	BYTE PTR es:DIRECTORY[bx+3],0
; Line 97
	push	0
	push	1
	push	ds
	push	OFFSET DGROUP:$S3792_PointerPos
	call	FAR PTR WINQUERYPOINTERPOS
; Line 98
	push	0
	push	1
	push	ds
	push	OFFSET DGROUP:$S3792_PointerPos
	push	1
	push	0
	call	FAR PTR WINWINDOWFROMPOINT
	mov	WORD PTR $S3793_Pointed,ax
	mov	WORD PTR $S3793_Pointed+2,dx
; Line 99
	push	dx
	push	ax
	call	FAR PTR WINQUERYWINDOWDC
	mov	WORD PTR $S3789_PointedDC,ax
	mov	WORD PTR $S3789_PointedDC+2,dx
; Line 102
	push	dx
	push	ax
	push	0
	push	0
	push	0
	push	0
	push	0
	push	16899
	call	FAR PTR GREENTRY4
	mov	es,$T20001
	mov	WORD PTR es:HANDLE0,ax
	mov	WORD PTR es:HANDLE0+2,dx
; Line 103
	push	WORD PTR $S3789_PointedDC+2
	push	WORD PTR $S3789_PointedDC
	push	0
	push	1
	push	0
	push	0
	push	0
	push	16899
	call	FAR PTR GREENTRY4
	mov	es,$T20002
	mov	WORD PTR es:HANDLE1,ax
	mov	WORD PTR es:HANDLE1+2,dx
; Line 104
	push	WORD PTR $S3789_PointedDC+2
	push	WORD PTR $S3789_PointedDC
	push	0
	push	2
	push	0
	push	0
	push	0
	push	16899
	call	FAR PTR GREENTRY4
	mov	es,$T20003
	mov	WORD PTR es:HANDLE2,ax
	mov	WORD PTR es:HANDLE2+2,dx
; Line 105
	push	WORD PTR $S3789_PointedDC+2
	push	WORD PTR $S3789_PointedDC
	push	0
	push	3
	push	0
	push	0
	push	0
	push	16899
	call	FAR PTR GREENTRY4
	mov	es,$T20004
	mov	WORD PTR es:HANDLE3,ax
	mov	WORD PTR es:HANDLE3+2,dx
; Line 106
	push	WORD PTR $S3789_PointedDC+2
	push	WORD PTR $S3789_PointedDC
	push	0
	push	1
	push	0
	push	0
	push	0
	push	16899
	call	FAR PTR GREENTRY4
	mov	$S3790_Nhvio,ax
; Line 107
	mov	WORD PTR $S3791_Length,79
; Line 108
	push	80
	push	0
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,8
; Line 109
	mov	ax,$S3790_Nhvio
	cwd	
	or	dx,ax
	je	$I3830
; Line 110
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	push	ds
	push	OFFSET DGROUP:$S3791_Length
	push	0
	push	0
	push	WORD PTR $S3790_Nhvio
	call	FAR PTR VIOREADCHARSTR
; Line 111
	mov	ax,OFFSET XCUR
	mov	dx,SEG XCUR
	push	dx
	push	ax
	mov	ax,OFFSET YCUR
	mov	dx,SEG YCUR
	push	dx
	push	ax
	push	WORD PTR $S3790_Nhvio
	call	FAR PTR VIOGETCURPOS
; Line 112
; Line 113
	jmp	$I3831
$I3830:
; Line 114
	push	ds
	push	OFFSET DGROUP:$SG3832
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	call	FAR PTR _strcpy
	add	sp,8
; Line 115
	push	10
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	mov	bx,ax
	lea	ax,WORD PTR AVIOTEXT[bx]
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	push	WORD PTR $S3790_Nhvio
	call	FAR PTR _itoa
	add	sp,8
; Line 116
	push	ds
	push	OFFSET DGROUP:$SG3833
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	call	FAR PTR _strcat
	add	sp,8
; Line 117
	push	16
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	mov	bx,ax
	lea	ax,WORD PTR AVIOTEXT[bx]
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	push	WORD PTR $S3789_PointedDC+2
	push	WORD PTR $S3789_PointedDC
	call	FAR PTR _ltoa
	add	sp,10
; Line 118
	push	ds
	push	OFFSET DGROUP:$SG3834
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	call	FAR PTR _strcat
	add	sp,8
; Line 119
	push	16
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	mov	bx,ax
	lea	ax,WORD PTR AVIOTEXT[bx]
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	push	WORD PTR $S3793_Pointed+2
	push	WORD PTR $S3793_Pointed
	call	FAR PTR _ltoa
	add	sp,10
; Line 120
	push	ds
	push	OFFSET DGROUP:$SG3835
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	call	FAR PTR _strcat
	add	sp,8
; Line 121
$I3831:
; Line 122
	mov	ax,WORD PTR $S3793_Pointed
	mov	dx,WORD PTR $S3793_Pointed+2
	cmp	WORD PTR $S3794_LastPointed,ax
	jne	$L20005
	cmp	WORD PTR $S3794_LastPointed+2,dx
	je	$I3836
$L20005:
; Line 123
	push	WORD PTR $S3793_Pointed+2
	push	WORD PTR $S3793_Pointed
	push	4
	call	FAR PTR WINQUERYWINDOWULONG
	mov	WORD PTR [bp-4],ax	;HHeap
	mov	WORD PTR [bp-2],dx
; Line 124
	push	WORD PTR $S3793_Pointed+2
	push	WORD PTR $S3793_Pointed
	push	0
	call	FAR PTR WINQUERYWINDOWULONG
	mov	es,$T20006
	mov	WORD PTR es:USERPTR,ax
	mov	WORD PTR es:USERPTR+2,dx
; Line 125
	or	ax,dx
	je	$I3837
; Line 126
	push	dx
	mov	ax,OFFSET SEGSIZE
	mov	dx,SEG SEGSIZE
	push	dx
	push	ax
	call	FAR PTR DOSSIZESEG
; Line 127
	jmp	SHORT $I3838
	nop	
$I3837:
; Line 128
	mov	es,$T20007
	sub	ax,ax
	mov	WORD PTR es:SEGSIZE+2,ax
	mov	WORD PTR es:SEGSIZE,ax
; Line 129
$I3838:
; Line 130
	push	WORD PTR [bp-2]
	push	WORD PTR [bp-4]	;HHeap
	push	0
	push	0
	call	FAR PTR WINAVAILMEM
	mov	es,$T20008
	mov	WORD PTR es:MAXAVAIL,ax
; Line 131
; Line 132
$I3836:
	push	WORD PTR TARGETWINDOW+2
	push	WORD PTR TARGETWINDOW
	les	bx,DWORD PTR [bp+8]	;pQmsg
	mov	ax,WORD PTR es:[bx+4]
	add	ah,16
	push	ax
	push	WORD PTR es:[bx+8]
	push	WORD PTR es:[bx+6]
	push	WORD PTR es:[bx+12]
	push	WORD PTR es:[bx+10]
	call	FAR PTR WINSENDMSG
; Line 133
	mov	ax,WORD PTR $S3793_Pointed
	mov	dx,WORD PTR $S3793_Pointed+2
	mov	WORD PTR $S3794_LastPointed,ax
	mov	WORD PTR $S3794_LastPointed+2,dx
; Line 134
	jmp	$I3851
$I3820:
; Line 138
; Line 139
	les	bx,DWORD PTR [bp+8]	;pQmsg
	mov	ax,QUERYTEXT
	cmp	WORD PTR es:[bx+4],ax
	je	$JCC1048
	jmp	$I3851
$JCC1048:
;	PtrPos = -26
;	Pted = -30
;	PtedDC = -16
;	hvio = -18
;	Lgth = -38
;	TestProcess = -36
; Line 146
	push	0
	push	1
	lea	ax,WORD PTR [bp-26]	;PtrPos
	push	ss
	push	ax
	call	FAR PTR WINQUERYPOINTERPOS
; Line 147
	push	0
	push	1
	lea	ax,WORD PTR [bp-26]	;PtrPos
	push	ss
	push	ax
	push	1
	push	0
	call	FAR PTR WINWINDOWFROMPOINT
	mov	WORD PTR [bp-30],ax	;Pted
	mov	WORD PTR [bp-28],dx
; Line 148
	push	dx
	push	ax
	call	FAR PTR WINQUERYWINDOWDC
	mov	WORD PTR [bp-16],ax	;PtedDC
	mov	WORD PTR [bp-14],dx
; Line 149
	push	dx
	push	ax
	push	0
	push	1
	push	0
	push	0
	push	0
	push	16899
	call	FAR PTR GREENTRY4
	mov	WORD PTR [bp-18],ax	;hvio
; Line 150
	mov	WORD PTR [bp-38],79	;Lgth
; Line 151
	push	80
	push	0
	mov	ax,OFFSET SHAREDTEXT1
	mov	dx,SEG SHAREDTEXT1
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,8
; Line 152
	mov	ax,WORD PTR [bp-18]	;hvio
	cwd	
	or	dx,ax
	je	$I3848
	mov	ax,OFFSET SHAREDTEXT1
	mov	dx,SEG SHAREDTEXT1
	push	dx
	push	ax
	lea	ax,WORD PTR [bp-38]	;Lgth
	push	ss
	push	ax
	push	0
	push	0
	push	WORD PTR [bp-18]	;hvio
	call	FAR PTR VIOREADCHARSTR
; Line 153
	jmp	SHORT $I3849
	nop	
$I3848:
	push	ds
	push	OFFSET DGROUP:$SG3850
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	call	FAR PTR _strcpy
	add	sp,8
$I3849:
; Line 154
	lea	ax,WORD PTR [bp-36]	;TestProcess
	push	ss
	push	ax
	call	FAR PTR DOSGETPID
; Line 155
	push	WORD PTR TARGETWINDOW+2
	push	WORD PTR TARGETWINDOW
	push	8193
	push	0
	push	WORD PTR [bp-36]	;TestProcess
	push	0
	push	0
	call	FAR PTR WINSENDMSG
; Line 157
; Line 158
; Line 159
	jmp	SHORT $I3851
	nop	
$I3819:
; Line 160
	push	ds
	push	OFFSET DGROUP:$SG3852
	push	ds
	push	OFFSET DGROUP:$S3813_hSpyDll
	call	FAR PTR DOSGETMODHANDLE
; Line 161
	push	WORD PTR [bp+14]
	push	WORD PTR [bp+12]	;habSpy
	push	0
	push	0
	push	1
	push	SEG INPUTHOOKPROC
	push	OFFSET INPUTHOOKPROC
	push	WORD PTR $S3813_hSpyDll
	call	FAR PTR WINRELEASEHOOK
; Line 162
	push	0
	push	1
	push	0
	push	0
	push	0
	push	0
	push	0
	push	0
	call	FAR PTR WINBROADCASTMSG
; Line 163
$I3851:
; Line 164
	sub	ax,ax
	pop	ds
	ASSUME DS: DGROUP
	leave	
	ret	10

INPUTHOOKPROC	ENDP
; Line 172
WHATISD_TEXT      ENDS
CONST      SEGMENT
	ORG	$+50
$T20009	DW SEG CREATESTRUCT 
CONST      ENDS
WHATISD_TEXT      SEGMENT
	ASSUME	CS: WHATISD_TEXT
	PUBLIC	SENDMSGHOOKPROC
SENDMSGHOOKPROC	PROC FAR
	enter	WORD PTR 0,0
	push	di
	push	si
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
	ASSUME DS: DGROUP
;	habSpy = 12
;	pSmh = 8
;	bTask = 6
; Line 174
	mov	ax,WORD PTR TARGETWINDOW
	or	ax,WORD PTR TARGETWINDOW+2
	jne	$JCC1333
	jmp	$I3859
$JCC1333:
; Line 175
	les	bx,DWORD PTR [bp+8]	;pSmh
	mov	ax,WORD PTR es:[bx+8]
	cmp	ax,1
	jne	$JCC1348
	jmp	$SC3868
$JCC1348:
	cmp	ax,52
	je	$SC3864
	cmp	ax,57
	jne	$JCC1361
	jmp	$I3866
$JCC1361:
	cmp	ax,67
	je	$JCC1369
	jmp	$I3871
$JCC1369:
; Line 194
$SC3864:
; Line 178
	push	0
	push	1
	push	ds
	push	OFFSET DGROUP:$S3792_PointerPos
	call	FAR PTR WINQUERYPOINTERPOS
; Line 179
	push	0
	push	1
	push	ds
	push	OFFSET DGROUP:$S3792_PointerPos
	push	1
	push	0
	call	FAR PTR WINWINDOWFROMPOINT
	mov	WORD PTR $S3793_Pointed,ax
	mov	WORD PTR $S3793_Pointed+2,dx
; Line 180
	push	dx
	push	ax
	call	FAR PTR WINQUERYWINDOWDC
	mov	WORD PTR $S3789_PointedDC,ax
	mov	WORD PTR $S3789_PointedDC+2,dx
; Line 181
	push	dx
	push	ax
	push	0
	push	1
	push	0
	push	0
	push	0
	push	16899
	call	FAR PTR GREENTRY4
	mov	$S3790_Nhvio,ax
; Line 182
	mov	WORD PTR $S3791_Length,79
; Line 183
	push	80
	push	0
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,8
; Line 184
	mov	ax,$S3790_Nhvio
	cwd	
	or	dx,ax
	je	$I3865
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	push	ds
	push	OFFSET DGROUP:$S3791_Length
	push	0
	push	0
	push	WORD PTR $S3790_Nhvio
	call	FAR PTR VIOREADCHARSTR
; Line 185
	jmp	SHORT $I3866
	nop	
$I3865:
	push	ds
	push	OFFSET DGROUP:$SG3867
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	call	FAR PTR _strcpy
	add	sp,8
$I3866:
; Line 186
	push	WORD PTR TARGETWINDOW+2
	push	WORD PTR TARGETWINDOW
	les	bx,DWORD PTR [bp+8]	;pSmh
	mov	ax,WORD PTR es:[bx+8]
	add	ah,16
	push	ax
	push	WORD PTR es:[bx+6]
	push	WORD PTR es:[bx+4]
	push	WORD PTR es:[bx+2]
	push	WORD PTR es:[bx]
	call	FAR PTR WINSENDMSG
; Line 196
	jmp	SHORT $I3871
	nop	
$SC3868:
; Line 189
	les	bx,DWORD PTR [bp+8]	;pSmh
	mov	ax,WORD PTR es:[bx]
	mov	dx,WORD PTR es:[bx+2]
	mov	es,$T20009
	mov	di,OFFSET CREATESTRUCT
	mov	si,ax
	push	ds
	mov	ds,dx
	ASSUME DS: NOTHING
	mov	cx,21
	rep	movsw
	pop	ds
	ASSUME DS: DGROUP
; Line 190
	jmp	SHORT $I3866
$I3859:
; Line 196
; Line 197
	push	ds
	push	OFFSET DGROUP:$SG3872
	push	ds
	push	OFFSET DGROUP:$S3858_hSpyDll
	call	FAR PTR DOSGETMODHANDLE
; Line 198
	push	WORD PTR [bp+14]
	push	WORD PTR [bp+12]	;habSpy
	push	0
	push	0
	push	0
	push	SEG SENDMSGHOOKPROC
	push	OFFSET SENDMSGHOOKPROC
	push	WORD PTR $S3858_hSpyDll
	call	FAR PTR WINRELEASEHOOK
; Line 199
	push	0
	push	1
	push	0
	push	0
	push	0
	push	0
	push	0
	push	0
	call	FAR PTR WINBROADCASTMSG
; Line 200
$I3871:
; Line 201
	pop	ds
	ASSUME DS: DGROUP
	pop	si
	pop	di
	leave	
	ret	10

SENDMSGHOOKPROC	ENDP
; Line 202
	PUBLIC	TEXTWINDOWPROC
TEXTWINDOWPROC	PROC FAR
	enter	WORD PTR 48,0
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
	ASSUME DS: DGROUP
;	hwnd = 16
;	msg = 14
;	mp1 = 10
;	mp2 = 6
;	hps = -24
;	hps1 = -28
;	rc = -16
;	lHdc = -20
;	pHdc = -36
;	Journal2 = -32
; Line 208
	mov	ax,WORD PTR [bp+14]	;msg
	cmp	ax,35
	je	$JCC1686
	jmp	$SD3898
$JCC1686:
; Line 211
	mov	ax,WORD PTR $S3795_Subclassed
	or	ax,WORD PTR $S3795_Subclassed+2
	jne	$JCC1698
	jmp	$SB3885
$JCC1698:
;	Subcl = -44
;	hab = -48
;	Err = -40
; Line 215
	mov	ax,WORD PTR $S3795_Subclassed
	mov	dx,WORD PTR $S3795_Subclassed+2
	mov	WORD PTR [bp-44],ax	;Subcl
	mov	WORD PTR [bp-42],dx
; Line 216
	sub	ax,ax
	mov	WORD PTR $S3795_Subclassed+2,ax
	mov	WORD PTR $S3795_Subclassed,ax
; Line 217
	push	ds
	push	OFFSET DGROUP:$SG3893
	push	ax
	push	ax
	call	FAR PTR DOSDELETE
; Line 218
	push	WORD PTR [bp-42]
	push	WORD PTR [bp-44]	;Subcl
	call	FAR PTR WINQUERYANCHORBLOCK
	mov	WORD PTR [bp-48],ax	;hab
	mov	WORD PTR [bp-46],dx
; Line 219
	push	dx
	push	ax
	call	FAR PTR WINGETLASTERROR
	mov	WORD PTR [bp-40],ax	;Err
	mov	WORD PTR [bp-38],dx
; Line 220
	push	ds
	push	OFFSET DGROUP:$SG3894
	push	0
	push	2
	push	0
	push	0
	push	0
	push	0
	push	0
	push	570
	call	FAR PTR GREENTRY5
	mov	WORD PTR JOURNAL1,ax
	mov	WORD PTR JOURNAL1+2,dx
; Line 221
	push	WORD PTR [bp-46]
	push	WORD PTR [bp-48]	;hab
	call	FAR PTR WINGETLASTERROR
	mov	WORD PTR ERR1,ax
	mov	WORD PTR ERR1+2,dx
; Line 222
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	push	0
	push	0
	lea	ax,WORD PTR [bp-16]	;rc
	push	ss
	push	ax
	call	FAR PTR WINBEGINPAINT
	mov	WORD PTR [bp-24],ax	;hps
	mov	WORD PTR [bp-22],dx
; Line 223
	push	dx
	push	ax
	call	FAR PTR GPIQUERYDEVICE
	mov	WORD PTR [bp-20],ax	;lHdc
	mov	WORD PTR [bp-18],dx
; Line 224
	push	WORD PTR [bp-46]
	push	WORD PTR [bp-48]	;hab
	call	FAR PTR WINGETLASTERROR
	mov	WORD PTR ERR2,ax
	mov	WORD PTR ERR2+2,dx
; Line 225
	push	WORD PTR [bp-18]
	push	WORD PTR [bp-20]	;lHdc
	push	WORD PTR JOURNAL1+2
	push	WORD PTR JOURNAL1
	push	0
	push	0
	push	0
	push	572
	call	FAR PTR GREENTRY4
	mov	SUCCESS1,ax
; Line 226
	push	WORD PTR [bp-46]
	push	WORD PTR [bp-48]	;hab
	call	FAR PTR WINGETLASTERROR
	mov	WORD PTR ERR2,ax
	mov	WORD PTR ERR2+2,dx
; Line 227
	push	WORD PTR [bp-22]
	push	WORD PTR [bp-24]	;hps
	call	FAR PTR WINENDPAINT
; Line 228
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	push	0
	push	0
	lea	ax,WORD PTR [bp-16]	;rc
	push	ss
	push	ax
	call	FAR PTR WINBEGINPAINT
	mov	WORD PTR [bp-28],ax	;hps1
	mov	WORD PTR [bp-26],dx
; Line 229
	mov	ax,WORD PTR [bp-24]	;hps
	mov	dx,WORD PTR [bp-22]
	cmp	WORD PTR [bp-28],ax	;hps1
	jne	$L20010
	cmp	WORD PTR [bp-26],dx
	jne	$JCC1973
	jmp	$I3895
$JCC1973:
$L20010:
; Line 230
	push	400
	push	10
	call	FAR PTR DOSBEEP
; Line 231
	push	450
	push	10
	call	FAR PTR DOSBEEP
; Line 232
	push	500
	push	10
	call	FAR PTR DOSBEEP
; Line 233
	push	WORD PTR [bp-18]
	push	WORD PTR [bp-20]	;lHdc
	push	WORD PTR JOURNAL1+2
	push	WORD PTR JOURNAL1
	push	0
	push	0
	push	0
	push	573
	call	FAR PTR GREENTRY4
	mov	SUCCESS2,ax
; Line 234
	push	WORD PTR [bp-26]
	push	WORD PTR [bp-28]	;hps1
	call	FAR PTR GPIQUERYDEVICE
	mov	WORD PTR [bp-20],ax	;lHdc
	mov	WORD PTR [bp-18],dx
; Line 235
	push	ds
	push	OFFSET DGROUP:$SG3896
	push	0
	push	0
	call	FAR PTR DOSDELETE
; Line 236
	push	ds
	push	OFFSET DGROUP:$SG3897
	push	0
	push	2
	push	0
	push	0
	push	0
	push	0
	push	0
	push	570
	call	FAR PTR GREENTRY5
	mov	WORD PTR JOURNAL1,ax
	mov	WORD PTR JOURNAL1+2,dx
; Line 237
	push	WORD PTR [bp-46]
	push	WORD PTR [bp-48]	;hab
	call	FAR PTR WINGETLASTERROR
	mov	WORD PTR ERR1,ax
	mov	WORD PTR ERR1+2,dx
; Line 238
	push	WORD PTR [bp-18]
	push	WORD PTR [bp-20]	;lHdc
	push	WORD PTR JOURNAL1+2
	push	WORD PTR JOURNAL1
	push	0
	push	0
	push	0
	push	572
	call	FAR PTR GREENTRY4
	mov	SUCCESS1,ax
; Line 239
	push	WORD PTR [bp-46]
	push	WORD PTR [bp-48]	;hab
	call	FAR PTR WINGETLASTERROR
	mov	WORD PTR ERR2,ax
	mov	WORD PTR ERR2+2,dx
; Line 240
; Line 241
$I3895:
	push	WORD PTR [bp-26]
	push	WORD PTR [bp-28]	;hps1
	call	FAR PTR WINENDPAINT
; Line 242
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	push	WORD PTR [bp+14]	;msg
	push	WORD PTR [bp+12]
	push	WORD PTR [bp+10]	;mp1
	push	WORD PTR [bp+8]
	push	WORD PTR [bp+6]	;mp2
	call	DWORD PTR $S3796_OldProc
; Line 243
	push	WORD PTR [bp-46]
	push	WORD PTR [bp-48]	;hab
	call	FAR PTR WINGETLASTERROR
	mov	WORD PTR ERR3,ax
	mov	WORD PTR ERR3+2,dx
; Line 244
	push	WORD PTR [bp-18]
	push	WORD PTR [bp-20]	;lHdc
	push	WORD PTR JOURNAL1+2
	push	WORD PTR JOURNAL1
	push	0
	push	0
	push	0
	push	573
	call	FAR PTR GREENTRY4
	mov	SUCCESS2,ax
; Line 245
	push	WORD PTR [bp-46]
	push	WORD PTR [bp-48]	;hab
	call	FAR PTR WINGETLASTERROR
	mov	WORD PTR ERR3,ax
	mov	WORD PTR ERR3+2,dx
; Line 257
	push	WORD PTR [bp-42]
	push	WORD PTR [bp-44]	;Subcl
	push	WORD PTR $S3796_OldProc+2
	push	WORD PTR $S3796_OldProc
	call	FAR PTR WINSUBCLASSWINDOW
; Line 258
	sub	ax,ax
	mov	WORD PTR $S3796_OldProc+2,ax
	mov	WORD PTR $S3796_OldProc,ax
; Line 259
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	push	ax
	push	ax
	push	1
	call	FAR PTR WININVALIDATERECT
; Line 260
; Line 261
	pop	ds
	ASSUME DS: DGROUP
	leave	
	ret	14
	nop	
$SD3898:
; Line 263
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	push	WORD PTR [bp+14]	;msg
	push	WORD PTR [bp+12]
	push	WORD PTR [bp+10]	;mp1
	push	WORD PTR [bp+8]
	push	WORD PTR [bp+6]	;mp2
	call	DWORD PTR $S3796_OldProc
; Line 264
$SB3885:
; Line 266
	pop	ds
	ASSUME DS: DGROUP
	leave	
	ret	14

TEXTWINDOWPROC	ENDP
WHATISD_TEXT	ENDS
END
