;	Static Name Aliases
;
	TITLE   whatis.c
	.8087
WHATIS_TEXT	SEGMENT  WORD PUBLIC 'CODE'
WHATIS_TEXT	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME  CS: WHATIS_TEXT, DS: DGROUP, SS: DGROUP
PUBLIC  HEAPNUMBER
PUBLIC  LASTWINDOW
EXTRN	__acrtused:ABS
EXTRN	_atoi:FAR
EXTRN	WINQUERYSYSTEMATOMTABLE:FAR
EXTRN	WINENDPAINT:FAR
EXTRN	WINQUERYWINDOWTEXT:FAR
EXTRN	WINSETWINDOWTEXT:FAR
EXTRN	WINSETHOOK:FAR
EXTRN	WINQUERYFOCUS:FAR
EXTRN	GPICHARSTRINGAT:FAR
EXTRN	WININVALIDATERECT:FAR
EXTRN	WINADDATOM:FAR
EXTRN	WINWINDOWFROMID:FAR
EXTRN	WINBEGINPAINT:FAR
EXTRN	WINRELEASEHOOK:FAR
EXTRN	WININVALIDATEREGION:FAR
EXTRN	WINCREATESTDWINDOW:FAR
EXTRN	_strcmp:FAR
EXTRN	WINSENDMSG:FAR
EXTRN	WINWINDOWFROMPOINT:FAR
EXTRN	WINDEFDLGPROC:FAR
EXTRN	WINQUERYWINDOW:FAR
EXTRN	_strcpy:FAR
EXTRN	WINCREATEMSGQUEUE:FAR
EXTRN	WINQUERYCLASSNAME:FAR
EXTRN	WINDESTROYMSGQUEUE:FAR
EXTRN	_strlen:FAR
EXTRN	DOSBEEP:FAR
EXTRN	WINQUERYQUEUEINFO:FAR
EXTRN	WINQUERYACTIVEWINDOW:FAR
EXTRN	WINFILLRECT:FAR
EXTRN	GPISETBACKCOLOR:FAR
EXTRN	WINQUERYWINDOWDC:FAR
EXTRN	WINGETMSG:FAR
EXTRN	GPISETCOLOR:FAR
EXTRN	WININITIALIZE:FAR
EXTRN	WINQUERYWINDOWPROCESS:FAR
EXTRN	WINTERMINATE:FAR
EXTRN	WINSENDDLGITEMMSG:FAR
EXTRN	GPISETBACKMIX:FAR
EXTRN	WINQUERYPOINTERPOS:FAR
EXTRN	INPUTHOOKPROC:FAR
EXTRN	WINDISPATCHMSG:FAR
EXTRN	WINQUERYWINDOWUSHORT:FAR
EXTRN	SENDMSGHOOKPROC:FAR
EXTRN	WINREGISTERCLASS:FAR
EXTRN	WINPOSTMSG:FAR
EXTRN	WINDLGBOX:FAR
EXTRN	WINDEFWINDOWPROC:FAR
EXTRN	WINQUERYWINDOWULONG:FAR
EXTRN	WINDESTROYWINDOW:FAR
EXTRN	WINDISMISSDLG:FAR
EXTRN	WINREGISTERUSERMSG:FAR
EXTRN	_sprintf:FAR
EXTRN	WINQUERYWINDOWPTR:FAR
EXTRN	WINSETWINDOWPOS:FAR
EXTRN	DOSGETMODHANDLE:FAR
EXTRN	MAXAVAIL:WORD
EXTRN	SEGSIZE:DWORD
EXTRN	USERPTR:DWORD
EXTRN	XCUR:WORD
EXTRN	YCUR:WORD
EXTRN	QUERYTEXT:WORD
EXTRN	HANDLE0:DWORD
EXTRN	HANDLE1:DWORD
EXTRN	HANDLE2:DWORD
EXTRN	HANDLE3:DWORD
EXTRN	JOURNAL1:DWORD
EXTRN	SUCCESS1:WORD
EXTRN	SUCCESS2:WORD
EXTRN	ERR1:DWORD
EXTRN	ERR2:DWORD
EXTRN	ERR3:DWORD
_BSS      SEGMENT
COMM NEAR	HEAPSUM:	 4:	 256
COMM NEAR	HDLL:	BYTE:	 2
COMM NEAR	HWNDFRAME:	BYTE:	 4
COMM NEAR	SPYWINDOW:	BYTE:	 4
COMM NEAR	HMQ:	BYTE:	 4
COMM NEAR	TESTPID:	BYTE:	 2
COMM NEAR	HAB:	BYTE:	 4
COMM NEAR	HWNDCLIENT:	BYTE:	 4
COMM NEAR	LASTCREATED:	BYTE:	 4
COMM NEAR	CLASSNAME:	 1:	 80
COMM NEAR	MENUCLASS:	 1:	 10
COMM NEAR	WINDOW:	BYTE:	 4
_BSS      ENDS
EXTRN	CREATESTRUCT:BYTE
_BSS      SEGMENT
COMM NEAR	THEBUFFER:	 1:	 256
_BSS      ENDS
EXTRN	SHAREDTEXT:BYTE
EXTRN	SHAREDTEXT1:BYTE
EXTRN	AVIOTEXT:BYTE
EXTRN	DIRECTORY:BYTE
EXTRN	TARGETWINDOW:DWORD
_DATA      SEGMENT
$SG3815	DB	'WITT_QUERY_TEXT',  00H
$SG3817	DB	'MyWindow',  00H
$SG3818	DB	'#%d',  00H
$SG3819	DB	00H
$SG3820	DB	'MyWindow',  00H
$SG3821	DB	'WHATISD',  00H
	ORG	$+7
$SG3891	DB	'Jrn %p Succ1 %d Succ2 %d Err1 %p Err2 %p Err3 %p',  00H
$SG3892	DB	'Post result %d WindowProc %p,GreGetHandle 0=%p, 1=%p, 2=%p, '
	DB	'3=%p',  00H
$SG3893	DB	'Testpid %d Spy Text is >%s< ',  00H
$SG3894	DB	'Pointed Queue handle is: %p ,Queue size %d,Dir : %s',  00H
$SG3895	DB	'Window Pointed handle is: %p ,Parent %p,Owner %p',  00H
$SG3896	DB	'Styl %4.4lX,Pid %d,DC=%p,Heap Av:%d,Sel:%4.4X,Siz:%p',  00H
$SG3897	DB	'Handle of Active:%p , of Focus:%p        ',  00H
$SG3903	DB	' ',  00H
$SG3904	DB	'Window Class is %s : standard FRAME %-80s',  00H
$SG3906	DB	' ',  00H
$SG3907	DB	'Window Class is %s : standard TITLEBAR %-80s',  00H
$SG3909	DB	' ',  00H
$SG3910	DB	'Window Class is %s : standard BUTTON %-80s',  00H
$SG3912	DB	' ',  00H
$SG3913	DB	'Window Class is %s : standard ENTRYFIELD %-80s',  00H
$SG3915	DB	' ',  00H
$SG3916	DB	'Window Class is %s : standard COMBOBOX  %-80s',  00H
$SG3918	DB	' ',  00H
$SG3919	DB	'Window Class is %s : standard LISTBOX  Count %d;Visible Item'
	DB	's First %d;Last %d %-80s',  00H
$SG3921	DB	' ',  00H
$SG3922	DB	'Window Class is %s : standard MENU  %-80s',  00H
$SG3924	DB	' ',  00H
$SG3925	DB	'Window Class is %s : standard MLE  %-80s',  00H
$SG3927	DB	' ',  00H
$SG3928	DB	'Window Class is %s : standard SCROLLBAR; range:%d,%d ; pos:%'
	DB	'd  %-80s',  00H
$SG3930	DB	' ',  00H
$SG3931	DB	'Window Class is %s : standard STATIC  %-80s',  00H
$SG3933	DB	' ',  00H
$SG3934	DB	'Window Class is %s  %-80s',  00H
$SG3936	DB	' Is a true Frame !!!!!!!!! ',  00H
$SG3942	DB	' ',  00H
$SG3943	DB	'Window id is TITLEBAR %-80s',  00H
$SG3945	DB	' ',  00H
$SG3946	DB	'Window id is FRAME MENU%-80s',  00H
$SG3948	DB	' ',  00H
$SG3949	DB	'Window id is FRAME SYSMENU%-80s',  00H
$SG3951	DB	' ',  00H
$SG3952	DB	'Window id is FRAME MINMAX%-80s',  00H
$SG3954	DB	' ',  00H
$SG3955	DB	'Window id is FRAME CLIENT%-80s',  00H
$SG3957	DB	' ',  00H
$SG3958	DB	'Window id is FRAME HORIZONTAL SCROLL BAR%-80s',  00H
$SG3960	DB	' ',  00H
$SG3961	DB	'Window id is FRAME VERTICAL SCROLL BAR%-80s',  00H
$SG3963	DB	' ',  00H
$SG3964	DB	'Window id is %d %4.4X %-80s',  00H
$SG3969	DB	'Menu Item is MINIMIZE',  00H
$SG3972	DB	'Menu Item is RESTORE',  00H
$SG3975	DB	'Menu Item is MAXIMIZE',  00H
$SG3977	DB	' ',  00H
$SG3978	DB	'Menu Item %d is %s %-80s',  00H
$SG3980	DB	' ',  00H
$SG3981	DB	'No Menu item selected %-80s',  00H
$SG3983	DB	' ',  00H
$SG3984	DB	'Window Text is "%s" %-80s',  00H
$SG3986	DB	'OS/2 Window',  00H
$SG3987	DB	'Salut',  00H
$SG3988	DB	' ',  00H
$SG3989	DB	'Avio: Cursor x=%d y=%d, First line  =%s %-80s',  00H
$SG3990	DB	'Saved Focus Handle :%p , Last Created Window :%p    ',  00H
$SG4008	DB	'Pid:%d  Heap:%d',  00H
	ORG	$-1495
LASTWINDOW	DD	00H
HEAPNUMBER	DW	00H
_DATA      ENDS
CONST      SEGMENT
$T20000	DW SEG HAB 
$T20001	DW SEG HMQ 
$T20002	DW SEG QUERYTEXT 
$T20003	DW SEG HWNDFRAME 
$T20004	DW SEG HWNDCLIENT 
$T20005	DW SEG TARGETWINDOW 
$T20006	DW SEG HDLL 
CONST      ENDS
WHATIS_TEXT      SEGMENT
	ASSUME	CS: WHATIS_TEXT
; Line 52
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
; Line 53
; Line 54
; Line 55
; Line 56
; Line 111
	PUBLIC	_main
_main	PROC FAR
	push	bp
	mov	bp,sp
	sub	sp,30
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
	ASSUME DS: DGROUP
;	qmsg = -30
;	flCreate = -4
; Line 114
	sub	ax,ax
	push	ax
	call	FAR PTR WININITIALIZE
	mov	es,$T20000
	mov	WORD PTR es:HAB,ax
	mov	WORD PTR es:HAB+2,dx
; Line 115
	push	dx
	push	ax
	sub	ax,ax
	push	ax
	call	FAR PTR WINCREATEMSGQUEUE
	mov	es,$T20001
	mov	WORD PTR es:HMQ,ax
	mov	WORD PTR es:HMQ+2,dx
; Line 118
	call	FAR PTR WINQUERYSYSTEMATOMTABLE
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3815
	push	ds
	push	ax
	call	FAR PTR WINADDATOM
	mov	es,$T20002
	mov	es:QUERYTEXT,ax
; Line 119
	cmp	ax,-16384
	jae	$I3816
; Line 120
	mov	ax,1400
	push	ax
	mov	ax,20
	push	ax
	call	FAR PTR DOSBEEP
; Line 121
	mov	ax,1200
	push	ax
	mov	ax,20
	push	ax
	call	FAR PTR DOSBEEP
; Line 122
	mov	ax,1000
	push	ax
	mov	ax,20
	push	ax
	call	FAR PTR DOSBEEP
; Line 123
	mov	ax,1200
	push	ax
	mov	ax,20
	push	ax
	call	FAR PTR DOSBEEP
; Line 124
	mov	ax,1400
	push	ax
	mov	ax,20
	push	ax
	call	FAR PTR DOSBEEP
; Line 125
; Line 126
$I3816:
; Line 129
	mov	es,$T20000
	push	WORD PTR es:HAB+2
	push	WORD PTR es:HAB
	mov	es,$T20002
	push	WORD PTR es:QUERYTEXT
	mov	ax,25
	push	ax
	mov	ax,1
	push	ax
	mov	ax,25
	push	ax
	mov	ax,1
	push	ax
	mov	ax,25
	push	ax
	call	FAR PTR WINREGISTERUSERMSG
; Line 137
	mov	es,$T20000
	push	WORD PTR es:HAB+2
	push	WORD PTR es:HAB
	mov	ax,OFFSET DGROUP:$SG3817
	push	ds
	push	ax
	mov	ax,OFFSET MYWINDOWPROC
	mov	dx,SEG MYWINDOWPROC
	push	dx
	push	ax
	mov	ax,4
	cwd	
	push	dx
	push	ax
	sub	ax,ax
	push	ax
	call	FAR PTR WINREGISTERCLASS
; Line 138
	mov	ax,4
	mov	dx,-1
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3818
	push	ds
	push	ax
	mov	ax,OFFSET MENUCLASS
	mov	dx,SEG MENUCLASS
	push	dx
	push	ax
	call	FAR PTR _sprintf
	add	sp,12
; Line 140
	mov	WORD PTR [bp-4],-14273	;flCreate
	mov	WORD PTR [bp-2],8
; Line 153
	mov	ax,1
	cwd	
	push	dx
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	lea	ax,WORD PTR [bp-4]	;flCreate
	push	ss
	push	ax
	mov	ax,OFFSET DGROUP:$SG3820
	push	ds
	push	ax
	mov	ax,OFFSET DGROUP:$SG3819
	push	ds
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	mov	ax,256
	push	ax
	mov	ax,OFFSET HWNDCLIENT
	mov	dx,SEG HWNDCLIENT
	push	dx
	push	ax
	call	FAR PTR WINCREATESTDWINDOW
	mov	es,$T20003
	mov	WORD PTR es:HWNDFRAME,ax
	mov	WORD PTR es:HWNDFRAME+2,dx
; Line 154
	mov	es,$T20004
	mov	ax,WORD PTR es:HWNDCLIENT
	mov	dx,WORD PTR es:HWNDCLIENT+2
	mov	es,$T20005
	mov	WORD PTR es:TARGETWINDOW,ax
	mov	WORD PTR es:TARGETWINDOW+2,dx
; Line 159
	mov	es,$T20003
	push	WORD PTR es:HWNDFRAME+2
	push	WORD PTR es:HWNDFRAME
	mov	ax,3
	cwd	
	push	dx
	push	ax
	mov	ax,50
	push	ax
	push	ax
	mov	ax,450
	push	ax
	mov	ax,250
	push	ax
	mov	ax,139
	push	ax
	call	FAR PTR WINSETWINDOWPOS
; Line 162
	mov	ax,OFFSET DGROUP:$SG3821
	push	ds
	push	ax
	mov	ax,OFFSET HDLL
	mov	dx,SEG HDLL
	push	dx
	push	ax
	call	FAR PTR DOSGETMODHANDLE
; Line 163
	mov	es,$T20000
	push	WORD PTR es:HAB+2
	push	WORD PTR es:HAB
	sub	ax,ax
	push	ax
	push	ax
	mov	ax,1
	push	ax
	mov	ax,OFFSET INPUTHOOKPROC
	mov	dx,SEG INPUTHOOKPROC
	push	dx
	push	ax
	mov	es,$T20006
	push	WORD PTR es:HDLL
	call	FAR PTR WINSETHOOK
; Line 164
	mov	es,$T20000
	push	WORD PTR es:HAB+2
	push	WORD PTR es:HAB
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	mov	ax,OFFSET SENDMSGHOOKPROC
	mov	dx,SEG SENDMSGHOOKPROC
	push	dx
	push	ax
	mov	es,$T20006
	push	WORD PTR es:HDLL
	call	FAR PTR WINSETHOOK
; Line 169
	jmp	SHORT $L20032
$FC3823:
; Line 170
	mov	es,$T20000
	push	WORD PTR es:HAB+2
	push	WORD PTR es:HAB
	lea	ax,WORD PTR [bp-30]	;qmsg
	push	ss
	push	ax
	call	FAR PTR WINDISPATCHMSG
$L20032:
	mov	es,$T20000
	push	WORD PTR es:HAB+2
	push	WORD PTR es:HAB
	lea	ax,WORD PTR [bp-30]	;qmsg
	push	ss
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	push	ax
	call	FAR PTR WINGETMSG
	or	ax,ax
	jne	$FC3823
; Line 171
	mov	es,$T20000
	push	WORD PTR es:HAB+2
	push	WORD PTR es:HAB
	sub	ax,ax
	push	ax
	push	ax
	mov	ax,1
	push	ax
	mov	ax,OFFSET INPUTHOOKPROC
	mov	dx,SEG INPUTHOOKPROC
	push	dx
	push	ax
	mov	es,$T20006
	push	WORD PTR es:HDLL
	call	FAR PTR WINRELEASEHOOK
; Line 172
	mov	es,$T20000
	push	WORD PTR es:HAB+2
	push	WORD PTR es:HAB
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	mov	ax,OFFSET SENDMSGHOOKPROC
	mov	dx,SEG SENDMSGHOOKPROC
	push	dx
	push	ax
	mov	es,$T20006
	push	WORD PTR es:HDLL
	call	FAR PTR WINRELEASEHOOK
; Line 173
	mov	es,$T20005
	sub	ax,ax
	mov	WORD PTR es:TARGETWINDOW+2,ax
	mov	WORD PTR es:TARGETWINDOW,ax
; Line 175
	mov	es,$T20003
	push	WORD PTR es:HWNDFRAME+2
	push	WORD PTR es:HWNDFRAME
	call	FAR PTR WINDESTROYWINDOW
; Line 176
	mov	es,$T20001
	push	WORD PTR es:HMQ+2
	push	WORD PTR es:HMQ
	call	FAR PTR WINDESTROYMSGQUEUE
; Line 177
	mov	es,$T20000
	push	WORD PTR es:HAB+2
	push	WORD PTR es:HAB
	call	FAR PTR WINTERMINATE
; Line 178
	pop	ds
	ASSUME DS: DGROUP
	mov	sp,bp
	pop	bp
	ret	
	nop	

_main	ENDP
; Line 183
WHATIS_TEXT      ENDS
CONST      SEGMENT
$T20007	DW SEG TESTPID 
$T20008	DW SEG HEAPSUM 
$T20009	DW SEG CREATESTRUCT 
$T20010	DW SEG LASTCREATED 
$T20011	DW SEG WINDOW 
$T20013	DW SEG ERR3 
$T20014	DW SEG ERR2 
$T20015	DW SEG ERR1 
$T20016	DW SEG SUCCESS2 
$T20017	DW SEG SUCCESS1 
$T20018	DW SEG JOURNAL1 
$T20019	DW SEG HANDLE3 
$T20020	DW SEG HANDLE2 
$T20021	DW SEG HANDLE1 
$T20022	DW SEG HANDLE0 
$T20023	DW SEG SEGSIZE 
$T20024	DW SEG USERPTR 
$T20025	DW SEG MAXAVAIL 
$T20028	DW SEG SHAREDTEXT 
$T20029	DW SEG YCUR 
$T20030	DW SEG XCUR 
CONST      ENDS
WHATIS_TEXT      SEGMENT
	ASSUME	CS: WHATIS_TEXT
	PUBLIC	MYWINDOWPROC
MYWINDOWPROC	PROC FAR
	push	bp
	mov	bp,sp
	sub	sp,514
	push	si
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
	ASSUME DS: DGROUP
;	ItemId = -498
;	Active = -496
;	Focus = -4
;	Parent = -78
;	Owner = -482
;	HitTest = -38
;	Style = -10
;	Pid = -508
;	Tid = -6
;	WindowDC = -512
;	wHmq = -506
;	Messages = -60
;	Mqinfo = -74
;	Range = -502
;	Pos = -12
;	Count = -62
;	ResRes = -26
;	Ptr = -58
;	Info = -492
;	hwnd = 16
;	msg = 14
;	mp1 = 10
;	mp2 = 6
;	command = -514
;	hps = -488
;	rc = -54
;	pt = -34
;	PointerPos = -24
;	WinId = -484
;	ClassId = -16
;	Length = -14
;	Buffer = -478
; Line 206
	mov	ax,WORD PTR [bp+14]	;msg
	cmp	ax,4097
	jne	$JCC752
	jmp	$SC3877
$JCC752:
	jbe	$JCC757
	jmp	$L20031
$JCC757:
	cmp	ax,32
	jne	$JCC765
	jmp	$SC3881
$JCC765:
	cmp	ax,35
	jne	$JCC773
	jmp	$SC3890
$JCC773:
	cmp	ax,41
	jne	$JCC781
	jmp	$SC3991
$JCC781:
	cmp	ax,79
	jne	$JCC789
	jmp	$SC3889
$JCC789:
	jmp	$SD3888
	nop	
$SC3863:
; Line 209
	mov	es,$T20007
	mov	ax,WORD PTR [bp+10]	;mp1
	mov	WORD PTR es:TESTPID,ax
; Line 210
$L20034:
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	sub	ax,ax
	push	ax
	push	ax
	mov	ax,1
	push	ax
	call	FAR PTR WININVALIDATERECT
; Line 211
$SB3860:
; Line 468
	sub	ax,ax
	jmp	$L20039
	nop	
$SC3864:
; Line 213
	mov	ax,1
	cwd	
	push	dx
	push	ax
	sub	ax,ax
	push	ax
	call	FAR PTR WINQUERYFOCUS
	mov	WORD PTR [bp-4],ax	;Focus
	mov	WORD PTR [bp-2],dx
; Line 214
	push	dx
	push	ax
	mov	es,$T20002
	push	WORD PTR es:QUERYTEXT
	jmp	$L20037
$SC3865:
; Line 217
	push	WORD PTR [bp+8]
	push	WORD PTR [bp+6]	;mp2
	lea	ax,WORD PTR [bp-508]	;Pid
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-6]	;Tid
	push	ss
	push	ax
	call	FAR PTR WINQUERYWINDOWPROCESS
; Line 218
	mov	WORD PTR [bp-498],0	;ItemId
	jmp	SHORT $F3866
$FC3867:
	inc	WORD PTR [bp-498]	;ItemId
$F3866:
	mov	ax,HEAPNUMBER
	cmp	WORD PTR [bp-498],ax	;ItemId
	jae	$FB3868
; Line 219
	mov	ax,WORD PTR [bp-508]	;Pid
	mov	bx,WORD PTR [bp-498]	;ItemId
	shl	bx,1
	shl	bx,1
	mov	es,$T20008
	cmp	WORD PTR es:HEAPSUM[bx],ax
	jne	$FC3867
$FB3868:
; Line 221
	mov	ax,HEAPNUMBER
	cmp	WORD PTR [bp-498],ax	;ItemId
	jne	$I3870
; Line 222
	inc	WORD PTR HEAPNUMBER
; Line 223
	mov	si,WORD PTR [bp-498]	;ItemId
	mov	cl,2
	shl	si,cl
	mov	ax,WORD PTR [bp-508]	;Pid
	mov	es,$T20008
	mov	WORD PTR es:HEAPSUM[si],ax
; Line 225
	cmp	WORD PTR [bp+10],0	;mp1
	je	$I3871
	mov	ax,WORD PTR [bp+10]	;mp1
	mov	WORD PTR es:HEAPSUM[si+2],ax
; Line 226
	jmp	$SB3860
	nop	
$I3871:
	mov	bx,WORD PTR [bp-498]	;ItemId
	shl	bx,1
	shl	bx,1
	mov	WORD PTR es:HEAPSUM[bx+2],512
; Line 227
	jmp	$SB3860
$I3870:
; Line 228
	cmp	WORD PTR [bp+10],0	;mp1
	je	$I3874
	mov	ax,WORD PTR [bp+10]	;mp1
	mov	bx,WORD PTR [bp-498]	;ItemId
	shl	bx,1
	shl	bx,1
	mov	es,$T20008
	add	WORD PTR es:HEAPSUM[bx+2],ax
; Line 229
	jmp	$SB3860
	nop	
$I3874:
	mov	bx,WORD PTR [bp-498]	;ItemId
	shl	bx,1
	shl	bx,1
	mov	es,$T20008
	add	BYTE PTR es:HEAPSUM[bx+3],2
; Line 230
; Line 231
	jmp	$SB3860
	nop	
$SC3877:
; Line 236
	mov	es,$T20009
	push	WORD PTR es:CREATESTRUCT+40
	push	WORD PTR es:CREATESTRUCT+38
	push	WORD PTR es:CREATESTRUCT+8
	call	FAR PTR WINWINDOWFROMID
	mov	es,$T20010
	mov	WORD PTR es:LASTCREATED,ax
	mov	WORD PTR es:LASTCREATED+2,dx
	jmp	$L20034
$SC3879:
; Line 244
	mov	ax,1
	cwd	
	push	dx
	push	ax
	lea	ax,WORD PTR [bp-24]	;PointerPos
	push	ss
	push	ax
	call	FAR PTR WINQUERYPOINTERPOS
; Line 245
	mov	ax,1
	cwd	
	push	dx
	push	ax
	lea	ax,WORD PTR [bp-24]	;PointerPos
	push	ss
	push	ax
	mov	ax,1
	push	ax
	sub	ax,ax
	push	ax
	call	FAR PTR WINWINDOWFROMPOINT
	mov	es,$T20011
	mov	WORD PTR es:WINDOW,ax
	mov	WORD PTR es:WINDOW+2,dx
; Line 246
	mov	ax,WORD PTR LASTWINDOW
	mov	dx,WORD PTR LASTWINDOW+2
	cmp	WORD PTR es:WINDOW,ax
	je	$JCC1160
	jmp	$L20034
$JCC1160:
	cmp	WORD PTR es:WINDOW+2,dx
	jne	$JCC1170
	jmp	$SB3860
$JCC1170:
	jmp	$L20034
$SC3881:
; Line 255
	mov	ax,WORD PTR [bp+10]	;mp1
	mov	WORD PTR [bp-514],ax	;command
; Line 256
	cmp	ax,261
	je	$SC3887
	cmp	ax,361
	je	$SC3886
$SD3888:
; Line 272
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	push	WORD PTR [bp+14]	;msg
	push	WORD PTR [bp+12]
	push	WORD PTR [bp+10]	;mp1
	push	WORD PTR [bp+8]
	push	WORD PTR [bp+6]	;mp2
	call	FAR PTR WINDEFWINDOWPROC
	jmp	$EX3830
$SC3886:
; Line 264
	mov	ax,1
	cwd	
	push	dx
	push	ax
	mov	es,$T20003
	push	WORD PTR es:HWNDFRAME+2
	push	WORD PTR es:HWNDFRAME
	mov	ax,OFFSET MYDLGPROC
	mov	dx,SEG MYDLGPROC
	push	dx
	push	ax
	sub	ax,ax
	push	ax
	mov	ax,257
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	call	FAR PTR WINDLGBOX
; Line 265
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	call	FAR PTR WININVALIDATEREGION
; Line 274
	jmp	$SB3860
	nop	
$SC3887:
; Line 269
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	mov	ax,41
$L20055:
	push	ax
$L20037:
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	push	ax
	call	FAR PTR WINPOSTMSG
; Line 270
	jmp	$SB3860
$SC3889:
; Line 280
	mov	ax,1
$L20039:
	sub	dx,dx
	jmp	$EX3830
$SC3890:
; Line 286
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	sub	ax,ax
	push	ax
	push	ax
	lea	ax,WORD PTR [bp-54]	;rc
	push	ss
	push	ax
	call	FAR PTR WINBEGINPAINT
	mov	WORD PTR [bp-488],ax	;hps
	mov	WORD PTR [bp-486],dx
; Line 287
	mov	WORD PTR [bp-50],30
	mov	WORD PTR [bp-48],0
; Line 288
	push	dx
	push	ax
	lea	ax,WORD PTR [bp-54]	;rc
	push	ss
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	call	FAR PTR WINFILLRECT
; Line 289
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	mov	ax,7
	cwd	
	push	dx
	push	ax
	call	FAR PTR GPISETCOLOR
; Line 290
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	sub	ax,ax
	push	ax
	push	ax
	call	FAR PTR GPISETBACKCOLOR
; Line 291
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	mov	ax,2
	cwd	
	push	dx
	push	ax
	call	FAR PTR GPISETBACKMIX
; Line 293
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	mov	ax,122
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	push	ax
	call	FAR PTR WINPOSTMSG
	mov	WORD PTR [bp-26],ax	;ResRes
; Line 294
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	mov	ax,122
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	push	ax
	call	FAR PTR WINPOSTMSG
	mov	WORD PTR [bp-26],ax	;ResRes
; Line 295
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	mov	ax,122
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	push	ax
	call	FAR PTR WINPOSTMSG
	mov	WORD PTR [bp-26],ax	;ResRes
; Line 296
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,-3
	push	ax
	call	FAR PTR WINQUERYWINDOWPTR
	mov	WORD PTR [bp-58],ax	;Ptr
	mov	WORD PTR [bp-56],dx
; Line 297
	mov	WORD PTR [bp-34],10	;pt
	mov	WORD PTR [bp-32],0
	mov	WORD PTR [bp-30],230
	mov	WORD PTR [bp-28],0
; Line 304
	mov	es,$T20013
	push	WORD PTR es:ERR3+2
	push	WORD PTR es:ERR3
	mov	es,$T20014
	push	WORD PTR es:ERR2+2
	push	WORD PTR es:ERR2
	mov	es,$T20015
	push	WORD PTR es:ERR1+2
	push	WORD PTR es:ERR1
	mov	es,$T20016
	push	WORD PTR es:SUCCESS2
	mov	es,$T20017
	push	WORD PTR es:SUCCESS1
	mov	es,$T20018
	push	WORD PTR es:JOURNAL1+2
	push	WORD PTR es:JOURNAL1
	mov	ax,OFFSET DGROUP:$SG3891
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,28
; Line 305
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	lea	ax,WORD PTR [bp-34]	;pt
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	sub	cx,cx
	push	cx
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR GPICHARSTRINGAT
; Line 306
	mov	es,$T20019
	push	WORD PTR es:HANDLE3+2
	push	WORD PTR es:HANDLE3
	mov	es,$T20020
	push	WORD PTR es:HANDLE2+2
	push	WORD PTR es:HANDLE2
	mov	es,$T20021
	push	WORD PTR es:HANDLE1+2
	push	WORD PTR es:HANDLE1
	mov	es,$T20022
	push	WORD PTR es:HANDLE0+2
	push	WORD PTR es:HANDLE0
	push	WORD PTR [bp-56]
	push	WORD PTR [bp-58]	;Ptr
	push	WORD PTR [bp-26]	;ResRes
	mov	ax,OFFSET DGROUP:$SG3892
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,30
; Line 307
	mov	WORD PTR [bp-34],10	;pt
	mov	WORD PTR [bp-32],0
	mov	WORD PTR [bp-30],210
	mov	WORD PTR [bp-28],0
; Line 308
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	lea	ax,WORD PTR [bp-34]	;pt
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	sub	cx,cx
	push	cx
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR GPICHARSTRINGAT
; Line 309
	mov	ax,OFFSET SHAREDTEXT1
	mov	dx,SEG SHAREDTEXT1
	push	dx
	push	ax
	mov	es,$T20007
	push	WORD PTR es:TESTPID
	mov	ax,OFFSET DGROUP:$SG3893
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,14
; Line 310
	mov	WORD PTR [bp-34],10	;pt
	mov	WORD PTR [bp-32],0
	mov	WORD PTR [bp-30],190
	mov	WORD PTR [bp-28],0
; Line 311
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	lea	ax,WORD PTR [bp-34]	;pt
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	sub	cx,cx
	push	cx
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR GPICHARSTRINGAT
; Line 312
	mov	WORD PTR [bp-34],10	;pt
	mov	WORD PTR [bp-32],0
	mov	WORD PTR [bp-30],170
	mov	WORD PTR [bp-28],0
; Line 313
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,-4
	push	ax
	call	FAR PTR WINQUERYWINDOWULONG
	mov	WORD PTR [bp-506],ax	;wHmq
	mov	WORD PTR [bp-504],dx
; Line 314
	push	dx
	push	ax
	lea	ax,WORD PTR [bp-74]	;Mqinfo
	push	ss
	push	ax
	mov	ax,12
	push	ax
	call	FAR PTR WINQUERYQUEUEINFO
; Line 315
	mov	ax,OFFSET DIRECTORY
	mov	dx,SEG DIRECTORY
	push	dx
	push	ax
	push	WORD PTR [bp-68]
	push	WORD PTR [bp-504]
	push	WORD PTR [bp-506]	;wHmq
	mov	ax,OFFSET DGROUP:$SG3894
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,18
; Line 316
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	lea	ax,WORD PTR [bp-34]	;pt
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	sub	cx,cx
	push	cx
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR GPICHARSTRINGAT
; Line 317
	mov	WORD PTR [bp-34],10	;pt
	mov	WORD PTR [bp-32],0
	mov	WORD PTR [bp-30],150
	mov	WORD PTR [bp-28],0
; Line 319
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	call	FAR PTR WINQUERYWINDOWDC
	mov	WORD PTR [bp-512],ax	;WindowDC
	mov	WORD PTR [bp-510],dx
; Line 320
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,5
	push	ax
	sub	ax,ax
	push	ax
	call	FAR PTR WINQUERYWINDOW
	mov	WORD PTR [bp-78],ax	;Parent
	mov	WORD PTR [bp-76],dx
; Line 321
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,4
	push	ax
	sub	ax,ax
	push	ax
	call	FAR PTR WINQUERYWINDOW
	mov	WORD PTR [bp-482],ax	;Owner
	mov	WORD PTR [bp-480],dx
; Line 322
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	lea	ax,WORD PTR [bp-508]	;Pid
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-6]	;Tid
	push	ss
	push	ax
	call	FAR PTR WINQUERYWINDOWPROCESS
; Line 323
	push	WORD PTR [bp-480]
	push	WORD PTR [bp-482]	;Owner
	push	WORD PTR [bp-76]
	push	WORD PTR [bp-78]	;Parent
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,OFFSET DGROUP:$SG3895
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,20
; Line 324
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	lea	ax,WORD PTR [bp-34]	;pt
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	sub	cx,cx
	push	cx
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR GPICHARSTRINGAT
; Line 325
	mov	WORD PTR [bp-34],10	;pt
	mov	WORD PTR [bp-32],0
	mov	WORD PTR [bp-30],130
	mov	WORD PTR [bp-28],0
; Line 326
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,-2
	push	ax
	call	FAR PTR WINQUERYWINDOWULONG
	mov	WORD PTR [bp-10],ax	;Style
	mov	WORD PTR [bp-8],dx
; Line 328
	mov	es,$T20023
	push	WORD PTR es:SEGSIZE+2
	push	WORD PTR es:SEGSIZE
	mov	es,$T20024
	push	WORD PTR es:USERPTR+2
	mov	es,$T20025
	push	WORD PTR es:MAXAVAIL
	push	WORD PTR [bp-510]
	push	WORD PTR [bp-512]	;WindowDC
	push	WORD PTR [bp-508]	;Pid
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3896
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,26
; Line 329
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	lea	ax,WORD PTR [bp-34]	;pt
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	sub	cx,cx
	push	cx
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR GPICHARSTRINGAT
; Line 330
	mov	ax,1
	cwd	
	push	dx
	push	ax
	sub	ax,ax
	push	ax
	call	FAR PTR WINQUERYFOCUS
	mov	WORD PTR [bp-4],ax	;Focus
	mov	WORD PTR [bp-2],dx
; Line 331
	mov	ax,1
	cwd	
	push	dx
	push	ax
	sub	ax,ax
	push	ax
	call	FAR PTR WINQUERYACTIVEWINDOW
	mov	WORD PTR [bp-496],ax	;Active
	mov	WORD PTR [bp-494],dx
; Line 332
	mov	WORD PTR [bp-34],10	;pt
	mov	WORD PTR [bp-32],0
	mov	WORD PTR [bp-30],110
	mov	WORD PTR [bp-28],0
; Line 333
	push	WORD PTR [bp-2]
	push	WORD PTR [bp-4]	;Focus
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3897
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,16
; Line 334
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	lea	ax,WORD PTR [bp-34]	;pt
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	sub	cx,cx
	push	cx
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR GPICHARSTRINGAT
; Line 335
	mov	WORD PTR [bp-34],10	;pt
	mov	WORD PTR [bp-32],0
	mov	WORD PTR [bp-30],90
	mov	WORD PTR [bp-28],0
; Line 336
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,80
	push	ax
	mov	ax,OFFSET CLASSNAME
	mov	dx,SEG CLASSNAME
	push	dx
	push	ax
	call	FAR PTR WINQUERYCLASSNAME
; Line 337
	mov	ax,OFFSET CLASSNAME+1
	mov	dx,SEG CLASSNAME
	push	dx
	push	ax
	call	FAR PTR _atoi
	add	sp,4
	mov	WORD PTR [bp-16],ax	;ClassId
; Line 338
	sub	ax,1
	cmp	ax,9
	jbe	$JCC2671
	jmp	$SD3932
$JCC2671:
	add	ax,ax
	xchg	ax,bx
	jmp	WORD PTR cs:$L20026[bx]
$SC3902:
; Line 340
	mov	ax,OFFSET DGROUP:$SG3903
	push	ds
	push	ax
	mov	ax,OFFSET CLASSNAME
	mov	dx,SEG CLASSNAME
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3904
$L20040:
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,16
; Line 341
	jmp	$SB3899
	nop	
$SC3905:
; Line 343
	mov	ax,OFFSET DGROUP:$SG3906
	push	ds
	push	ax
	mov	ax,OFFSET CLASSNAME
	mov	dx,SEG CLASSNAME
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3907
	jmp	SHORT $L20040
$SC3908:
; Line 346
	mov	ax,OFFSET DGROUP:$SG3909
	push	ds
	push	ax
	mov	ax,OFFSET CLASSNAME
	mov	dx,SEG CLASSNAME
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3910
	jmp	SHORT $L20040
$SC3911:
; Line 349
	mov	ax,OFFSET DGROUP:$SG3912
	push	ds
	push	ax
	mov	ax,OFFSET CLASSNAME
	mov	dx,SEG CLASSNAME
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3913
	jmp	SHORT $L20040
$SC3914:
; Line 352
	mov	ax,OFFSET DGROUP:$SG3915
	push	ds
	push	ax
	mov	ax,OFFSET CLASSNAME
	mov	dx,SEG CLASSNAME
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3916
	jmp	SHORT $L20040
$SC3917:
; Line 355
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,-16383
	push	ax
	call	FAR PTR WINWINDOWFROMID
	push	dx
	push	ax
	mov	ax,419
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	push	ax
	call	FAR PTR WINSENDMSG
	mov	WORD PTR [bp-502],ax	;Range
	mov	WORD PTR [bp-500],dx
; Line 356
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,-16383
	push	ax
	call	FAR PTR WINWINDOWFROMID
	push	dx
	push	ax
	mov	ax,418
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	push	ax
	call	FAR PTR WINSENDMSG
	mov	WORD PTR [bp-12],ax	;Pos
; Line 357
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,352
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	push	ax
	call	FAR PTR WINSENDMSG
	mov	WORD PTR [bp-62],ax	;Count
; Line 360
	mov	ax,OFFSET DGROUP:$SG3918
	push	ds
	push	ax
	mov	ax,WORD PTR [bp-12]	;Pos
	sub	ax,WORD PTR [bp-500]
	add	ax,WORD PTR [bp-502]	;Range
	add	ax,WORD PTR [bp-62]	;Count
	dec	ax
	push	ax
	push	WORD PTR [bp-12]	;Pos
	push	WORD PTR [bp-62]	;Count
	mov	ax,OFFSET CLASSNAME
	mov	dx,SEG CLASSNAME
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3919
$L20048:
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,22
; Line 361
	jmp	$SB3899
$SC3920:
; Line 363
	mov	ax,OFFSET DGROUP:$SG3921
	push	ds
	push	ax
	mov	ax,OFFSET CLASSNAME
	mov	dx,SEG CLASSNAME
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3922
	jmp	$L20040
	nop	
$SC3923:
; Line 366
	mov	ax,OFFSET DGROUP:$SG3924
	push	ds
	push	ax
	mov	ax,OFFSET CLASSNAME
	mov	dx,SEG CLASSNAME
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3925
	jmp	$L20040
	nop	
$SC3926:
; Line 369
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,419
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	push	ax
	call	FAR PTR WINSENDMSG
	mov	WORD PTR [bp-502],ax	;Range
	mov	WORD PTR [bp-500],dx
; Line 370
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,418
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	push	ax
	call	FAR PTR WINSENDMSG
	mov	WORD PTR [bp-12],ax	;Pos
; Line 373
	mov	ax,OFFSET DGROUP:$SG3927
	push	ds
	push	ax
	push	WORD PTR [bp-12]	;Pos
	push	WORD PTR [bp-500]
	push	WORD PTR [bp-502]	;Range
	mov	ax,OFFSET CLASSNAME
	mov	dx,SEG CLASSNAME
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3928
	jmp	$L20048
	nop	
$SC3929:
; Line 376
	mov	ax,OFFSET DGROUP:$SG3930
	push	ds
	push	ax
	mov	ax,OFFSET CLASSNAME
	mov	dx,SEG CLASSNAME
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3931
	jmp	$L20040
	nop	
$SD3932:
; Line 379
	mov	ax,OFFSET DGROUP:$SG3933
	push	ds
	push	ax
	mov	ax,OFFSET CLASSNAME
	mov	dx,SEG CLASSNAME
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3934
	jmp	$L20040
	nop	
$L20026:
		DW	$SC3902
		DW	$SC3914
		DW	$SC3908
		DW	$SC3920
		DW	$SC3929
		DW	$SC3911
		DW	$SC3917
		DW	$SC3926
		DW	$SC3905
		DW	$SC3923
$SB3899:
; Line 381
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,80
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	ax
	push	ax
	call	FAR PTR WINSENDMSG
	mov	WORD PTR [bp-492],ax	;Info
	mov	WORD PTR [bp-490],dx
; Line 382
	test	BYTE PTR [bp-492],1	;Info
	je	$I3935
; Line 383
	mov	ax,OFFSET DGROUP:$SG3936
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _strcpy
	add	sp,8
; Line 384
; Line 385
$I3935:
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	lea	ax,WORD PTR [bp-34]	;pt
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	sub	cx,cx
	push	cx
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR GPICHARSTRINGAT
; Line 386
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,-1
	push	ax
	call	FAR PTR WINQUERYWINDOWUSHORT
	mov	WORD PTR [bp-484],ax	;WinId
; Line 387
	sub	ax,-32766
	cmp	ax,6
	ja	$SD3962
	add	ax,ax
	xchg	ax,bx
	jmp	WORD PTR cs:$L20027[bx]
$SC3941:
; Line 389
	mov	ax,OFFSET DGROUP:$SG3942
	push	ds
	push	ax
	mov	ax,OFFSET DGROUP:$SG3943
$L20049:
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,12
; Line 390
	jmp	SHORT $SB3938
$SC3944:
; Line 392
	mov	ax,OFFSET DGROUP:$SG3945
	push	ds
	push	ax
	mov	ax,OFFSET DGROUP:$SG3946
	jmp	SHORT $L20049
$SC3947:
; Line 395
	mov	ax,OFFSET DGROUP:$SG3948
	push	ds
	push	ax
	mov	ax,OFFSET DGROUP:$SG3949
	jmp	SHORT $L20049
$SC3950:
; Line 398
	mov	ax,OFFSET DGROUP:$SG3951
	push	ds
	push	ax
	mov	ax,OFFSET DGROUP:$SG3952
	jmp	SHORT $L20049
$SC3953:
; Line 401
	mov	ax,OFFSET DGROUP:$SG3954
	push	ds
	push	ax
	mov	ax,OFFSET DGROUP:$SG3955
	jmp	SHORT $L20049
$SC3956:
; Line 404
	mov	ax,OFFSET DGROUP:$SG3957
	push	ds
	push	ax
	mov	ax,OFFSET DGROUP:$SG3958
	jmp	SHORT $L20049
$SC3959:
; Line 407
	mov	ax,OFFSET DGROUP:$SG3960
	push	ds
	push	ax
	mov	ax,OFFSET DGROUP:$SG3961
	jmp	SHORT $L20049
$SD3962:
; Line 410
	mov	ax,OFFSET DGROUP:$SG3963
	push	ds
	push	ax
	push	WORD PTR [bp-484]	;WinId
	push	WORD PTR [bp-484]	;WinId
	mov	ax,OFFSET DGROUP:$SG3964
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,16
; Line 411
	jmp	SHORT $SB3938
$L20027:
		DW	$SC3947
		DW	$SC3941
		DW	$SC3950
		DW	$SC3944
		DW	$SC3959
		DW	$SC3956
		DW	$SC3953
$SB3938:
; Line 412
	mov	WORD PTR [bp-34],10	;pt
	mov	WORD PTR [bp-32],0
	mov	WORD PTR [bp-30],70
	mov	WORD PTR [bp-28],0
; Line 413
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	lea	ax,WORD PTR [bp-34]	;pt
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	sub	cx,cx
	push	cx
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR GPICHARSTRINGAT
; Line 414
	cmp	WORD PTR [bp-16],4	;ClassId
	je	$JCC3520
	jmp	$I3965
$JCC3520:
; Line 417
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,394
	push	ax
	mov	ax,1
	cwd	
	push	dx
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	call	FAR PTR WINSENDMSG
	mov	WORD PTR [bp-498],ax	;ItemId
; Line 418
	cmp	ax,-1
	jne	$JCC3565
	jmp	$I3966
$JCC3565:
; Line 419
	cmp	WORD PTR [bp-484],-32764	;WinId
	jne	$I3967
; Line 420
	cmp	ax,-32766
	jne	$I3968
	mov	ax,OFFSET DGROUP:$SG3969
	jmp	SHORT $L20057
$I3968:
; Line 421
	cmp	WORD PTR [bp-498],-32760	;ItemId
	jne	$I3971
	mov	ax,OFFSET DGROUP:$SG3972
$L20057:
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,8
; Line 422
	jmp	$I3985
$I3971:
	cmp	WORD PTR [bp-498],-32765	;ItemId
	je	$JCC3624
	jmp	$I3985
$JCC3624:
	mov	ax,OFFSET DGROUP:$SG3975
	jmp	SHORT $L20057
$I3967:
; Line 423
; Line 427
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,395
	push	ax
	mov	ax,WORD PTR [bp-498]	;ItemId
	sub	dx,dx
	or	dl,79
	push	dx
	push	ax
	mov	ax,OFFSET SHAREDTEXT
	mov	dx,SEG SHAREDTEXT
	push	dx
	push	ax
	call	FAR PTR WINSENDMSG
	mov	WORD PTR [bp-14],ax	;Length
; Line 428
	mov	bx,ax
	mov	es,$T20028
	mov	BYTE PTR es:SHAREDTEXT[bx],0
; Line 429
	mov	ax,OFFSET DGROUP:$SG3977
	push	ds
	push	ax
	mov	ax,OFFSET SHAREDTEXT
	mov	dx,SEG SHAREDTEXT
	push	dx
	push	ax
	push	WORD PTR [bp-498]	;ItemId
	mov	ax,OFFSET DGROUP:$SG3978
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,18
; Line 430
; Line 431
	jmp	$I3985
$I3966:
; Line 432
	mov	ax,OFFSET DGROUP:$SG3980
	push	ds
	push	ax
	mov	ax,OFFSET DGROUP:$SG3981
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,12
; Line 433
; Line 434
	jmp	$I3985
	nop	
$I3965:
; Line 435
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,79
	push	ax
	mov	ax,OFFSET SHAREDTEXT
	mov	dx,SEG SHAREDTEXT
	push	dx
	push	ax
	call	FAR PTR WINQUERYWINDOWTEXT
; Line 436
	mov	ax,OFFSET DGROUP:$SG3983
	push	ds
	push	ax
	mov	ax,OFFSET SHAREDTEXT
	mov	dx,SEG SHAREDTEXT
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3984
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,16
; Line 437
	mov	ax,OFFSET DGROUP:$SG3986
	push	ds
	push	ax
	mov	ax,OFFSET SHAREDTEXT
	mov	dx,SEG SHAREDTEXT
	push	dx
	push	ax
	call	FAR PTR _strcmp
	add	sp,8
	or	ax,ax
	jne	$I3985
; Line 438
	mov	ax,OFFSET DGROUP:$SG3987
	push	ds
	push	ax
	mov	ax,OFFSET SHAREDTEXT
	mov	dx,SEG SHAREDTEXT
	push	dx
	push	ax
	call	FAR PTR _strcpy
	add	sp,8
; Line 439
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,OFFSET SHAREDTEXT
	mov	dx,SEG SHAREDTEXT
	push	dx
	push	ax
	call	FAR PTR WINSETWINDOWTEXT
; Line 440
; Line 441
$I3985:
; Line 442
	mov	WORD PTR [bp-34],10	;pt
	mov	WORD PTR [bp-32],0
	mov	WORD PTR [bp-30],50
	mov	WORD PTR [bp-28],0
; Line 443
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	lea	ax,WORD PTR [bp-34]	;pt
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	sub	cx,cx
	push	cx
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR GPICHARSTRINGAT
; Line 444
	mov	ax,OFFSET DGROUP:$SG3988
	push	ds
	push	ax
	mov	ax,OFFSET AVIOTEXT
	mov	dx,SEG AVIOTEXT
	push	dx
	push	ax
	mov	es,$T20029
	push	WORD PTR es:YCUR
	mov	es,$T20030
	push	WORD PTR es:XCUR
	mov	ax,OFFSET DGROUP:$SG3989
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,20
; Line 445
	mov	WORD PTR [bp-34],10	;pt
	mov	WORD PTR [bp-32],0
	mov	WORD PTR [bp-30],30
	mov	WORD PTR [bp-28],0
; Line 446
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	lea	ax,WORD PTR [bp-34]	;pt
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	sub	cx,cx
	push	cx
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR GPICHARSTRINGAT
; Line 448
	mov	es,$T20010
	push	WORD PTR es:LASTCREATED+2
	push	WORD PTR es:LASTCREATED
	mov	es,$T20011
	push	WORD PTR es:WINDOW+2
	push	WORD PTR es:WINDOW
	mov	ax,24
	push	ax
	call	FAR PTR WINQUERYWINDOWULONG
	push	dx
	push	ax
	mov	ax,OFFSET DGROUP:$SG3990
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,16
; Line 449
	mov	WORD PTR [bp-34],10	;pt
	mov	WORD PTR [bp-32],0
	mov	WORD PTR [bp-30],10
	mov	WORD PTR [bp-28],0
; Line 450
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	lea	ax,WORD PTR [bp-34]	;pt
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR _strlen
	add	sp,4
	sub	cx,cx
	push	cx
	push	ax
	lea	ax,WORD PTR [bp-478]	;Buffer
	push	ss
	push	ax
	call	FAR PTR GPICHARSTRINGAT
; Line 451
	mov	es,$T20011
	mov	ax,WORD PTR es:WINDOW
	mov	dx,WORD PTR es:WINDOW+2
	mov	WORD PTR LASTWINDOW,ax
	mov	WORD PTR LASTWINDOW+2,dx
; Line 452
	push	WORD PTR [bp-486]
	push	WORD PTR [bp-488]	;hps
	call	FAR PTR WINENDPAINT
; Line 453
	jmp	$SB3860
$SC3991:
; Line 458
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwnd
	mov	ax,42
	jmp	$L20055
$L20031:
	cmp	ax,4148
	jne	$JCC4239
	jmp	$L20034
$JCC4239:
	cmp	ax,4153
	jne	$JCC4247
	jmp	$SC3865
$JCC4247:
	cmp	ax,4163
	jne	$JCC4255
	jmp	$L20034
$JCC4255:
	cmp	ax,4208
	jne	$JCC4263
	jmp	$SC3879
$JCC4263:
	cmp	ax,4212
	jne	$JCC4271
	jmp	$SC3864
$JCC4271:
	cmp	ax,8193
	jne	$JCC4279
	jmp	$SC3863
$JCC4279:
	jmp	$SD3888
	nop	
$EX3830:
	pop	ds
	ASSUME DS: DGROUP
	pop	si
	mov	sp,bp
	pop	bp
	ret	14

MYWINDOWPROC	ENDP
; Line 474
	PUBLIC	MYDLGPROC
MYDLGPROC	PROC FAR
	push	bp
	mov	bp,sp
	sub	sp,82
	push	si
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
	ASSUME DS: DGROUP
;	hwndDlg = 16
;	msg = 14
;	mp1 = 10
;	mp2 = 6
;	PidBuffer = -80
;	ItemId = -82
; Line 477
	mov	ax,WORD PTR [bp+14]	;msg
	cmp	ax,32
	je	$SC4009
	cmp	ax,59
	jne	$SD4016
; Line 484
	mov	WORD PTR [bp-82],0	;ItemId
	jmp	SHORT $F4005
	nop	
$FC4006:
	inc	WORD PTR [bp-82]	;ItemId
$F4005:
	mov	ax,HEAPNUMBER
	cmp	WORD PTR [bp-82],ax	;ItemId
	jae	$L20059
; Line 486
	mov	si,WORD PTR [bp-82]	;ItemId
	mov	cl,2
	shl	si,cl
	mov	es,$T20008
	push	WORD PTR es:HEAPSUM[si+2]
	push	WORD PTR es:HEAPSUM[si]
	mov	ax,OFFSET DGROUP:$SG4008
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-80]	;PidBuffer
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,12
; Line 492
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwndDlg
	mov	ax,258
	push	ax
	mov	ax,353
	push	ax
	mov	ax,-1
	sub	dx,dx
	push	dx
	push	ax
	lea	ax,WORD PTR [bp-80]	;PidBuffer
	push	ss
	push	ax
	call	FAR PTR WINSENDDLGITEMMSG
; Line 493
	jmp	SHORT $FC4006
	nop	
$SC4009:
; Line 501
	mov	ax,WORD PTR [bp+10]	;mp1
	cmp	ax,1
	jb	$L20059
	cmp	ax,2
	ja	$L20059
; Line 508
; Line 510
; Line 505
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwndDlg
	mov	ax,1
	push	ax
	call	FAR PTR WINDISMISSDLG
; Line 506
$L20059:
	sub	ax,ax
	cwd	
	jmp	SHORT $EX3997
	nop	
$SD4016:
; Line 517
	push	WORD PTR [bp+18]
	push	WORD PTR [bp+16]	;hwndDlg
	push	WORD PTR [bp+14]	;msg
	push	WORD PTR [bp+12]
	push	WORD PTR [bp+10]	;mp1
	push	WORD PTR [bp+8]
	push	WORD PTR [bp+6]	;mp2
	call	FAR PTR WINDEFDLGPROC
$EX3997:
	pop	ds
	ASSUME DS: DGROUP
	pop	si
	mov	sp,bp
	pop	bp
	ret	14

MYDLGPROC	ENDP
WHATIS_TEXT	ENDS
END
