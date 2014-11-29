
/*******************************************************************************
 * File Name   : RIFFSAMP.C
 *
 * Description : This file contains the C source code required for the RIFF
 *               compound file sample program.  This sample allows the user
 *               to Add a wave file to a RIFF compound file.  The user can
 *               delete the wave element from the RIFF compound file, play
 *               the wave element from the RIFF compound file and compact
 *               the RIFF compound file by deleting the data for all deleted
 *               entries in Compound File Resource group of a compound file.
 *               There are two list boxes, one list box contains the names of
 *               wave files which can be added to the RIFF compound file.
 *               The other list box has the names of the elements in RIFF
 *               compound file.  The following abbreviations are used
 *               throughout the sample:
 *               CGRP - Compound File Resource Group
 *               CTOC - Compound File Table of Contents
 *
 *
 * Concepts    : This program illustrates how to create a RIFF compound file,
 *               how to Add a Wave file to RIFF compound file, how to play and
 *               delete elements from a Compound file and how to compact a
 *               RIFF file.  This program also illustrates how to use
 *               variable length structures.
 *
 * MMPM/2 API's: List of all MMPM/2 API's that are used in this module.
 *
 *               mciSendCommand
 *                  MCI_ACQUIREDEVICE
 *                  MCI_OPEN
 *                  MCI_PLAY
 *                  MCI_CLOSE
 *               mciGetErrorString
 *               mmioOpen
 *               mmioRead
 *               mmioClose
 *               mmioCFGetInfo
 *               mmioCFOpen
 *               mmioCFAddElement
 *               mmioCFDeleteEntry
 *               mmioCFCompact
 *
 *
 * Required
 *    Files    : riffsamp.c         Source Code.
 *               riffsamp.h         Include file.
 *               riffsamp.dlg       Dialog definition.
 *               riffsamp.rc        Resources.
 *               riffsamp.ipf       Help text.
 *               makefile           Make file.
 *               riffsamp.def       Linker definition file.
 *               riffsamp.ico       RiffSamp icon.
 *               riff1.wav          1st Wave File Played.
 *               riff2.wav          2nd Wave File Played.
 *               riff3.wav          3rd Wave File Played.
 *
 * Copyright (C) IBM  1993
 ******************************************************************************/

#define  INCL_DOS                   /* required to use Dos APIs.           */
#define  INCL_WIN                   /* required to use Win APIs.           */
#define  INCL_PM                    /* required to use PM APIs.            */
#define  INCL_OS2MM                 /* required for MCI and MMIO headers   */
#define  INCL_WINHELP               /* required to use IPF.                */
#define  INCL_SECONDARYWINDOW       /* required for secondary window       */

#include <os2.h>
#include <stdio.h>
#include <os2me.h>
#include <stdlib.h>

#define INCL_HEAP
#define INCL_DBCSCRT
#include <sw.h>
#include <swp.h>

#include "riffsamp.h"               /* Dialog Ids, Resource Ids etc        */

#define RIFF_COMPOUND_FILE "RIFFSAMP.BND"
#define HELP_FILE "riffsamp.HLP"
#define STRING_SIZE 356
/*******************************************************************/

/* Procedure / Function prototypes */

#pragma linkage( MainDialogProc, system)
MRESULT EXPENTRY MainDialogProc(HWND, ULONG, MPARAM, MPARAM);
void main(int i,char *p[]);
void InitializeDialog(void);
void Finalize(void);
VOID InitializeHelp( VOID );
USHORT ShowAMessage( USHORT usTitleId,
                     USHORT usTextId,
                     ULONG  ulMessageBoxStyle,
                     BOOL   fMCIError);
BOOL FillListBoxWithWaves(HWND hwnd);
void OpenRIFFCompoundFile(ULONG ulFlags);
BOOL AddElementToRIFF(PSZ achFileName,SHORT sFileNum,HWND hwnd);
BOOL DeleteEntryFromRIFF(PSZ pszElementName, SHORT sFile,HWND hwnd);
PMMCTOCENTRY GetCTOCEntryInfo(PSZ pszElementName);
BOOL CompactTheRIFFCompoundFile();
void  EnableButtons(HWND hwnd);
void PlayElement(HWND hwnd,PSZ pszElementName);
void CloseRiffCompoundFile(HWND hwnd);
/*******************************************************************/

/* Global Variables */

HAB      hab;                           /* anchor block handle           */
QMSG     qmsg;                          /* Message Que                   */
HMQ      hmq;
HWND     hwndDiag;                      /* Application Dialog handle     */
HWND     hwndFrame;                     /* Application Frame handle      */
BOOL     fPassedDevice = FALSE;         /* Do we own the Audio device    */
CHAR     achHelpTitle[STRING_SIZE];     /* Title for the Help            */
HWND     hwndHelpInstance;              /* Handle to Help window         */
HELPINIT hmiHelpStructure;              /* Help init. structure          */
BOOL     fPlayDisable  = FALSE;         /* Is Play pushbutton disabled   */
BOOL     fDelDisable   = FALSE;         /* Is Delete pushbutton disabled */
BOOL     fCompactDisable = FALSE;       /* Is Compact pushbutton disabled*/
BOOL     fAddDisable   = FALSE;         /* Is Add pushbutton disabled    */
USHORT   usWaveDeviceId;                /* Device Id to play             */
HMMIO    hmmioCF = (HMMIO)0;            /* Handle to RIFF compound file  */
BOOL     fRIFFExists  = FALSE;          /* If RIFF file is open          */



/******************************************************************************
 * Name         : main
 *
 * Description  : This function calls the Intialize procedure to prepare
 *                everything for the program's operation, enters the
 *                message loop, then call Finalize to shut everything down
 *                when the program is terminated.
 *
 * Concepts     : None.
 *
 * MMPM/2 API's : None.
 *
 * Parameters   : argc - Number of parameters passed into the program.
 *                argv - Command line parameters.
 *
 * Return       : None.
 *
 *************************************************************************/
void main(int argc, char *argv[])
{
InitializeDialog();

/* Handle the messages: */
while(WinGetMsg(hab,&qmsg,0,0,0))
        WinDispatchMsg(hab,&qmsg);

Finalize();

}

/*************************************************************************
 * Name         : InitializeDialog
 *
 * Description  : This function performs the necessary initializations and
 *                setups that are required to show/run a dialog box as a
 *                main window.  The message queue will be created, as will
 *                the dialog box.
 *
 * Concepts     : Secondary Windows is used to create and show a dialog box.
 *                A system clock is displayed till the application is loaded.
 *
 * MMPM/2 API's : WinLoadSecondaryWindow
 *                WinQuerySecondaryHWND
 *
 * Parameters   : None.
 *
 * Return       : None.
 *
 *************************************************************************/
void InitializeDialog()
{
CHAR achTitle[STRING_SIZE] = "";
CHAR achDefaultSize[STRING_SIZE] = "";


   /*
    * Setup and initialize the dialog window.
    * Change pointer to a waiting pointer first, since this might take a
    * couple of seconds.
    */


   WinSetPointer(
      HWND_DESKTOP,        /* Desktop window handle.                    */
      WinQuerySysPointer(  /* This API will give the handle of the PTR. */
         HWND_DESKTOP,     /* Desktop window handle.                    */
         SPTR_WAIT,        /* The waiting icon.                         */
         FALSE ) );        /* Return the system pointer's handle.       */

   /* Initialize the Dialog window */
   hab = WinInitialize(0);

   /* create a message queue for the window */
   hmq = WinCreateMsgQueue(hab,0);



   /* Load the Dialog - This will return the Handle to the Frame */
   hwndFrame =
       WinLoadSecondaryWindow(HWND_DESKTOP
                            ,HWND_DESKTOP
                            ,(PFNWP)MainDialogProc
                            ,(HMODULE)NULL
                            ,ID_DLG_MAIN
                            ,(PVOID)NULL);


   /*
    * By specifying the QS_DIALOG flag we will get the handle to the Dialog of
    * the frame
    */
   hwndDiag = WinQuerySecondaryHWND(hwndFrame,QS_DIALOG);

   /* Get the string for default size */
   WinLoadString(
      hab,
      (HMODULE) NULL,
      IDS_DEFAULT_SIZE,
      (SHORT) sizeof( achDefaultSize),
      achDefaultSize);
   /* Add Default Size menu item to system menu of the secondary window. */
   WinInsertDefaultSize(hwndFrame, achDefaultSize);

   /*
    * Get the window title string from the Resource string table
    * and set it as the window text for the dialog window.
    */
   WinLoadString(
      hab,
      (HMODULE) NULL,
      IDS_MAIN_WINDOW_TITLE,
      (SHORT) sizeof( achTitle),
      achTitle);

   /* Set the Title of the Dialog */
   WinSetWindowText( hwndFrame, achTitle);


   /*
    * Now that we're done with initialization, so we don't the need the waiting
    * icon, so we are going to change the pointer back to the arrow.
    */

   WinSetPointer(
      HWND_DESKTOP,        /* Desktop window handle.                    */
      WinQuerySysPointer(  /* This API will give the handle of the PTR. */
         HWND_DESKTOP,     /* Desktop window handle.                    */
         SPTR_ARROW,       /* The Arrow icon.                           */
         FALSE ) );        /* Return the system pointer's handle.       */

   /*
    * Initialize the help structure and associate the help instance to this
    * dialog via it's handle to anchor block.
    */

   InitializeHelp( );

}

/******************************************************************************
 * Name        : InitializeHelp
 *
 * Description : This procedure will set up the initial values in the global
 *               help structure.  This help structure will then be passed on
 *               to the Help Manager when the Help Instance is created.  The
 *               handle to the Help Instance will be kept for later use.
 *
 * Concepts    : None.
 *
 * MMPM/2 API's: None.
 *
 * Parameters  : None.
 *
 * Return      : None.
 *
 ******************************************************************************/
VOID InitializeHelp( VOID )
{
   /*  Get the size of the initialization structure. */
   hmiHelpStructure.cb              = sizeof( HELPINIT );

   hmiHelpStructure.ulReturnCode    = (ULONG) NULL; /* RC from initialization.*/
   hmiHelpStructure.pszTutorialName = (CHAR) NULL;  /* No tutorial program.   */

   hmiHelpStructure.phtHelpTable    = (PVOID)(0xffff0000 | ID_RIFFSAMP_HELPTABLE);

   /* The action bar is not used, so set the following to NULL. */
   hmiHelpStructure.hmodAccelActionBarModule = (HMODULE) NULL;
   hmiHelpStructure.idAccelTable             = (USHORT) NULL;
   hmiHelpStructure.idActionBar              = (USHORT) NULL;

   /* The Help window title. */
   WinLoadString(
      hab,                             /* HAB for this dialog box.            */
      (HMODULE) NULL,                  /* Get the string from the .exe file.  */
      IDS_HELP_WINDOW_TITLE,           /* Which string to get.                */
      (SHORT) STRING_SIZE,             /* The size of the buffer.             */
      achHelpTitle );                  /* The buffer to place the string.     */

   hmiHelpStructure.pszHelpWindowTitle  =
      achHelpTitle;

   /* The Help table is in the executable file. */
   hmiHelpStructure.hmodHelpTableModule = (HMODULE) NULL;

   /* The help panel ID is not displayed. */
   hmiHelpStructure.fShowPanelId       = (ULONG) NULL;

   /* The library that contains the RIFF Sample help panels. */
   hmiHelpStructure.pszHelpLibraryName  = HELP_FILE;

   /*
    * Create the Help Instance for IPF.
    * Give IPF the Anchor Block handle and address of the IPF initialization
    * structure, and check that creation of Help was a success.
    */
   hwndHelpInstance = WinCreateHelpInstance(
                         hab,                   /* Anchor Block Handle.       */
                         &hmiHelpStructure );   /* Help Structure.            */

   if ( hwndHelpInstance == (HWND) NULL )
   {
     ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_HELP_CREATION_ERROR,
           MB_OK | MB_INFORMATION | MB_MOVEABLE |  MB_APPLMODAL ,
           FALSE);
   }
   else
   {
      if ( hmiHelpStructure.ulReturnCode )
      {
       ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_HELP_CREATION_ERROR,
           MB_OK | MB_INFORMATION | MB_MOVEABLE |  MB_APPLMODAL ,
           FALSE);

      WinDestroyHelpInstance( hwndHelpInstance );
      }
   }  /* End of IF checking the creation of the Help Instance. */

   /* Associate the Help Instance of the IPF to this dialog window. */
   if ( hwndHelpInstance != (HWND) NULL )
   {
     WinAssociateHelpInstance(
        hwndHelpInstance,     /* The handle of the Help Instance.             */
        hwndFrame );          /* Associate to this dialog window.             */
   }

}  /* End of InitializeHelp */

/*************************************************************************
 * Name         : Finalize
 *
 * Description  : This routine is called after the message dispatch loop
 *                has ended because of a WM_QUIT message.  The code will
 *                destroy the help instance, messages queue, and window.
 *
 * Concepts     : None.
 *
 * MMPM/2 API's : None.
 *
 * Parameters   : None.
 *
 * Return       : None.
 *
 *************************************************************************/
VOID Finalize( VOID )
{
   /* Destroy the Help Instance for this dialog window. */
   if ( hwndHelpInstance != (HWND) NULL)
   {
      WinDestroyHelpInstance( hwndHelpInstance );
   }

   WinDestroySecondaryWindow( hwndFrame );
   WinDestroyMsgQueue( hmq );
   WinTerminate( hab );

}  /* End of Finalize */
/*******************************************************************************
 * Name        : MainDialogProc
 *
 * Description : This function controls the main dialog box.  It will handle
 *               received messages such as pushbutton notifications, Device
 *               sharing messages, etc
 *
 *               The following messages are handled specifically by this
 *               routine.
 *
 *                  WM_INITDLG
 *                  WM_CLOSE
 *                  WM_HELP
 *                  WM_COMMAND
 *                  MM_MCIPASSDEVICE
 *                  WM_ACTIVATE
 *
 *
 * Concepts    : This function illustrates the concept how the messages are
 *               handled to play, add, delete and Compact elements from a
 *               RIFF compound file.  A RIFF compound file is created when
 *               the application recieves the WM_INITDLG message.  Then
 *               appropriate functions are called when user presses
 *               ADD, DELETE, COMPACT and PLAY push buttons.  The concept
 *               of device sharing is also illustrated by this.
 *
 *
 *
 * MMPM/2 API's : mciSendCommand
 *                   MCI_ACQUIREDEVICE
 *
 * Parameters   : hwnd - Handle for the Main dialog box.
 *                msg  - Message received by the dialog box.
 *                mp1  - Parameter 1 for the just recieved message.
 *                mp2  - Parameter 2 for the just recieved message.
 *
 * Return       :
 *
 *
 ******************************************************************************/
MRESULT EXPENTRY MainDialogProc(
                HWND hwnd,
                 ULONG msg,
                 MPARAM mp1,
                 MPARAM mp2)
{
HPOINTER  hpProgramIcon;                /* handle to program's icon        */
ULONG     ulReturn;                     /* Ret Code for Dos and MCI calls  */
USHORT    usCommandMessage;             /* high-word                       */
SHORT     sFile;                        /* File Index Number to be added   */
CHAR      achFileName[CCHMAXPATH] = ""; /* File which user selected        */
static MCI_GENERIC_PARMS  lpWaveGeneric;/* Used for MCI_ACQUIREDEVICE      */



switch (msg)
  {



    case WM_CLOSE:
       WinPostMsg( hwnd, WM_QUIT, 0L, 0L );
       return ((MRESULT) NULL);

    case WM_HELP :
       /*
        * The dialog window has recieved a request for help from the user,
        * i.e., the Help pushbutton was pressed.  Send the HM_DISPLAY_HELP
        * message to the Help Instance with the IPF resource identifier
        * for the correct HM panel.  This will show the help panel for this
        * sample program.
        */
       WinSendMsg(
          hwndHelpInstance,
          HM_DISPLAY_HELP,
          MPFROMSHORT( 1 ),
          MPFROMSHORT( HM_RESOURCEID ) );
       return( 0 );

    case WM_INITDLG:
       /*
        * Initialize the dialog window.
        * Change pointer to a waiting pointer first, since this might take a
        * couple of seconds.
        */

       WinSetPointer(
          HWND_DESKTOP,        /* Desktop window handle.                    */
          WinQuerySysPointer(  /* This API will give the handle of the PTR. */
             HWND_DESKTOP,     /* Desktop window handle.                    */
             SPTR_WAIT,        /* The waiting icon.                         */
             FALSE ) );        /* Return the system pointer's handle.       */

       hpProgramIcon =
          WinLoadPointer(
             HWND_DESKTOP,
             (HMODULE) NULL, /* Where the resource is kept. (Exe file)      */
             ID_ICON );      /* Which icon to use.                          */

       WinDefSecondaryWindowProc(
          hwnd,              /* Dialog window handle.                       */
          WM_SETICON,        /* Message to the dialog.  Set it's icon.      */
          (MPARAM) hpProgramIcon,
          (MPARAM) 0 );      /* mp2 no value.                               */

       /*
        * Find all files with .wav extensions and fill the list box with
        * them.
        */
       FillListBoxWithWaves(hwnd);

       /* Disable Delete push button because there is no element to delete */
       WinEnableWindow(WinWindowFromID(hwnd,ID_PB_DEL),
                       FALSE);
       fDelDisable = TRUE;

       /* Disable Compact push button because there is no deleted elements */
       WinEnableWindow(WinWindowFromID(hwnd,ID_PB_COMPACT),
                       FALSE);
       fCompactDisable = TRUE;

       /* Disable play push button because there is no element to play */
       WinEnableWindow(WinWindowFromID(hwnd,ID_PB_PLAY),
                       FALSE);
       fPlayDisable = TRUE;

       /*
        * Create the RIFF compound file for reading and writing.  The Riff
        * compound file is only created once during initialization, but is
        * closed and opened whenever we compact the RIFF file.
        */
       OpenRIFFCompoundFile(MMIO_CREATE | MMIO_READWRITE);
       if (!fRIFFExists)
          {
          /* Disable Add push button because RIFF could not be created */
          WinEnableWindow(WinWindowFromID(hwnd,ID_PB_ADD),
                          FALSE);
          fAddDisable = TRUE;
          }

       return((MRESULT)NULL);

    /*  Service all of the button pushes */
    case WM_COMMAND:
       switch (SHORT1FROMMP(mp1)) {
         case ID_PB_ADD:
            if (fRIFFExists)
               {
               /*
                * Riff File Exists so Query what wave file the user has
                * selected to ADD to the RIFF compound file.  This call
                * will return back the Index of the List Box item selected.
                */
               sFile = (SHORT) WinSendMsg( WinWindowFromID( hwnd,
                                                ID_LB_ADD_FILE),
                                           LM_QUERYSELECTION,
                                           (MPARAM) LIT_FIRST,
                                           (MPARAM) NULL);

               /* No item was selected, so display an error and break */
               if (sFile == LIT_NONE)
                  {
                  ShowAMessage(
                              IDS_ERROR_TITLE,
                              IDS_NO_FILES_SELECTED,
                              MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
                              FALSE);

                  break;
                  }

               /* Query for the Actual text of the wave file */
               WinQueryLboxItemText(WinWindowFromID(hwnd,ID_LB_ADD_FILE),
                                    (LONG) sFile,
                                    achFileName,
                                    CCHMAXPATH);

               /*
                * Add the wave file which the user selected to the RIFF
                * compound file.  mmioCFAddElement is used to add an element.
                */
               if (AddElementToRIFF(
                             achFileName,
                             sFile,
                             hwnd))
                  {
                  /* Delete the wave file name from the Add file list box */
                  WinSendMsg( WinWindowFromID( hwnd, ID_LB_ADD_FILE),
                              LM_DELETEITEM,
                              (MPARAM) sFile,
                              (MPARAM) 0);

                  /* Select the first item in the Add file list box */
                  WinSendMsg( WinWindowFromID (hwnd, ID_LB_ADD_FILE),
                              LM_SELECTITEM,
                              (MPARAM) 0,
                              (MPARAM) TRUE);
                  /*
                   * Check if Add, Delete, Play or Compact buttons need to be
                   * enabled or disabled.
                   * Compact push button is enabled if the RIFF compound file
                   * contains at least one non-deleted entry and at least one
                   * deleted entry.  Otherwise it is disabled.
                   * Play push button is enabled if the RIFF compound file
                   * contains at least one non-deleted entry.  Otherwise it
                   * is disabled.
                   * Delete push button is enabled if the RIFF compound file
                   * contains at least one non-deleted entry.  Otherwise it is
                   * disabled
                   * Add push button is enabled if the Add list box contains
                   * at least one wave file.  Otherwise it is disabled
                   * The current status of these push buttons is maintained
                   * in their respective global variables e.g. fCompactDisable
                   *
                   */
                  EnableButtons(hwnd);
                  }
               }
            else
               {
               /* Riff was not created, so display an error */
               ShowAMessage(
                           IDS_ERROR_TITLE,
                           IDS_CANT_PROCESS_MESSAGE,
                           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
                           FALSE);
               }

         break;

         case ID_PB_DEL:
            if (fRIFFExists)
               {
               /*
                * Riff File Exists so Query what wave entry the user has
                * selected to be deleted from RIFF compound file.  This call
                * will return back the Index of the List Box item selected.
                */
               sFile = (SHORT) WinSendMsg( WinWindowFromID( hwnd, ID_LB_FILE_TOC),
                                           LM_QUERYSELECTION,
                                           (MPARAM) LIT_FIRST,
                                           (MPARAM) NULL);

               /* No item was selected, so display an error and break */
               if (sFile == LIT_NONE)
                  {
                  ShowAMessage(
                              IDS_ERROR_TITLE,
                              IDS_NO_CTOC_ENTRIES_SELECTED,
                              MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
                              FALSE);

                  break;
                  }

               /* Query for the actual text of the entry selected */
               WinQueryLboxItemText(WinWindowFromID(hwnd,ID_LB_FILE_TOC),
                                   (LONG) sFile,
                                   achFileName,
                                   CCHMAXPATH);

               /*
                * Delete the entry which the user selected from the RIFF
                * compound file.  When the entry is deleted from a RIFF file
                * the function deletes a compound file table of contents (CTOC)
                * entry from an open RIFF compound file.  The data is still
                * present in the CGRP of RIFF file.  In order to delete the
                * data a mmioCFCompact or mmioCFCopy API should be used.
                */
               if (DeleteEntryFromRIFF(achFileName,
                                       sFile,
                                       hwnd))
                  {
                  /*
                   * Entry was deleted succesfull from CTOC of RIFF, so
                   * Delete the entry name from the TOC list box.
                   */
                  WinSendMsg( WinWindowFromID( hwnd, ID_LB_FILE_TOC),
                              LM_DELETEITEM,
                              (MPARAM) sFile,
                              (MPARAM) 0);

                  /* Select the first item in the CTOC list box */
                  WinSendMsg( WinWindowFromID (hwnd, ID_LB_FILE_TOC),
                              LM_SELECTITEM,
                              (MPARAM) 0,
                              (MPARAM) TRUE);
                  /*
                   * Check to see which  buttons need to be enabled and which
                   * buttons need to be disabled
                   */

                  EnableButtons(hwnd);
                  }
               }
            else
               {
               /* Riff was not created, so display an error */
               ShowAMessage(
                           IDS_ERROR_TITLE,
                           IDS_CANT_PROCESS_MESSAGE,
                           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
                           FALSE);
               }
         break;

         case ID_PB_PLAY:
            if (fRIFFExists)
               {
               /*
                * Riff File Exists so Query what wave entry the user has
                * selected to be played from RIFF compound file.  This call
                * will return back the Index of the List Box item selected.
                */
                sFile =
                    (SHORT) WinSendMsg( WinWindowFromID( hwnd,
                                                 ID_LB_FILE_TOC),
                                        LM_QUERYSELECTION,
                                        (MPARAM) LIT_FIRST,
                                        (MPARAM) NULL);

               /* No item was selected, so display an error and break */
               if (sFile == LIT_NONE)
                  {
                  ShowAMessage(
                              IDS_ERROR_TITLE,
                              IDS_NO_CTOC_ENTRIES_SELECTED,
                              MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
                              FALSE);

                  break;
                  }
                /* Query for the name of the element from the list box */
                WinQueryLboxItemText(WinWindowFromID(hwnd,ID_LB_FILE_TOC),
                                     (LONG) sFile,
                                     achFileName,
                                     CCHMAXPATH);

                /*
                 * Play the element from the RIFF file which the user just
                 * selected.  In order to play the element the element name
                 * is concatanated with the RIFF file name and passed in
                 * ElementName for MCI_OPEN message.
                 */
                PlayElement(hwnd,achFileName);
                }
            else
               {
               /* Riff was not created, so display an error */
               ShowAMessage(
                           IDS_ERROR_TITLE,
                           IDS_CANT_PROCESS_MESSAGE,
                           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
                           FALSE);
               }
         break;

         case ID_PB_COMPACT:

            if (fRIFFExists)
               {
                /*
                 * Riff File already exists so we want to compact it.
                 * Close the Riff Compound file because in order to compact a
                 * RIFF compound file it should be Closed.
                 */
               CloseRiffCompoundFile(hwnd);
               /*
                * Compact the RIFF compound file.  Compacting basically
                * deletes the CGRP (Compound file resource group)
                * entries for all entries marked deleted in
                * CTOC (Compound File Table of Contents).
                * When an entry is deleted it is only marked deleted
                * in CTOC (table of contents) but the actual data still
                * resides in CGRP.  In order to compact it mmioCFCompact is
                * used.  mmioCFCopy can also be used to accomplist this.
                * However, mmioCFCopy generates another copy of the file.
                * This function returns TRUE if the compacting is successful.
                */
               if (CompactTheRIFFCompoundFile())
                  {
                  /*
                   * The file was successfully compacted so Open the RIFF file
                   * to be used later.  More advanced applications should
                   * open the RIFF compound file only when needed.
                   */

                  OpenRIFFCompoundFile(MMIO_READWRITE);
                  /*
                   * After compacting the file disable or enable push buttons as
                   * needed
                   */
                  EnableButtons(hwnd);
                  }
               }
            else
               {
               /* Riff was not created, so display an error */
               ShowAMessage(
                           IDS_ERROR_TITLE,
                           IDS_CANT_PROCESS_MESSAGE,
                           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
                           FALSE);
               }
          break;
       }
      return( (MRESULT) 0);


    case MM_MCIPASSDEVICE:
       if (SHORT1FROMMP(mp2) == MCI_GAINING_USE)
          {
          /* Have gained use of the Audio Device so update the flag */
          fPassedDevice = FALSE;

           }
       else
          {
          /* Have lost use of the Audio Device so update the flag */
          fPassedDevice = TRUE;

          }
       break;


    /*
     * We use the WM_ACTIVATE message to participate in device sharing.
     * We first check to see if this is an activate or a deactivate
     * message (indicated by mp1). Then, we check to see if we've passed
     * control of the device that we use.  If these conditions are true,
     * then we issue an acquire device command to regain control of the
     * device, since we're now the active window on the screen.
     *
     * This is one possible method that can be used to implement
     * device sharing. For applications that are more complex
     * than this sample program, developers may wish to take
     * advantage of a more robust method of device sharing.
     * This can be done by using the MCI_ACQUIRE_QUEUE flag on
     * the MCI_ACQUIREDEVICE command.  Please refer to the MMPM/2
     * documentation for more information on this flag.
     */
    case WM_ACTIVATE:
       /* First we check to see if we've passed control of the device */
       if ((BOOL)mp1 && fPassedDevice == TRUE)
          {
          lpWaveGeneric.hwndCallback =  (ULONG) hwnd;
          ulReturn = mciSendCommand( usWaveDeviceId,
                                     MCI_ACQUIREDEVICE,
                                     (ULONG)MCI_WAIT,
                                     (PVOID)&lpWaveGeneric,
                                     (USHORT)NULL );
          if (ulReturn != 0)
             {
             /* This is an MCI Error so show the message by passing TRUE */
             ShowAMessage(
                     IDS_ERROR_TITLE,
                     ulReturn,
                     MB_OK | MB_INFORMATION | MB_MOVEABLE |  MB_APPLMODAL,
                     TRUE);
             }

          }
       return( WinDefSecondaryWindowProc( hwnd, msg, mp1, mp2 ) );
       default:
          return ( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
    }
}






/******************************************************************************
 * Name        : ShowAMessage
 *
 * Description : This function will show text in a message box.
 *               If the Message is an MCI Error then the text is pulled
 *               by issuing a mciGetErrorString.  The text is retrieved by the
 *               Return code of the mci call which failed.
 *               If the Message is not an MCI Error then the text
 *               is pulled from the string table that is originally kept
 *               in the resource file .  The text is retrieved by the
 *               message id that is passed into this function.  This id is
 *               used in the WinLoadString OS/2 API to get the correct
 *               string from the table.
 *
 * Concepts    : None.
 *
 * MMPM/2 API's: mciGetErrorString
 *
 * Parameters  : usTitleId         - The tile id that identifies which string
 *                                   to retrieve from the string table.
 *               usTextId          - The message text to be placed in the
 *                                   message box.  This value is the Return
 *                                   value for the failed MCI call if fMCIError
 *                                   is true and is the Message id (application
 *                                   owned) if fMCIError is False.
 *               ulMessageBoxStyle - The style of the message box.  Which
 *                                   icons, buttons, etc., to show.
 *               fMCIError         - If the Error is an MCIError (TRUE) or
 *                                   if it is an application message id (FALSE).
 *
 * Return      : The result from the message box.
 *
 ******************************************************************************/
USHORT ShowAMessage( USHORT usTitleId,
                     USHORT usTextId,
                     ULONG  ulMessageBoxStyle,
                     BOOL   fMCIError)
{

   CHAR   achStringBuffer[ STRING_SIZE ];       /* Title String Buffer.      */
   USHORT usMessageBoxResult;                   /* RC from WinMessageBox.    */
   CHAR   achMessageText[STRING_SIZE];          /* Message String Buffer     */
   ULONG  ulReturn;                             /* Return from MCI ERROR     */

   /*
    * Load the Title for the message box from the string table defined in
    * the resources.
    */

   WinLoadString(
      hab,                           /* HAB for this dialog box.            */
      (HMODULE) NULL,                /* Get the string from the .exe file.  */
      usTitleId,                     /* Which string to get.                */
      (SHORT) STRING_SIZE,           /* The size of the buffer.             */
      achStringBuffer );             /* The buffer to place the string.     */

   /* If we are to show a MCIERROR, then load it by mciGetErrorString API */
   if (fMCIError)
     {
        ulReturn =
            mciGetErrorString(
                            usTextId,
                            (PSZ)achMessageText,
                            (USHORT) STRING_SIZE );


           /*
            * Make sure that the retrieving of the error string was successfull.
            */
           if ( ulReturn != MCIERR_SUCCESS )
             {
              WinLoadString(
                    hab,                       /* HAB for this dialog box.   */
                    (HMODULE) NULL,            /* Get string from .exe file. */
                    IDS_CANT_PROCESS_MESSAGE,  /* Which string to get.       */
                    (SHORT) STRING_SIZE,       /* The size of the buffer.    */
                    achMessageText);           /* Buffer to place the string.*/

              ulMessageBoxStyle = MB_OK | MB_INFORMATION | MB_MOVEABLE | MB_HELP |
                                  MB_APPLMODAL;
             }  /* End of IF testing for failed Get Error String API. */

     }
   else
     {
     /*
      This is not an MCIError so load the Text for Message Box from
      the string table defined in the resources.
      */
       WinLoadString(
           hab,                     /* HAB for this dialog box.   */
           (HMODULE) NULL,          /* Get string from .exe file. */
           usTextId,                /* Which string to get.       */
           (SHORT) STRING_SIZE,     /* The size of the buffer.    */
           achMessageText );        /* Buffer to place the string.*/
     }
   usMessageBoxResult =
      WinMessageBox(
         HWND_DESKTOP,              /* Parent handle of the message box.   */
         hwndDiag,                  /* Owner handle of the message box.    */
         achMessageText,            /* String to show in the message box.  */
         achStringBuffer,           /* Title to shown in the message box.  */
         ID_MESSAGE_BOX,            /* Message Box Id.                     */
         ulMessageBoxStyle );       /* The style of the message box.       */

   return( usMessageBoxResult );

}  /* End of ShowAMessage */

/******************************************************************************
 * Name        : FillListBoxWithWaves
 *
 * Description : This function will fill the Add File List Box with all the
 *               wave files which are found in the current directory.
 *               All the files with .wav extension are going to be added to
 *               this list and the focus is going to be on the first wave
 *               file.  These wave files can be later added on to the compound
 *               RIFF file by selecting them from the Add File list box.
 *               This function is called during initialization.
 *
 * Concepts    : None.
 *
 * MMPM/2 API's:
 *
 * Parameters  : hwnd              - Handle to the dialog of the application
 *
 * Return      : None.
 *
 ******************************************************************************/

BOOL FillListBoxWithWaves(HWND hwnd)
{
   CHAR          achPath[256] = "";
   FILEFINDBUF3  ffb;
   ULONG         ulCount = 1;
   APIRET        rc;
   HDIR          hdir = HDIR_CREATE;
   BOOL          fFound = FALSE;

   /* Search for all files with .wav extension */
   strcpy(achPath,"*.wav");

   /* Find the first file matching this .wav pattern */
   if (!(rc = DosFindFirst(achPath,       /* file name pattern        */
                   &hdir,                 /* dir handle               */
                   0,                     /* normal files             */
                   (PVOID)&ffb,           /* result buffer            */
                   sizeof(ffb),           /* size of buffer           */
                   &ulCount,              /* number to find = 1       */
                   FIL_STANDARD)))        /* return level 1 file info */
      {
      /* Found at least on file */
      fFound = TRUE;
      /* Insert the found file into the ADD file list box */
      WinSendMsg( WinWindowFromID( hwnd, ID_LB_ADD_FILE),
                  LM_INSERTITEM,
                 (MPARAM) LIT_END,
                 ffb.achName);

      ulCount = 1;
      /* Keep on searching for more files till no more exist */

      while (!DosFindNext(hdir,           /* dir handle            */
                          &ffb,           /* result buffer         */
                          sizeof(ffb),    /* size of result buffer */
                          &ulCount))      /* number to find        */
           {

           WinSendMsg( WinWindowFromID( hwnd, ID_LB_ADD_FILE),
                       LM_INSERTITEM,
                      (MPARAM) LIT_END,
                      ffb.achName);
           ulCount = 1;
           }

      /* select the first wave in the listbox by default */

      WinSendMsg( WinWindowFromID (hwnd, ID_LB_ADD_FILE),
                  LM_SELECTITEM,
                  (MPARAM) 0,
                  (MPARAM) TRUE);
      }
   else
     {
     /* No Wave files found, so display a message */
     ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_NO_WAVE_FILES,
           MB_OK | MB_INFORMATION | MB_MOVEABLE |  MB_APPLMODAL ,
           FALSE);

     }

  return fFound;

}

/******************************************************************************
 * Name        : OpenRIFFCompoundFile
 *
 * Description : This function will either Open the RIFF compound file
 *               or  Create the Riff Compound file depending upon the ulFlags
 *               passed in.  This function is first called to create the RIFF
 *               file and then later it is called to open it.  This sample
 *               program keeps the RIFFSAMP.BND compound file open at all
 *               time and only closes the file to compact it.
 *               After performing these actions the RIFF file is opened again.
 *               A global variable fRIFFExists is set to TRUE or FALSE
 *               depending upon if the opening of the RIFF file was successful.
 *
 * Concepts    : How to open and create a compound RIFF file.
 *
 * MMPM/2 API's: mmioCFOpen
 *
 * Parameters  : ulFlags              - Flags for opening the file.
 *                                      MMIO_CREATE creates the file
 *                                      MMIO_READWRITE opens it for reading and
 *                                      writing
 *
 * Return      : None.
 *
 ******************************************************************************/
void OpenRIFFCompoundFile(ULONG ulFlags)
{

 /*
  * Either Opens the  RIFF file  for reading and writing or creates it
  * depeneding on the ulFlags.  If ulFlags = MMIO_CREATE | MMIO_READWRITE
  * then the RIFF file is created for reading and writing.  This creation is
  * only done once during the initialization of the dialog.
  * If ulFlags = MMIO_READWRITE then the RIFF file is opened for reading and
  * writing.
 */
 hmmioCF = mmioCFOpen( (PSZ)RIFF_COMPOUND_FILE,
                        NULL,
                        NULL,
                        ulFlags);

 /* Call the error routine if RIFF compound file could not be created */
 if (!hmmioCF)
    {
     ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_RIFF_CREATION_ERROR,
           MB_OK | MB_INFORMATION | MB_MOVEABLE |  MB_APPLMODAL ,
           FALSE);
     fRIFFExists = FALSE;
    }
 else
    {
    fRIFFExists = TRUE;
    }

}

/******************************************************************************
 * Name        : AddElementToRIFF
 *
 * Description : This function will first call another function to get the
 *               pmmctocentry structure.  Then a mmio call is made to try to
 *               find the pszElementName in CTOC.  If the
 *               the entry does not exist, then the wave file is opened
 *               and read into a buffer and then added to the Compound RIFF
 *               file.  mmioCFAddElement is used to add the Wave file into
 *               compound file.  This mmio call takes care of adding the entry
 *               name to the CTOC and adding the element data to CGRP.  CTOC
 *               list box is updated with the new element being added.
 *               e.g. if the user selects RIFF1.WAV to be added to the
 *               compound file, an element called "RIFF1" is added if
 *               it already does not exist.
 *
 *
 * Concepts    : How to Add an element and an entry to a compound RIFF file.
 *
 * MMPM/2 API's: mmioOpen
 *               mmioStringToFOURCC
 *               mmioRead
 *               mmioCFAddElement
 *               mmioCFFindEntry
 *
 * Parameters  : pszEntryName         - Name of the Entry (or the wave file)
 *                                      to be added to the compound RIFF file
 *               sFileNum             - Index of the wave file in the Add File
 *                                      list box.
 *               hwnd                 - Handle to the Dialog.
 *
 * Return      : TRUE                 - If RIFF added successfully
 *               FALSE                - If RIFF could not be added.
 *
 ******************************************************************************/
BOOL AddElementToRIFF(PSZ pszEntryName,
                      SHORT sFileNum,
                      HWND hwnd)
{

  ULONG        dwReturn;
  FOURCC       fccType;
  MMIOINFO     mmioinfo;
  HMMIO        hmmioSource;
  FILESTATUS3  FileInfoBuf;
  CHAR         *pchAppBuffer;
  CHAR         achElementName[CCHMAXPATH] = "";
  PSZ          psz;
  ULONG        ulRC;
  ULONG        ulFlags;
  LONG         lBytesRead;
  PMMCTOCENTRY pmmctocentry;
  BOOL         fSuccess = FALSE;

  /*
   * Convert the entry name to the element name.  Element name is the
   * identifier for the entry name which is kept in CTOC.
   * Entry name may be the full file name e.g. sound.wav while the
   * element name is sound.  Element name is used to find an element, add an element.
   * Entry name is only used  to open the wave file and read it
   * Look for the period of the .WAV extension and then copy into the
   * element name everything except .WAV extension.
   */
  if (psz = strstr(pszEntryName,"."))
    {
     strncpy(achElementName,pszEntryName,psz - pszEntryName);
    }
  else
    {
     /* If no period found then display a message and return */
     ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_CANT_PROCESS_MESSAGE,
           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
           FALSE);
     return FALSE ;
    }

  /*
   * Get the pointer to pmmctocentry from GetCTOCEntryInfo function.
   * This function adds the pszElementName to the end of the variable
   * length structure MMCTOCENTRY.  If a NULL is returned then there was
   * was an error.  This structure is used to find if an entry already
   * exists in the CTOC.
   */
  if (!(pmmctocentry = GetCTOCEntryInfo(achElementName)))
    {
    /* a Null was returned becaus of an error so returned back */
    return FALSE;
    }

  /* Try to find the pszElementName in the CTOC by this API */
  ulRC =
      mmioCFFindEntry (hmmioCF,
                       pmmctocentry,
                       (ULONG) 0);

  if (ulRC == MMIOERR_CF_ENTRY_NOT_FOUND)
     {
     /*
      * Entry for the selected wave not found in CTOC, so add the entry and
      * element data to CTOC and CGRP.
      * First make sure the selected wave file exists in the current directory
      * and if it does then find the size of the wave file to be used later.
      */
     if (ulRC =
             (ULONG) DosQueryPathInfo(pszEntryName,
                                      FIL_STANDARD,
                                      &FileInfoBuf,
                                      sizeof(FileInfoBuf)))
        {
        /*
         * Either the wave does not exist in the current directory or some other
         * occured so display an error and return
         */
        ShowAMessage(
                    IDS_ERROR_TITLE,
                    IDS_FILE_NOT_FOUND,
                    MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
                    FALSE);
        return FALSE;
        }  /* endif */
     }
  else
    if (ulRC )
       {
       /*
        * If any other error occured while searching using mmioCFFindEntry
        * then display a message and return FALSE
        */
       ShowAMessage(
                   IDS_ERROR_TITLE,
                   IDS_CANT_PROCESS_MESSAGE,
                   MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
                   FALSE);
       return FALSE;
       }
    else
       if (ulRC == MMIO_CF_SUCCESS)
          {
          /*
           * CTOC entry was found so we don't want to add it again, so
           * display a message and return back.
           */

          ShowAMessage(
                   IDS_ERROR_TITLE,
                   IDS_ENTRY_ALREADY_EXISTS,
                   MB_OK | MB_INFORMATION | MB_MOVEABLE |  MB_APPLMODAL ,
                   FALSE);

          return FALSE;
          }

  /* Initialize the mmioinfo structure for opening the wave file */
  memset (&mmioinfo,'\0',sizeof(mmioinfo));

  /* Initialize the IOProc to use on the wave file */
  mmioinfo.fccIOProc = FOURCC_DOS;

  /* Open the Wave File for Reading */
  hmmioSource = mmioOpen( (PSZ)pszEntryName,
                               &mmioinfo,
                               MMIO_READ );

  /* if could not open wave file then display an error and return back */
  if (!hmmioSource)
     {
     ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_COULD_NOT_OPEN_WAVE,
           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
           FALSE);
     return FALSE;
     }

  /*
   * Wave file openened successfully, so allocate memory into a buffer
   * where the data for the wave file can be read into.
   */

  if ((pchAppBuffer = (PCHAR)calloc( FileInfoBuf.cbFile,sizeof(CHAR) ))
       == NULL)
     {
     /* Could not allocate memory so display an error and return */
     ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_CANNOT_GET_MEMORY,
           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
           FALSE);
     return FALSE;
     }

  /* Read the wave file into the Application Buffer */

  lBytesRead = mmioRead( hmmioSource,
                         pchAppBuffer,
                         FileInfoBuf.cbFile );

  if (lBytesRead != (LONG)FileInfoBuf.cbFile)
     {

     /*
      * If All bytes could not be read then there was an error, so diplay
      * an error and quit.
      */
     ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_CANT_PROCESS_MESSAGE,
           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
           FALSE);

     return FALSE;
     }

  /* Convert the string WAVE to FOURCC which is used for adding an element */
  fccType  =  mmioStringToFOURCC( "WAVE", MMIO_TOUPPER );

  ulFlags = (ULONG) 0;
  /*
   * Use the mmio API to add the entry to CTOC and element data to CGRP
   * mmioCFAddEntry could also be used, but it will only add the entry of
   * the element to the CTOC and inorder to add the element data to CGRP
   * mmioCFAddElement API has to be used.  If the entry does not exist
   * in CTOC when this API is called then it is added to CTOC.
   */
  ulRC = mmioCFAddElement(hmmioCF,           /* handle to compound RIFF file */
                          achElementName,    /* Element name to be added     */
                          fccType,           /* FOURCC type                  */
                          pchAppBuffer,      /* Pointer to element data      */
                          FileInfoBuf.cbFile,/* Size of element data         */
                          ulFlags);          /* Flags used                   */


  if (ulRC)
    {
     /* Element could not be added so display a message and return */
     ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_CANT_PROCESS_MESSAGE,
           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
           FALSE);
     return FALSE;
    }
  /* Add the element name to the CTOC list box */
  WinSendMsg( WinWindowFromID( hwnd, ID_LB_FILE_TOC),
              LM_INSERTITEM,
              (MPARAM) LIT_END,
              achElementName);

  /* Select the first item in the CTOC list box */
  WinSendMsg( WinWindowFromID (hwnd, ID_LB_FILE_TOC),
              LM_SELECTITEM,
              (MPARAM) 0,
              (MPARAM) TRUE);


  return TRUE;

}


/******************************************************************************
 * Name        : DeleteEntryFromRIFF
 *
 * Description : This function will Delete the requested element name
 *               from the RIFF compound file.  The Element entry is deleted
 *               from CTOC but the element data is still present in CGRP.  In
 *               order to delete element data from CGRP mmioCFCompact should
 *               be used.
 *
 * Concepts    : How to Delete an element entry in a CTOC RIFF compound file.
 *
 * MMPM/2 API's: mmioCFDeleteEntry
 *
 * Parameters  : pszElementName       - Name of the Element for which we
 *                                      want to delete from the CTOC.  The user
 *                                      had selected this entry.
 *               sFileNum             - Index for the Element Name in List Box
 *               hwnd                 - Handle to Dialog.
 *
 * Return      : NONE.
 *
 ******************************************************************************/
BOOL DeleteEntryFromRIFF(PSZ   pszElementName,
                         SHORT sFileNum,
                         HWND  hwnd)
{
  PMMCTOCENTRY pmmctocentry;
  ULONG        ulRC;
  CHAR         achEntryName[CCHMAXPATH] = "";
                               /* Full Name of the Entry                    */
  /*
   * Get the pointer to pmmctocentry from GetCTOCEntryInfo function.
   * This function adds the pszElementName to the end of the variable
   * length structure MMCTOCENTRY.  If a NULL is returned then there was
   * was an error.
   */
  if (!(pmmctocentry = GetCTOCEntryInfo(pszElementName)))
    {
    /* a Null was returned because of an error so returned back */
    return FALSE;
    }

  /*
   * The Element name has been added to the end of the variable length
   * structure pmmctocentry.  Try to find the pszElementName in the CTOC
   * by this API
   */
  ulRC =
      mmioCFDeleteEntry (hmmioCF,
                         pmmctocentry,
                         (ULONG) 0);

  if (ulRC)
       {
       /* If any other error occured, display a message and return FALSE */
       ShowAMessage(
             IDS_ERROR_TITLE,
             IDS_CANT_PROCESS_MESSAGE,
             MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
             FALSE);
       return FALSE;
       }

  strcpy(achEntryName,pszElementName);
  strcat(achEntryName,".WAV");

  /* Add the element name to the ADD File list box */
  WinSendMsg( WinWindowFromID( hwnd, ID_LB_ADD_FILE),
              LM_INSERTITEM,
              (MPARAM) LIT_END,
              achEntryName);

  /* Select the first item in the Add File list box */
  WinSendMsg( WinWindowFromID (hwnd, ID_LB_ADD_FILE),
              LM_SELECTITEM,
              (MPARAM) 0,
              (MPARAM) TRUE);


  return TRUE;

}

/******************************************************************************
 * Name        : GetCTOCEntryInfo
 *
 * Description : This function will allocate memory for PMMCTOCENTRY structure
 *               by using the mmioCFGetInfo call.  Since this structure is of
 *               variable length, and is used to either delete an entry or
 *               find an entry we need to add the element name to the end
 *               of this structure.  A pointer to the PMMCTOCENTRY is returned.
 *               This function is called either when trying to add an entry
 *               of either when trying to delete an entry.  The element name
 *               to be added or deleted is placed at the end of the structure.
 *
 *
 * Concepts    : How to determine the size of the MMCTOCENTRY structure and
 *               copy the pszElementName at the end of this variable length
 *               structure.  The concept of retreving MMCFINFO structure is
 *               also illustrated.
 *
 * MMPM/2 API's: mmioCFGetInfo
 *               mmioStringToFOURCC
 *
 * Parameters  : pszElementName       - Name of the Element for which we
 *                                      are looking in the CTOC.  This
 *                                      element name needs to added at the
 *                                      end of the MMCTOCENTRY variable
 *                                      length structure.
 *
 * Return      : PMMCTOCENTRY         - Pointer to MMCTOCENTRY structure with
 *                                      the pszElementName at the end.
 *               NULL                 - If an error occurs.
 *
 ******************************************************************************/
PMMCTOCENTRY GetCTOCEntryInfo(PSZ pszElementName)
{
  ULONG        ulRC;           /* Return codes                              */
  ULONG        ulCBytes;       /* Number of Bytes to read                   */
  MMCFINFO     mmcfinfo;       /* Variable length structure                 */
  USHORT       usEntrySize;    /* Size for the Entry for Element in CTOC    */
  USHORT       usNameSize;     /* Size for the Name of the Element in CTOC  */
  USHORT       usExEntFields;  /* Number of extra entry fields for mmcfinfo */
  PMMCTOCENTRY pmmctocentry;   /* Variable length structure                 */
  CHAR         *pTemp;         /* The Element Name is copied in mmctocentry */

  /*
   * Since there can be variable length arrays after MMCFINFO structure, call
   * mmioCFGetInfo with ulCBytes equal to the size of the ULONG.  This will
   * return the first field of the CTOC header, which happens to the size
   * of the header.  This value can be used in another call to get the full
   * structure.
   */
  ulCBytes = (ULONG) sizeof(ULONG);

  /*
   * Get the first field of the CTOC header which will tell us the size
   * of the MMCFINFO structure (variable size).
  */
  ulRC =
       mmioCFGetInfo(hmmioCF,
                     &mmcfinfo,
                     ulCBytes);

  if (ulRC != ulCBytes)
    {
    /*
     * The mmioCFGetInfo call was not successful and the first field of the
     * CTOC header could not be retrieved, so display an error and return FALSE
     */
    ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_CANT_PROCESS_MESSAGE,
           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
           FALSE);
    return NULL;
    }

  /*
   * The call was successfull so retrieve the size of the variable length
   * structure so we can retrieve the full structure.
   */
  ulCBytes = mmcfinfo.ulHeaderSize;

  /*
   * Retrieve the full mmcfinfo structure.  This call will give us back the
   * size for each entry or the size of MMCTOCENTRY structure,
   * and the size for the name of the entry which is at the end of the
   * structure, so we can input into the MMCTOCENTRY structure
   * (variable length) the name of  element we are searching for.
   */
  ulRC =
       mmioCFGetInfo(hmmioCF,
                     &mmcfinfo,
                     ulCBytes);

  if (ulRC != ulCBytes)
    {
    /*
     * The mmioCFGetInfo call was not successful and mmcfinfo structure
     * could not be retrieved, so display an error and return FALSE
     */
     ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_CANT_PROCESS_MESSAGE,
           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
           FALSE);
     return NULL;
    }

  /* Contains the size for variable length MMCTOCENTRY structure */
  usEntrySize   = (USHORT) mmcfinfo.usEntrySize;

  /* Contains the size for the Name of the element for each entry */
  usNameSize    = (USHORT) mmcfinfo.usNameSize;
  usExEntFields = (USHORT) mmcfinfo.usExEntFields;

  /*
   * Allocate memory for pmmctocentry based on the size of each entry in CTOC
   * header structure.
   */
  if ((pmmctocentry =
            (PMMCTOCENTRY) calloc(1,
                                  (ULONG)(usEntrySize)))
                                                  == NULL)
     {
     /* Could not allocate memory, so display an error and return FALSE */
     ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_CANNOT_GET_MEMORY,
           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
           FALSE);
     return NULL;

     }

  /* Align the pointer pTemp to point to the MMCTOCENTRY structure */
  pTemp = (CHAR *)(pmmctocentry);
  /*
   * Since the MMCTOCENTRY can be followed by the element name (which is going)
   * we need to input the element name at the end of the
   * variable length structure.  So go to end of the structure where the
   * space for Element Name starts.
   */
  pTemp += usEntrySize - usNameSize;

  /* Copy the element name (MAX of usNameSize bytes) into the structure */
  strncpy( pTemp,
          pszElementName,
          min( strlen(pszElementName), usNameSize ) );

  /* Input the FOURCCC of the Element type */
  pmmctocentry->ulMedType =
                   (ULONG) mmioStringToFOURCC( (PSZ)"WAVE",
                                               MMIO_TOUPPER );
  return pmmctocentry;

}

/******************************************************************************
 * Name        : CompactTheRIFFCompoundFile
 *
 * Description : This function will compact the RIFF compound file.  When
 *               any entries are deleted from a compound file their entries
 *               are only deleted from CTOC on a RIFF file.  Their data
 *               still exists in the CGRP of the RIFF file.  In order to
 *               delete the data mmioCFCompact is used.  mmioCFCompact
 *               deletes all deleted entries into the same file.  mmioCFCopy
 *               can also be used and it copies the non-deleted entries of
 *               a compound file into another file.
 *
 *
 *
 * Concepts    : How to compact a file with deleted entries.
 *
 * MMPM/2 API's: mmioCFCompact
 *
 * Parameters  :
 *
 *
 * Return      : TRUE                 - If Compacting is successfull
 *               FALSE                - If an error occurs.
 *
 ******************************************************************************/
BOOL CompactTheRIFFCompoundFile()
{
  ULONG  ulRC;


  /* Compact the RIFF_COMPOUND_FILE.  The file is already closed */
  ulRC =
      mmioCFCompact(RIFF_COMPOUND_FILE,
                    0L);

  if (ulRC)
     {
     /* An error occured compacting the file so display an error */
     ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_CANT_PROCESS_MESSAGE,
           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
           FALSE);
      return FALSE;
     }

 return TRUE;

}

/******************************************************************************
 * Name        : EnableButtons
 *
 * Description : This function either enables or Disable push buttons depending
 *               on mmcfinfo structure.
 *               Play pushbutton - If there is at least one non-deleted element
 *               in compound file then enable the pushbutton else disable it.
 *
 *               Compact pushbutton - If there is at least one deleted element
 *               and one non-deleted element in the compound file then enable
 *               it otherwise disable it.
 *
 *               Add pushbutton - If there is at least one wave file to be
 *               added in the add list box then enable it else disable it.
 *
 *               Delete pushbutton - If there is at least one non-deleted
 *               element in compound file then enable it else disable it.
 *
 *
 *
 * Concepts    : How to use MMCFINFO structure to find out how many
 *               elements are deleted, how many are used and how many are
 *               total.
 *
 * MMPM/2 API's: mmioCFGetInfo
 *
 * Parameters  : HWND                 - hwnd to the application
 *
 *
 * Return      :
 *
 ******************************************************************************/
void  EnableButtons(HWND hwnd)
{
  ULONG    ulCBytes;
  MMCFINFO mmcfinfo;
  ULONG    ulRC;
  SHORT    sItemCount;
  /*
   * Since there can be variable length arrays after MMCFINFO structure, call
   * mmioCFGetInfo with ulCBytes equal to the size of the ULONG.  This will
   * return the first field of the CTOC header, which happens to the size
   * of the header.  This value can be used in another call to get the full
   * structure.
   */
  ulCBytes = (ULONG) sizeof(ULONG);

  /* Get the first field of the CTOC header */
  ulRC =
       mmioCFGetInfo(hmmioCF,
                     &mmcfinfo,
                     ulCBytes);

  if (ulRC != ulCBytes)
    {
    /*
     * The mmioCFGetInfo call was not successful and the first field of the
     * CTOC header could not be retrieved, so display an error and return FALSE
     */
    ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_CANT_PROCESS_MESSAGE,
           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
           FALSE);
    return ;
    }

  /*
   * The call was successfull so retrieve the size of the variable length
   * structure so we can retrieve the full structure
   */
  ulCBytes = mmcfinfo.ulHeaderSize;

  /*
   * Retrieve the full mmcfinfo structure.  This call will give us back the
   * size for each entry, and the size for the name of the entry, so we
   * can input into the MMCTOCENTRY structure (variable length) the name of
   * element we are searching for.
   */
  ulRC =
       mmioCFGetInfo(hmmioCF,
                     &mmcfinfo,
                     ulCBytes);

  if (ulRC != ulCBytes)
    {
    /*
     * The mmioCFGetInfo call was not successful and mmcfinfo structure
     * could not be retrieved, so display an error and return FALSE
     */
     ShowAMessage(
           IDS_ERROR_TITLE,
           IDS_CANT_PROCESS_MESSAGE,
           MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
           FALSE);
     return ;
    }

  if (mmcfinfo.ulEntriesDeleted &&
     (mmcfinfo.ulEntriesTotal - mmcfinfo.ulEntriesUnused != mmcfinfo.ulEntriesDeleted))
     {
      /*
       * If there is at least on Deleted entry and one non-deleted entry in
       * the RIFF file then try to enable it, else disable it
       */
      if (fCompactDisable)
         {
         /* The push button was disabled, so enable it */
         WinEnableWindow( WinWindowFromID(hwnd, ID_PB_COMPACT),
                          TRUE);
         fCompactDisable = FALSE;
         }
     }
  else
    {
      /* Either there is no deleted entry or there is no non-deleted entry */

      if (!fCompactDisable)
         {
         /* push button was enabled, so disable it */
         WinEnableWindow( WinWindowFromID(hwnd, ID_PB_COMPACT),
                          FALSE);
         fCompactDisable = TRUE;
         }
    }

  /*  Check if there are any entries which are being used by elements */
  if ((mmcfinfo.ulEntriesTotal - mmcfinfo.ulEntriesUnused)
               !=  mmcfinfo.ulEntriesDeleted)
     {
      /* Entries found which are being used */
      if (fPlayDisable)
         {
         /*
          * Play button was disabled, so enable it since there is at least
          * one non-deleted entry
          */

         WinEnableWindow( WinWindowFromID(hwnd, ID_PB_PLAY),
                          TRUE);
         fPlayDisable = FALSE;
         }


      if (fDelDisable)
         {
         /*
          * Delete button was disabled, so enable it since there is at least
          * one non-deleted entry
          */
         WinEnableWindow( WinWindowFromID(hwnd, ID_PB_DEL),
                          TRUE);
         fDelDisable = FALSE;
         }
     }
  else
    {
     /* All entries are either deleted or there are no entries in RIFF file */
     if (!fPlayDisable)
        {
        /* Play button is enabled, so disable it */
         WinEnableWindow( WinWindowFromID(hwnd, ID_PB_PLAY),
                          FALSE);
         fPlayDisable = TRUE;
         }


     if (!fDelDisable)
        {
        /* Delete button is enabled, so disable it */
        WinEnableWindow( WinWindowFromID(hwnd, ID_PB_DEL),
                         FALSE);
        fDelDisable = TRUE;
        }
    }

  /* Query for if there are any entries in the ADD file list box */
  sItemCount =
     (SHORT)   WinSendMsg( WinWindowFromID(hwnd,ID_LB_ADD_FILE),
                           LM_QUERYITEMCOUNT,
                           (MPARAM) 0,
                           (MPARAM) 0);
  if (sItemCount == 0)
     {
     /* No entries exist in ADD file list box so disable it */
     if (!fAddDisable)
        {
        WinEnableWindow( WinWindowFromID(hwnd,ID_PB_ADD),
                         FALSE);
        fAddDisable = TRUE;
        }
     }
  else
    {
    /* Entries exist in ADD file list box so enable it */
    if (fAddDisable)
       {
        WinEnableWindow( WinWindowFromID(hwnd,ID_PB_ADD),
                         TRUE);
        fAddDisable = FALSE;
       }
    }
}

/******************************************************************************
 * Name        : PlayElement
 *
 * Description : This procedure will open the wave audio device to play
 *               the element which the user selected the CTOC list box.
 *               Then the element will be played.
 *
 *
 * Concepts    : How to Open a Device to play a wave file from a RIFF
 *               compound file and then how to play the file.
 *
 * MMPM/2 API's:  mciSendCommand         MCI_OPEN
 *                                       MCI_PLAY
 *                                       MCI_CLOSE
 *
 * Parameters  : hwnd                 - Handle to the applciation
 *               pszElementName       - Name of the element to play.
 *
 * Return      : None.
 *
 *************************************************************************/
void PlayElement(HWND hwnd,
                 PSZ pszElementName)
{
MCI_PLAY_PARMS  mciPlayParameters;              /* Used for MCI_PLAY messages */
MCI_OPEN_PARMS  lpWaveOpen;                     /* Used for MCI_OPEN messages */
ULONG           ulReturn;                       /* Used for ret code for API  */
CHAR            achElementName[CCHMAXPATH] = "";/* Name of the wave to play   */
MCI_GENERIC_PARMS mciGenericParms;


   /*
    * Initialize the MCI_OPEN_PARMS data structure with hwndMainDialogBox
    * as callback handle for MM_MCIPASSDEVICE, then issue the MCI_OPEN
    * command with the mciSendCommand function.  No alias is used.
    */

   lpWaveOpen.hwndCallback       =  hwnd;
   lpWaveOpen.pszAlias       = (PSZ) NULL;


   /* Open the correct waveform device with the MCI_OPEN message to MCI. */
   lpWaveOpen.pszDeviceType  =
         (PSZ) (MAKEULONG(MCI_DEVTYPE_WAVEFORM_AUDIO,1));
   /*
    * Concatanate the RIFF file name and the Element name to be played.
    * In order to play an element from a compound file, we need to
    * concatanate the two
    */
   strcpy(achElementName,RIFF_COMPOUND_FILE);
   strcat(achElementName,"+");
   strcat(achElementName,pszElementName);

   /*  The name of the element to play the wave. */
   lpWaveOpen.pszElementName = (PSZ)achElementName;


   /*  Open the waveform file in the ELEMENT mode. */
   ulReturn =
       mciSendCommand(0,
                      MCI_OPEN,
                      MCI_WAIT | MCI_OPEN_ELEMENT | MCI_READONLY |
                      MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE,
                      (PVOID) &lpWaveOpen,
                      (USHORT) NULL);

   if (ulReturn != 0)
      {
      /* MCI Open was not successfull so show a message by passing a TRUE */
       ShowAMessage(
             IDS_ERROR_TITLE,
             ulReturn,
             MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
             TRUE);
      }

   /* Save the Device Id in the global variable to be used later */
   usWaveDeviceId = lpWaveOpen.usDeviceID;

   /* Initialize the mciPlayParameters structure */
   memset(&mciPlayParameters,'\0',sizeof(mciPlayParameters));
   mciPlayParameters.hwndCallback =  hwnd;

   /* Send the Play Command to begin the playing of the RIFF element. */
   ulReturn
            = mciSendCommand(usWaveDeviceId,
                             MCI_PLAY,
                             MCI_WAIT,
                             (PVOID) &mciPlayParameters,
                             0);
   if (ulReturn)
      {
      /*
       * The MCI_PLAY was not successfull so display an error by passing
       * TRUE.
       */
      ShowAMessage(
             IDS_ERROR_TITLE,
             ulReturn,
             MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
             TRUE);
      return ;
      }

   /* Done Playing so want to close the device */
   memset(&mciGenericParms,'\0',sizeof(mciGenericParms));
   ulReturn = mciSendCommand(usWaveDeviceId,
                             MCI_CLOSE,
                             MCI_WAIT,
                             (PVOID) &mciGenericParms,
                             0);

   if (ulReturn)
      {
      /*
       * The MCI_CLOSE was not successfull so display an error by passing
       * TRUE.
       */
      ShowAMessage(
            IDS_ERROR_TITLE,
            ulReturn,
            MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
            TRUE);
      return ;
      }
}

/******************************************************************************
 * Name        : CloseRiffCompoundFile
 *
 * Description : This procedure will close the RIFF compound file.
 *               The file is only closed when trying to compact the file.
 *
 *
 * Concepts    : How to Close  a RIFF file.
 *
 * MMPM/2 API's: mmioCFClose
 *
 *
 * Parameters  : None.
 *
 *
 * Return      : None.
 *
 *************************************************************************/

void CloseRiffCompoundFile(HWND hwndDiag)
{

 ULONG ulRC;

 /* Close the RIFF compound file, by passing the handle of the RIFF file */
 ulRC = mmioCFClose(hmmioCF,(ULONG) 0);

 if (ulRC)
    {
    /* An error occured so display a message */
    ShowAMessage(
          IDS_ERROR_TITLE,
          IDS_CANT_PROCESS_MESSAGE,
          MB_CANCEL | MB_ERROR | MB_MOVEABLE |  MB_HELP ,
          FALSE);
    }

}
