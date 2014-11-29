/********************************************************************/
/*  Licensed Materials - Property of IBM                            */
/*                                                                  */
/*                                                                  */
/* Copyright (C) International Business Machines Corp., 1994.       */
/* Copyright (C) Apple Computer, Inc., 1994                         */
/*                                                                  */
/*  US Government Users Restricted Rights -                         */
/*  Use, duplication, or disclosure restricted                      */
/*  by GSA ADP Schedule Contract with IBM Corp.                     */
/*                                                                  */
/*  Contains: Shell Plug-In intallation routine for the sample      */
/*            dispatch monitor.                                     */
/*                                                                  */
/*  IBM Change History (most recent first):                         */
/*  143654 11/13/95 ced   Apple DR4 IDL changes eliminated          */
/*                        ODShellPlugIn class.                      */
/*  140030 10/12/95 jso   Added CreateShellPlugIn method            */
/*  121993 5/11/95  ced   First checked in                          */
/*                                                                  */
/********************************************************************/

#define INCL_WIN
#define INCL_ODAPI
#define INCL_ODDRAFT
#define INCL_ODCONTAINER
#define INCL_ODDOCUMENT
#define INCL_ODSTORAGESYSTEM
#define INCL_ODSESSION
#define INCL_ODDISPATCHER
#define INCL_ODDISPATCHMODULE
#include <os2.h>

#ifndef _SHPLUGIN_
#include <shplugin.h>
#endif

#include "smpdspm.xh"

/* Define event types handled by Standard Dispatch Module */

static const ODEventType StdEventTypes[] = {
      WM_BUTTON1DOWN
     ,WM_BUTTON2DOWN
     ,WM_BUTTON3DOWN
     ,WM_BUTTON1UP
     ,WM_BUTTON2UP
     ,WM_BUTTON3UP
     ,WM_MOUSEMOVE
     ,WM_CHORD
     ,WM_BUTTON1MOTIONSTART
     ,WM_BUTTON1MOTIONEND
     ,WM_BUTTON2MOTIONSTART
     ,WM_BUTTON2MOTIONEND
     ,WM_BUTTON3MOTIONSTART
     ,WM_BUTTON3MOTIONEND
     ,WM_CONTEXTHELP
     ,WM_BUTTON1CLICK
     ,WM_BUTTON2CLICK
     ,WM_BUTTON3CLICK
     ,WM_BUTTON1DBLCLK
     ,WM_BUTTON2DBLCLK
     ,WM_BUTTON3DBLCLK
     ,WM_BEGINDRAG
     ,WM_ENDDRAG
     ,WM_SINGLESELECT
     ,WM_CONTEXTMENU
     ,WM_BEGINSELECT
     ,WM_ENDSELECT
     ,WM_OPEN
     ,WM_TEXTEDIT
     ,WM_CHAR
     ,WM_PAINT
     ,WM_ACTIVATE
     ,WM_COMMAND
     ,WM_INITMENU
     ,WM_TIMER
     ,WM_SIZE
     ,WM_RENDERFMT
     ,DM_DRAGOVER
     ,DM_DRAGLEAVE
     ,DM_DROP
     ,DM_RENDER
     ,DM_RENDERCOMPLETE
     ,DM_ENDCONVERSATION
     ,WM_MENUSELECT
     ,WM_DRAWITEM
     ,WM_MEASUREITEM
     ,OD_HELP
};

const ODULong NumStdEventTypes = sizeof(StdEventTypes) / sizeof(ODEventType);

extern "C" {
APIRET APIENTRY MyShellPlugInInstallProc( Environment*, ODDraft*, ODShellPlugInActionCodes*);
}

APIRET APIENTRY MyShellPlugInInstallProc( Environment* ev,
    ODDraft* draft, ODShellPlugInActionCodes* action)
{
    ODSession* session = draft->GetDocument(ev)->GetContainer(ev)->GetStorageSystem(ev)->GetSession(ev);
    ODDispatcher* dispatcher = session->GetDispatcher(ev);

    ODDispatchModule* dspmod = new SampleDispatchMonitor;
    dspmod->InitDispatchModule(ev, session);
    for (int i = 0; i < NumStdEventTypes; i++)
    {
       dispatcher->AddMonitor(ev, StdEventTypes[i], dspmod);
    }
    return 0;
}

