/*****************************************************/
/*                                                                                     */
/*  FileName: MLEINST.CMD                                                    */
/*                                                                                     */
/*  Purpose:  Installation command file for the MLEPart             */
/*               calls the Opendoc Rexx API to register the            */
/*               part                                                                 */
/*                                                                                     */
/*  Change History:                                                              */
/*               3/14/95 - <jso> first checked in                            */
/*****************************************************/


/* add the registration function  ( and utilities )*/

SAY " Adding functions ";
call RXFUNCADD 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

call SysCls
SAY

/* Check to see if the functions are already loaded and unload them if they are */
rc = RXFUNCQUERY('ODUnLoadOpenDocFuncs');
SAY " Rexx function Query on UnloadFuncs rc is " rc;
IF rc == 0 then
    call ODUnLoadOpenDocFuncs


 rc = RXFUNCQUERY('ODLoadOpenDocFuncs');                                                  
 SAY " rc = " rc;                                                                         
 IF rc \= 0 then DO                                                                       
   SAY " Adding ODLoadOpendocfuncs"                                                             
   rc = RXFUNCADD('ODLoadOpenDocFuncs', 'OPENDOC', 'ODLoadOpenDocFuncs');     
   SAY " RXFUNCADD rc for ODLoadOpendocFuncs = " rc                                       
      IF rc <  0 THEN DO                                                                        
        SAY " Unable to add  Opendoc functions, EXITING";                                      
        RETURN;                                                                                 
      END                                                                                       
 END                                                                                            

call ODLoadOpenDocFuncs;



className = "MLEPart"
dllName    = "MLEPart"
cTemplate = "TRUE";
somIRName = ""


SAY "Registering the MLEPart" 

rc = ODRegisterPartHandlerClass( className, dllName, cTemplate, somIRName);

IF rc < 0  then 
        SAY " Registration failed";
else
        SAY "MLE part successfully registered";
RETURN
