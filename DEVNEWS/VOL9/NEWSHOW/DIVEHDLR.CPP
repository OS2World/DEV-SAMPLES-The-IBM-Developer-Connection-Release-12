
// divehdlr.cpp
//
//    This is the class implementation.  Code originally by Maria Ingold,
//    minor modifications by Larry Kyrala.


#ifndef _DIVEHDLR_CPP_
#define _DIVEHDLR_CPP_

// os2 h

#define INCL_WIN
#include <os2.h>

// ioc h

#include <ihandler.hpp>
#include <iframe.hpp>



#include "divehdlr.hpp"


IDiveHandler& IDiveHandler::handleEventsFor( IWindow *frame )
{
   WinSetVisibleRegionNotify( frame->handle(), TRUE );
   this->IHandler::handleEventsFor( frame );
   return( *this );
}


IDiveHandler& IDiveHandler::stopHandlingEventsFor( IWindow *frame )
{
   WinSetVisibleRegionNotify( frame->handle(), FALSE );
   this->IHandler::stopHandlingEventsFor( frame );
   return( *this );
}


Boolean IDiveHandler::dispatchHandlerEvent( IEvent &event )
{
   switch (event.eventId())
   {
      case WM_VRNDISABLED:
         return( DiveDisabled( event ) );
         break;

      case WM_VRNENABLED:
         return( DiveEnabled( event ) );
         break;

      default:
         break;
   }
   return(false);
}


#endif

