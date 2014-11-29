;	Static Name Aliases
;
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
PUBLIC  AVIOHMQ
PUBLIC  WROTETOSESS
EXTRN	WINPOSTQUEUEMSG:FAR
EXTRN	VIOREADCHARSTR:FAR
EXTRN	VIOGETCURPOS:FAR
EXTRN	DOSGETINFOSEG:FAR
_BSS      SEGMENT
COMM NEAR	SESSIONS:	 1:	 32000
COMM NEAR	XCURSOR:	 2:	 16
COMM NEAR	YCURSOR:	 2:	 16
_BSS      ENDS
_DATA      SEGMENT
AVIOHMQ	DD	00H
WROTETOSESS	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
_DATA      ENDS
GVIOSPY_TEXT      SEGMENT
	ASSUME	CS: GVIOSPY_TEXT
; Line 18
; Line 5
; Line 17
; Line 21
; Line 30
; Line 31
; Line 32
; Line 25
; Line 41
; Line 770
; Line 1522
; Line 2700
; Line 2804
; Line 2851
; Line 2941
; Line 42
; Line 43
; Line 45
; Line 6
; Line 7
; Line 8
; Line 9
; Line 39
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
; Line 42
	mov	WORD PTR [bp-6],OFFSET _AxDxFunction	;PAxDxFunction
; Line 44
	call	WORD PTR [bp-6]	;PAxDxFunction
	mov	WORD PTR [bp-4],ax	;AxDxOnEntry
	mov	WORD PTR [bp-2],dx
; Line 46
	call	_ReadScreen
; Line 48
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
; Line 52
	PUBLIC	_AxDxFunction
_AxDxFunction	PROC NEAR
	ret	
	nop	

_AxDxFunction	ENDP
; Line 53
	PUBLIC	_ReadScreen
_ReadScreen	PROC NEAR
	enter	WORD PTR 16,0
	push	si
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
	ASSUME DS: DGROUP
;	PGInfoSeg = -6
;	PLInfoSeg = -14
;	Session = -8
;	GInfoSel = -10
;	LInfoSel = -16
;	Length = -2
; Line 61
	lea	ax,WORD PTR [bp-10]	;GInfoSel
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-16]	;LInfoSel
	push	ss
	push	ax
	call	FAR PTR DOSGETINFOSEG
; Line 62
	mov	dx,WORD PTR [bp-10]	;GInfoSel
	sub	ax,ax
	mov	WORD PTR [bp-6],ax	;PGInfoSeg
	mov	WORD PTR [bp-4],dx
; Line 63
	mov	dx,WORD PTR [bp-16]	;LInfoSel
	mov	WORD PTR [bp-14],ax	;PLInfoSeg
	mov	WORD PTR [bp-12],dx
; Line 64
	les	bx,DWORD PTR [bp-14]	;PLInfoSeg
	mov	ax,WORD PTR es:[bx+8]
	mov	WORD PTR [bp-8],ax	;Session
; Line 65
	cmp	ax,16
	ja	$EX3768
	mov	si,ax
	shl	si,1
	mov	WORD PTR WROTETOSESS[si],1
; Line 69
	mov	WORD PTR [bp-2],2000	;Length
; Line 70
	imul	bx,ax,2000
	lea	ax,WORD PTR SESSIONS[bx]
	mov	dx,SEG SESSIONS
	push	dx
	push	ax
	lea	ax,WORD PTR [bp-2]	;Length
	push	ss
	push	ax
	push	0
	push	0
	push	0
	call	FAR PTR VIOREADCHARSTR
; Line 71
	lea	ax,WORD PTR XCURSOR[si]
	mov	dx,SEG XCURSOR
	push	dx
	push	ax
	lea	ax,WORD PTR YCURSOR[si]
	mov	dx,SEG YCURSOR
	push	dx
	push	ax
	push	0
	call	FAR PTR VIOGETCURPOS
; Line 74
	mov	ax,WORD PTR AVIOHMQ
	or	ax,WORD PTR AVIOHMQ+2
	je	$EX3768
; Line 75
	push	WORD PTR AVIOHMQ+2
	push	WORD PTR AVIOHMQ
	push	4097
	push	0
	push	WORD PTR [bp-8]	;Session
	push	0
	push	0
	call	FAR PTR WINPOSTQUEUEMSG
; Line 76
; Line 77
$EX3768:
	pop	ds
	ASSUME DS: DGROUP
	pop	si
	leave	
	ret	

_ReadScreen	ENDP
GVIOSPY_TEXT	ENDS
END
