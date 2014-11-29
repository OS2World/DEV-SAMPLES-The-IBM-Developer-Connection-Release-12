/*  IDSS_Cmp_Opts: -As -Zpe -G2s                                     */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*  First lines must be C compiler options.                          */
/*-------------------------------------------------------------------*/
/*  OPtions -W3 and -DLINT_ARGS are forced by COS2 IDSS process.     */
/*                                                                   */
/*                                                                   */
/*  Warning: If you supply -DLINT_ARGS option or any -Dxxx options   */
/*           twice, the IBM-C2 compiler will loop and  your  COS2    */
/*           IDSS process will abort on time out.                    */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*           G set printer to condensed mode, Double printing      */
/*                                                                   */
#pragma linesize(132)
#pragma pagesize(63)
#pragma title("GOS/2 WatchDog Version 2.00")
#pragma subtitle("Header")
#pragma page(1)
/*****************************************************************************
*       Author:        P.Guillon - D/0768 CER IBM La Gaude                   *
*                                                                            *
*       Environment:   PCAT, PS/2  OS/2  1.1 or higher                       *
*                                                                            *
*       OS2 Syntax:    WATCHDOG  ?                                           *
*                      WATCHDOG  [xxx.xxx] [/E or /D] [/Q]                   *
*                      WATCHDOG  /U [/Q]                                     *
*                                                                            *
*                          ?           Help                                  *
*                          xxx.xxx     Decimal time in minute                *
*                                        (2 minutes by default)              *
*                                          0.25 < xx.xxx < 120               *
*                          /E          Enable  (Default)                     *
*                          /D          Disable                               *
*                          /Q          Quiet (no messages)                   *
*                          /U          Uninstall                             *
*                                                                            *
*       This program watchs PC,PS's activities. It's a small resident        *
*       Process that automatically REBOOT OS/2  if isn't wakended  by        *
*       itself or by  RESETTPS  program during the timeout count pro-        *
*       grammed.                                                             *
*                                                                            *
*       The companion program RESETTPS, that reset  WATCHDOG  timeout        *
*       count,is usually invoked by VM to make sure that the HOST to         *
*       PS connection isn't lost.                                            *
*                                                                            *
*       The REBOOT function is assumed by the companion bimodal  Cha-        *
*       racter Device Driver SERVBOOT.SYS.                                   *
*                                                                            *
*       The design of this little program was done with in  mind  the        *
*       IDSS PS/2 La Gaude OS/2 servers. It's an OS/2  adaptation  of        *
*       DOS WATCHDOG 1.02.                                                   *
*----------------------------------------------------------------------------*
*      What you need to Compile this Program:                                *
*                                                                            *
*           Required Files:                                                  *
*                                                                            *
*                WATCHDOG.C    - Source code for this C Program.             *
*                WATCHDOG.DEF  - Make window compatible Program.             *
*                WATCHDOG.MAK  - Make file                                   *
*                                                                            *
*           Required Libraries:                                              *
*                                                                            *
*                SLIBCE.LIB   - This is the Protect mode/standard combined   *
*                               small model run-time library.                *
*                DOSCALLS.LIB - DLL API Import Library.                      *
*                                                                            *
*           Required Programs: IBM C/2 Compiler (Version 1.1 or later)       *
*                              IBM Linker/2 (Version 1.1 or later)           *
*                                                                            *
*****************************************************************************/

#pragma subtitle("DLL Link References - History")
#pragma page(1)
/*****************************************************************************
*                                                                            *
*           Dynamic Link References:                                         *
*                                                                            *
*                DosAllocShrSeg - Allocate a "named" shareable memory block  *
*                DosGetShrSeg   - Obtain access to a "named" shared-memory   *
*                DosFreeSeg     - Deallocate a memory block                  *
*                                                                            *
*                DosCreateSem   - Create a system semaphore                  *
*                DosSemRequest  - Attempt to set (claim) a semaphore         *
*                DosSemClear    - Clear (release) a semaphore                *
*                DosCloseSem    - Close a system semaphore                   *
*                                                                            *
*                DosGetVersion  - Get the OS/2 Version number                *
*                DosOpen        - Open or Create a file                      *
*                DosDevIOCtl    - Generic Device I/O control                 *
*                                                                            *
*                DosBeep        - Make a musical tone on speaker             *
*                DosSleep       - Pause execution for a specified interval   *
*                DosExit        - Terminate process or one thread            *
*                                                                            *
******************************************************************************
*                                                                            *
*    Initial Release Version 1:                                              *
*                                                                            *
*             - 09/01/89  T.Liethoudt's  Release  Version 1.0                *
*                                                                            *
*    Modified by Pete Guillon D/0768                                         *
*                                                                            *
*             - 06/22/90  Change MWATCHDO.CMD to make file WATCHDOG.MAK      *
*                         Add main function prototype                        *
*                         Change IPL device driver (DEMOD$ to IPLOS2$)       *
*----------------------------------------------------------------------------*
*                                                                            *
*    Release Version 2:                                                      *
*                                                                            *
*             - 09/28/90  Release Version 2.00                               *
*                                                                            *
*                         It's the OS/2 version of DOS WATCHDOG. This new    *
*                         program has the same options that DOS version:     *
*                                                                            *
*                            - Help                                          *
*                            - Variable Time out count                       *
*                            - Enable, Disable capability                    *
*                            - Quiet mode                                    *
*                            - Uninstal option                               *
*                                                                            *
*****************************************************************************/

#pragma subtitle("Include - Define - F prototypes - Global")
#pragma page(1)
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define INCL_BASE
#define INCL_DOSMEMMGR             /*  Required by the DosGetShrSeg function */
#include <OS2.h>

#include "watchdog.h"              /*  Watchdog Micellaneous definitions     */

#define SLEEP_DELAY   1000L        /*  Sleep delay in ms (set to 1 s) */
#define DEFAULT_T_OUT  120L        /*  Default Timeout is 2 minutes   */
#define COUNT_MIN       .25        /*  Mini count = .25 minutes (15s) */
#define COUNT_MAX      120.        /*  Maxi count = 120 minutes (2H)  */

                                   /* CASE OPTIONS ------------------ */
#define ENABLE           'E'               /*  Enable Case            */
#define DISABLE          'D'               /*  Disable Case           */
#define QUIET            'Q'               /*  Quiet  Case            */
#define UNINSTALL        'U'               /*  Uninstall Case         */

                                   /* INSTALL MASK ------------------ */
#define INST_C_MASK 0x80                   /*  Timeout count bit mask */
#define INST_E_MASK 0x40                   /*  Enable bit mask        */
#define INST_D_MASK 0x20                   /*  Disable bit mask       */
#define INST_Q_MASK 0x10                   /*  Quiet bit mask         */
#define INST_U_MASK 0x08                   /*  Uninstall bit mask     */

                                   /* DO-MI-SOL Play loop count ----- */
#define GET_SHAR_MEM_FAILED    1           /*  DosAllocShrSeg         */
#define DOS_VERS_FAILED        2           /*  DosGetVersion          */
#define OPEN_FAILED            3           /*  DosOpen                */
#define SHAR_MEM_ACCESS_FAILED 4           /*  DosGetShrSeg           */
#define IOCTL_FAILED           5           /*  DosDevIOCtl            */
#define IPL_FAILED             6           /*  IPL fail               */

#define NON_EXCLUSIVE  1           /*  Non exclusive system semaphore */

#define NOT_INSTALLED  0           /*  Not all ready installed        */
#define INSTALLED      1           /*  All ready installed            */

VOID   logo_msg(VOID);                  /*  Print Logo function prototype    */
VOID   help_msg(VOID);                  /*  Print HelpMsg function prototype */
                          /*  Open ServBoot Device Driver function prototype */
USHORT Open_ServBoot_Dev_Driver(VOID);
USHORT Re_Boot_OS2(VOID);               /*  ReBoot OS/2 Function prototype   */
VOID   Play(USHORT);                    /*  Play Function prototype          */
                                        /*  Get parms function prototype     */
USHORT get_parms(INT ,CHAR * *, USHORT, WATCH FAR *);
VOID   main(INT, CHAR * *);             /*  main function prototype          */

HFILE hfDevice_Handle;                  /* Global Device driver handle       */

#pragma subtitle("main() function")
#pragma page(1)
VOID main(INT iArgc,CHAR * argv[])
{

   USHORT usReturn_Code,           /*  Dos function Return Code       */
          usRc;                    /*  Local function return Code     */

   USHORT usSize = WATCH_LENGTH;   /*  DosAllocShrSeg request size in Bytes  */
   PSZ    pszName = SHARENAME;              /*  Shareable memory block name  */
   SEL    selMem_Selector;                  /*  Receive Segment selector     */

   PSZ     pszSemName = SEMNAME;            /*  System semaphore name        */
   HSEM    hsemSemHandle;                   /*  System semaphore Handle      */


   WATCH FAR * Watch_Ptr;          /*  WATCH pointer                         */

                                   /* Get Access to Shareable memory Block   */
   usReturn_Code = DosGetShrSeg(pszName, & selMem_Selector);

   if (usReturn_Code == ERROR_FILE_NOT_FOUND) {
                                   /* if ServBoot Device Driver Installed    */
      if (usReturn_Code = Open_ServBoot_Dev_Driver() == 0) {

         if (usReturn_Code=DosAllocShrSeg(usSize,/* Get Shareable mem Block  */
                                          pszName,
                                          & selMem_Selector) == 0) {

            Watch_Ptr = (WATCH FAR *) MAKEP(selMem_Selector,0);

                                                /* Initialize Default parms  */
            Watch_Ptr->Counter_Init_Value = DEFAULT_T_OUT;
            Watch_Ptr->Counter = Watch_Ptr->Counter_Init_Value ;
            Watch_Ptr->Flag = ENABLE_MASK;

                                                /*  Get Install parameters   */
            if (usRc= get_parms(iArgc, argv, NOT_INSTALLED, Watch_Ptr) != 0) {
               DosFreeSeg(selMem_Selector);     /*  Then Deallocate memory   */
               DosExit((USHORT) 0,usRc); }      /*  And Exit if some error   */

            DosCreateSem(NON_EXCLUSIVE,         /*  Create System semaphore  */
                         & hsemSemHandle,
                         pszSemName);

            DosSemRequest(hsemSemHandle,      /*  Waits for shared resources */
                          SEM_INDEFINITE_WAIT);

#pragma page(1)
            while ((Watch_Ptr->Flag & U_MASK) == 0){  /* WATCHDOG Loop */

               DosSemClear(hsemSemHandle);    /*  Release  shared resources  */

               if (usReturn_Code = DosSleep(SLEEP_DELAY) != NO_ERROR) {
                  logo_msg();
                  printf("\n -*-*- DosSleep Error Code %04x -*-*-\n",
                                                                usReturn_Code);
                  DosBeep(262,500);DosBeep(440,1000);DosBeep(262,500);
               } /* End if */

               DosSemRequest(hsemSemHandle,  /*  Waits for shared resources  */
                             SEM_INDEFINITE_WAIT);

               if (Watch_Ptr->Flag & ENABLE_MASK) {
                  if (--Watch_Ptr->Counter <= 0) {
#ifdef DEBUG
                     usReturn_Code = system("ECHO Temporary REBOOT\n");
                     Watch_Ptr->Counter = Watch_Ptr->Counter_Init_Value ;
#else
                           /*  Reboot OS/2 - DosExit SHOULD NEVER be invoked */
                     DosExit((USHORT) 0,usReturn_Code = Re_Boot_OS2() );
#endif
                  } /* End if */
               } /* endif */

            } /* endwhile WATCHDOG Loop */

            DosCloseSem(hsemSemHandle);         /*  Close system semaphore   */

            if ((Watch_Ptr->Flag & QUIET_MASK) == 0)
                printf("\n  -*- WATCHDOG uninstalled successfully -*-\n");

            DosFreeSeg(selMem_Selector);     /*  Deallocate Shared memory    */
            DosExit((USHORT) 0,(USHORT) 0);  /*  and Exit whith Rc = 0       */

         } else {     /* Get Shareable memory Block with some Error */
            logo_msg(); /* Print Logo and Error Message             */
            printf("\n -*-*- Cannot Get Shareable Memory Block -*-*-\n");
            printf("  -*-     Program Aborted with RC %04x    -*-\n",
                                                               usReturn_Code);
            Play((USHORT)GET_SHAR_MEM_FAILED);   /* Play DO-MI-SOL-DO....    */
            DosExit((USHORT) 0,usReturn_Code);   /* Then Exit With Rc Error  */
         } /* endif - Get Shareable memory Block */

      } else {               /* Else ServBoot Device Driver not Installed    */
         DosExit((USHORT) 0,usReturn_Code);      /* Exit With Rc Error       */
      } /* endif ServBoot Device Driver Installed */

#pragma page(1)

   } else {  /* Not ERROR_FIND_NOT_FOUND, WATCHDOG maybe all Ready installed */
      if (usReturn_Code == 0) {
         Watch_Ptr = (WATCH FAR *) MAKEP(selMem_Selector,0);

         DosOpenSem( & hsemSemHandle,             /*  Open System semaphore  */
                     pszSemName);

         DosSemRequest(hsemSemHandle,         /*  Waits for shared resources */
                       SEM_INDEFINITE_WAIT);

         Watch_Ptr->Counter = Watch_Ptr->Counter_Init_Value ;
                                              /*  Program allready Installed */
                                              /*  Get Change options         */
         usRc = get_parms(iArgc, argv, INSTALLED, Watch_Ptr);

         DosSemClear(hsemSemHandle);          /*  Release  shared resources  */

         DosFreeSeg(selMem_Selector);         /*  Freeing Shared memory      */
         DosExit((USHORT) 0,usRc);            /*  And Exit with Rc           */
      } else {  /* Unexpected Error From Get Access to Share Memory Block    */
         logo_msg(); /* Print Logo and Error Message                         */
         printf("\n -*-*- Cannot Get Access to Shareable Memory Block -*-*-\n");
         printf("       -*-     Program Aborted with RC %04x    -*-\n",
                                            /* Play DO-MI-SOL-DO-SOL-MI-DO   */
                                                              usReturn_Code);
         Play((USHORT)SHAR_MEM_ACCESS_FAILED);
         DosExit((USHORT) 0,usReturn_Code);      /* Then Exit With Rc Error  */
      } /* endif - Program allready installed */

   } /* endif - ERROR_FILE_NOT_FOUND                                         */

} /* end of main program */

#pragma subtitle("logo_msg(), hlp_msg() functions")
#pragma page(1)

VOID   logo_msg(VOID)                 /*  Print Logo Msg function definition */
{
   printf("\n ษอออออออออออออออออออออออออออออออออออออป\n") ;
   printf(  " บ    -*- IBM  -*-    บ\n") ;
   printf(  " บ             CER LA GAUDE            บ\n") ;
   printf(  " บ                                     บ\n") ;
   printf(  " บ        WATCHDOG Version 2.00        บ\n") ;
   printf(  " บ                 by                  บ\n") ;
   printf(  " บ            Pete Guillon             บ\n") ;
   printf(  " ศอออออออออออออออออออออออออออออออออออออผ\n") ;
   return ;
} /* End logo_msg() function */



VOID   help_msg(VOID)             /*  Print Help Message function definition */
{
   printf(" This program watchs  PC, PS's activities. It's a small resident  Process  that\n") ;
   printf(" automatically REBOOT OS/2 if isn't wakended by itself or by  RESETTPS  program\n") ;
   printf(" during the timeout count programmed. At any time it can be disabled or enabled\n") ;
   printf(" and his timeout count changed. OS/2 Syntax is:\n") ;
   printf("     WATCHDOG  ?\n") ;
   printf("     WATCHDOG  [xxx.xxx] [/E  |  /D] [/Q]\n") ;
   printf("     WATCHDOG  /U [/Q]\n\n") ;
   printf("         ?           Help\n") ;
   printf("         xxx.xxx     Decimal time in minute (2 by default) 0.25 < xxx.xxx < 120\n") ;
   printf("         /E          Enable  (Default)\n") ;
   printf("         /D          Disable\n") ;
   printf("         /Q          Quiet (no messages)\n") ;
   printf("         /U          Uninstall\n") ;
   return ;
} /* End help_msg() function */

#pragma subtitle("get_parms() functions")
#pragma page(1)
USHORT get_parms(INT iArg_C,CHAR * *Arg_V,USHORT Inst_Status,WATCH FAR * W_Ptr)
{
   UCHAR  uchInstall_Flag = 0;              /*  Install flag byte            */
   USHORT usGet_Parms_Rc = 0;               /* Get Parms return code         */

   INT iWi;                                 /* Working Index                 */

   float TimeOutCount;                      /* Working floating TimeoutCount */

   CHAR Count_Arg_Buff[81],                 /* Count Argument Working buffer */
        Working_Buffer[81],                 /* Working count buffer          */
        * arg_p,                            /* Current Argument pointer      */
        * token;                            /* Token Pointer                 */

   while ((iArg_C > 1) && (usGet_Parms_Rc == 0)) {

      arg_p = strupr(Arg_V[iArg_C-- -1]);   /* Capitalize Current Argument   */

      if (arg_p[0] == '/'  && arg_p[2] == '\0') {   /* If /x Argument        */
         switch (arg_p[1]) {
            case ENABLE:                        /* /E  Option                */
               uchInstall_Flag |= INST_E_MASK;  /* Set Enable bit Mask       */
               break;
            case DISABLE:                       /* /D  Option                */
               uchInstall_Flag |= INST_D_MASK;  /*  Set Disable bit Mask     */
               break;
            case QUIET:                         /* /Q Option                 */
               uchInstall_Flag |= INST_Q_MASK;  /* Add Quiet bit Mask        */
               break;
            case UNINSTALL:                     /* /U Option                 */
               uchInstall_Flag |= INST_U_MASK;  /* Add Quiet bit Mask        */
               break;
            default:
               usGet_Parms_Rc = 2;              /* Set Error RC = 2          */
         } /* endswitch */

      } else {                                  /* Not /x Argument           */
         if (arg_p[0] == '?'  && arg_p[1] == '\0') {
            usGet_Parms_Rc = 1;                 /* ? Option                  */

         } else {                               /* Maybe Valid Timeout Count */
            strcpy(Count_Arg_Buff,"0");         /* Heading zero for Strok    */
            token = strtok(strcat(Count_Arg_Buff,arg_p),".");

            for (iWi = 0; iWi < strlen(token) && usGet_Parms_Rc == 0; iWi++)
               if (isdigit(token[iWi]) == 0)    /* Error if not digit        */
                  usGet_Parms_Rc = 2;

            if (usGet_Parms_Rc == 0) {          /* Valid fixed digits        */
               strcpy(Working_Buffer,token);    /* Put Fixed part in Buffer  */
               strcat(Working_Buffer,".");      /* Add . to it               */
               token = strtok(NULL," ");        /* Get decimal part          */

               for (iWi = 0; iWi < strlen(token) && usGet_Parms_Rc == 0; iWi++)
                  if (isdigit(token[iWi]) == 0) /* Error if not digit        */
                     usGet_Parms_Rc = 2;

               if (usGet_Parms_Rc == 0) {         /* If Valid decimal digits   */
                  strcat(Working_Buffer,token); /* Add decimal part in Buff  */
                  sscanf(Working_Buffer,"%f",&TimeOutCount);

                                       /* Set Error Code if outside limits   */
                  if ((TimeOutCount < (float) COUNT_MIN) ||
                      (TimeOutCount > (float) COUNT_MAX)) usGet_Parms_Rc = 2;
                                       /* Otherwise Add Count bit Mask       */
                  else uchInstall_Flag |= INST_C_MASK;

               } /* endif Valid decimal digits */
            } /* endif Valid Fixed digits */
         } /* endif Else Maybe Valid timeout Count */
      } /* endif Else No /x Argument */
   } /* endwhile */

                     /* If all parameters individually Valid Process Options */
   if (usGet_Parms_Rc ==0) {

      if (uchInstall_Flag & INST_U_MASK ) {

         if (uchInstall_Flag & ~(INST_U_MASK | INST_Q_MASK) ) {
            usGet_Parms_Rc = 2;             /* Only /Q is valid with /U      */
            logo_msg();                     /* Display Logo   */
            help_msg();                     /* Display Help   */

         } else {
            if (Inst_Status == NOT_INSTALLED) {
               usGet_Parms_Rc = 3;          /* Set RC = 3     */
               logo_msg();                  /* Display Logo   */
               printf("\n -*-*- Cannot uninstall WATCHDOG program -*-*-\n");
               printf("      -*-    Program not installed    -*-\n");

            } else {                        /* Uninstall Pgm  */
               W_Ptr->Flag |= U_MASK;
               if (uchInstall_Flag & INST_Q_MASK) W_Ptr->Flag |= QUIET_MASK;

            } /* endif  */
         } /* endif  */

      } else {            /* Test if not Enable and Disable Options together */
         if ( (uchInstall_Flag & INST_E_MASK)   &&
              (uchInstall_Flag & INST_D_MASK))  {
            usGet_Parms_Rc = 2;             /* /E not Valid with /D          */
            logo_msg();                     /* Display Logo   */
            help_msg();                     /* Display Help   */

         } else {                  /* Process. All Options are valid.        */

            if (uchInstall_Flag & INST_C_MASK) {
               W_Ptr->Counter_Init_Value = (ULONG) (TimeOutCount * 60) ;
               W_Ptr->Counter = W_Ptr->Counter_Init_Value ;
            } /* endif */

            if (uchInstall_Flag & INST_E_MASK)
               W_Ptr->Flag |= ENABLE_MASK;
            else if (uchInstall_Flag & INST_D_MASK)
               W_Ptr->Flag &= ~ENABLE_MASK;

            if ((uchInstall_Flag & INST_Q_MASK) == 0) {
               logo_msg();                  /* Display Logo   */

               if (Inst_Status == NOT_INSTALLED)
                  printf("\n -*-*- Program  installed successfully -*-*-\n");

               else {
                  printf("\n -*-*- Watchdog Time Out ");

                  if (W_Ptr->Flag & ENABLE_MASK) printf("Enabled");
                  else printf("Disabled");

                  printf(" set to %07.3f Minutes -*-*-\n",
                                    (float)W_Ptr->Counter_Init_Value / 60);
               } /* endif */
            } /* endif */
         } /* endif */
      } /* endif */

   } else {                        /* No all parmameters  individually Valid */
      logo_msg();                           /* Display Logo   */
      help_msg();                           /* Display Help   */

   } /* endif /* All parameters individually Valid  */

   return usGet_Parms_Rc;                   /* Return Rc to Caller           */

} /* End get_parms() function */

#pragma subtitle("Open ServBoot Device Driver function")
#pragma page(1)
#define DEV_NAME         "SREBOOT$" /* Device Driver basic Name  (all ver)   */
#define DEV_PSEUDO_PATH  "\\DEV\\"  /* Device Driver pseudoPath (ver >= 1.2) */
#define DEVNAME_MAX_L    14

#define SYSTEM_ATTRIB  4                  /* File Attribute = System         */
#define OPEN_IF_EXIST  1                  /* OFlag = open, fail if not exist */
#define DENY_MODE      0x40               /* OMode = Deny None               */

                         /*  Open ServBoot Device Driver function definition */
USHORT Open_ServBoot_Dev_Driver(VOID)
{
   CHAR   Device_Driver_Name[DEVNAME_MAX_L] ;   /* Holds Device Driver name  */
   USHORT usVersion ,                           /* Holds OS/2 Version number */
          usAction ,                            /* Required for DosOpen      */
          usRc ;                                /* Return code from OS/2 API */
                                /* If we can get the version                 */
   if ( (usRc = DosGetVersion( & usVersion)) == 0 ) {

      if (LOUCHAR(usVersion) <= 10)           /* If minor version is <= 10   */
         strcpy(Device_Driver_Name,DEV_NAME); /* Use Device Driver name only */

      else {        /* Else minor version > 1  Use full (pseudo-)pathname    */
         strcpy(Device_Driver_Name, DEV_PSEUDO_PATH); /* Get Pseudo Path     */
         strcat(Device_Driver_Name, DEV_NAME);        /* then Add it's Name  */
      } /* endif */

      /* Get Device Driver Handle                                            */
      if ((usRc = DosOpen((PSZ) Device_Driver_Name,  /* Device Driver name   */
                & hfDevice_Handle,                   /* DD Handle (returned) */
                & usAction,               /* Action (returned, UNUSED)       */
                (ULONG) 0,                /* File Size not applicable        */
                (USHORT) SYSTEM_ATTRIB,   /* File Attribute = System         */
                (USHORT) OPEN_IF_EXIST,   /* OFlag = open, fail if not exist */
                (USHORT) DENY_MODE,       /* OMode = Deny None               */
                (ULONG) 0) )              /* RESERVED                        */
                != 0 ) {                  /* If DosOpen failed, error:       */

         logo_msg();             /* Print Logo and Error Message             */
         printf("\n -*-*- ServBoot Device Driver Failed to Open -*-*-\n");
         printf("  -*-       Program Aborted with RC %04x      -*-\n", usRc);
         Play((USHORT)OPEN_FAILED);         /* Play DO-MI-SOL-DO-SOL-MI-DO   */
      } /* endif DosOpen */

   } else {                               /* Get Dos Version failed          */
      logo_msg();                /* Print Logo and Error Message             */
      printf("\n -*-*-      Cannot Get Dos Version      -*-*-\n");
      printf("  -*-    Program Aborted with RC %04x    -*-\n", usRc);
      Play((USHORT)DOS_VERS_FAILED);      /* Play DO-MI-SOL-DO-SOL-MI-DO     */
   } /* endif DosGetVersion  */

   return usRc;                           /* Return to caller                */

} /* End Open_ServBoot_Dev_Driver() function */

#pragma subtitle("Re_Boot_O2 and Play functions")
#pragma page(1)

#define SERVBOOT_CAT    0x80            /*  SERVBOOT Device Driver Category  */
#define SERVBOOT_FUNC   0x40            /*  SERVBOOT IOCTL Function          */


USHORT Re_Boot_OS2(VOID)               /*  Re_Boot_OS2 Function definition   */
{
   USHORT usRc ;                              /* Return code from OS/2 API   */

   if ( (usRc = DosDevIOCtl( (PVOID) NULL,    /* Data buffer (UNUSED)        */
              (PVOID) NULL,                   /* Parm list (UNUSED)          */
              (USHORT) SERVBOOT_FUNC,         /* Function = Desired command  */
              (USHORT) SERVBOOT_CAT,          /* Category = ServBoot DD      */
              hfDevice_Handle) )              /* DosOpen handle              */
              != 0) {                         /* If IOCtl failed, send       */
                                              /*  error message              */
      logo_msg();                /* Print Logo and Error Message             */
      printf("\n -*-*- ServBoot Device Driver IOCTL Failed -*-*-\n");
      printf("  -*-      Program Aborted with RC %04x     -*-\n", usRc);
      Play((USHORT)IOCTL_FAILED);          /* Play DO-MI-SOL-DO-SOL-MI-DO    */
   } else {
      logo_msg();                /* Print Logo and Error Message             */
      printf("\n -*-*- ServBoot Device Driver Does Not IPL -*-*-\n");
      printf("  -*-      Program Aborted with RC %04x     -*-\n", usRc = 0xFF);
      Play((USHORT)IPL_FAILED);            /* Play DO-MI-SOL-DO-SOL-MI-DO    */
   } /* endif */

   return usRc ;                            /* SHOULD NEVER RETURN           */

} /* End Re_Boot_OS2() function */


VOID Play(USHORT usCount)                     /*  Play Function definition   */
{
                                  /* Play DO_MI-SOL-DO-SOL-MI-DO Count times */
   DosBeep(262,250);

   for ( ; usCount > 0 ; usCount--) {
      DosBeep(330,250);DosBeep(392,250);DosBeep(494,250);
      DosBeep(392,250);DosBeep(330,250);DosBeep(262,250);
   } /* endfor */

   return ;

} /* End Play() function */
