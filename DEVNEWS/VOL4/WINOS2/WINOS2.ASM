;	Static Name Aliases
;
	TITLE   winos2.c
	.286p
	.287
INCLUDELIB	LLIBCE
WINOS2_TEXT	SEGMENT  WORD PUBLIC 'CODE'
WINOS2_TEXT	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
$$SYMBOLS	SEGMENT  BYTE PUBLIC 'DEBSYM'
$$SYMBOLS	ENDS
$$TYPES	SEGMENT  BYTE PUBLIC 'DEBTYP'
$$TYPES	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME DS: DGROUP, SS: DGROUP
PUBLIC  _VDD_NAME
PUBLIC  _OS2VERSION
EXTRN	__acrtused:ABS
EXTRN	__aFchkstk:FAR
EXTRN	_OS2TEST:FAR
EXTRN	_printf:FAR
EXTRN	_GETVDDAPIENTRY:FAR
COMM FAR	_VDDPROCADR:	BYTE:	 4
_DATA      SEGMENT
$SG101	DB	'VDD.SYS',  00H
$SG109	DB	'OS/2  Version is %d',  00H
$SG111	DB	'Not running on OS/2',  00H
_VDD_NAME	DD	DGROUP:$SG101
_OS2VERSION	DW	00H
_DATA      ENDS
WINOS2_TEXT      SEGMENT
	ASSUME	CS: WINOS2_TEXT
; Line 1
; Line 10
	PUBLIC	_main
_main	PROC FAR
	xor	ax,ax
	call	FAR PTR __aFchkstk
; Line 12
	call	FAR PTR _OS2TEST
	mov	WORD PTR _OS2VERSION,ax
; Line 13
	or	ax,ax
	je	$I107
; Line 14
	push	ax
	push	ds
	push	OFFSET DGROUP:$SG109
	call	FAR PTR _printf
	add	sp,6
; Line 15
	jmp	SHORT $I110
$I107:
; Line 16
	push	ds
	push	OFFSET DGROUP:$SG111
	call	FAR PTR _printf
	add	sp,4
$I110:
; Line 17
	call	FAR PTR _GETVDDAPIENTRY
; Line 19
	ret	

_main	ENDP
WINOS2_TEXT	ENDS
END
