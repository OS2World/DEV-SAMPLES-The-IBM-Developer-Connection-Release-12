#define INCL_WIN
#include <os2.h>


/* For the following four routines, switch on the panel id and perform */
/* some action */

/* This routine is executed when beginning a do it */

VOID _tutBeginDoIt( USHORT usPanelID)

{
   switch (usPanelID) {
      default:
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Begin Do It!","",0,0);
        break;
      } /* endswitch */
   return;
}

/* This routine is executed when exiting a do it */

VOID _tutEndDoIt( USHORT usPanelID)

{
   switch (usPanelID) {
      default:
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "End Do It!","",0,0);
        break;
      } /* endswitch */
   return;
}

/* This routine is executed when beginning practice */

VOID _tutBeginPractice( USHORT usPanelID)

{
   switch (usPanelID) {
      default:
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Begin Practice","",0,0);
        break;
      } /* endswitch */
   return;
}

/* This routine is executed when exiting practice */

VOID _tutEndPractice( USHORT usPanelID)

{
   switch (usPanelID) {
      default:
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "End Practice","",0,0);
        break;
      } /* endswitch */
   return;
}

/* If you do not want Do It!, return FALSE */

BOOL _tutDoIt(VOID)
{
   return TRUE;
}

/* If you have multiple intro panels, return 0 here and make sure you */
/* specify a section name for your intros in the RC file. */

ULONG _tutQueryIntroPanel( ULONG ulSecondPanel)
{
   return ulSecondPanel;
}

/* If you don't want to have a Congratulations panel, return 0 */

ULONG _tutQueryCongratPanel( ULONG ulLastPanel )
{
   return ulLastPanel;
}

/* The title panel should be the first panel, so do not change this */

ULONG _tutQueryTitlePanel( ULONG ulFirstPanel )
{
   return ulFirstPanel;
}

/* The mouseover help can be on any panel.  Change it here */

ULONG _tutQueryMouseOverPanel( ULONG ulSecondPanel )
{
   return ulSecondPanel;
}


/* To change the height of the section buttons, change the return here */

ULONG _tutQuerySectionButtonHeight(ULONG ulDefaultHeight)
{
   return ulDefaultHeight; /* 23 */
}

/* To change the overall spacing of the tutorial, change the return here */

ULONG _tutQuerySpacing( ULONG ulDefaultSpacing)
{
   return ulDefaultSpacing; /* 3 */
}

/* This should be changed to indicate what panel in your help file is */
/* the actual help for the tutorial */

ULONG _tutQueryDefaultHelp(VOID)
{
   return 38;
}

/* This should be changed to indicate your help file */

PSZ _tutQueryHelpFile(VOID)
{
   return "TUTORIAL.HLP";
}

/* This is a helper routine */
/* Given an object ID, this routine opens that object */

VOID tutOpenObject( PCHAR pchObjectID )
{
   HOBJECT hobject;

   hobject = WinQueryObject(pchObjectID);
#define OPEN_DEFAULT 3
   WinOpenObject( hobject, OPEN_DEFAULT, TRUE);
   return;
}

