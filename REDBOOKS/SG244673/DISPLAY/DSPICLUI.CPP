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
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINSTDDRAG
#define INCL_WINWINDOWMGR
#define INCL_WINPOINTERS
#define INCL_DOS
#define INCL_WIN
#include <os2.h>

#include "od2iclui.h"

#include <ifont.hpp>
#include <ibase.hpp>
#include <ihandle.hpp>
#include <iwindow.hpp>
#include <iframe.hpp>
#include "dspiclui.hpp"

//------------------------------+
//  DispWindow constructor      |
//------------------------------+
DispWindow::DispWindow( IWindow* dw) :
             IMultiCellCanvas( idwin,dw,dw)
{
  IPresSpaceHandle   hps;

  hps = IPresSpaceHandle();
  ICommandHandler::handleEventsFor( this );
  IMouseClickHandler::handleEventsFor( this );

  setColor(IColor(IColor::cyan));


  output = new IStaticText(DSP_ID,this,this);
  output->setText("OPENDOC+ICLUI");

  IFont tempFont;                       
  tempFont.setOutline(true);
  tempFont.setPointSize(15);
  tempFont.setName("Helv");
  output->setFont(tempFont);            
  output->setAlignment(IStaticText::centerRight);
  addToCell(output,1,1);
  setRowHeight(1,5,true);
  setColumnWidth(1,10,true);

  output->setAutoDeleteObject();
  setAutoDeleteObject();
}

//------------------------------+
//  DispWindow commandhandler   |
//------------------------------+
Boolean DispWindow::command(ICommandEvent& evt)
{
  Boolean rc=false;
  return rc;

}

//------------------------------+
//  DispWindow commandhandler   |
//------------------------------+
Boolean DispWindow::mouseClicked( IMouseClickEvent& evt)
{
  Boolean rc=false;
  return rc;

}

HWND ICLWindow(HWND  hwnd)
{
  HWND   hwndRet = (HWND)0;
  IWindow* facetWin=new IWindow(hwnd);
  DispWindow* pmw = new DispWindow( facetWin );
  if(pmw)
     hwndRet=pmw->handle();
  return hwndRet;
}

void ICLSetOutput(HWND hwnd, char* pstr)
{
  DispWindow* DpInstance;
  long x;
  Boolean flag;
  ISize tempSize;
  IFont tempFont;               
  IPresSpaceHandle hps;
  DpInstance =(DispWindow*)IWindow::windowWithHandle(hwnd);
  if(DpInstance)
  {
     if(pstr)
     {
        hps=DpInstance->presSpace();
        tempSize=DpInstance->size();

        x=tempSize.height();
        tempSize.setWidth(x);

        tempFont.setName("Helv");
        tempFont.setPointSize(x/2);
        tempFont.setOutline(true);
        tempFont.beginUsingFont(hps);

        DpInstance->output->setFont(tempFont);
        DpInstance->output->setText(pstr);
        DpInstance->refresh(true);
     }
  }
}

