/* ******************************************************************** *
 * Copyright International Business Machines Corporation 1995, 1996     *
 * DISCLAIMER OF WARRANTIES.                                            *
 * The following [enclosed] code is sample code created by IBM          *
 * Corporation. This sample code is not part of any standard or IBM     *
 * product and is provided to you solely for the purpose of assisting   *
 * you in the development of your applications.  The code is provided   *
 * "AS IS". IBM MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT  *
 * NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS *
 * FOR A PARTICULAR PURPOSE, REGARDING THE FUNCTION OR PERFORMANCE OF   *
 * THIS CODE.  IBM shall not be liable for any damages arising out of   *
 * your use of the sample code, even if they have been advised of the   *
 * possibility of such damages.                                         *
 *                                                                      *
 * DISTRIBUTION.                                                        *
 * This sample code can be freely distributed, copied, altered, and     *
 * incorporated into other software, provided that it bears the above   *
 * Copyright notice and DISCLAIMER intact.                              *
 * -------------------------------------------------------------------- *
 * This file is part of the source for OpenDoc parts document in:       *
 *  IBM International Technical Support Organization Bulletin (Redbook) *
 *  "OpenDoc : An Introduction to Part Development" (SG24-4673)         *
 *                                                                      *
 * ******************************************************************** *
 */
// This file was obtained from the sample parts delivered
// with the DevCon 8 versionof the OpenDoc Toolkit.

#ifndef SOM_Module_xfrmiter_Source
#define SOM_Module_xfrmiter_Source
#endif
#define XEmbeddedFramesIterator_Class_Source
#define VARIABLE_MACROS

#include "XFrmIter.xih"

#include "ordcoll.h"

SOM_Scope void  SOMLINK InitXEmbeddedFramesIterator(XEmbeddedFramesIterator *somSelf, 
                                                    Environment *ev, 
                                                    ODxOrderedCollection* ocol)
{
    XEmbeddedFramesIteratorData *somThis = XEmbeddedFramesIteratorGetData(somSelf);
    XEmbeddedFramesIteratorMethodDebug("XEmbeddedFramesIterator","InitXEmbeddedFramesIterator");

    _iter = ocol->CreateIterator();
}

SOM_Scope ODFrame*  SOMLINK First(XEmbeddedFramesIterator *somSelf, 
                                  Environment *ev)
{
    XEmbeddedFramesIteratorData *somThis = XEmbeddedFramesIteratorGetData(somSelf);
    XEmbeddedFramesIteratorMethodDebug("XEmbeddedFramesIterator","First");

    return (ODFrame*) _iter->First();
}

SOM_Scope ODFrame*  SOMLINK Next(XEmbeddedFramesIterator *somSelf, 
                                 Environment *ev)
{
    XEmbeddedFramesIteratorData *somThis = XEmbeddedFramesIteratorGetData(somSelf);
    XEmbeddedFramesIteratorMethodDebug("XEmbeddedFramesIterator","Next");

    return (ODFrame*) _iter->Next();
}

SOM_Scope ODBoolean  SOMLINK IsNotComplete(XEmbeddedFramesIterator *somSelf, 
                                           Environment *ev)
{
    XEmbeddedFramesIteratorData *somThis = XEmbeddedFramesIteratorGetData(somSelf);
    XEmbeddedFramesIteratorMethodDebug("XEmbeddedFramesIterator","IsNotComplete");

    return _iter->IsNotComplete();
}

SOM_Scope void  SOMLINK somInit(XEmbeddedFramesIterator *somSelf)
{
    XEmbeddedFramesIteratorData *somThis = XEmbeddedFramesIteratorGetData(somSelf);
    XEmbeddedFramesIteratorMethodDebug("XEmbeddedFramesIterator","somInit");

    XEmbeddedFramesIterator_parent_ODEmbeddedFramesIterator_somInit(somSelf);
    _iter = 0;
}

SOM_Scope void  SOMLINK somUninit(XEmbeddedFramesIterator *somSelf)
{
    XEmbeddedFramesIteratorData *somThis = XEmbeddedFramesIteratorGetData(somSelf);
    XEmbeddedFramesIteratorMethodDebug("XEmbeddedFramesIterator","somUninit");

    delete _iter;
    XEmbeddedFramesIterator_parent_ODEmbeddedFramesIterator_somUninit(somSelf);
}

