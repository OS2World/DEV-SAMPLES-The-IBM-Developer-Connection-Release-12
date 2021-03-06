/*
 *  This file was generated by the SOM Compiler and Emitter Framework.
 *  Generated using: 
 *      SOM Emitter emitxtm: 2.38
 */

#ifndef SOM_Module_xfrmiter_Source
#define SOM_Module_xfrmiter_Source
#endif
#define XEmbeddedFramesIterator_Class_Source
#define VARIABLE_MACROS

#include "XFrmIter.xih"

#include "ordcoll.h"

SOM_Scope void  SOMLINK XEmbeddedFramesIteratorInitXEmbeddedFramesIterator(XEmbeddedFramesIterator *somSelf, 
                                                                            Environment *ev, 
                                                                           ODxOrderedCollection* ocol)
{
    XEmbeddedFramesIteratorData *somThis = XEmbeddedFramesIteratorGetData(somSelf);
    XEmbeddedFramesIteratorMethodDebug("XEmbeddedFramesIterator","XEmbeddedFramesIteratorInitXEmbeddedFramesIterator");

    _iter = ocol->CreateIterator();
}

SOM_Scope ODFrame*  SOMLINK XEmbeddedFramesIteratorFirst(XEmbeddedFramesIterator *somSelf, 
                                                          Environment *ev)
{
    XEmbeddedFramesIteratorData *somThis = XEmbeddedFramesIteratorGetData(somSelf);
    XEmbeddedFramesIteratorMethodDebug("XEmbeddedFramesIterator","XEmbeddedFramesIteratorFirst");

    return (ODFrame*) _iter->First();
}

SOM_Scope ODFrame*  SOMLINK XEmbeddedFramesIteratorNext(XEmbeddedFramesIterator *somSelf, 
                                                         Environment *ev)
{
    XEmbeddedFramesIteratorData *somThis = XEmbeddedFramesIteratorGetData(somSelf);
    XEmbeddedFramesIteratorMethodDebug("XEmbeddedFramesIterator","XEmbeddedFramesIteratorNext");

    return (ODFrame*) _iter->Next();
}

SOM_Scope ODBoolean  SOMLINK XEmbeddedFramesIteratorIsNotComplete(XEmbeddedFramesIterator *somSelf, 
                                                                   Environment *ev)
{
    XEmbeddedFramesIteratorData *somThis = XEmbeddedFramesIteratorGetData(somSelf);
    XEmbeddedFramesIteratorMethodDebug("XEmbeddedFramesIterator","XEmbeddedFramesIteratorIsNotComplete");

    return _iter->IsNotComplete();
}

SOM_Scope void  SOMLINK XEmbeddedFramesIteratorsomInit(XEmbeddedFramesIterator *somSelf)
{
    XEmbeddedFramesIteratorData *somThis = XEmbeddedFramesIteratorGetData(somSelf);
    XEmbeddedFramesIteratorMethodDebug("XEmbeddedFramesIterator","XEmbeddedFramesIteratorsomInit");

    XEmbeddedFramesIterator_parent_ODEmbeddedFramesIterator_somInit(somSelf);
    _iter = 0;
}

SOM_Scope void  SOMLINK XEmbeddedFramesIteratorsomUninit(XEmbeddedFramesIterator *somSelf)
{
    XEmbeddedFramesIteratorData *somThis = XEmbeddedFramesIteratorGetData(somSelf);
    XEmbeddedFramesIteratorMethodDebug("XEmbeddedFramesIterator","XEmbeddedFramesIteratorsomUninit");

    delete _iter;
    XEmbeddedFramesIterator_parent_ODEmbeddedFramesIterator_somUninit(somSelf);
}

