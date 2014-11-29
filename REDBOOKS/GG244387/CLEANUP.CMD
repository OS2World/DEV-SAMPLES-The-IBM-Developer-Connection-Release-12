/* CLEANUP.CMD */
/* This a utility to remove existing data for one server */
/* from the control center workstation                   */
/* Provide the name of a server as a parameter           */
/* ******* */
ARG SrvName
If SrvName = "" THEN Exit
'CD 'SrvName
IF rc <> 0 THEN Exit
'CD SCRIPTS'
IF rc = 0 THEN Do
	'DEL *.BAT'
	'DEL *.CMD'
	'CD ..'
	'RD SCRIPTS'
	End
'DEL *.DAT'
'DEL *.BKP'
'DEL *.ACL'
'DEL *.LOG'
'DEL *.INI'
'DEL *.PRO'
'DEL *.CMD'
'CD ..'
'RD 'SrvName
'CD LS'
'CD 'SrvName
IF rc <> 0 THEN Exit
'DEL *.CMD'
'DEL *.LOG'
'CD ..'
'RD 'SrvName
'CD ..'
