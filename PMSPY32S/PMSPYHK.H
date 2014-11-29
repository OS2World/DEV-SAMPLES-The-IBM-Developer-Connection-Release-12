/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                                          ³*/
/*³ PROGRAM NAME: PMSPY                                                      ³*/
/*³ -------------                                                            ³*/
/*³  A PM program that is used to look at or 'spy' on the message queue of   ³*/
/*³  other PM applications windows.                                          ³*/
/*³                                                                          ³*/
/*³ COPYRIGHT:                                                               ³*/
/*³ ----------                                                               ³*/
/*³  Copyright (C) International Business Machines Corp., 1992               ³*/
/*³                                                                          ³*/
/*³ DISCLAIMER OF WARRANTIES:                                                ³*/
/*³ -------------------------                                                ³*/
/*³  The following [enclosed] code is sample code created by IBM Corporation.³*/
/*³  This sample code is not part of any standard IBM product and is provided³*/
/*³  to you solely for the purpose of assisting you in the development of    ³*/
/*³  your applications.  The code is provided "AS IS", without warranty of   ³*/
/*³  any kind.  IBM shall not be liable for any damages arising out of your  ³*/
/*³  use of the sample code, even if they have been advised of the           ³*/
/*³  possibility of such damages.                                            ³*/
/*³                                                                          ³*/
/*³ For details on what this program does etc., please see the PMSPY.C file. ³*/
/*³                                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ PMSPYHK.H                                                                ³*/
/*³                                                                          ³*/
/*³ "Hook" DLL include file                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ PMSPY DLL version definitions                                            ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define DLLVERSION      110            /* 1.10 */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Typedefs                                                                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
typedef ULONG          MSG,            /* PM message                          */
                     *PMSG;            /* pointer to a PM message             */

typedef USHORT        SPYEE;           /* PMSPY instance ID                   */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Macros                                                                   ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define SPY_BAD_SPYEE     ((SPYEE) -1) /* unable to Register another SPYer */
#define SPY_BAD_HWND      ((HWND)  -1) /* invalid HWND value */
#define SPY_BAD_HMQ       ((HMQ)   -1) /* invalid HMQ  value */
#define SPY_BAD_HAB       ((HAB)   -1) /* invalid HAB  value */

#define BOOL_TRUE         ((BOOL) TRUE)
#define BOOL_FALSE        ((BOOL) FALSE)

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Constants                                                                ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#define SPY_HOOK_DLL      "PMSPYHK"    /* DLL name with "Hook" routines */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routines for adding/removing PMSPY instances                             ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
SPYEE  SpyRegister           (HWND    hwndSpy,  /* window to handle SPY data */
                              MSG     MsgSpy);  /* message to SEND to window */

BOOL   SpyDeRegister         (SPYEE idSpy);     /* SPY instance */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routines for dealing with a specific PMSPY instance                      ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL   SpySetTarget          (SPYEE idSpy,      /* SPY instance */
                              HWND  hwndTarget, /* Window to SPY */
                              HMQ   hmqTarget); /* Queue  to SPY */

BOOL   SpyUnSetTarget        (SPYEE idSpy);     /* SPY instance */

HWND   SpyQueryTargetWindow  (SPYEE idSpy);     /* SPY instance */

HMQ    SpyQueryTargetQueue   (SPYEE idSpy);     /* SPY instance */

BOOL   SpySetTargetIsWindow  (SPYEE idSpy,      /* SPY instance */
                              BOOL  fTargetIsWindow); /* SPYing on window? */

BOOL   SpyQueryTargetIsWindow(SPYEE idSpy);   /* SPY instance */

BOOL   SpySetTargetMsgRange  (SPYEE idSpy,      /*SPY instance */
                              MSG   msgLow,     /* lowest  MSG in range */
                              MSG   msgHigh);   /* highest MSG in range */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Routines that return PMSPY DLL information                               ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
USHORT SpyDllVersion(VOID);

USHORT SpyQueryDllRegisterMax(VOID);
