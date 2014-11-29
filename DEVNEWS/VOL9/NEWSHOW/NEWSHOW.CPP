
// newshow.cpp
//
//    This sample illustrates the usage of the new DIVE APIs in a
//    C++ class.  The sample is meant to duplicate the functionality
//    of the TOOLKIT\SAMPLES\MM\DIVE\SHOW.EXE sample, although the
//    use of a class is designed to make the sample much easier to
//    understand, while maintaining the speed of the original C sample code.
//
//    (c)1995 IBM
//
//    started:     07/12/95
//    finished:    07/24/95
//
//    newshow sample and DIVE class written by Larry Kyrala
//    DIVE handler class written by Maria Ingold
//    1AM debug assistance, thanks to Mike Kaply!  :)


// os2 & mm h

#include <os2.h>
#include <dive.h>

// ioc h

#include <iwindow.hpp>
#include <iframe.hpp>
#include <ihandler.hpp>
#include <icanvas.hpp>
#include <ithread.hpp>

#include <ibase.hpp>
#include <iapp.hpp>

// my h

#include "dive.hpp"
#include "divehdlr.hpp"
#include "divewind.hpp"


main() {
   StandardWindow  myDiveWindow;

   IApplication::current().run();
}


