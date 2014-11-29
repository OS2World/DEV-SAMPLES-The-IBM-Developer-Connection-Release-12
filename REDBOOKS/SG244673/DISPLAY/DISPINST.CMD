/********************************************************************/
/*                                                                  */
/*  FileName: DispInst.CMD                                          */
/*                                                                  */
/*  Purpose:  Installation command file for the DisplayPart         */
/*               calls the Opendoc Rexx API to register the part    */
/*                                                                  */
/********************************************************************/

/* add the registration function  ( and utilities )*/

/*********************************************************/
/*   Make one's changes to the following two lines  */
/*********************************************************/
className  = "DisplayPart"
dllName    = "DispPart"

SAY " Adding functions ";
call RXFUNCADD 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

call SysCls
SAY
SAY "********************************************"
SAY "*                                          *"
SAY "*     Registering the DisplayPart          *"
SAY "*                                          *"
SAY "********************************************"

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

SAY "Registering the DisplayPart Part"

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
        SAY "DisplayPart successfully registered";
RETURN
