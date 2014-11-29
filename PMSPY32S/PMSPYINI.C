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
/*³ PMSPYINI.C                                                               ³*/
/*³                                                                          ³*/
/*³ Routines to Read/Write PMSPY.INI                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Includes                                                                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#include "pmspy.h"                      /* Resource symbolic identifiers*/
#include "pmspyINI.h"                   /* our interface definitions    */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ PMSPY INI file data items                                                ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
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

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Write Agent's PMSPY data to INI file                                     ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL WriteSpyINI(HWND      hFrame,   /* HWND(Agent's frame) */
                 LONG      Agent,    /* one-origin "agent" */
                 PINI_DATA pIni)     /* INI data to write */
{
  CHAR szAgent[32],
       szData[128];

  HINI hINI          = NULLH;
  BOOL bOK           = BOOL_FALSE;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Open the PMSPY "profile"                                               ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  if ( (hINI = PrfOpenProfile( WinQueryAnchorBlock(hFrame),
                               szINI)) != NULLH )
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Determine which "agent" this data is for                             ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    sprintf(szAgent, Strings[IDS_FMT_AGENT], Agent);

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Write Agent's FONT to the PMSPY "profile"                            ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    PrfWriteProfileString(hINI, szAgent, szKeyFont, pIni->szListFont);

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Write Agent's LOG to the PMSPY "profile"                             ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    PrfWriteProfileString(hINI, szAgent, szKeyLog, pIni->szLog);

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Write Agent's PROFILE to the PMSPY "profile"                         ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    PrfWriteProfileString(hINI, szAgent, szKeyPro, pIni->szProfile);

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Write Agent's WINDOW POSITION to the PMSPY "profile"                 ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
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

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Close the PMSPY "profile"                                            ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    PrfCloseProfile(hINI);

    bOK = BOOL_TRUE;
  }

  return( bOK );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Read Agent's PMSPY data from INI file (or set to default values)         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL ReadSpyINI(HWND      hFrame,   /* HWND(Agent's frame) */
                LONG      Agent,    /* one-origin "agent" */
                PINI_DATA pIni)     /* INI data to read */
{
  CHAR szAgent[32];

  HINI hINI          = NULLH;
  BOOL bOK           = BOOL_FALSE;

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Set the INI data to 'default' values...                                ³*/
  /*³                                                                        ³*/
  /*³                                                                        ³*/
  /*³ - list font                                                            ³*/
  /*³                                                                        ³*/
  /*³ - window position:  x  = left edge                                     ³*/
  /*³                     y  = above bottom row of icons                     ³*/
  /*³                     cX = 6/10 screen width                             ³*/
  /*³                     cY = 4/10 screen height                            ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
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

  /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
  /*³ Open the PMSPY "profile"                                               ³*/
  /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
  if ( (hINI = PrfOpenProfile( WinQueryAnchorBlock(hFrame),
                               szINI)) != NULLH )
  {
    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Determine which "agent" this data is for                             ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    sprintf(szAgent, Strings[IDS_FMT_AGENT], Agent);

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Read Agent's previous FONT                                           ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    PrfQueryProfileString(hINI,
                          szAgent,
                          szKeyFont,
                          pIni->szListFont,           /* default */
                          pIni->szListFont,           /* target & size */
                          (LONG)sizeof(pIni->szListFont) );

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Read Agent's previous LOG                                            ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    PrfQueryProfileString(hINI,
                          szAgent,
                          szKeyLog,
                          pIni->szLog,                /* default */
                          pIni->szLog,                /* target & size */
                          (LONG)sizeof(pIni->szLog) );

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Read Agent's previous PROfile                                        ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    PrfQueryProfileString(hINI,
                          szAgent,
                          szKeyPro,
                          pIni->szProfile,            /* default */
                          pIni->szProfile,            /* target & size */
                          (LONG)sizeof(pIni->szProfile) );

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Read Agent's previous WINDOW POSITION                                ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
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

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³ Close the PMSPY "profile"                                            ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
    PrfCloseProfile(hINI);

    bOK = BOOL_TRUE;
  }

  return( bOK );
}
