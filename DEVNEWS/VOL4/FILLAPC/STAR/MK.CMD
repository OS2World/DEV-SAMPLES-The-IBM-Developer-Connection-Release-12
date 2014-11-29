/*                                                                      */
/*                                                                      */
/* Simple front end to make. Doesn't rely of config.sys having defined  */
/* the toolkit and c/c++ compiler paths.                                */
/*                                                                      */
/*                                                                      */

'@echo off'
parse arg drive fn rest

if drive = "?" then signal help
if drive = "" then drive = left(directory(),2)
if fn = "" then fn = "makefile"

setlocal
'SET BASEDIR='drive


'set include=%BASEDIR%\toolkt21\c\os2h;%BASEDIR%\ibmcpp\include;'
'set lib=%BASEDIR%\toolkt21\os2lib;%BASEDIR%\ibmcpp\lib'


'SET TOOLSPATH=%BASEDIR%\ibmcpp\bin;%BASEDIR%\toolkt21\os2bin'
'SET DATAPATH=%BASEDIR%\ibmcpp\locale;%BASEDIR%\ibmcpp\chelp;'

'SET PATH=%TOOLSPATH%;%PATH%'
'SET DPATH=%DATAPATH%;%DPATH%'

say "Building : "fn

'echo on'
'nmake /nologo /e /f' fn rest '2>&1'
'@echo off'

endlocal

say ' '

exit

help:
say " Usage: mk <drive> <makefile> <parameters> "


