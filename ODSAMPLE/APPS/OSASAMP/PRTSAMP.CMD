/*****************************************************/
/*                                                                                     */
/*  FileName: Prtsamp.CMD                                                    */
/*                                                                                     */
/*  Purpose:  Installation command file for Print Sample            */
/*               calls the OSA Rexx API to register Print Sample     */
/*               as an OSA aware Application                              */
/*                                                                                     */
/*  Change History:                                                              */
/*               10/6/95 - <vlr> first checked in                            */
/*****************************************************/

call RxFuncAdd 'SysLoadFuncs', 'REXXUTIL', 'SysLoadFuncs'
call SysLoadFuncs

SAY "*********************************"

  call RxFuncAdd 'OSALoadFuncs', 'OPENDOC', 'OSALoadFuncs'

  call OSALoadFuncs

  rc = OSAInstallApplication('OSA PRINT SAMPLE', 'prtsamp.exe', prtsamp.AET,0,0,0,0)

  SAY " OSAInstallApp  rc = " rc

  call OSAUnloadFuncs
SAY "*********************************"
RETURN

