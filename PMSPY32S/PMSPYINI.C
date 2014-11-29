/*��������������������������������������������������������������������������Ŀ*/
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
/*����������������������������������������������������������������������������*/

/*��������������������������������������������������������������������������Ŀ*/
/*� PMSPYINI.C                                                               �*/
/*�                                                                          �*/
/*� Routines to Read/Write PMSPY.INI                                         �*/
/*����������������������������������������������������������������������������*/

/*��������������������������������������������������������������������������Ŀ*/
/*� Includes                                                                 �*/
/*����������������������������������������������������������������������������*/
#include "pmspy.h"                      /* Resource symbolic identifiers*/
#include "pmspyINI.h"                   /* our interface definitions    */

/*��������������������������������������������������������������������������Ŀ*/
/*� PMSPY INI file data items                                                �*/
/*����������������������������������������������������������������������������*/
static CHAR szINI[]            = "PMSPY.INI",
            szAPPL[]           = "PMSPY",

            /**** Defaults ****/

            szDefFont[]        = "10.System Monospaced",
            szDefProfile[]     = "*.PRO",
            szDefLog[]         = "*.LOG",

            /**** INI file "keys" ****/

            szKeyFont[]        = "FONT",
            szKeyLog[]         = "LOG",
            szKeyPro[]         = "PROFILE",
            szKeyPosX[]        = "POS_X",
            szKeyPosY[]        = "POS_Y",
            szKeySizeCX[]      = "SIZE_CX",
            szKeySizeCY[]      = "SIZE_CY";

/*��������������������������������������������������������������������������Ŀ*/
/*� Write Agent's PMSPY data to INI file                                     �*/
/*����������������������������������������������������������������������������*/
BOOL WriteSpyINI(HWND      hFrame,   /* HWND(Agent's frame) */
                 LONG      Agent,    /* one-origin "agent" */
                 PINI_DATA pIni)     /* INI data to write */
{
  CHAR szAgent[32],
       szData[128];

  HINI hINI          = NULLH;
  BOOL bOK           = BOOL_FALSE;

  /*������������������������������������������������������������������������Ŀ*/
  /*� Open the PMSPY "profile"                                               �*/
  /*��������������������������������������������������������������������������*/
  if ( (hINI = PrfOpenProfile( WinQueryAnchorBlock(hFrame),
                               szINI)) != NULLH )
  {
    /*����������������������������������������������������������������������Ŀ*/
    /*� Determine which "agent" this data is for                             �*/
    /*������������������������������������������������������������������������*/
    sprintf(szAgent, Strings[IDS_FMT_AGENT], Agent);

    /*����������������������������������������������������������������������Ŀ*/
    /*� Write Agent's FONT to the PMSPY "profile"                            �*/
    /*������������������������������������������������������������������������*/
    PrfWriteProfileString(hINI, szAgent, szKeyFont, pIni->szListFont);

    /*����������������������������������������������������������������������Ŀ*/
    /*� Write Agent's LOG to the PMSPY "profile"                             �*/
    /*������������������������������������������������������������������������*/
    PrfWriteProfileString(hINI, szAgent, szKeyLog, pIni->szLog);

    /*����������������������������������������������������������������������Ŀ*/
    /*� Write Agent's PROFILE to the PMSPY "profile"                         �*/
    /*������������������������������������������������������������������������*/
    PrfWriteProfileString(hINI, szAgent, szKeyPro, pIni->szProfile);

    /*����������������������������������������������������������������������Ŀ*/
    /*� Write Agent's WINDOW POSITION to the PMSPY "profile"                 �*/
    /*������������������������������������������������������������������������*/
    if ( WinQueryWindowPos(hFrame, &pIni->swpAgent) )
    {
       sprintf(szData, "%d", pIni->swpAgent.x);
       PrfWriteProfileString(hINI, szAgent, szKeyPosX,   szData);

       sprintf(szData, "%d", pIni->swpAgent.y);
       PrfWriteProfileString(hINI, szAgent, szKeyPosY,   szData);

       sprintf(szData, "%d", pIni->swpAgent.cx);
       PrfWriteProfileString(hINI, szAgent, szKeySizeCX, szData);

       sprintf(szData, "%d", pIni->swpAgent.cy);
       PrfWriteProfileString(hINI, szAgent, szKeySizeCY, szData);
    }

    /*����������������������������������������������������������������������Ŀ*/
    /*� Close the PMSPY "profile"                                            �*/
    /*������������������������������������������������������������������������*/
    PrfCloseProfile(hINI);

    bOK = BOOL_TRUE;
  }

  return( bOK );
}

/*��������������������������������������������������������������������������Ŀ*/
/*� Read Agent's PMSPY data from INI file (or set to default values)         �*/
/*����������������������������������������������������������������������������*/
BOOL ReadSpyINI(HWND      hFrame,   /* HWND(Agent's frame) */
                LONG      Agent,    /* one-origin "agent" */
                PINI_DATA pIni)     /* INI data to read */
{
  CHAR szAgent[32];

  HINI hINI          = NULLH;
  BOOL bOK           = BOOL_FALSE;

  /*������������������������������������������������������������������������Ŀ*/
  /*� Set the INI data to 'default' values...                                �*/
  /*�                                                                        �*/
  /*�                                                                        �*/
  /*� - list font                                                            �*/
  /*�                                                                        �*/
  /*� - window position:  x  = left edge                                     �*/
  /*�                     y  = above bottom row of icons                     �*/
  /*�                     cX = 6/10 screen width                             �*/
  /*�                     cY = 4/10 screen height                            �*/
  /*��������������������������������������������������������������������������*/
  memset(pIni, 0, sizeof(*pIni) );

  strcpy(pIni->szListFont, szDefFont);
  strcpy(pIni->szLog,      szDefLog);
  strcpy(pIni->szProfile,  szDefProfile);

  pIni->swpAgent.x  = 0;

  pIni->swpAgent.y  = (SHORT)
                      (WinQuerySysValue(HWND_DESKTOP, SV_CYICON)      *2);

  pIni->swpAgent.cx = (SHORT)
                      (WinQuerySysValue(HWND_DESKTOP, SV_CXFULLSCREEN)*6)/10;

  pIni->swpAgent.cy = (SHORT)
                      (WinQuerySysValue(HWND_DESKTOP, SV_CYFULLSCREEN)*4)/10;

  /*������������������������������������������������������������������������Ŀ*/
  /*� Open the PMSPY "profile"                                               �*/
  /*��������������������������������������������������������������������������*/
  if ( (hINI = PrfOpenProfile( WinQueryAnchorBlock(hFrame),
                               szINI)) != NULLH )
  {
    /*����������������������������������������������������������������������Ŀ*/
    /*� Determine which "agent" this data is for                             �*/
    /*������������������������������������������������������������������������*/
    sprintf(szAgent, Strings[IDS_FMT_AGENT], Agent);

    /*����������������������������������������������������������������������Ŀ*/
    /*� Read Agent's previous FONT                                           �*/
    /*������������������������������������������������������������������������*/
    PrfQueryProfileString(hINI,
                          szAgent,
                          szKeyFont,
                          pIni->szListFont,           /* default */
                          pIni->szListFont,           /* target & size */
                          (LONG)sizeof(pIni->szListFont) );

    /*����������������������������������������������������������������������Ŀ*/
    /*� Read Agent's previous LOG                                            �*/
    /*������������������������������������������������������������������������*/
    PrfQueryProfileString(hINI,
                          szAgent,
                          szKeyLog,
                          pIni->szLog,                /* default */
                          pIni->szLog,                /* target & size */
                          (LONG)sizeof(pIni->szLog) );

    /*����������������������������������������������������������������������Ŀ*/
    /*� Read Agent's previous PROfile                                        �*/
    /*������������������������������������������������������������������������*/
    PrfQueryProfileString(hINI,
                          szAgent,
                          szKeyPro,
                          pIni->szProfile,            /* default */
                          pIni->szProfile,            /* target & size */
                          (LONG)sizeof(pIni->szProfile) );

    /*����������������������������������������������������������������������Ŀ*/
    /*� Read Agent's previous WINDOW POSITION                                �*/
    /*������������������������������������������������������������������������*/
    pIni->swpAgent.x  = PrfQueryProfileInt(hINI,
                                           szAgent,
                                           szKeyPosX,
                                           pIni->swpAgent.x);   /* default */

    pIni->swpAgent.y  = PrfQueryProfileInt(hINI,
                                           szAgent,
                                           szKeyPosY,

                                           pIni->swpAgent.y);   /* default */
    pIni->swpAgent.cx = PrfQueryProfileInt(hINI,
                                           szAgent,
                                           szKeySizeCX,
                                           pIni->swpAgent.cx);  /* default */

    pIni->swpAgent.cy = PrfQueryProfileInt(hINI,
                                           szAgent,
                                           szKeySizeCY,
                                           pIni->swpAgent.cy);  /* default */

    /*����������������������������������������������������������������������Ŀ*/
    /*� Close the PMSPY "profile"                                            �*/
    /*������������������������������������������������������������������������*/
    PrfCloseProfile(hINI);

    bOK = BOOL_TRUE;
  }

  return( bOK );
}
