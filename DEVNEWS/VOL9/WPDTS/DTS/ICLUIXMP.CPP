
/*
 *  MODULE: ICLUIXMP.CPP
 *
 *  DESCRIPTION: Main source code for the Iclui-WPS example using
 *               Direct-to-SOM
 *
 *  Author: Alan Auerbach (auerbach@vnet.ibm.com)
 */

#define INCL_GPI
#include "icluixmp.hh"
#include "icluixmp.hpp"
#include <inotebk.hpp>
#include "icluixmp.h"

/******************************************************************************/
/*  Global Data                                                               */
/******************************************************************************/
HMODULE  hmod;                                // Module handle for our .DLL

/******************************************************************************/
/*                                                                            */
/*  The constructor for MyViewWindow                                          */
/*                                                                            */
/******************************************************************************/
MyViewWindow::MyViewWindow(IcluiExample *Object)
   : IFrameWindow(),                           // Default constructor is fine.
     tiPrimary(this, Object->wpQueryTitle()),  // Set the object title
     staVariable(IC_FRAME_CLIENT_ID, this, this) // Initialize the static text
{
   PVIEWITEM    pViewItem;                     // a Workplace Shell structure

   myObject = Object;                          // Tuck away the SOM object ptr

   myUseItem =                                 // Allocate object memory
      (PUSEITEM) myObject->wpAllocMem( sizeof(USEITEM) + sizeof(VIEWITEM), NULL);

   pViewItem = (PVIEWITEM) (myUseItem+1);      // Viewitem comes just after Useitem
   memset (myUseItem, 0, sizeof(USEITEM) + sizeof(VIEWITEM));

   myUseItem->type = USAGE_OPENVIEW;           // The usage is a VIEW
   pViewItem->view = OPEN_MYVIEW;              // The view is MYVIEW
   pViewItem->handle = handle();               // The handle is this.handle()

   myObject->wpAddToObjUseList(myUseItem);     // Tell the Shell

   myObject->wpRegisterView(handle(), "My View"); // Make me act like an object

   setAutoDeleteObject();                      // Let the C++ object be deleted
   setFocus();                                 // Give focus to myself
   show();                                     // Show myself
   refreshView();                              // Pick up correct instance var.

   staVariable.setAlignment(IStaticText::centerCenter); // center my text
   setClient(&staVariable);                    // just for thoroughness
}

/******************************************************************************/
/* MyViewWindow::refreshView                                                  */
/*                                                                            */
/*    Looks at the SOM object's instance variable and sets the static text    */
/*                                                                            */
/******************************************************************************/
void MyViewWindow::refreshView()
{
   if (myObject->wpQueryaVariable() == VARIABLE_VALUEA)
   {
      staVariable.setText("my variable is A");
   }
   else
   {
      staVariable.setText("my variable is B");
   }
}

/******************************************************************************/
/*  MyViewWindow::~MyViewWindow                                               */
/*                                                                            */
/*  The destructor is called automatically when the window is closed.         */
/*  Clean up the VIEWITEM record for this view.                               */
/*                                                                            */
/******************************************************************************/
MyViewWindow::~MyViewWindow()
{
   myObject->wpDeleteFromObjUseList(myUseItem);    // remove the useitem
   myObject->wpFreeMem((PBYTE)myUseItem);          // and free its memory
}

/******************************************************************************/
/*  SettingsPage constructor                                                  */
/******************************************************************************/
SettingsPage::SettingsPage(IcluiExample *Object, IWindow *notebook)
   : IFrameWindow( IResourceId (IDDLG_SETVARIABLE, IDynamicLinkLibrary(hmod)),
                   notebook, notebook),
     rbA(IDR_VALUEA, this),              // radio button for value "A"
     rbB(IDR_VALUEB, this),              // radio button for value "B"
     pbUndo(IDC_RESET, this),            // "Undo" push button
     pbDefault(IDC_DEFAULT, this)        // "Default" push button
{
   myObject = Object;                    // Tuck away the SOM object ptr
   aVariableInitial = myObject->wpQueryaVariable();  // get initial value
   if (aVariableInitial == VARIABLE_VALUEA)  // set initial radio button
   {
      rbA.select().setFocus();
   }
   else
   {
      rbB.select().setFocus();
   }
   ICommandHandler::handleEventsFor(this);  // set up command handler
   ISelectHandler::handleEventsFor(this);   // and select handler
}

/******************************************************************************/
/*  SettingsPage destructor -- just turn off handlers                         */
/******************************************************************************/
SettingsPage::~SettingsPage()
{
   ICommandHandler::stopHandlingEventsFor(this);  // turn off command handler
   ISelectHandler::stopHandlingEventsFor(this);   // turn off select handler
}

/******************************************************************************/
/* Settings page command handler                                              */
/******************************************************************************/
Boolean SettingsPage::command(ICommandEvent& cmdevt)
{
   switch(cmdevt.commandId())
   {
      case IDC_RESET:                       // UNDO button pushed.
           if (aVariableInitial == VARIABLE_VALUEA)
           {
              rbA.select();                 // Select the radio button "A"
           }
           else
           {
              rbB.select();                 // Select the radio button "B"
           }
           myObject->wpSetaVariable(aVariableInitial);  // go to initial value
           return true;

      case IDC_DEFAULT:
           rbA.select();                    // Our default is "A"
           myObject->wpSetaVariable(VARIABLE_VALUEA); // set instance variable
           return true;

      default:
           break;
   }
   return false;
}

/******************************************************************************/
/*  Select handler -- handle radio button selections                          */
/******************************************************************************/
Boolean SettingsPage::selected(IControlEvent& evt)
{
   switch (evt.controlId())            // which control was selected?
   {
      case IDR_VALUEA:                 // if "A", set the instance variable
           myObject->wpSetaVariable(VARIABLE_VALUEA);
           break;

      case IDR_VALUEB:                 // if "B", set the instance variable
           myObject->wpSetaVariable(VARIABLE_VALUEB);
           break;
   }
   return false;
}

/******************************************************************************/
/*  METHOD wpQueryaVariable                                                   */
/******************************************************************************/
ULONG  IcluiExample::wpQueryaVariable()
{
    return aVariable;       // return our instance variable's value
}

/******************************************************************************/
/*  METHOD wpSetaVariable                                                     */
/******************************************************************************/
void  IcluiExample::wpSetaVariable(ULONG value)
{
    aVariable = value;      // set the instance variable
    wpRefreshViews();       // Update our open views
    wpSaveDeferred();       // Make the change persistant
}

/******************************************************************************/
/*  METHOD wpRefreshViews                                                     */
/******************************************************************************/
void  IcluiExample::wpRefreshViews()
{
    PVIEWITEM  pViewItem;           // That WPS data structure again
    MyViewWindow *aview;

    pViewItem = wpFindViewItem(VIEW_ANY, NULL);  // Loop through views
    while (pViewItem)               // Do we have one?
    {
       if (pViewItem->view == OPEN_MYVIEW)  // Is it our open view?
       {
          /*
           * If the view type is MYVIEW, we know that the handle
           * belongs to an instance of "MyViewWindow".
           */
          aview = (MyViewWindow*) IWindow::windowWithHandle(pViewItem->handle);
          if (aview)  // Make sure we got one.
          {
             aview->refreshView();  // and tell it to update itself.
          }
       }
       pViewItem = wpFindViewItem(VIEW_ANY, pViewItem);  // Next??
    }
}


/******************************************************************************/
/*  METHOD wpModifyPopupMenu                                                  */
/*                                                                            */
/*    Adds the menu item for "My View"                                        */
/*                                                                            */
/******************************************************************************/
BOOL  IcluiExample::wpModifyPopupMenu( HWND hwndMenu, HWND hwndCnr,
                                             ULONG iPosition)
{
    wpInsertPopupMenuItems( hwndMenu, 0,
                               hmod, ID_MYVIEWMENU, WPMENUID_OPEN);

    return WPAbstract::wpModifyPopupMenu( hwndMenu, hwndCnr, iPosition);
}

/******************************************************************************/
/*  METHOD wpAddSettingsPages                                                 */
/******************************************************************************/
BOOL  IcluiExample::wpAddSettingsPages( HWND hwndNotebook)
{
    PAGEINFO        pageinfo;          // Another WPS structure
    SettingsPage    *aSettingsPage;

    WPAbstract::wpAddSettingsPages(hwndNotebook); // Add parent pages first

    IWindow  *notebook = new IWindow(hwndNotebook);  // Wrap the Notebook
    notebook->setAutoDeleteObject();                 // ...for convenience

    aSettingsPage = new SettingsPage(this, notebook); // construct ours
    aSettingsPage->setAutoDeleteObject();            // ...for convenience

    memset(&pageinfo,0,sizeof(PAGEINFO));
    pageinfo.cb                 = sizeof(PAGEINFO);
    pageinfo.usPageStyleFlags   = BKA_MAJOR;
    pageinfo.usPageInsertFlags  = BKA_FIRST;
    pageinfo.pszName            = "ICLUI";
    pageinfo.pCreateParams      = this;
    pageinfo.usSettingsFlags    = SETTINGS_PAGE_NUMBERS;
    pageinfo.hwndPage           = aSettingsPage->handle(); // <-- this is the key.

    wpInsertSettingsPage( hwndNotebook, &pageinfo );
    return TRUE;
}

/******************************************************************************/
/*  METHOD wpRestoreState                                                     */
/******************************************************************************/
BOOL  IcluiExample::wpRestoreState( ULONG ulReserved)
{
    WPAbstract::wpRestoreState( ulReserved);  // call parent before
    wpRestoreLong("IcluiExample",       // restore our instance var.
                   ICLUIXMP_VARIABLE,
                   &aVariable);
    return TRUE;
}

/******************************************************************************/
/*  METHOD wpSaveState                                                        */
/******************************************************************************/
BOOL  IcluiExample::wpSaveState()
{
    wpSaveLong("IcluiExample",           // save our instance var.
                ICLUIXMP_VARIABLE,
                aVariable);

    return WPAbstract::wpSaveState();      // call parent after
}

/******************************************************************************/
/*  METHOD wpOpen                                                             */
/******************************************************************************/
HWND  IcluiExample::wpOpen(HWND hwndCnr, ULONG ulView, ULONG param)
{
    switch (ulView)
    {
       case OPEN_MYVIEW:       // is it our view?
            {
               MyViewWindow *aWindow = new MyViewWindow( this );
               return aWindow->handle();
            }

       default:                // otherwise just call parent.
            return WPAbstract::wpOpen( hwndCnr, ulView, param);
    }
}

/******************************************************************************/
/*  METHOD wpMenuItemSelected                                                 */
/******************************************************************************/
BOOL  IcluiExample::wpMenuItemSelected( HWND hwndFrame,
                                              ULONG ulMenuId)
{
    BOOL  rc;
    switch (ulMenuId)
    {
       case ID_MYVIEW:       // Does someone want our view?
            /*
             *  This is the way to show it:
             */
            wpViewObject(NULLHANDLE, ulMenuId, 0);
            break;

       default:
            break;
    }

    return WPAbstract::wpMenuItemSelected( hwndFrame, ulMenuId);
}

/******************************************************************************/
/*  METHOD wpclsInitData                                                      */
/*                                                                            */
/*     Perform class initialization.                                          */
/*                                                                            */
/******************************************************************************/
void  M_IcluiExample::wpclsInitData()
{
    M_WPAbstract::wpclsInitData();             // call parent first.

    DosQueryModuleHandle("ICLUIXMP", &hmod);  // Store away our module handle.
}

/******************************************************************************/
/*  METHOD wpclsQueryTitle                                                    */
/*                                                                            */
/*    Return our class' Title                                                 */
/*                                                                            */
/******************************************************************************/
PSZ  M_IcluiExample::wpclsQueryTitle()
{
    return "IBM Open Class: User Interface Example";
}

/******************************************************************************/
/*  METHOD wpclsQueryDefaultView                                              */
/*                                                                            */
/*      Return our class' default view.                                       */
/*                                                                            */
/******************************************************************************/
ULONG  M_IcluiExample::wpclsQueryDefaultView()
{
    return OPEN_MYVIEW;
}

/******************************************************************************/
/*  METHOD wpclsQueryStyle                                                    */
/*                                                                            */
/*     return our class style...OUR class is templatable.                     */
/*                                                                            */
/******************************************************************************/
ULONG  M_IcluiExample::wpclsQueryStyle()
{
    return (M_WPAbstract::wpclsQueryStyle() & ~CLSSTYLE_NEVERTEMPLATE);
}

/******************************************************************************/
/*  METHOD wpclsCreateDefaultTemplates                                        */
/*                                                                            */
/*      But don't create a default template.                                  */
/*                                                                            */
/******************************************************************************/
BOOL  M_IcluiExample::wpclsCreateDefaultTemplates( WPObject* Folder)
{
    return TRUE;      // causes template to *not* be created in templates folder.
}

