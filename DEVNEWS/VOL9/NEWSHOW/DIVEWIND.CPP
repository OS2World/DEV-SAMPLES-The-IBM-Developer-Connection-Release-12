
// divewind.cpp
//
//    This file contains the class implementations for divewind.hpp.


#ifndef _DIVEWIND_CPP_
#define _DIVEWIND_CPP_


// os2 & mm h

#include <os2.h>
#include <dive.h>

// ioc h

#include <iwindow.hpp>
#include <iframe.hpp>
#include <ihandler.hpp>
#include <icanvas.hpp>
#include <ithread.hpp>

// my h

#include "dive.hpp"
#include "divehdlr.hpp"
#include "divewind.hpp"



DiveWindow::DiveWindow( unsigned long windowID, IWindow *parent, IWindow *owner )
           :ICanvas( windowID, parent, owner )
{
   myImage[0] = myDive.loadImage( "tpg20000.bmp" );
   myImage[1] = myDive.loadImage( "tpg20001.bmp" );
   myImage[2] = myDive.loadImage( "tpg20002.bmp" );
   myImage[3] = myDive.loadImage( "tpg20003.bmp" );
   myImage[4] = myDive.loadImage( "tpg20004.bmp" );
   myImage[5] = myDive.loadImage( "tpg20005.bmp" );
   myImage[6] = myDive.loadImage( "tpg20006.bmp" );
   myImage[7] = myDive.loadImage( "tpg20007.bmp" );
   myImage[8] = myDive.loadImage( "tpg20008.bmp" );
   myImage[9] = myDive.loadImage( "tpg20009.bmp" );
   myImage[10] = myDive.loadImage( "tpg20010.bmp" );
   myImage[11] = myDive.loadImage( "tpg20011.bmp" );
   myImage[12] = myDive.loadImage( "tpg20012.bmp" );
   myImage[13] = myDive.loadImage( "tpg20013.bmp" );
   myImage[14] = myDive.loadImage( "tpg20014.bmp" );
   myImage[15] = myDive.loadImage( "tpg20015.bmp" );


   IDiveHandler::handleEventsFor( this );

   paintThread.start( new IThreadMemberFn<DiveWindow>(*this, paintMe) );
   paintThread.setPriority( IApplication::idleTime, 31 );
}



void DiveWindow::paintMe()
{
   int i = 0;

   while( true )
   {
      if ( pleaseBlit )
      {
         myDive.blitImage( myImage[i++] );
         if ( i > 15 )
            i = 0;
      }
   ICurrentThread::current().sleep( 0 );
   }
}




StandardWindow::StandardWindow()
               :IFrameWindow( "NewShow: A C++ Dive class sample.", 1001 )
{
     pDiveWindow = new DiveWindow( 1002, this, this );

     setClient( pDiveWindow );
     sizeTo( frameRectFor(IRectangle(pDiveWindow->getSize().width(),
                                     pDiveWindow->getSize().height())).size() );
     setFocus();
     show();

     sendEvent( IEvent(pDiveWindow, WM_VRNENABLED, 0, 0) );
}



StandardWindow::~StandardWindow()
{
   delete pDiveWindow;
}



#endif

