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
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES
#define INCL_GPIREGIONS
#define INCL_GPITRANSFORMS
#define INCL_GPILCIDS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINSTDDRAG
#define INCL_WINWINDOWMGR
#define INCL_WINPOINTERS
#define INCL_WINMENUS
#define INCL_DOSMODULEMGR

#include <iapp.hpp>                    /* C++ user interface library   */
#include <icheckbx.hpp>                /* headers                      */
#include <icmdhdr.hpp>              
#include <ientryfd.hpp>
#include <ifont.hpp>
#include <ifontdlg.hpp>
#include <ilistbox.hpp>
#include <imcelcv.hpp>
#include <imenubar.hpp>
#include <imsgbox.hpp>
#include <inotebk.hpp>
#include <ipushbut.hpp>
#include <irect.hpp>
#include <ireslib.hpp>
#include <iselhdr.hpp>
#include <isplitcv.hpp>
#include <istattxt.hpp>
#include <istring.hpp>
#include <ititle.hpp>

#include <os2.h>
#include <stdlib.h>                    /* C library headers            */

#include "iswin.hpp"

#include "od2iclui.h"
#include "itsorc.h"

MainWindow  :: MainWindow(unsigned long ulWindowId)
             : IFrameWindow(IFrameWindow::defaultStyle() |
                            IFrameWindow::minimizedIcon,
                            ulWindowId)
{
  titleBar = new ITitle(this);

  titleBar->setObjectText(IApplication::current().userResourceLibrary().
                          loadString(IDS_TITLE));

  menuBar = new IMenuBar(IDM_MENU, 
                         this);

  ICommandHandler::handleEventsFor(this); 
                                          
  IFrameWindow::show(); 

  return;
} 

MainWindow :: ~MainWindow()
{
  return;
}

MainWindow :: command(ICommandEvent& event)
{
  switch (event.commandId())
  {
    case IDM_HELP_ABOUT:
      mainDialog = new MainDialog(this);
 
      mainDialog->showModally();

      delete mainDialog;

      break;

    case IDM_HELP_MESSAGE:
    {
      IMessageBox msgBox(this);

      msgBox.setTitle(IDS_MESSAGE_TITLE);

      IString msgString =
          IApplication::current().userResourceLibrary().
            loadString(IDS_MESSAGE_STRING);

      msgBox.show(msgString,
                  IMessageBox :: okButton         |
                  IMessageBox :: errorIcon        |
                  IMessageBox :: applicationModal);
    }

      break;
    
    default:
      break;
  } 

  return(false);
} 

MainDialog :: MainDialog(IWindow *ownerWnd)
            : IFrameWindow(IDD_ABOUT, ownerWnd)
{
  handleEventsFor(this);              /* Set event handler for this    */
                                      /* dialog                        */

  return;
} 

MainDialog :: MainDialog()
            : IFrameWindow(IDD_ABOUT)
{
  handleEventsFor(this);              /* Set event handler for this    */
                                      /* dialog                        */

  return;
} 

MainDialog :: ~MainDialog()
{
  return;
}

MainDialog :: command(ICommandEvent& event)
{
  switch (event.commandId())
  {
    case IDD_ABOUT_OK:
      dismiss(IDD_ABOUT_OK); 

      break;

    default:
      break;
  } 

  return(false);
} 

VOID ISWindow()
{
  IApplication::current().setUserResourceLibrary("ITSOCNTR");

  MainWindow* pmw = new MainWindow(ICON_VIEW_ID);
}

VOID ISDialog()
{
  MainDialog *mainDialog;

  IApplication::current().setUserResourceLibrary("ITSOCNTR");

  mainDialog = new MainDialog();
 
  mainDialog->showModally();

  delete mainDialog;
}

