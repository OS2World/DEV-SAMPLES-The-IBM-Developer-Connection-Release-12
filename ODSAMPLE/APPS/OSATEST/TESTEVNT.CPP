
#define INCL_WINSTDFILE  /* Window Standard File Functions       */
#define INCL_WINBUTTONS
#define INCL_WINDIALOGS
#define INCL_WINMLE
#define INCL_WINERRORS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINSTDFILE
#define INCL_WINSTDFONT
#define INCL_WINWINDOWMGR
#define INCL_WINENTRYFIELDS
#define INCL_WINLISTBOXES
#define INCL_WINSWITCHLIST
#define INCL_ODAPI
#define INCL_ODOSA
#define INCL_OSAAPI
#define INCL_OSA
#define INCL_OSACOMPONENT
#define INCL_ODCOMPONENT
#include <os2.h>

/* c language includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <process.h>
#include <memory.h>
#include <sys\types.h>
#include <sys\stat.h>

/* application includes */
#include "osatest.h"
#include "osadlg.h"
#include "pmassert.h"


typedef  struct _EVENTDATA_PARM {
  PMAIN_PARM pmp;
  LONG     eventType;
  DescType prop;
  LONG     formAddrType;
  CHAR     formNameData[CCHMAXPATH];
  LONG     formUniqueIDData;
  LONG     formAbsPosData;
  BOOL     bAbsPosOrdinal;
  BOOL     bReplyValue;
  LONG     lReplyValue;
  CHAR     szReplyValue[CCHMAXPATH];
  BOOL     bSetValue;
  LONG     lSetValue;
  CHAR     szSetValue[CCHMAXPATH];
  CHAR     szTemp[CCHMAXPATH];
  CHAR     szTargetApp[CCHMAXPATH];
} EVENTDATA_PARM;
typedef EVENTDATA_PARM *PEVENTDATA_PARM;

typedef struct _OBJSPECTOKEN {
  CHAR          szToken[CCHMAXPATH];
  struct _OBJSPECTOKEN  *Next;
} OBJSPECTOKEN;
typedef OBJSPECTOKEN *POBJSPECTOKEN;

void testformname();
void testGetData(PEVENTDATA_PARM pEventDataParms);
void testSetData(PEVENTDATA_PARM pEventDataParms);
void testdocshell(PEVENTDATA_PARM pEventDataParms, ULONG);

void CreateObjectSpecifierForPart(PSZ szTemp, AEDesc *pobjspec);
void TokenizeObjSpec(PSZ szTemp, POBJSPECTOKEN *ppHeadList);
void CreateObjSpecFromToken(POBJSPECTOKEN pToken, AEDesc *pobjspec);
void GetTargetApp(HWND hwnd, PEVENTDATA_PARM ped);

void CreateObjectSpecifierForPart(PSZ szTemp, AEDesc *pobjspec)
{
   POBJSPECTOKEN pObjSpecToken = NULL;
   POBJSPECTOKEN pTemp;

   TokenizeObjSpec(szTemp, &pObjSpecToken);

   CreateObjSpecFromToken(pObjSpecToken, pobjspec);

   /* free tokens */
   while(pObjSpecToken)
   {
     pTemp = pObjSpecToken->Next;
     free(pObjSpecToken);
     pObjSpecToken = pTemp;
   }

}

void TokenizeObjSpec(PSZ szTemp, POBJSPECTOKEN *ppHeadList)
{
   POBJSPECTOKEN pToken,pToken2;
   PSZ           pszWord;

   pszWord = strtok(szTemp, " ");

   *ppHeadList = (POBJSPECTOKEN) malloc(sizeof(OBJSPECTOKEN));
   (*ppHeadList)->Next = NULL;
   if(!pszWord)
     strcpy((*ppHeadList)->szToken,"");
   else
   {
     strcpy((*ppHeadList)->szToken,pszWord);
     pToken = *ppHeadList;
     while(pszWord = strtok(NULL," "))
     {
       pToken2 = (POBJSPECTOKEN) malloc(sizeof(OBJSPECTOKEN));
       strcpy(pToken2->szToken,pszWord);
       pToken2->Next = NULL;
       pToken->Next = pToken2;
       pToken = pToken2;
     }
   }
}


void CreateObjSpecFromToken(POBJSPECTOKEN pToken, AEDesc *pobjspec)
{
   CHAR szName[256];
   CHAR szMisc[256];
   LONG formAddrType;
   LONG formData;
   BOOL bAbsPosOrdinal;
   AEDesc *pDesc;
   AEDesc *pNullDesc;

   /* test for end of recursion */
   if(pToken)
   {
      if(stricmp(pToken->szToken,"part")==0)
      {
         pToken = pToken->Next;
         /* check for unique id */
         if(stricmp(pToken->szToken,"id")==0)
         {
            if(!(pToken = pToken->Next))
              return;
            if(isdigit(pToken->szToken[0]))
            {
              formAddrType = formUniqueID;
              formData = atol(pToken->szToken);
            }
            else return;
         }
         /* check for a name */
         else if(pToken->szToken[0]=='\"')
         {
           formAddrType = formName;
           strcpy(szName, pToken->szToken);
           /* zap the double quotes */
           szName[strlen(szName)-1] = '\0';
           strcpy(szMisc,szName+1);
           strcpy(szName,szMisc);
         }
         /* check for abs pos number */
         else if(isdigit(pToken->szToken[0]))
         {
           formAddrType = formAbsolutePosition;
           bAbsPosOrdinal = FALSE;
           formData = atol(pToken->szToken);
         }
         /* check for abs pos ordinals */
         else if(strcmpi(pToken->szToken,"kAEFirst")==0)
         {
           formAddrType = formAbsolutePosition;
           bAbsPosOrdinal = TRUE;
           formData = kAEFirst;
         }
         else if(strcmpi(pToken->szToken,"kAEMiddle")==0)
         {
           formAddrType = formAbsolutePosition;
           bAbsPosOrdinal = TRUE;
           formData = kAEMiddle;
         }
         else if(strcmpi(pToken->szToken,"kAELast")==0)
         {
           formAddrType = formAbsolutePosition;
           bAbsPosOrdinal = TRUE;
           formData = kAELast;
         }
         else if(strcmpi(pToken->szToken,"kAEAny")==0)
         {
           formAddrType = formAbsolutePosition;
           bAbsPosOrdinal = TRUE;
           formData = kAEAny;
         }
         else if(strcmpi(pToken->szToken,"kAEAll")==0)
         {
           formAddrType = formAbsolutePosition;
           bAbsPosOrdinal = TRUE;
           formData = kAEAll;
         }
         else /* treat the unknown keyword as a name */
         {
           formAddrType = formName;
           strcpy(szName, pToken->szToken);
         }
      }

      /* skip "of" */
      if(pToken->Next)
        pToken = pToken->Next;

      CreateObjSpecFromToken(pToken->Next, pobjspec);

      /* dynamically allocate desc ptrs to use in creating object specifiers */
      /* --these need to be freed after the send is over, so we need a better */
      /*   mem mgmt scheme.                                                   */
      pDesc = (AEDesc *) calloc(1,sizeof(AEDesc));
      if(!pToken->Next)
      {
        pNullDesc = (AEDesc *) calloc(1,sizeof(AEDesc));
        pNullDesc->descriptorType = typeNull;
        pNullDesc->dataHandle = 0;
      }
      /* Create the object specifier based on the addr type */
      switch (formAddrType){
         case formUniqueID:
            AECreateDesc(typeLongInteger, (Ptr) &formData, sizeof(LONG), pDesc);
            AECreateObjSpecifier(cPart, pToken->Next ? pobjspec: pNullDesc, formUniqueID, pDesc, 1, pobjspec);
            break;
         case formName:
            AECreateDesc(typeChar, (Ptr) szName, strlen(szName), pDesc);
            AECreateObjSpecifier(cPart, pToken->Next ? pobjspec: pNullDesc, formName, pDesc, 1, pobjspec);
            break;
         case formAbsolutePosition:
            if(bAbsPosOrdinal)
              AECreateDesc(typeAbsoluteOrdinal, (Ptr) &formData, sizeof(LONG), pDesc);
            else
              AECreateDesc(typeLongInteger, (Ptr) &formData, sizeof(LONG), pDesc);
            AECreateObjSpecifier(cPart, pToken->Next ? pobjspec: pNullDesc, formAbsolutePosition, pDesc, 1, pobjspec);
            break;
         default:
            ;
      } /* endswitch */
   }
   return;
}

void testformname()
{

  AEDesc    objspec, color;
  ULONG     abspos=1;
  AEDesc          nullDesc = {typeNull, 0};
  AEDesc          id, id2, pr;
  DescType prop = pBackgroundColor;
  OSAEvent      event = {typeNull, 0};
  OSAEvent      reply = {typeNull, 0};
  AEDesc        address;
  PID           pid;


  AECreateDesc(typeLongInteger, (Ptr) &abspos, sizeof(LONG), &id);

  AECreateObjSpecifier(cPart, &nullDesc, formAbsolutePosition, &id, 1, &objspec);

  AEGetPID("OpenDoc", &pid);
  AECreateDesc(typePID, &pid, sizeof(pid), &address);
  AECreateOSAEvent(kCoreEventClass, kAEPrintDocuments, &address,
              kAutoGenerateReturnID, kAnyTransactionID, &event);
  AEDisposeDesc(&address);
  AEPutParamDesc(&event, keyDirectObject, &objspec);
  AESend(&event, &reply, kAENoReply, kAENormalPriority,
              kAEDefaultTimeout, 0,0);

  AEDisposeDesc(&event);
  AEDisposeDesc(&objspec);

}

void testGetData(PEVENTDATA_PARM pEventDataParms)
{

  AEDesc    objspec;
  AEDesc          nullDesc = {typeNull, 0};
  AEDesc          id, pr;
  OSAEvent      event = {typeNull, 0};
  OSAEvent      reply = {typeNull, 0};
  AEDesc        address;
  PID           pid;
  DescType    typeCode;
  AEDesc      replyDesc;
  Size        dataSize;
  ULONG       rc;


  switch (pEventDataParms->formAddrType)
  {
     case formName:
        AECreateDesc(typeChar, (Ptr) pEventDataParms->formNameData, strlen(pEventDataParms->formNameData), &id);
        AECreateObjSpecifier(cPart, &nullDesc, formName, &id, 1, &objspec);
        break;
     case formUniqueID:
        AECreateDesc(typeLongInteger, (Ptr) &(pEventDataParms->formUniqueIDData), sizeof(LONG), &id);
        AECreateObjSpecifier(cPart, &nullDesc, formUniqueID, &id, 1, &objspec);
        break;
     case formAbsolutePosition:
        if(pEventDataParms->bAbsPosOrdinal)
          AECreateDesc(typeAbsoluteOrdinal, (Ptr) &(pEventDataParms->formAbsPosData), sizeof(LONG), &id);
        else
          AECreateDesc(typeLongInteger, (Ptr) &(pEventDataParms->formAbsPosData), sizeof(LONG), &id);
        AECreateObjSpecifier(cPart, &nullDesc, formAbsolutePosition, &id, 1, &objspec);
        break;
     default:
        /* CreatePartObjSpecFromString(pEventDataParms->szTemp, &objspec); */
        CreateObjectSpecifierForPart(pEventDataParms->szTemp, &objspec);
        break;
  } /* endswitch */

  AECreateDesc(typeType, (Ptr) &(pEventDataParms->prop), sizeof(DescType), &pr);

  AECreateObjSpecifier(cProperty, &objspec, formPropertyID, &pr, 1, &objspec);


  AEGetPID(pEventDataParms->szTargetApp, &pid);
  AECreateDesc(typePID, &pid, sizeof(pid), &address);

  AECreateOSAEvent(kAECoreSuite, kAEGetData, &address,
              kAutoGenerateReturnID, kAnyTransactionID, &event);

  AEDisposeDesc(&address);
  AEPutParamDesc(&event, keyDirectObject, &objspec);
  rc = AESend(&event, &reply, kAEWaitReply, kAENormalPriority,
              kAEDefaultTimeout, 0,0);


  switch (pEventDataParms->prop){
      case pBackgroundColor:
        if(!(rc=AEGetParamDesc(&reply, keyAEResult, typeInteger, &replyDesc)))
        {
          dataSize = sizeof(LONG);
          AEGetDescData(&replyDesc, &typeCode, (Ptr) &(pEventDataParms->lReplyValue),
                        (Size)sizeof(LONG), &dataSize);
        }
        else
          pEventDataParms->lReplyValue = -1;
        break;
      case pName:
      case pComment:
      case pAuthor:
      case pKind:
      case pEditorName:
        if(!AEGetParamDesc(&reply, keyAEResult, typeChar, &replyDesc))
        {
          AESizeOfDescData(&replyDesc,&dataSize);

          AEGetDescData(&replyDesc, &typeCode, (Ptr) pEventDataParms->szReplyValue,
                        (Size)(CCHMAXPATH-1), &dataSize);
          pEventDataParms->szReplyValue[dataSize] = '\0';
        }
        else
          strcpy(pEventDataParms->szReplyValue,"ERROR");
          /* string */
        break;
      case pBundled:
      case pStationery:
      case pShowLinks:
        if(!AEGetParamDesc(&reply, keyAEResult, typeBoolean, &replyDesc))
        {
          dataSize = sizeof(LONG);
          AEGetDescData(&replyDesc, &typeCode, (Ptr) &(pEventDataParms->bReplyValue),
                        (Size)sizeof(LONG), &dataSize);
        }
        else
          pEventDataParms->lReplyValue = -1;
          /* boolean */
        break;
      case pASCreationDate:
      case pASModificationDate:
          /* some date format */
        break;
      case pSize:
      case pUniqueID:
        if(!AEGetParamDesc(&reply, keyAEResult, typeLongInteger, &replyDesc))
        {
          dataSize = sizeof(LONG);
          AEGetDescData(&replyDesc, &typeCode, (Ptr) &(pEventDataParms->lReplyValue),
                        (Size)sizeof(LONG), &dataSize);
        }
        else
          pEventDataParms->lReplyValue = -1;
          /* long integer */
        break;
      case pIcon:
        break;
      case pViewType:
        break;
      case pCategory:
        break;
     default:
        ;
  } /* endswitch */

  AEDisposeDesc(&event);
  AEDisposeDesc(&objspec);
  AEDisposeDesc(&reply);
  AEDisposeDesc(&replyDesc);

}

void testSetData(PEVENTDATA_PARM pEventDataParms)
{

  AEDesc    objspec;
  AEDesc          nullDesc = {typeNull, 0};
  AEDesc          id, pr;
  AEDesc        dataDesc;
  OSAEvent      event = {typeNull, 0};
  OSAEvent      reply = {typeNull, 0};
  AEDesc        address;
  PID           pid;
  DescType    typeCode;
  AEDesc      replyDesc;
  Size        dataSize;


  switch (pEventDataParms->formAddrType)
  {
     case formName:
        AECreateDesc(typeChar, (Ptr) pEventDataParms->formNameData, strlen(pEventDataParms->formNameData), &id);
        AECreateObjSpecifier(cPart, &nullDesc, formName, &id, 1, &objspec);
        break;
     case formUniqueID:
        AECreateDesc(typeLongInteger, (Ptr) &(pEventDataParms->formUniqueIDData), sizeof(LONG), &id);
        AECreateObjSpecifier(cPart, &nullDesc, formUniqueID, &id, 1, &objspec);
        break;
     case formAbsolutePosition:
        if(pEventDataParms->bAbsPosOrdinal)
          AECreateDesc(typeAbsoluteOrdinal, (Ptr) &(pEventDataParms->formAbsPosData), sizeof(LONG), &id);
        else
          AECreateDesc(typeLongInteger, (Ptr) &(pEventDataParms->formAbsPosData), sizeof(LONG), &id);
        AECreateObjSpecifier(cPart, &nullDesc, formAbsolutePosition, &id, 1, &objspec);
        break;
     default:
        /* CreatePartObjSpecFromString(pEventDataParms->szTemp, &objspec); */
        CreateObjectSpecifierForPart(pEventDataParms->szTemp, &objspec);
  } /* endswitch */

  AECreateDesc(typeType, (Ptr) &(pEventDataParms->prop), sizeof(DescType), &pr);

  AECreateObjSpecifier(cProperty, &objspec, formPropertyID, &pr, 1, &objspec);


  AEGetPID(pEventDataParms->szTargetApp, &pid);
  AECreateDesc(typePID, &pid, sizeof(pid), &address);

  AECreateOSAEvent(kAECoreSuite, kAESetData, &address,
                   kAutoGenerateReturnID, kAnyTransactionID, &event);

  AEDisposeDesc(&address);
  AEPutParamDesc(&event, keyDirectObject, &objspec);


  switch (pEventDataParms->prop){
      case pBackgroundColor:
        AECreateDesc(typeInteger, &(pEventDataParms->lSetValue), sizeof(ULONG), &dataDesc);
        break;
      case pName:
      case pComment:
        AECreateDesc(typeChar, pEventDataParms->szSetValue, strlen(pEventDataParms->szSetValue), &dataDesc);
          /* string */
        break;
      case pBundled:
      case pStationery:
      case pShowLinks:
        AECreateDesc(typeBoolean, &(pEventDataParms->bSetValue), sizeof(BOOL), &dataDesc);
          /* boolean */
        break;
      case pIcon:
        break;
      case pViewType:
        break;
      case pCategory:
        break;
     default:
        ;
  } /* endswitch */


  AEPutParamDesc(&event, keyAEData, &dataDesc);
  AESend(&event, &reply, kAENoReply, kAENormalPriority,
              kAEDefaultTimeout, 0,0);


  AEDisposeDesc(&event);
  AEDisposeDesc(&objspec);
}

void testdocshell(PEVENTDATA_PARM pEventDataParms,ULONG eventID)
{
  OSAEvent      event = {typeNull, 0};
  OSAEvent      reply = {typeNull, 0};
  AEDesc        address;
  PID           pid;

  AEGetPID(pEventDataParms->szTargetApp, &pid);
  AECreateDesc(typePID, &pid, sizeof(pid), &address);
  AECreateOSAEvent(kCoreEventClass, eventID, &address,
              kAutoGenerateReturnID, kAnyTransactionID, &event);
  AEDisposeDesc(&address);
  AESend(&event, &reply, kAENoReply, kAENormalPriority,
              kAEDefaultTimeout, 0,0);
  AEDisposeDesc(&event);
}


void GetTargetApp(HWND hwnd, PEVENTDATA_PARM ped)
{
   HWND hwndApp;
   CHAR szTemp[CCHMAXPATH];

   hwndApp = WinWindowFromID(hwnd, IDD_APPCOMBO);
   WinQueryWindowText(hwndApp, CCHMAXPATH-1, ped->szTargetApp);
   if(strcmpi(ped->szTargetApp,"OpenDoc")==0)
   {
     hwndApp = WinWindowFromID(hwnd, IDD_ODDOCUMENT);
     WinQueryWindowText(hwndApp, CCHMAXPATH-1, szTemp);
     if(szTemp[0])
     {
       strcat(ped->szTargetApp," - ");
       strcat(ped->szTargetApp,szTemp);
     }
   }

}

MRESULT EXPENTRY OSAOpendocEventsDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   PEVENTDATA_PARM  ped;
   HWND hwndButton;
   CHAR szTemp[CCHMAXPATH];
   ULONG size;
   PSZ   pszBuffer,pszTemp;
   SWP   swp;

   switch(msg)
   {
      case WM_INITDLG:
         ped = (PEVENTDATA_PARM) malloc(sizeof(EVENTDATA_PARM));
         ped->pmp = (PMAIN_PARM) mp2;
         ped->pmp->hwndTestEvent = hwnd;
         if(ped->pmp->Profile.testevnt_cx != 0)
         {
           WinSetWindowPos(hwnd,
                           HWND_TOP,
                           ped->pmp->Profile.testevnt_x,
                           ped->pmp->Profile.testevnt_y,
                           ped->pmp->Profile.testevnt_cx,
                           ped->pmp->Profile.testevnt_cy,
                           SWP_MOVE | SWP_SHOW );
         }
         else
         {
          WinQueryTaskSizePos(WinQueryAnchorBlock(hwnd),0,&swp);
          WinSetWindowPos(hwnd,
                          HWND_TOP,
                          swp.x,
                          swp.y,
                          swp.cx,
                          swp.cy,
                          SWP_MOVE | SWP_SHOW );
         }
         WinSetWindowULong( hwnd, QWL_USER, (ULONG)ped );

         /* set default address mode to formAbsolutePosition/INDEX=1 */
         hwndButton = WinWindowFromID(hwnd, IDD_ABS);
         WinSendMsg(hwndButton, BM_CLICK,
                                 MPFROMSHORT( TRUE ),
                                 0L );
         hwndButton = WinWindowFromID(hwnd, IDD_ABSIDX);
         WinSendMsg(hwndButton, BM_CLICK,
                                 MPFROMSHORT( TRUE ),
                                 0L );
         hwndButton = WinWindowFromID(hwnd, IDD_ABSIDXENTRY);
         WinSetWindowText(hwndButton, "1");
         ped->formAddrType   = formAbsolutePosition;
         ped->formAbsPosData = 1;


         /* setup app combo */
         hwndButton = WinWindowFromID(hwnd, IDD_APPCOMBO);
         WinSetWindowText(hwndButton, "OpenDoc");

         OSAListApplications(NULL,&size, OSA_APPLICATION);
         if(size)
         {
           pszBuffer = (char *) malloc(size);
           OSAListApplications(pszBuffer,&size, OSA_APPLICATION);
         }

         pszTemp = pszBuffer;
         while(strlen(pszBuffer))
         {
          WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)pszBuffer));
          pszBuffer = pszBuffer + (strlen(pszBuffer) + 1);
         }
         free((PVOID)pszTemp);

         /* setup event combo */
         hwndButton = WinWindowFromID(hwnd, IDD_EVENTCOMBO);
         WinSetWindowText(hwndButton, "kAEGetData");
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"kAEGetData"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"kAESetData"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"kAEPrintDocuments"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"kAEQuitApplication"));

         /* init properties for get data event */
         WinSendMsg(hwnd, WM_GETDATA_INIT,0,0);

         /* setup abs pos combo */
         hwndButton = WinWindowFromID(hwnd, IDD_ABSORDCOMBO);
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"kAEFirst"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"kAEMiddle"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"kAELast"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"kAEAny"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"kAEAll"));

         return MRFROMSHORT(FALSE);


      case WM_SETDATA_INIT:

         ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
         ped->eventType = kAESetData;
         hwndButton = WinWindowFromID(hwnd, IDD_PROPCOMBO);
         WinSetWindowText(hwndButton, "pBackgroundColor");
         WinEnableWindow(WinWindowFromID(hwnd, IDD_DATA),TRUE);
         WinSendMsg(hwndButton, LM_DELETEALL, 0, 0);
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pBackgroundColor"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pName"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pComment"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pBundled"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pStationery"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pShowLinks"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pIcon"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pViewType"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pCategory"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pEditorName"));
         ped->prop = pBackgroundColor;
         break;

      case WM_GETDATA_INIT:

         ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
         ped->eventType = kAEGetData;
         hwndButton = WinWindowFromID(hwnd, IDD_PROPCOMBO);
         WinSetWindowText(hwndButton, "pBackgroundColor");
         WinEnableWindow(WinWindowFromID(hwnd, IDD_DATA),FALSE);
         WinSendMsg(hwndButton, LM_DELETEALL, 0, 0);
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pBackgroundColor"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pName"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pComment"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pAuthor"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pKind"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pEditorName"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pBundled"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pStationery"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pShowLinks"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pASCreationDate"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pASModificationDate"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pSize"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pUniqueID"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pIcon"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pViewType"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pCategory"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pClass"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pDefaultType"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pBestType"));
         WinSendMsg(hwndButton, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((PSZ)"pContainer"));
         ped->prop = pBackgroundColor;
         break;

      case WM_PRINTEVENT_INIT:
         /* gray out all */
         ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
         ped->eventType = kAEPrintDocuments;
         break;

      case WM_QUITEVENT_INIT:
         /* gray out all */
         ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
         ped->eventType = kAEQuitApplication;
         break;

      case WM_CONTROL:

         if ( SHORT2FROMMP ( mp1 ) == BN_CLICKED )
         {
            /* track the currently selected button */
            switch(SHORT1FROMMP(mp1)) {

              case IDD_NAME:
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_NAMEENTRY),TRUE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_IDENTRY),FALSE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_SPECIFY),FALSE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_ABSORDCOMBO),FALSE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_ABSIDXENTRY),FALSE);
                 ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
                 ped->formAddrType = formName;
                 break;
              case IDD_ID:
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_IDENTRY),TRUE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_NAMEENTRY),FALSE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_SPECIFY),FALSE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_ABSORDCOMBO),FALSE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_ABSIDXENTRY),FALSE);
                 ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
                 ped->formAddrType = formUniqueID;
                 break;
              case IDD_ABS:
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_IDENTRY),FALSE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_NAMEENTRY),FALSE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_SPECIFY),FALSE);
                 ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
                 if(ped->bAbsPosOrdinal)
                   WinEnableWindow(WinWindowFromID(hwnd, IDD_ABSORDCOMBO),TRUE);
                 else
                   WinEnableWindow(WinWindowFromID(hwnd, IDD_ABSIDXENTRY),TRUE);
                 ped->formAddrType = formAbsolutePosition;
                 break;
              case IDD_ABSORD:
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_ABSORDCOMBO),TRUE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_ABSIDXENTRY),FALSE);
                 ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
                 ped->bAbsPosOrdinal = TRUE;
                 break;
              case IDD_ABSIDX:
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_ABSIDXENTRY),TRUE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_ABSORDCOMBO),FALSE);
                 ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
                 ped->bAbsPosOrdinal = FALSE;
                 break;
              case IDD_PARTDESC:
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_SPECIFY),TRUE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_IDENTRY),FALSE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_NAMEENTRY),FALSE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_ABSORDCOMBO),FALSE);
                 WinEnableWindow(WinWindowFromID(hwnd, IDD_ABSIDXENTRY),FALSE);
                 ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
                 ped->formAddrType = 0;
                 break;
              case IDD_EVENTCOMBO:
                 ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
                 hwndButton = WinWindowFromID(hwnd, IDD_EVENTCOMBO);
                 WinQueryWindowText(hwndButton, CCHMAXPATH-1, szTemp);
                 if(strcmp(szTemp,"kAEGetData")==0)
                 {
                   ped->eventType = kAEGetData;
                   WinSendMsg(hwnd, WM_GETDATA_INIT,0,0);
                 }
                 else if(strcmp(szTemp,"kAESetData")==0)
                 {
                   ped->eventType = kAESetData;
                   WinSendMsg(hwnd, WM_SETDATA_INIT,0,0);
                 }
                 else if(strcmp(szTemp,"kAEPrintDocuments")==0)
                 {
                   ped->eventType = kAEPrintDocuments;
                   WinSendMsg(hwnd, WM_PRINTEVENT_INIT,0,0);
                 }
                 else if(strcmp(szTemp,"kAEQuitApplication")==0)
                 {
                   ped->eventType = kAEQuitApplication;
                   WinSendMsg(hwnd, WM_QUITEVENT_INIT,0,0);
                 }
                 break;
              case IDD_PROPCOMBO:
                 ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
                 hwndButton = WinWindowFromID(hwnd, IDD_PROPCOMBO);
                 WinQueryWindowText(hwndButton, CCHMAXPATH-1, szTemp);
                 if(strcmp(szTemp,"pBackgroundColor")==0)
                   ped->prop = pBackgroundColor;
                 else if(strcmp(szTemp,"pName")==0)
                   ped->prop = pName;
                 else if(strcmp(szTemp,"pComment")==0)
                   ped->prop = pComment;
                 else if(strcmp(szTemp,"pAuthor")==0)
                   ped->prop = pAuthor;
                 else if(strcmp(szTemp,"pKind")==0)
                   ped->prop = pKind;
                 else if(strcmp(szTemp,"pEditorName")==0)
                   ped->prop = pEditorName;
                 else if(strcmp(szTemp,"pBundled")==0)
                   ped->prop = pBundled;
                 else if(strcmp(szTemp,"pStationery")==0)
                   ped->prop = pStationery;
                 else if(strcmp(szTemp,"pShowLinks")==0)
                   ped->prop = pShowLinks;
                 else if(strcmp(szTemp,"pASCreationData")==0)
                   ped->prop = pASCreationDate;
                 else if(strcmp(szTemp,"pASModificationData")==0)
                   ped->prop = pASModificationDate;
                 else if(strcmp(szTemp,"pSize")==0)
                   ped->prop = pSize;
                 else if(strcmp(szTemp,"pUniqueID")==0)
                   ped->prop = pUniqueID;
                 else if(strcmp(szTemp,"pIcon")==0)
                   ped->prop = pIcon;
                 else if(strcmp(szTemp,"pViewType")==0)
                   ped->prop = pViewType;
                 else if(strcmp(szTemp,"pCategory")==0)
                   ped->prop = pCategory;
                 else if(strcmp(szTemp,"pClass")==0)
                   ped->prop = pClass;
                 else if(strcmp(szTemp,"pDefaultType")==0)
                   ped->prop = pDefaultType;
                 else if(strcmp(szTemp,"pBestType")==0)
                   ped->prop = pBestType;
                 else if(strcmp(szTemp,"pContainer")==0)
                   ped->prop = pContainer;
                 break;
              case IDD_ABSORDCOMBO:
                 ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
                 hwndButton = WinWindowFromID(hwnd, IDD_ABSORDCOMBO);
                 WinQueryWindowText(hwndButton, CCHMAXPATH-1, szTemp);
                 if(strcmp(szTemp,"kAEFirst")==0)
                   ped->formAbsPosData = kAEFirst;
                 else if(strcmp(szTemp,"kAEMiddle")==0)
                   ped->formAbsPosData = kAEMiddle;
                 else if(strcmp(szTemp,"kAELast")==0)
                   ped->formAbsPosData = kAELast;
                 else if(strcmp(szTemp,"kAEAny")==0)
                   ped->formAbsPosData = kAEAny;
                 else if(strcmp(szTemp,"kAEAll")==0)
                   ped->formAbsPosData = kAEAll;
                 break;
            }
         }
         break;


      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1)){
            case IDD_SENDEVENT:
               ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               switch(ped->eventType){
                 case kAEGetData:
                 case kAESetData:
                    switch(ped->formAddrType){
                       case formName:
                          hwndButton = WinWindowFromID(hwnd, IDD_NAMEENTRY);
                          WinQueryWindowText(hwndButton, CCHMAXPATH-1, ped->formNameData);
                          break;
                       case formUniqueID:
                          hwndButton = WinWindowFromID(hwnd, IDD_IDENTRY);
                          WinQueryWindowText(hwndButton, CCHMAXPATH-1, szTemp);
                          ped->formUniqueIDData = atol(szTemp);
                          break;
                       case formAbsolutePosition:
                          if(!ped->bAbsPosOrdinal)
                          {
                            hwndButton = WinWindowFromID(hwnd, IDD_ABSIDXENTRY);
                            WinQueryWindowText(hwndButton, CCHMAXPATH-1, szTemp);
                            ped->formAbsPosData = atol(szTemp);
                          }
                          break;
                       default:  /* user specified */
                          hwndButton = WinWindowFromID(hwnd, IDD_SPECIFY);
                          WinQueryWindowText(hwndButton, CCHMAXPATH-1, ped->szTemp);
                          if(*(ped->szTemp) == '\0')
                          {
                            hwndButton = WinWindowFromID(hwnd, IDD_RESULT);
                            WinSetWindowText(hwndButton, "ERROR");
                            return (MRESULT)NULL;
                          }
                          break;
                    }
               }

               switch(ped->eventType){
                 case kAEPrintDocuments:
                    GetTargetApp(hwnd, ped);
                    testdocshell(ped,kAEPrintDocuments);
                    break;
                 case kAEQuitApplication:
                    GetTargetApp(hwnd, ped);
                    testdocshell(ped,kAEQuitApplication);
                    break;
                 case kAESetData:
                    hwndButton = WinWindowFromID(hwnd, IDD_DATA);
                    switch(ped->prop){
                       case pName:
                       case pComment:
                          WinQueryWindowText(hwndButton, CCHMAXPATH-1, ped->szSetValue);
                          break;
                       case pBundled:
                       case pStationery:
                       case pShowLinks:
                          WinQueryWindowText(hwndButton, CCHMAXPATH-1, szTemp);
                          strupr(szTemp);
                          if(strcmp(szTemp,"TRUE")==0)
                            ped->bSetValue = TRUE;
                          else if(strcmp(szTemp,"FALSE")==0)
                            ped->bSetValue = FALSE;
                          break;
                       case pBackgroundColor:
                          WinQueryWindowText(hwndButton, CCHMAXPATH-1, szTemp);
                          ped->lSetValue = atol(szTemp);
                          break;
                       default:
                          ped->prop = 0;
                          break;
                    }
                    if(ped->prop)
                    {
                      GetTargetApp(hwnd, ped);
                      testSetData(ped);
                    }
                    break;
                 case kAEGetData:
                    GetTargetApp(hwnd, ped);
                    testGetData(ped);
                    hwndButton = WinWindowFromID(hwnd, IDD_RESULT);
                    switch(ped->prop){
                       case pName:
                       case pComment:
                       case pAuthor:
                       case pKind:
                       case pEditorName:
                          WinSetWindowText(hwndButton, ped->szReplyValue);
                          break;
                       case pBundled:
                       case pStationery:
                       case pShowLinks:
                          if(ped->bReplyValue == TRUE)
                            WinSetWindowText(hwndButton, "TRUE");
                          else if(ped->bReplyValue == FALSE)
                            WinSetWindowText(hwndButton, "FALSE");
                          else
                            WinSetWindowText(hwndButton, "ERROR");
                          break;
                       case pBackgroundColor:
                       case pSize:
                       case pUniqueID:
                          if(ped->lReplyValue==-1)
                            WinSetWindowText(hwndButton, "ERROR");
                          else
                          {
                            _ltoa(ped->lReplyValue,szTemp,10);
                            WinSetWindowText(hwndButton, szTemp);
                          }
                          break;
                    }
                    break;
               }
               break;
            case IDD_CANCELEVENT:
               ped = (PEVENTDATA_PARM) WinQueryWindowULong( hwnd, QWL_USER );
               // Save window size/pos info
               WinQueryWindowPos(hwnd, &swp);
               ped->pmp->Profile.testevnt_cx = swp.cx;
               ped->pmp->Profile.testevnt_cy = swp.cy;
               ped->pmp->Profile.testevnt_x = swp.x;
               ped->pmp->Profile.testevnt_y = swp.y;
               ped->pmp->hwndTestEvent = NULL;
               free((PVOID)ped);
               WinDismissDlg(hwnd, TRUE);
               break;
            default:
               ;
         } /* endswitch */
         break;

      default:
         return(WinDefDlgProc(hwnd, msg, mp1, mp2));
   }
   return (MRESULT)NULL;
}   /*  end of OSAOpendocEventsDlgProc()  */

