/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*�                                                                          �*/
/*� PROGRAM NAME: PMSPY                                                      �*/
/*� -------------                                                            �*/
/*�  A PM program that is used to look at or 'spy' on the message queue of   �*/
/*�  other PM applications windows.                                          �*/
/*�                                                                          �*/
/*� COPYRIGHT:                                                               �*/
/*� ----------                                                               �*/
/*�  Copyright (C) International Business Machines Corp., 1992               �*/
/*�                                                                          �*/
/*� DISCLAIMER OF WARRANTIES:                                                �*/
/*� -------------------------                                                �*/
/*�  The following [enclosed] code is sample code created by IBM Corporation.�*/
/*�  This sample code is not part of any standard IBM product and is provided�*/
/*�  to you solely for the purpose of assisting you in the development of    �*/
/*�  your applications.  The code is provided "AS IS", without warranty of   �*/
/*�  any kind.  IBM shall not be liable for any damages arising out of your  �*/
/*�  use of the sample code, even if they have been advised of the           �*/
/*�  possibility of such damages.                                            �*/
/*�                                                                          �*/
/*� For details on what this program does etc., please see the PMSPY.C file. �*/
/*�                                                                          �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� PMSPYHK.H                                                                �*/
/*�                                                                          �*/
/*� "Hook" DLL include file                                                  �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� PMSPY DLL version definitions                                            �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
#define DLLVERSION      110            /* 1.10 */

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� Typedefs                                                                 �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
typedef ULONG          MSG,            /* PM message                          */
                     *PMSG;            /* pointer to a PM message             */

typedef USHORT        SPYEE;           /* PMSPY instance ID                   */

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� Macros                                                                   �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
#define SPY_BAD_SPYEE     ((SPYEE) -1) /* unable to Register another SPYer */
#define SPY_BAD_HWND      ((HWND)  -1) /* invalid HWND value */
#define SPY_BAD_HMQ       ((HMQ)   -1) /* invalid HMQ  value */
#define SPY_BAD_HAB       ((HAB)   -1) /* invalid HAB  value */

#define BOOL_TRUE         ((BOOL) TRUE)
#define BOOL_FALSE        ((BOOL) FALSE)

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� Constants                                                                �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
#define SPY_HOOK_DLL      "PMSPYHK"    /* DLL name with "Hook" routines */

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� Routines for adding/removing PMSPY instances                             �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
SPYEE  SpyRegister           (HWND    hwndSpy,  /* window to handle SPY data */
                              MSG     MsgSpy);  /* message to SEND to window */

BOOL   SpyDeRegister         (SPYEE idSpy);     /* SPY instance */

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� Routines for dealing with a specific PMSPY instance                      �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
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

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� Routines that return PMSPY DLL information                               �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
USHORT SpyDllVersion(VOID);

USHORT SpyQueryDllRegisterMax(VOID);
