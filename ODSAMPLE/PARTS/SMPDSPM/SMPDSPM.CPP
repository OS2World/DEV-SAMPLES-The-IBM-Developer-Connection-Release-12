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
/*  Contains:  Sample OpenDoc dispatch monitor.  This monitor       */
/*             prints events to the pmprintf window application.    */
/*                                                                  */
/*  IBM Change History (most recent first):                         */
/*  121993 5/11/95 ced   First checked in.                          */
/*                                                                  */
/********************************************************************/

#ifndef SOM_Module_smpdspm_Source
#define SOM_Module_smpdspm_Source
#endif
#define SampleDispatchMonitor_Class_Source

#include "smpdspm.xih"
#include "messages.h"

#include <stdio.h>

extern "C" {
int pmprintf(const char* f, ...);
}

SOM_Scope ODBoolean  SOMLINK SampleDispatchMonitor__Dispatch(SampleDispatchMonitor *somSelf,
                                                              Environment *ev,
                                                             ODEventData* event,
                                                             ODEventInfo* eventInfo)
{
    /* SampleDispatchMonitorData *somThis = SampleDispatchMonitorGetData(somSelf); */
    SampleDispatchMonitorMethodDebug("SampleDispatchMonitor","SampleDispatchMonitor__Dispatch");

    const char *msg;
    char work[12];

    /* Try to find string for message id */
    const int array_size = sizeof(messages) / sizeof(message);

    /* Should replace this with b-tree search since array is already sorted. */
    for (int i=0; i<array_size; i++)
       if (messages[i].id == event->msg) break;

    if (i < array_size)
       msg = messages[i].text;
    else {
       sprintf(work, "%08X", event->msg);
       msg = work;
    }


    pmprintf("hwnd=%08X, msg=%s, mp1=%08X, mp2=%08X\n",
             event->hwnd,
             msg,
             event->mp1,
             event->mp2);

    return kODFalse;
}

