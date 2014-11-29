/* ******************************************************************** *
 * Copyright International Business Machines Corporation 1995, 1996     *
 * DISCLAIMER OF WARRANTIES.                                            *
 * The following [enclosed] code is sample code created by IBM          *
 * Corporation. This sample code is not part of any standard or IBM     *
 * product and is provided to you solely for the purpose of assisting   *
 * you in the development of your applications.  The code is provided   *
 * "AS IS". IBM MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT  *
 * NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS *
 * FOR A PARTICULAR PURPOSE, REGARDING THE FUNCTION OR PERFORMANCE OF   *
 * THIS CODE.  IBM shall not be liable for any damages arising out of   *
 * your use of the sample code, even if they have been advised of the   *
 * possibility of such damages.                                         *
 *                                                                      *
 * DISTRIBUTION.                                                        *
 * This sample code can be freely distributed, copied, altered, and     *
 * incorporated into other software, provided that it bears the above   *
 * Copyright notice and DISCLAIMER intact.                              *
 * -------------------------------------------------------------------- *
 * This file is part of the source for OpenDoc parts document in:       *
 *  IBM International Technical Support Organization Bulletin (Redbook) *
 *  "OpenDoc : An Introduction to Part Development" (SG24-4673)         *
 *                                                                      *
 * ******************************************************************** *
 */
/* add the registration function  ( and utilities )*/

SAY " Adding functions ";
call RXFUNCADD 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

call SysCls
SAY
SAY "**********************************************"
SAY "*                                            *"
SAY "*     Registering the ITSOContainer Part     *"
SAY "*                                            *"
SAY "**********************************************"

/* Check to see if the functions are already loaded and unload them if
   they are
*/

rc = RXFUNCQUERY('ODLoadOpenDocFuncs')
SAY " Rexx function Query on UnloadFuncs rc is " rc


rc = RXFUNCQUERY('ODLoadOpenDocFuncs');
SAY " rc = " rc;
IF rc \= 0 then DO
    SAY " Adding ODLoadOpenDocFuncs"
    rc = RXFUNCADD('ODLoadOpenDocFuncs', 'OPENDOC', 'ODLoadOpenDocFuncs');
    SAY " RXFUNCADD rc for ODLoadOpendocFuncs = " rc
    IF rc <  0 THEN DO
       SAY " Unable to add  Opendoc functions, EXITING";
       RETURN;
    END
END

call ODLoadOpenDocFuncs

/*********************************************************/
/*   Make one's changes to the following two lines  */
/*********************************************************/

className  = "ITSOContainerPart"
dllName    = "itsocntr"

SAY "Registering the ITSOContainer Part"

rc = ODRegisterPartHandlerClass( className, dllName);
SAY " rc = " rc;
/* rc of 0 = successfully added, 4 = successfully replaced */

IF rc \=  0 & rc \= 4 then do
        call ODUnLoadOpenDocFuncs
        call RxFuncDrop 'ODLoadOpenDocFuncs'
        SAY " Registration failed, rc = " rc;
        return;
end

        call ODUnLoadOpenDocFuncs
        call RxFuncDrop 'ODLoadOpenDocFuncs'
        SAY "ITSOContainer Part successfully registered";
RETURN
