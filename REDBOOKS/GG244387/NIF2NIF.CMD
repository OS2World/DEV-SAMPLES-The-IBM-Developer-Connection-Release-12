/* NIFTONIF.CMD */
/* Utility for migrating NIF files */
ARG NIFName
IF NIFName = "" THEN Call helpNIF
IF NIFName = "?" THEN Call helpNIF
MSNIFName = '\MIGRATE\NIF\'NIFName'.NIF'
NIFXports = ""
Do while Lines(MSNIFName)
	CurLine = LineIn(MSNIFName)
	PARSE VAR CurLine Parm1 Parm2 Parm3
	IF Parm1 = "Model" THEN NIFTitle = Parm3
	IF Parm1 = "Xports" THEN NIFXports = Parm3
	IF Parm1 = "DeviceDriver" THEN NIFDevDrv = Parm3
	IF Parm1 = "Type" THEN NIFType = Parm3
End 
Call LineOut MSNIFName
MSProName = '\MIGRATE\NIF\'NIFName'.INI'
Do while Lines(MSProName)
	CurLine = LineIn(MSProName)
	PARSE VAR CurLine Parm1 Parm2 Parm3
	IF Parm1 = "drivername" THEN NIFDevName = Parm3
End 
Call LineOut MSProName
'REN \MIGRATE\NIF\'NIFName'.NIF 'NIFName'.LMF'
NewNIFName = '\MIGRATE\NIF\'NIFName'.NIF'
NIFComp = "["NIFName"]"
NIFType = "Type = "NIFType
NIFTitle = "Title = "NIFTitle
NIFDevDrv = "Name = "NIFDevDrv
NIFDevName = "Drivername = "NIFDevName
Call LineOut NewNIFName, NIFComp
Call LineOut NewNIFName, NIFType
Call LineOut NewNIFName, NIFTitle
Call LineOut NewNIFName, "Version = 0.9"
Call LineOut NewNIFName, NIFDevName
IF NIFXports <> "" THEN Do
	NIFXports = "Xports = "NIFXports
	Call LineOut NewNIFName, NIFXports
	End
Call LineOut NewNIFName, "[File]"
Call LineOut NewNIFName, NIFDevDrv
Call LineOut NewNIFName
Exit
/* ****************************************************** */
helpNIF:
Say 'Before using this utility you should copy the .NIF file'
Say 'to the \MIGRATE\NIF subdirectory, copy the corresponding'
Say 'PROTOCOL.INI file as well and change its name to the same'
Say 'name as the .NIF file, with a .INI extension.'
Say 'Like : CARD.NIF and CARD.INI.'
Say '********************************************************'
Say 'You then start NIFTONIF, giving it the name of the .NIF '
Say 'file (like CARD), without any extension.'
Say '******************************************************** '
Say 'Your original NIF will be renamed with a .LMF extension.'
Say 'The result will be a .NIF file suitable for LAN Server.'
Say 'This does not mean that the corresponding LAN Manager '
Say 'driver will work with LAN Server. You will have to test'
Say 'its operation. Usually DOS drivers will work but OS/2'
Say 'drivers are different. Drivers from the LAN Man client'
Say 'for OS/2 2.x are more likely to work.'
Say 'However the NIF file only allows LAPS to install the '
Say 'driver. Some drivers look for files in a path that is'
Say 'LAN Man related, so you may have to create the right'
Say 'directory structure by trial and error.'
Say 'Use the file NICBBS.TXT to get some phone numbers for'
Say 'card manufacturers.'
Exit
errStartNIF:
Say 'The syntax is : NIFTONIF NIFFileName'
Say 'where NIFFileName is the name of the NIF file to convert,'
Say 'without any extension. This file must be in \MIGRATE\NIF.'
Exit

