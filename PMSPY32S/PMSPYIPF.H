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
/*� PMSPYIPF.H                                                               �*/
/*�                                                                          �*/
/*� include for non-RC IPF panel definitions                                 �*/
/*�                                                                          �*/
/*�                                                                          �*/
/*� Notes:                                                                   �*/
/*� ------                                                                   �*/
/*� - for most IPF panel ID's, i've tried to use the PM resource ID          �*/
/*�   * really no need to generate another ID for IPF if we insure that      �*/
/*�     there is no overlap in the PM resource ID's                          �*/
/*�                                                                          �*/
/*� - we do define some IPF panel ID's; there are typically panels used      �*/
/*�   to support things not directly help requestable via F1 or a HELP button�*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
#define IPF_EMPTY_SUBTABLE    20000

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� HELP action bar Panel IDs                                                �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
#define PID_EXTHELP           20100        /* Extended help */
#define PID_KEYSHELP          20101        /* keys help */
#define PID_HELPHELP          20102        /* help for help */

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� Panel ID's for things which don't have a PM resource ID                  �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
#define PID_LIMITATIONS       20200
#define PID_HOW_IT_WORKS      20201
#define PID_AUTHORS           20202
#define PID_HOW_TO_INSTALL    20203
#define PID_DEFAULT_PROFILE   20204
#define PID_SPYLIST           20205
#define PID_SAVE              20206
#define PID_TRIGGER           20207

#define PID_HOWTO             20220
#define PID_HOWTO_DIALOGS     20221

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/
/*� Reference Panel IDs                                                      �*/
/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/
#define PID_R_PROFILE         20500
#define PID_R_CMDLINE         20501
#define PID_R_LOG_FILE        20502
