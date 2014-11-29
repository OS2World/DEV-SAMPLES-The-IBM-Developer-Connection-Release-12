/* ALIASES.CMD */
/* ************************************************************ */
/* Migrating from LAN Manager to LAN Server                     */
/* Processing the SRVAUTO.PRO file to create shares and aliases */
/* ******** */
Sharefile = '\FROMLM\SRVAUTO.PRO'
/* ******** */
ARG SrvName
If SrvName = "" Then Call SrvInput
Server = SrvName
/* *** */
Say "Processing shared resources for server "Server
Printer = 0;Disk = 0;Comm = ""
Do While Lines(Sharefile) > 0
	CurLine = Linein(Sharefile)
	If pos('NET SHARE',CurLine) > 0 Then Do
		MaxNumUsers = ''; Printer = 0
		Parse var CurLine . . AllLine
		SpAlEnd = pos('"',AllLine,2)
		Alias = Left(AllLine,SpAlEnd)
		Remaining = Right(AllLine,Length(AllLine)-SpAlEnd)
		If pos('/PRINT',CurLine) <> 0 Then call prnAlias
			Else call diskAlias
		If pos('/UNLIMITED',Remaining) <> 0 Then Do
			MaxNumUsers = '/UN' 
			End
			Else Do 
			MaxNumUsers = ''
			End	
		If pos('/USERS:',Remaining) <> 0 Then Do
			UserSize = Length(Remaining) - (pos('/USERS:',Remaining) - 1)
			Userlim = Right(Remaining,UserSize)
			Parse var Userlim MaxNumUsers Trash
			End
			Else Do
			nop
			End
		If pos('/REMARK:',Remaining) <> 0 Then Do
			RemSize = Length(Remaining) - (pos('/REMARK:',Remaining) + 8)
			Rem1 = Right(Remaining,RemSize);Rem = Left(Rem1,pos('"',Rem1))
			If Length(Rem) > 41 Then Do
			Rem = Left(Rem,40)||'"'
			End
			Else nop
			Rem = '/R:"'||Rem 
			End
			Else Do 
			Rem = ''
			End
		If pos('/COMM',Remaining) <> 0 Then Comm = "/COMM"
		If Printer = 1 Then Do
		'NET ALIAS' AliasName Server AliasName StartVal '/PRINT' Rem MaxNumUsers
		Printer = 0
		End
		Else Do
		If Printer = 8 Then Do
		'NET SHARE' Alias MaxNumUsers Rem '/PRINT'
		Printer = 0
		End
		Else nop
		If Disk = 1 Then Do
		'NET ALIAS' AliasName Server ResourceName StartVal Comm Rem MaxNumUsers
		Disk = 0
		Comm = ""
		End
		Else Do
			If Disk = 8 Then Do
			'NET SHARE' Alias MaxNumUsers Rem Comm 
			Disk = 0
			Comm = ""
			End
			Else nop
		End
	End
	Else Do
	If pos('NET COMM',CurLine) <> 0 Then Do
		'@'CurLine
		End
		Else nop
	End
End	
RC = Lineout(Sharefile)
Exit
/* ********************************************** */
prnAlias:
AliasName = Strip(Alias,'b','"') ; Printer = 1
If Length(AliasName) > 8 Then Printer = 8
If Words(AliasName) > 1 Then AliasName = '"'AliasName'"'
Return
/* ********************************************** */
/* ********************************************** */
diskAlias:
AliasEnd = pos('=',Alias) - 1;Alias1 = Left(Alias,AliasEnd)
AliasName = Right(Alias1,Length(Alias1)-1)
If AliasName = 'USERS' Then Return
If AliasName = 'NETLOGON' Then Return
If AliasName = 'PUBLIC' Then Return
If AliasName = 'REPL$' Then Return
Resource = Length(Alias) - (AliasEnd +1)
ResourceName = Right(Alias,Resource)
ResourceName = Left(ResourceName,Length(ResourceName)-1)
Disk = 1
If Length(AliasName) > 8 Then Disk = 8
If Words(AliasName) > 1 Then AliasName = '"'AliasName'"'
Return
SrvInput:
Say "Please provide the name of the server being migrated as"
Say "a parameter on the command line, like ALIASES SRV_1."
Exit
