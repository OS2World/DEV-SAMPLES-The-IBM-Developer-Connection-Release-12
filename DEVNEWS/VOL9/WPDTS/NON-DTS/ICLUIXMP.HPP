/******************************************************************************/
/*                                                                            */
/*   MODULE: ICLUIXMP.HPP                                                     */
/*                                                                            */
/*   Includes for ICLUI classes I use                                         */
/*                                                                            */
/******************************************************************************/
#include <iframe.hpp>                       // Frame Window
#include <ititle.hpp>                       // Title Bar
#include <istattxt.hpp>                     // Static Text
#include <icmdhdr.hpp>                      // Command Handler
#include <iselhdr.hpp>                      // Select Handler
#include <iradiobt.hpp>                     // Radio Button
#include <ipushbut.hpp>                     // Push Button
#include <itrace.hpp>                       // Trace

/******************************************************************************/
/*  Class MyViewWindow - Describes the custom object view.                    */
/******************************************************************************/
class MyViewWindow : public IFrameWindow
{
public:
     MyViewWindow (IcluiExample  *Object);     // constructor
     ~MyViewWindow();                       // destructor
     void refreshView();                    // member function to refresh
                                            //   display of instance variable

private:
     IStaticText       staVariable;         // Static Text field showing value
     IcluiExample     *myObject;            // Hold a pointer to the SOM object
     ITitle            tiPrimary;           // Title Bar
     PUSEITEM          myUseItem;           // Save the USEITEM pointer for this
                                            //   view.
};


/******************************************************************************/
/*  Class SettingsPage - describes the custom settings page for this object   */
/******************************************************************************/
class SettingsPage : public IFrameWindow   ,
                     public ICommandHandler,
                     public ISelectHandler
{
public:
     SettingsPage (IcluiExample  *Object, IWindow *notebook);
     ~SettingsPage();

protected:
     Boolean command(ICommandEvent& cmdevt);
     Boolean selected(IControlEvent& evt);

private:
     IcluiExample     *myObject;            // Hold a pointer to the SOM object
     IRadioButton      rbA;                 // Radio Button for "A" value
     IRadioButton      rbB;                 // Radio Button for "B" value
     IPushButton       pbUndo;              // "Undo"  push button
     IPushButton       pbDefault;           // "Default" push button
     ULONG             aVariableInitial;    // Initial state of instance variable
                                            // for Undo processing
};
