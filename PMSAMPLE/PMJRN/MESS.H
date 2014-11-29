/**********************************************************************\
*
* Module Name: MESS.H
\**********************************************************************/
/* Clipboard messages */
/* Control notification messages */
/* Journal Notification messages  */
/* Key/Character input msg     */
/* Mouse input messages */
/* General Window Messages */
 case (WM_ACTIVATE):
      strcpy(buffer,"WM_ACTIVATE");
      break;
 case (WM_ADJUSTWINDOWPOS):
      strcpy(buffer,"WM_ADJUSTWINDOWPOS");
      break;
 case (WM_BUTTON1DBLCLK):
      strcpy(buffer,"WM_BUTTON1DBLCLK");
      break;
 case (WM_BUTTON1DOWN):
      strcpy(buffer,"WM_BUTTON1DOWN");
      break;
 case (WM_BUTTON1UP):
      strcpy(buffer,"WM_BUTTON1UP");
      break;
 case (WM_BUTTON2DBLCLK):
      strcpy(buffer,"WM_BUTTON2DBLCLK");
      break;
 case (WM_BUTTON2DOWN):
      strcpy(buffer,"WM_BUTTON2DOWN");
      break;
 case (WM_BUTTON2UP):
      strcpy(buffer,"WM_BUTTON2UP");
      break;
 case (WM_CALCVALIDRECTS):
      strcpy(buffer,"WM_CALCVALIDRECTS");
      break;
 case (WM_CHAR):
      strcpy(buffer,"WM_CHAR");
      break;
 case (WM_VIOCHAR):
      strcpy(buffer,"WM_VIOCHAR");
      break;
 case (WM_CLOSE):
      strcpy(buffer,"WM_CLOSE");
      break;
 case (WM_COMMAND):
      strcpy(buffer,"WM_COMMAND");
      break;
 case (WM_CONTROL):
      strcpy(buffer,"WM_CONTROL");
      break;
 case (WM_CONTROLPOINTER):
      strcpy(buffer,"WM_CONTROLPOINTER");
      break;
 case (WM_CREATE):
      strcpy(buffer,"WM_CREATE");
      break;
 case (WM_DESTROY):
      strcpy(buffer,"WM_DESTROY");
      break;
 case (WM_DRAWITEM):
      strcpy(buffer,"WM_DRAWITEM");
      break;
 case (WM_ENABLE):
      strcpy(buffer,"WM_ENABLE");
      break;
 case (WM_FOCUSCHANGE):
      strcpy(buffer,"WM_FOCUSCHANGE");
      break;
 case (WM_FORMATFRAME):
      strcpy(buffer,"WM_FORMATFRAME");
      break;
 case (WM_HELP):
      strcpy(buffer,"WM_HELP");
      break;
 case (WM_HITTEST):
      strcpy(buffer,"WM_HITTEST");
      break;
 case (WM_HSCROLL):
      strcpy(buffer,"WM_HSCROLL");
      break;
 case (WM_INITDLG):
      strcpy(buffer,"WM_INITDLG");
      break;
 case (WM_INITMENU):
      strcpy(buffer,"WM_INITMENU");
      break;
 case (WM_JOURNALNOTIFY):
      strcpy(buffer,"WM_JOURNALNOTIFY");
      break;
 case (WM_MATCHMNEMONIC):
      strcpy(buffer,"WM_MATCHMNEMONIC");
      break;
 case (WM_MEASUREITEM):
      strcpy(buffer,"WM_MEASUREITEM");
      break;
 case (WM_MENUEND):
      strcpy(buffer,"WM_MENUEND");
      break;
 case (WM_MENUSELECT):
      strcpy(buffer,"WM_MENUSELECT");
      break;
 case (WM_MINMAXFRAME):
      strcpy(buffer,"WM_MINMAXFRAME");
      break;
 case (WM_MOUSEMOVE):
      strcpy(buffer,"WM_MOUSEMOVE");
      break;
 case (WM_MOVE):
      strcpy(buffer,"WM_MOVE");
      break;
 case (WM_NEXTMENU):
      strcpy(buffer,"WM_NEXTMENU");
      break;
 case (WM_NULL):
      strcpy(buffer,"WM_NULL");
      break;
 case (WM_PAINT):
      strcpy(buffer,"WM_PAINT");
      break;
 case (WM_QUERYACCELTABLE):
      strcpy(buffer,"WM_QUERYACCELTABLE");
      break;
 case (WM_QUERYTRACKINFO):
      strcpy(buffer,"WM_QUERYTRACKINFO");
      break;
 case (WM_QUERYWINDOWPARAMS):
      strcpy(buffer,"WM_QUERYWINDOWPARAMS");
      break;
 case (WM_QUIT):
      strcpy(buffer,"WM_QUIT");
      break;
 case (WM_RENDERALLFMTS):
      strcpy(buffer,"WM_RENDERALLFMTS");
      break;
 case (WM_RENDERFMT):
      strcpy(buffer,"WM_RENDERFMT");
      break;
 case (WM_SEM1):
      strcpy(buffer,"WM_SEM1");
      break;
 case (WM_SEM2):
      strcpy(buffer,"WM_SEM2");
      break;
 case (WM_SEM3):
      strcpy(buffer,"WM_SEM3");
      break;
 case (WM_SEM4):
      strcpy(buffer,"WM_SEM4");
      break;
 case (WM_SETACCELTABLE):
      strcpy(buffer,"WM_SETACCELTABLE");
      break;
 case (WM_SETFOCUS):
      strcpy(buffer,"WM_SETFOCUS");
      break;
 case (WM_SETSELECTION):
      strcpy(buffer,"WM_SETSELECTION");
      break;
 case (WM_SETWINDOWPARAMS):
      strcpy(buffer,"WM_SETWINDOWPARAMS");
      break;
 case (WM_SHOW):
      strcpy(buffer,"WM_SHOW");
      break;
 case (WM_SIZE):
      strcpy(buffer,"WM_SIZE");
      break;
 case (WM_SUBSTITUTESTRING):
      strcpy(buffer,"WM_SUBSTITUTESTRING");
      break;
 case (WM_SYSCOLORCHANGE):
      strcpy(buffer,"WM_SYSCOLORCHANGE");
      break;
 case (WM_SYSCOMMAND):
      strcpy(buffer,"WM_SYSCOMMAND");
      break;
 case (WM_SYSVALUECHANGED):
      strcpy(buffer,"WM_SYSVALUECHANGED");
      break;
 case (WM_TIMER):
      strcpy(buffer,"WM_TIMER");
      break;
 case (WM_TRACKFRAME):
      strcpy(buffer,"WM_TRACKFRAME");
      break;
 case (WM_TRANSLATEACCEL):
      strcpy(buffer,"WM_TRANSLATEACCEL");
      break;
 case (WM_UPDATEFRAME):
      strcpy(buffer,"WM_UPDATEFRAME");
      break;
 case (WM_VSCROLL):
      strcpy(buffer,"WM_VSCROLL");
      break;

/* Dynamic data exchange Messages */
 case (WM_DDE_ACK):
      strcpy(buffer,"WM_DDE_ACK");
      break;
 case (WM_DDE_ADVISE):
      strcpy(buffer,"WM_DDE_ADVISE");
      break;
 case (WM_DDE_DATA):
      strcpy(buffer,"WM_DDE_DATA");
      break;
 case (WM_DDE_EXECUTE):
      strcpy(buffer,"WM_DDE_EXECUTE");
      break;
 case (WM_DDE_FIRST):
      strcpy(buffer,"WM_DDE_FIRST");
      break;
/* case (WM_DDE_INITIATE):
      strcpy(buffer,"WM_DDE_INITIATE");
      break;*/
 case (WM_DDE_INITIATEACK):
      strcpy(buffer,"WM_DDE_INITIATEACK");
      break;
 case (WM_DDE_LAST):
      strcpy(buffer,"WM_DDE_LAST");
      break;
 case (WM_DDE_POKE):
      strcpy(buffer,"WM_DDE_POKE");
      break;
 case (WM_DDE_REQUEST):
      strcpy(buffer,"WM_DDE_REQUEST");
      break;
 case (WM_DDE_TERMINATE):
      strcpy(buffer,"WM_DDE_TERMINATE");
      break;
 case (WM_DDE_UNADVISE):
      strcpy(buffer,"WM_DDE_UNADVISE");
      break;

/* Button control messages */
 case (BM_CLICK):
      strcpy(buffer,"BM_CLICK");
      break;
 case (BM_QUERYCHECK):
      strcpy(buffer,"BM_QUERYCHECK");
      break;
 case (BM_QUERYCHECKINDEX):
      strcpy(buffer,"BM_QUERYCHECKINDEX");
      break;
 case (BM_QUERYHILITE):
      strcpy(buffer,"BM_QUERYHILITE");
      break;
 case (BM_SETCHECK):
      strcpy(buffer,"BM_SETCHECK");
      break;
 case (BM_SETDEFAULT):
      strcpy(buffer,"BM_SETDEFAULT");
      break;
 case (BM_SETHILITE):
      strcpy(buffer,"BM_SETHILITE");
      break;

/* Entry Field  messages */
 case (EM_CLEAR):
      strcpy(buffer,"EM_CLEAR");
      break;
 case (EM_COPY):
      strcpy(buffer,"EM_COPY");
      break;
 case (EM_CUT):
      strcpy(buffer,"EM_CUT");
      break;
 case (EM_PASTE):
      strcpy(buffer,"EM_PASTE");
      break;
 case (EM_QUERYCHANGED):
      strcpy(buffer,"EM_QUERYCHANGED");
      break;
 case (EM_QUERYFIRSTCHAR):
      strcpy(buffer,"EM_QUERYFIRSTCHAR");
      break;
 case (EM_QUERYSEL):
      strcpy(buffer,"EM_QUERYSEL");
      break;
 case (EM_SETFIRSTCHAR):
      strcpy(buffer,"EM_SETFIRSTCHAR");
      break;
 case (EM_SETSEL):
      strcpy(buffer,"EM_SETSEL");
      break;
 case (EM_SETTEXTLIMIT):
      strcpy(buffer,"EM_SETTEXTLIMIT");
      break;

/* Frame window related messages */
 case (WM_ERASEBACKGROUND):
      strcpy(buffer,"WM_ERASEBACKGROUND");
      break;
 case (WM_FLASHWINDOW):
      strcpy(buffer,"WM_FLASHWINDOW");
      break;
 case (WM_QUERYBORDERSIZE):
      strcpy(buffer,"WM_QUERYBORDERSIZE");
      break;
 case (WM_QUERYFOCUSCHAIN):
      strcpy(buffer,"WM_QUERYFOCUSCHAIN");
      break;
 case (WM_QUERYFRAMECTLCOUNT):
      strcpy(buffer,"WM_QUERYFRAMECTLCOUNT");
      break;
 case (WM_QUERYICON):
      strcpy(buffer,"WM_QUERYICON");
      break;
 case (WM_SETICON):
      strcpy(buffer,"WM_SETICON");
      break;

/* List Box Control Messages */
 case (LM_DELETEALL):
      strcpy(buffer,"LM_DELETEALL");
      break;
 case (LM_DELETEITEM):
      strcpy(buffer,"LM_DELETEITEM");
      break;
 case (LM_INSERTITEM):
      strcpy(buffer,"LM_INSERTITEM");
      break;
 case (LM_QUERYITEMCOUNT):
      strcpy(buffer,"LM_QUERYITEMCOUNT");
      break;
 case (LM_QUERYITEMHANDLE):
      strcpy(buffer,"LM_QUERYITEMHANDLE");
      break;
 case (LM_QUERYITEMTEXT):
      strcpy(buffer,"LM_QUERYITEMTEXT");
      break;
 case (LM_QUERYITEMTEXTLENGTH):
      strcpy(buffer,"LM_QUERYITEMTEXTLENGTH");
      break;
 case (LM_QUERYSELECTION):
      strcpy(buffer,"LM_QUERYSELECTION");
      break;
 case (LM_QUERYTOPINDEX):
      strcpy(buffer,"LM_QUERYTOPINDEX");
      break;
 case (LM_SEARCHSTRING):
      strcpy(buffer,"LM_SEARCHSTRING");
      break;
 case (LM_SELECTITEM):
      strcpy(buffer,"LM_SELECTITEM");
      break;
 case (LM_SETITEMHANDLE):
      strcpy(buffer,"LM_SETITEMHANDLE");
      break;
 case (LM_SETITEMHEIGHT):
      strcpy(buffer,"LM_SETITEMHEIGHT");
      break;
 case (LM_SETITEMTEXT):
      strcpy(buffer,"LM_SETITEMTEXT");
      break;
 case (LM_SETTOPINDEX):
      strcpy(buffer,"LM_SETTOPINDEX");
      break;

/* Menu control messages */
 case (MM_DELETEITEM):
      strcpy(buffer,"MM_DELETEITEM");
      break;
 case (MM_ENDMENUMODE):
      strcpy(buffer,"MM_ENDMENUMODE");
      break;
 case (MM_INSERTITEM):
      strcpy(buffer,"MM_INSERTITEM");
      break;
 case (MM_ISITEMVALID):
      strcpy(buffer,"MM_ISITEMVALID");
      break;
 case (MM_ITEMIDFROMPOSITION):
      strcpy(buffer,"MM_ITEMIDFROMPOSITION");
      break;
 case (MM_ITEMPOSITIONFROMID):
      strcpy(buffer,"MM_ITEMPOSITIONFROMID");
      break;
 case (MM_QUERYITEM):
      strcpy(buffer,"MM_QUERYITEM");
      break;
 case (MM_QUERYITEMATTR):
      strcpy(buffer,"MM_QUERYITEMATTR");
      break;
 case (MM_QUERYITEMCOUNT):
      strcpy(buffer,"MM_QUERYITEMCOUNT");
      break;
 case (MM_QUERYITEMTEXT):
      strcpy(buffer,"MM_QUERYITEMTEXT");
      break;
 case (MM_QUERYITEMTEXTLENGTH):
      strcpy(buffer,"MM_QUERYITEMTEXTLENGTH");
      break;
 case (MM_QUERYSELITEMID):
      strcpy(buffer,"MM_QUERYSELITEMID");
      break;
 case (MM_REMOVEITEM):
      strcpy(buffer,"MM_REMOVEITEM");
      break;
 case (MM_SELECTITEM):
      strcpy(buffer,"MM_SELECTITEM");
      break;
 case (MM_SETITEM):
      strcpy(buffer,"MM_SETITEM");
      break;
 case (MM_SETITEMATTR):
      strcpy(buffer,"MM_SETITEMATTR");
      break;
 case (MM_SETITEMHANDLE):
      strcpy(buffer,"MM_SETITEMHANDLE");
      break;
 case (MM_SETITEMTEXT):
      strcpy(buffer,"MM_SETITEMTEXT");
      break;
 case (MM_STARTMENUMODE):
      strcpy(buffer,"MM_STARTMENUMODE");
      break;

/* Scroll Bar messages */
 case (SBM_QUERYPOS):
      strcpy(buffer,"SBM_QUERYPOS");
      break;
 case (SBM_QUERYRANGE):
      strcpy(buffer,"SBM_QUERYRANGE");
      break;
 case (SBM_SETPOS):
      strcpy(buffer,"SBM_SETPOS");
      break;
 case (SBM_SETSCROLLBAR):
      strcpy(buffer,"SBM_SETSCROLLBAR");
      break;

/* Static control messages */
 case (SM_QUERYHANDLE):
      strcpy(buffer,"SM_QUERYHANDLE");
      break;
 case (SM_SETHANDLE):
      strcpy(buffer,"SM_SETHANDLE");
      break;

/* Title bar control messages */
 case (TBM_QUERYHILITE):
      strcpy(buffer,"TBM_QUERYHILITE");
      break;
 case (TBM_SETHILITE):
      strcpy(buffer,"TBM_SETHILITE");
      break;
