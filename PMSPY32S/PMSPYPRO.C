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
/*³ PMSPYPRO.C                                                               ³*/
/*³                                                                          ³*/
/*³ profile open / save procedures                                           ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Includes                                                                 ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
#include "pmspy.h"                      /* Resource symbolic identifiers      */
#include "pmspyINI.h"                   /* PMSPY.INI definitions              */

#include <ctype.h>

extern INI_DATA iniAgent;          /* we import this variable from pmspywin.c */

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ local variables                                                          ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
typedef enum {  kwError,                /* Profile keywords                   */
                kwComment,

                kwX,
                kwY,
                kwCX,
                kwCY,
                kwSPYON,
                kwLIST,
                kwFONT,
                kwINCLUDE,
                kwEXCLUDE,
                kwTRIGGER,
                kwCOLOR,
                kwDEFINE,
                kwGROUP,

                kwTOTAL

             } kwPROFILE;

typedef enum {  tokKW,                  /* line tokens                        */

                tokParm1,
                tokParm2,
                tokParm3,
                tokParm4,
                tokParm5,

                tokTOTAL,

                tokLAST = tokTOTAL - 1

             } tokPROFILE;

static PSZ NullString = "";

static WORD_LIST KeyWords[] = {
                                { &Strings[IDS_PRO_KW_COLOR],    kwCOLOR  },
                                { &Strings[IDS_PRO_KW_DEFINE],   kwDEFINE },
                                { &Strings[IDS_PRO_KW_EXCLUDE],  kwEXCLUDE},
                                { &Strings[IDS_PRO_KW_GROUP],    kwGROUP  },
                                { &Strings[IDS_PRO_KW_INCLUDE],  kwINCLUDE},
                                { &Strings[IDS_PRO_KW_TRIGGER],  kwTRIGGER},
                                { &Strings[IDS_PRO_KW_X],        kwX      },
                                { &Strings[IDS_PRO_KW_Y],        kwY      },
                                { &Strings[IDS_PRO_KW_CX],       kwCX     },
                                { &Strings[IDS_PRO_KW_CY],       kwCY     },
                                { &Strings[IDS_PRO_KW_SPYON],    kwSPYON  },
                                { &Strings[IDS_PRO_KW_LIST],     kwLIST   },
                                { &Strings[IDS_PRO_KW_FONT],     kwFONT   },
                                { &Strings[IDS_PRO_KW_COMMENT1], kwComment},
                                { &Strings[IDS_PRO_KW_COMMENT2], kwComment},
                                { &NullString,                   kwComment},

                                { NULL,                          kwError  }
                              };


static PSZ Tokens[tokTOTAL];

#define NoGroupID   0xFFFF


/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Parse Color Index                                                        ³*/
/*³ INPUT  : - pszToken    : token to use for <color>                        ³*/
/*³          - theDefault  : default color if no value specifed              ³*/
/*³                                                                          ³*/
/*³ OUTPUT : Color_Error if error detected                                   ³*/
/*³          otherwise, valid ColorX value (Color1..Color8) -or- 'default'   ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

static MSG_COLOR ParseColorIndex( PSZ       pszToken,
                                  MSG_COLOR theDefault)

{
  register MSG_COLOR logColor = Color_Error;     /* start a pessimist...      */


  do
  {
    if ( pszToken == NullString )
    {
      logColor = theDefault;

      break;
    }

    logColor = (MSG_COLOR) atoi(pszToken);

    if ( (logColor < Color_First) || (logColor > Color_Last) )
      break;             /* ignore if bad color index */

    logColor--;          /* convert from 1-origin to 0-origin */

  } while( TRUE == FALSE );

  return( logColor );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Parse Message command                                                    ³*/
/*³                                                                          ³*/
/*³ Format : Include/Exclude  = <id>                                         ³*/
/*³                                                                          ³*/
/*³                   <id>    = specific message name (like WM_USER, etc)    ³*/
/*³                           = specific group   name (like XXXX )           ³*/
/*³                           = All                                          ³*/
/*³                                                                          ³*/
/*³ Notes  : - the 'verb' has already been parsed off for us...              ³*/
/*³                                                                          ³*/
/*³ INPUT  : - isInclude  : T=Include, F=Exclude                             ³*/
/*³                                                                          ³*/
/*³ OUTPUT : TRUE if succesfull                                              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

static BOOL ParseMsgCmd( PSPY_DATA pSpy,
                         BOOL      Include)
{
           BOOL      isOK = BOOL_FALSE;      /* start as a pessimist               */
  register PMSG_ITEM pMsg;
  register PGRP_ITEM pGrp;

  do
  {
    /* process the <id> value */

    if ( Tokens[tokParm1] == NullString )
      break;

    /* check the <id> value */

    if (strcmp(Tokens[tokParm1], Strings[IDS_PRO_VALUE_ALL]) == 0)
    {
      IncludeAllMsgs( pSpy, Include, Color_Asis );
      isOK = BOOL_TRUE;
    }
    else
    {
      /* check if a specific message */

      if ( (pMsg = FindMsgName(pSpy, Tokens[tokParm1])) != NULL )
      {
        IncludeMsg(pSpy, pMsg->Msg, Include, Color_Asis);
        isOK = BOOL_TRUE;
        break;
      }

      /* check if a specific group */

      if ( (pGrp = FindGroupName(pSpy, Tokens[tokParm1])) != NULL )
      {
        /* process all messages in the GROUP */

        IncludeGroup(pSpy, pGrp, Include, Color_Asis);

        isOK = BOOL_TRUE;
        break;
      }

    } /* endif */

  } while( TRUE == FALSE );

  return( isOK );
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Tokenize the Input line                                                  ³*/
/*³ Tokens may be delimted by double quotes to include blanks, addition of   ³*/
/*³ numeric tokens is supported in this routine                              ³*/
/*³                                                                          ³*/
/*³ Example: input:  DEFINE "MY_MSG1"   0x1000 + 5      6                    ³*/
/*³          output: Tokens[0] = DEFINE                                      ³*/
/*³                  Tokens[1] = MY_MSG1                                     ³*/
/*³                  Tokens[2] = 0x1005                                      ³*/
/*³                  Tokens[3] = 6                                           ³*/
/*³                                                                          ³*/
/*³ INPUT  : - pszLine: pointer to the line                                  ³*/
/*³ OUTPUT : TRUE if succesfull                                              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
static VOID TokenizeLine(PSZ pszLine)
{
  INT i,n1,n2;
  for(/* Init */ i = 0;
      /* Term */ i <= tokLAST;
      /* Iter */ i++)
  {
    /* skip delimeters */
    while (*pszLine && (*pszLine == Strings[IDS_PRO_DELIM][0])) pszLine++;
    switch (*pszLine) {
      case 0:       /* no more tokens */
        Tokens[i] = NullString;
        break;
      case '"':     /* remove double quotes and find end of token */
        pszLine++;
        Tokens[i] = pszLine;
        while (*pszLine && (*pszLine != '"')) pszLine++;
        if (*pszLine == '"') {
          *pszLine = '\0';
          pszLine++;
        } /* endif */
        break;
      case '0':     /* is it an additional numeric expression ? */
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        Tokens[i] = pszLine;
        while (*pszLine && (*pszLine != Strings[IDS_PRO_DELIM][0])) pszLine++;
        if (*pszLine == Strings[IDS_PRO_DELIM][0]) {
          *pszLine = '\0';
          pszLine++;
        } /* endif */
        /* is the next character a '+' sign ? */
        while (*pszLine && (*pszLine == Strings[IDS_PRO_DELIM][0])) pszLine++;
        if (*pszLine == '+') {
          pszLine++;
          while (*pszLine && (*pszLine == Strings[IDS_PRO_DELIM][0])) pszLine++;
          sscanf(Tokens[i], "%i", &n1);
          sscanf(pszLine, "%i", &n2);
          while (*pszLine && (*pszLine != Strings[IDS_PRO_DELIM][0])) pszLine++;
          if (*pszLine == Strings[IDS_PRO_DELIM][0]) {
            *pszLine = '\0';
            pszLine++;
          } /* endif */
          sprintf(Tokens[i], "%#4.4X", n1+n2);
        } /* endif */
        break;
      default: /* a normal delimited token */
        Tokens[i] = pszLine;
        while (*pszLine && (*pszLine != Strings[IDS_PRO_DELIM][0])) pszLine++;
        if (*pszLine == Strings[IDS_PRO_DELIM][0]) {
          *pszLine = '\0';
          pszLine++;
        } /* endif */
    } /* endswitch */
  } /* endfor */
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Open the Profile                                                         ³*/
/*³ INPUT  : - hwndCur  : window handle of the client                        ³*/
/*³          - hwndFrame: window handle of the frame                         ³*/
/*³          - szFile   : filename                                           ³*/
/*³                     ===> if NULL, open the 'standard' PROFILE            ³*/
/*³ OUTPUT : TRUE if succesfull                                              ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL OpenProfile(HWND hwndCur,
                 HWND hwndFrame,
                 PSPY_DATA pSpy,
                 PSZ szFile)
{
  static PDTR_DATA  defProfile = {
                                   0,            /* resource location (.EXE) */
                                   IDT_HELP,     /* resource TYPE */
                                   IDT_DEFAULTS  /* resource ID */
                                 };

  FILE            *pProFile = NULL;
  CHAR            szLine[256];
  PSZ             psz;
  SWP             swp;
  register USHORT logColor, i;

  MSG_ITEM        proMsg;
  GRP_ITEM        proGrp;
  USHORT          usWork;

  /**************************************************************************
  * Set up default MSG values
  ***************************************************************************/
  memset(&proMsg, 0, sizeof(proMsg) );    /* set to all zeros                 */

  proMsg.Include = BOOL_TRUE;             /* include msg?                     */
  proMsg.ClrFG   = Color_Default;         /* logical FG color to DRAW         */
  proMsg.GroupID = NoGroupID;             /* GROUP this msg defined in        */

  proMsg.fMP1    = MP_Default;            /* MP1 Format                       */
  proMsg.fMP2    = MP_Default;            /* MP2 Format                       */

  /**************************************************************************
  * Set up default GRP values
  ***************************************************************************/
  memset(&proGrp, 0, sizeof(proGrp) );    /* set to all zeros                 */

  proGrp.GroupID = NoGroupID;             /* This GROUP's ID                  */
  proGrp.Include = BOOL_TRUE;             /* include GROUP?                   */
  proGrp.ClrFG   = Color_Default;         /* logical FG color to DRAW         */

  /**************************************************************************
  * Open the specified PROFILE
  ***************************************************************************/
  if ( szFile == NULL )
  {
    defProfile.hModule = hmodNonNLS;
    if ( OpenPDTR(&defProfile) == NULL)
    return(BOOL_FALSE);
  }
  else
  {
    if ( (pProFile = fopen(szFile, "r")) == NULL)
      return(BOOL_FALSE);
  }

  /**************************************************************************
  * Intialize the postional parameters
  ***************************************************************************/
  WinQueryWindowPos(hwndFrame, &swp);

  /**************************************************************************
  * Process each line until EOF...
  ***************************************************************************/
  while ( (szFile == NULL ? GetsPDTR(&defProfile, szLine, sizeof(szLine))
                          : fgets(szLine, sizeof(szLine), pProFile)) != NULL)
  {
    /* replace NEWLINE with a NULL */
    if (psz=strchr(szLine,'\n'))
      *psz = 0;

    /* Tokenize all parts of this line */
    TokenizeLine(szLine);

    /* Process this line's keyword */
    switch(FindWordInList(Tokens[tokKW], &KeyWords[0]))
    {
     case kwX:
          iniAgent.swpAgent.x  = swp.x  = atoi(Tokens[tokParm1]);
          break;
     case kwY:
          iniAgent.swpAgent.y  = swp.y  = atoi(Tokens[tokParm1]);
          break;
     case kwCX:
          iniAgent.swpAgent.cx = swp.cx = atoi(Tokens[tokParm1]);
          break;
     case kwCY:
          iniAgent.swpAgent.cy = swp.cy = atoi(Tokens[tokParm1]);
          break;
     case kwSPYON:
          if (strcmp(Tokens[tokParm1], Strings[IDS_PRO_KW_WINDOW])==0) {
            WinSendMsg(hwndCur, WM_COMMAND, MPFROMSHORT(IDD_WND), NULL);
          } else if (strcmp(Tokens[tokParm1], Strings[IDS_PRO_KW_QUEUE])==0) {
            WinSendMsg(hwndCur, WM_COMMAND, MPFROMSHORT(IDD_QUE), NULL);
          } /* endif */
          break;
     case kwLIST:
          if (strcmp(Tokens[tokParm1], Strings[IDS_PRO_KW_THAW])==0) {
            WinSendMsg(hwndCur, WM_COMMAND, MPFROMSHORT(ID_O_THAW), NULL);
          } else if (strcmp(Tokens[tokParm1], Strings[IDS_PRO_KW_FREEZE])==0) {
            WinSendMsg(hwndCur, WM_COMMAND, MPFROMSHORT(ID_O_FREEZE), NULL);
          } /* endif */
          break;
     case kwFONT:
          if (Tokens[tokParm1][0]) {
            FONT_DATA FData;
            FData.pfnSelected = (PFONTNAME)Tokens[tokParm1];
            FData.fUseData = BOOL_TRUE;
            WinSendMsg(hwndCur, PMSPY_FONT_RESULT, MPFROMP(&FData), NULL);
          } /* endif */
          break;

     case kwINCLUDE:
          ParseMsgCmd(pSpy, BOOL_TRUE);
          break;

     case kwEXCLUDE:
          ParseMsgCmd(pSpy, BOOL_FALSE);
          break;

     case kwTRIGGER:
          /* process: Trigger msg_name <thaw>|<freeze> */
          { register PMSG_ITEM pMsg;
            if ((pMsg = FindMsgName(pSpy, Tokens[tokParm1])) != NULL)
            {
              if (strcmp(Tokens[tokParm2], Strings[IDS_PRO_KW_THAW]) == 0)
              {
                pMsg->TriggerThaw   = BOOL_TRUE;
                pMsg->TriggerFreeze = BOOL_FALSE;
              }
              else if (strcmp(Tokens[tokParm2], Strings[IDS_PRO_KW_FREEZE]) == 0)
              {
                pMsg->TriggerFreeze = BOOL_TRUE;
                pMsg->TriggerThaw   = BOOL_FALSE;
              } /* endif */
            } /* endif */
          }
     break;

     case kwCOLOR:
          /* process: Color <index> <value> */

          do
          {
            if ( (logColor=ParseColorIndex(Tokens[tokParm1],
                                           Color_Error)) == Color_Error )
              break;             /* ignore if bad <color index> */

            if ( Tokens[tokParm2] == NullString )
              break;             /* ignore if bad <value> */

            /* look up PROFILE color name in table */

            for(i = 0;
                ( (ExternalColorTranslation[i].pszClrName != NULL) &&
                  (strcmp(ExternalColorTranslation[i].pszClrName, Tokens[tokParm2]) != 0));
                i++);

            if ( ExternalColorTranslation[i].pszClrName == NULL)
              break;

            pSpy->LogicalColorTranslation[logColor].iExtColor = i;

          } while(TRUE == FALSE);
          break;

     case kwDEFINE:

          /* process: Define <msg desc> <msg no> [<color index>] */

          do
          {
            /* Process <msg desc> */

            if (Tokens[tokParm1] == NullString)
              break;

            /* Process <msg no> */

            if ( !ConvertToNumber(Tokens[tokParm2], &usWork, 0, 0xFFFF) )
              break;

            proMsg.Msg = (MSG)usWork;

            /* Process <color index> (optional) */

            if ( (proMsg.ClrFG=ParseColorIndex(Tokens[tokParm3],
                                               proGrp.ClrFG)) == Color_Error )
              break;             /* ignore if bad <color index> */

            /* Add new message */

            proMsg.pDesc = AllocateMemory( strlen(Tokens[tokParm1]) + 1);
            strcpy(proMsg.pDesc, Tokens[tokParm1]);

            proMsg.TriggerThaw =
            proMsg.TriggerFreeze = BOOL_FALSE;

            AddMsg(pSpy, &proMsg);

          } while(TRUE == FALSE);
          break;

     case kwGROUP:

          /* process: Group <Group desc> [<color index>] */

          do
          {

            /* Process <Group desc> */

            if (Tokens[tokParm1] == NullString)
              break;

            /* Process <color index> (optional) */

            if ( (proGrp.ClrFG=ParseColorIndex(Tokens[tokParm2],
                                               Color_Default)) == Color_Error )
              break;             /* ignore if bad <color index> */

            /* Add new GROUP */

            proGrp.pDesc = AllocateMemory( strlen(Tokens[tokParm1]) + 1);
            strcpy(proGrp.pDesc, Tokens[tokParm1]);

            proMsg.GroupID = AddGroup(pSpy, &proGrp);  /* all MSGS to this GROUP now... */

          } while(TRUE == FALSE);
          break;

     case kwComment:
          break;

     default:
          break;
    } /* switch on KW values */
  } /* endwhile */

  /* Close the source of the PROFILE data */

  if (szFile == NULL)
    ClosePDTR(&defProfile);
  else
    fclose(pProFile);

  WinSetWindowPos(hwndFrame, HWND_TOP, swp.x, swp.y, swp.cx, swp.cy,
                  SWP_MOVE | SWP_SIZE);

  return(BOOL_TRUE);
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³ Save the Profile                                                         ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
BOOL SaveProfile(HWND hwndFrame,
                 PSPY_DATA pSpy,
                 PSZ       szFile)
{
  FILE  *pProFile = NULL;
  CHAR  szText[64];
  CHAR  szTextM[64];
  SWP   swp;
  register USHORT     i;
  register PMSG_ITEM  pMsg;
  register PGRP_ITEM  pGrp;

  if ( (pProFile = fopen(szFile, "w")) == NULL)
    return(BOOL_FALSE);

  fprintf(pProFile, Strings[IDS_PRO_HEADER],
          SPY_VERSION/100, (SPY_VERSION - 100 * (SPY_VERSION/100)), "\n");

  /* save position */
  WinQueryWindowPos(hwndFrame, &swp);
  fprintf(pProFile, Strings[IDS_PRO_WINPOS_HEADER], "\n", "\n\n");
  fprintf(pProFile, Strings[IDS_PRO_WINPOS_FMT],
          Strings[IDS_PRO_KW_X], swp.x, "\n");
  fprintf(pProFile, Strings[IDS_PRO_WINPOS_FMT],
          Strings[IDS_PRO_KW_Y], swp.y, "\n");
  fprintf(pProFile, Strings[IDS_PRO_WINPOS_FMT],
          Strings[IDS_PRO_KW_CX], swp.cx, "\n");
  fprintf(pProFile, Strings[IDS_PRO_WINPOS_FMT],
          Strings[IDS_PRO_KW_CY], swp.cy, "\n");

  /* save settings */
  fprintf(pProFile, Strings[IDS_PRO_SETTINGS_HEADER], "\n", "\n\n");
  fprintf(pProFile, Strings[IDS_PRO_SETTINGS_FMT],
          Strings[IDS_PRO_KW_SPYON],
          (SpyQueryTargetIsWindow(SpyInstance) ?
           Strings[IDS_PRO_KW_WINDOW] : Strings[IDS_PRO_KW_QUEUE]),
          "\n");
  fprintf(pProFile, Strings[IDS_PRO_SETTINGS_FMT],
          Strings[IDS_PRO_KW_LIST],
          (slmState == SLM_THAWED ?
           Strings[IDS_PRO_KW_THAW] : Strings[IDS_PRO_KW_FREEZE]),
          "\n");
  fprintf(pProFile, Strings[IDS_PRO_FONT_FMT],
          Strings[IDS_PRO_KW_FONT], "\"", iniAgent.szListFont, "\"", "\n");

  /* Save the COLOR settings: Color <i> <color name> */
  fprintf(pProFile, Strings[IDS_PRO_COLOR_HEADER], "\n", "\n\n");
  for( /* Initialize */  i = 0;              /* start @ first */
       /* Terminate  */  i < Color_Total;    /* stop at End-Of-Table */
       /* Iterate    */  i++                 /* try the next */
     )
  {
    strcpy(szText, "\"");
    strcat(szText, ExternalColorTranslation[pSpy->LogicalColorTranslation[i].iExtColor].pszClrName);
    strcat(szText, "\"");
    fprintf(pProFile,
            Strings[IDS_PRO_COLOR_FMT],
            Strings[IDS_PRO_KW_COLOR],
            i + 1,                    /* externally, it's ONE origin... */
            szText,
            "\n");
  }

  /* Save each GROUP and MSG */

  fprintf(pProFile, Strings[IDS_PRO_DEFINE_HEADER], "\n", "\n");

  for(/* Initialize */ pGrp = ProcessFirstGroup(pSpy); /* @ first GROUP */
      /* Terminate  */ pGrp != NULL;                   /* @ end of list */
      /* Iterate    */ pGrp = ProcessNextGroup(pSpy)   /* Try next GROUP */
     )
  {
    strcpy(szText, "\"");
    strcat(szText, pGrp->pDesc);
    strcat(szText, "\"");
    fprintf(pProFile,
            Strings[IDS_PRO_GROUP_FMT],
            "\n",
            Strings[IDS_PRO_KW_GROUP],
            szText,
            pGrp->ClrFG + 1,
            "\n\n");

    for( /* Initialize */  pMsg = ProcessFirstMsgInGroup(pSpy,pGrp->GroupID);/* start @ first MSG */
         /* Terminate  */  pMsg != NULL;                /* stop at End-Of-Table */
         /* Iterate    */  pMsg = ProcessNextMsgInGroup(pSpy)  /* try the next MSG */
       )
    {
      strcpy(szText, "\"");
      strcat(szText, pMsg->pDesc);
      strcat(szText, "\"");
      if ( pMsg->ClrFG == pGrp->ClrFG )                 /* MSG use same color? */
        fprintf(pProFile,
                Strings[IDS_PRO_DEFINE_FMT],
                Strings[IDS_PRO_KW_DEFINE],
                szText,
                pMsg->Msg,
                "\n");
      else
        fprintf(pProFile,
                Strings[IDS_PRO_DEFINE_FMT_COLOR],
                Strings[IDS_PRO_KW_DEFINE],
                szText,
                pMsg->Msg,
                pMsg->ClrFG + 1,
                "\n");
    } /* endif */
  } /* endfor */

  /* Save the FILTER settings */
  fprintf(pProFile, Strings[IDS_PRO_FIL_HEADER],  "\n", "\n\n");

  /* First, reset all the MSG 'SaveProcessed' flags */
  for( /* Initialize */  pMsg = ProcessFirstMsg(pSpy);/* start @ first MSG */
       /* Terminate  */  pMsg != NULL;                /* stop at End-Of-Table */
       /* Iterate    */  pMsg = ProcessNextMsg(pSpy)  /* try the next MSG */
     )
    pMsg->SaveProcessed = BOOL_FALSE;

  /* Second, process each GROUP */
  for(/* Initialize */ pGrp = ProcessFirstGroup(pSpy); /* @ first GROUP */
      /* Terminate  */ pGrp != NULL;                   /* @ end of list */
      /* Iterate    */ pGrp = ProcessNextGroup(pSpy)   /* Try next GROUP */
     )
  {
    strcpy(szText, "\"");
    strcat(szText, pGrp->pDesc);
    strcat(szText, "\"");
    fprintf(pProFile,
            Strings[IDS_PRO_FILTER_FMT],
            pGrp->Include ? Strings[IDS_PRO_KW_INCLUDE]
                          : Strings[IDS_PRO_KW_EXCLUDE],
            szText,
            "\n");

    /* Mark each MSG in this GROUP as processed if same Include state... */
    for( /* Initialize */ pMsg = ProcessFirstMsgInGroup(pSpy,pGrp->GroupID); /* start @ first MSG */
         /* Terminate  */ pMsg != NULL;                          /* stop at End-Of-Table */
         /* Iterate    */ pMsg = ProcessNextMsgInGroup(pSpy) /* try the next MSG */
       )
    {
      if (pMsg->Include == pGrp->Include)
        pMsg->SaveProcessed = BOOL_TRUE;
    }
  }
  fputc('\n', pProFile);

  /* Third, process each MSG looking for ones not processed */
  for( /* Initialize */  pMsg = ProcessFirstMsg(pSpy);/* start @ first MSG */
       /* Terminate  */  pMsg != NULL;                /* stop at End-Of-Table */
       /* Iterate    */  pMsg = ProcessNextMsg(pSpy)  /* try the next MSG */
     )
  {
    /* Skip undefined "User Defined" messages */
    if (!pMsg->SaveProcessed) {
      strcpy(szText, "\"");
      strcat(szText, pMsg->pDesc);
      strcat(szText, "\"");
      fprintf(pProFile,
              Strings[IDS_PRO_FILTER_FMT],
              pMsg->Include ? Strings[IDS_PRO_KW_INCLUDE]
                            : Strings[IDS_PRO_KW_EXCLUDE],
              szText,
              "\n");
    } /* endif */
  } /* endfor */

  /* Save the TRIGGER settings */
  fprintf(pProFile, Strings[IDS_PRO_TRIGGER_HEADER],  "\n", "\n");

  /* First, reset all the MSG 'SaveProcessed' flags */
  for( /* Initialize */  pMsg = ProcessFirstMsg(pSpy);/* start @ first MSG */
       /* Terminate  */  pMsg != NULL;                /* stop at End-Of-Table */
       /* Iterate    */  pMsg = ProcessNextMsg(pSpy)  /* try the next MSG */
     ) pMsg->SaveProcessed = BOOL_FALSE;
  fputc('\n', pProFile);

  /* Second, process each MSG */
  for( /* Initialize */  pMsg = ProcessFirstMsg(pSpy);/* start @ first MSG */
       /* Terminate  */  pMsg != NULL;                /* stop at End-Of-Table */
       /* Iterate    */  pMsg = ProcessNextMsg(pSpy)  /* try the next MSG */
     ) {
    if (!pMsg->SaveProcessed) {
      szText[0] = '\0';
      if (pMsg->TriggerThaw) {
        strcpy(szText, Strings[IDS_PRO_KW_THAW]);
        strcat(szText, " ");
      } /* endif */
      if (pMsg->TriggerFreeze) {
        strcat(szText, Strings[IDS_PRO_KW_FREEZE]);
        strcat(szText, "\n");
      } /* endif */
      if (szText[0]) {
        strcpy(szTextM, "\"");
        strcat(szTextM, pMsg->pDesc);
        strcat(szTextM, "\"");
        fprintf(pProFile,
                Strings[IDS_PRO_TRIGGER_FMT],
                Strings[IDS_PRO_KW_TRIGGER],
                szTextM,
                szText);
      } /* endif */
    } /* endif */
  } /* endfor */

  /* end of SAVE stuff */
  fprintf(pProFile, Strings[IDS_PRO_END], "\n", "\n");
  fclose(pProFile);
  return(BOOL_TRUE);
}
