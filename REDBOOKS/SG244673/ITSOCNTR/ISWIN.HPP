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
#ifndef ISWIN_HPP
#define ISWIN_HPP

#include <icheckbx.hpp>                /* C++ user interface library   */
#include <icmdhdr.hpp>                 /* headers                      */
#include <ientryfd.hpp>
#include <iframe.hpp>
#include <ilistbox.hpp>
#include <imcelcv.hpp>
#include <imenubar.hpp>
#include <inotebk.hpp>
#include <ipushbut.hpp>
#include <iselhdr.hpp>
#include <isplitcv.hpp>
#include <istattxt.hpp>
#include <ititle.hpp>

class MainDialog;

class MainWindow : public IFrameWindow,
                   public ICommandHandler
{
  public:
    MainWindow(unsigned long ulWndId);
    ~MainWindow();

    virtual Boolean         command(ICommandEvent& event);


  private:
    ITitle                  *titleBar;

    IMenuBar                *menuBar;

    MainDialog              *mainDialog;
};

class MainDialog : public IFrameWindow,
                   public ICommandHandler
{
  public:
    MainDialog();
    MainDialog(IWindow *ownerWnd);
    ~MainDialog();

    virtual Boolean command(ICommandEvent& event);
};

#endif

