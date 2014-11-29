/******************************************************************************/
/*                                                                            */
/*                     IBM Sample Virtual Device Driver                       */
/*                                                                            */
/*                 Copyright (c) IBM Corporation 1993                         */
/*                          All Rights Reserved                               */
/*                                                                            */
/*   DISCLAIMER OF WARRANTIES.  The following [enclosed] code is              */
/*   sample code created by IBM Corporation. This sample code is not          */
/*   part of any standard or IBM product and is provided to you solely        */
/*   for  the purpose of assisting you in the development of your             */
/*   applications.  The code is provided "AS IS", without                     */
/*   warranty of any kind.  IBM shall not be liable for any damages           */
/*   arising out of your use of the sample code, even if they have been       */
/*   advised of the possibility of   such damages.                            */
/*                                                                            */
/******************************************************************************/
/*      VDFT.H                                                                */
/*      MVDM Virtual 3270 Device Driver Exported Definitions                  */
/*                                                                            */
/*      For include file hierarchy, see MVDM.H                                */
/******************************************************************************/

/***    VDFT constants   ***/

#define TRUE            1
#define FALSE           0
#define ABORT           0x0002      /*  response value from VDHPopup       */
#define RETRY           0x0004      /*  response value from VDHPopup       */
#define IGNORE          0x0008      /*  response value from VDHPopup       */

#define DFT_NAME  "PDD.SYS"         /* name of PDD              */

#define StatusReg  0X2d0            /* for now ???? status reg addr   */
#define FirstReg   0x2d1            /* first non status reg to monitor*/

#define numPorts   10               /* number of ports to hook  */
#define numstat    1                /* number of status ports   */
#define DFT_IRQ    9                /* IRQ for DFT adapter      */

#define ENABLE_INTS     1           /* function code for enable  */
#define DISABLE_INTS    2           /* function code for disable  */

/***    VDFT data types  ***/

typedef USHORT VDFTPDB;             /* define PDB type */


/***    VDFT function prototypes    ***/

BYTE HOOKENTRY VDFTDaIn( ULONG , PCRF );
BYTE HOOKENTRY VDFTStIn( ULONG , PCRF );

VOID HOOKENTRY VDFTDaOut( BYTE, ULONG, PCRF );
VOID HOOKENTRY VDFTStOut( BYTE, ULONG, PCRF );

BOOL HOOKENTRY VDFTCreate(HVDM);
BOOL HOOKENTRY VDFTTerminate(HVDM);
BOOL HOOKENTRY VDFTPDBChange(HVDM, VDFTPDB);
BOOL HOOKENTRY VDFTPDBDestroy(HVDM, VDFTPDB);

BOOL PRIVENTRY RequestDirect();

SBOOL VDDENTRY VDFTPDDProc(ULONG, F16PVOID, F16PVOID );
VOID HOOKENTRY VDFT_EOI(PCRF);
VOID HOOKENTRY VDFT_IRET (PCRF);

BOOL EXPENTRY Start_Here(PSZ);

BOOL PRIVENTRY RequestDirect( VOID );
BOOL PRIVENTRY EnableInterrupts(VOID );
BOOL PRIVENTRY DisableInterrupts(VOID );
