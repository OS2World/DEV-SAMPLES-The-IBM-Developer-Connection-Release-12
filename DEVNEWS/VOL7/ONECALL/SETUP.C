// setup.c: printer setup routine
// Monte Copeland for DevCon 7

// os2 includes
#define INCL_BASE
#define INCL_PM
#define INCL_SPLDOSPRINT
#include <os2.h>

// c includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// local includes
#include "setup.h"
#include "pmassert.h"

// Private functions herein.
PPRQINFO3 APIENTRY FindQueue( PPRINTERSETUP );
MRESULT APIENTRY QueryPrintDlgProc( HWND, ULONG, MPARAM, MPARAM );



// ---------------------------------------------------------------------------------------------------
// SetupPrinter() returns zero for success.

ULONG APIENTRY SetupPrinter( PPRINTERSETUP pSetup, BOOL fShowDialog )
{
  BOOL            fOK;
  CHAR            szDefaultQueue[ LEN_WORKSTRING ];
  CHAR            szSavedQueue[ LEN_WORKSTRING ];
  CHAR            szWork[ LEN_WORKSTRING ];
  PCHAR           pch;
  PPRQINFO3       pqi;
  SIZEL           sizel;
  ULONG           cReturned;
  ULONG           cTotal;
  ULONG           cbNeeded;
  ULONG           ul;
  ULONG           ulrc;



  // Caller must set these items before calling.
  assert( pSetup->hab );
  assert( pSetup->lWorldCoordinates );
  assert( pSetup->hwndFrame );


  // no good unless I can open a PS
  pSetup->pDevOpenData = NULL;


  // Close the info DC's and PS's from any previous call.
  if( pSetup->hpsPrinterInfo )
  {
    GpiAssociate( pSetup->hpsPrinterInfo, (HDC)0 );
    GpiDestroyPS( pSetup->hpsPrinterInfo );
    pSetup->hpsPrinterInfo = (HPS)0;
  }

  if( pSetup->hdcPrinterInfo )
  {
    DevCloseDC( pSetup->hdcPrinterInfo );
    pSetup->hdcPrinterInfo = (HDC)0;
  }

  if( pSetup->pQueueInfo )
  {
    // Free the array of PRQINFO3 from previous call.
    free( pSetup->pQueueInfo );
    pSetup->pQueueInfo = NULL;
  }


  // Query how many queues exist on this computer and the
  // number of bytes needed to hold the array.
  ul = SplEnumQueue( NULL, 3, NULL, 0, &cReturned, &cTotal, &cbNeeded, NULL );
  if( cTotal == 0 )
  {
    // There are no queues on this computer!
    ulrc = SETUP_NOQUEUES;
    pSetup->cQueues = 0;
    goto depart;
  }


  // Allocate memory to store the newly enumerated queue information.
  pSetup->pQueueInfo = malloc( cbNeeded ) ;
  assert( pSetup->pQueueInfo );
  if( ! pSetup->pQueueInfo )
  {
    ulrc = SETUP_NOMEMORY;
    goto depart;
  }

  // Call system again to get the array of PRQINFO3 structures.
  ul = SplEnumQueue( NULL, 3, pSetup->pQueueInfo, cbNeeded, &cReturned, &cTotal, &cbNeeded, NULL );
  assert( ul == 0 );
  assert( cReturned == cTotal );
  pSetup->cQueues = cReturned;


  // Establish a default queue -- might need it.
  // Profiled queue name ends with a semicolon.
  ul = PrfQueryProfileString( HINI_PROFILE, "PM_SPOOLER", "QUEUE", NULL, szDefaultQueue, LEN_WORKSTRING );
  if( ul > 1 ) {
    // Trim off semicolon.
    pch = strchr( szDefaultQueue, ';' );
    assert( pch );
    *pch = 0;
  } else {
    // Hmmmm. Use the first one queue from the enumeration.
    strcpy( szDefaultQueue,  pSetup->pQueueInfo->pszName );
  }
  assert( strlen( szDefaultQueue ) );




  if( 0 == strlen( pSetup->szPreferredQueue ))
  {
    // No queue preference; use default.
    strcpy( pSetup->szPreferredQueue, szDefaultQueue );

    // Don't expect to see DRIVDATA without queue name.
    assert( pSetup->pDriverData == NULL );
  }



  // There is a chance that the preferred queue has been recently deleted.
  // Ensure the preferred queue still exists.

  pqi = FindQueue( pSetup );
  if( ! pqi )
  {
    // Not found. Use the default queue.
    strcpy( pSetup->szPreferredQueue, szDefaultQueue );

    if( pSetup->pDriverData )
    {
      free( pSetup->pDriverData );
      pSetup->pDriverData = NULL;
    }
  }
  else
  {
    // Preferred queue was found in the array. Do some additional validation
    // because it may have changed since last time in this function.

    fOK = TRUE;

    if( pSetup->pDriverData )
    {
      // Is driver data the right length?
      fOK = fOK && ( pqi->pDriverData->cb == pSetup->pDriverData->cb );

      // Is this queue still driving the same device?
      fOK = fOK && ( 0 == strcmp( pqi->pDriverData->szDeviceName,  pSetup->pDriverData->szDeviceName ));
    }

    if( !fOK )
    {
      assert( pSetup->pDriverData );
      free( pSetup->pDriverData );
      pSetup->pDriverData = NULL;
    }
  }



  // Find the queue again. If the last find failed, preferred queue name
  // was changed to default. This find will absolutely always succeed.

  pqi = FindQueue( pSetup );
  assert( pqi );

  if( !pSetup->pDriverData )
  {
    // Use driver data from the enumeration.
    pSetup->pDriverData = malloc( pqi->pDriverData->cb );
    assert( pSetup->pDriverData );
    if( ! pSetup->pDriverData )
    {
      ulrc = SETUP_NOMEMORY;
      goto depart;
    }
    memcpy( pSetup->pDriverData, pqi->pDriverData, pqi->pDriverData->cb );
  }


  assert( pSetup->pDriverData );
  assert( pSetup->pDriverData->cb > 0 );
  assert( pSetup->pDriverData->cb == pqi->pDriverData->cb );
  assert( 0 == strcmp( pqi->pDriverData->szDeviceName,  pSetup->pDriverData->szDeviceName ));


  /* Note that DriverData used on DevPostDeviceModes() called
  from QueryPrintDlgProc() is the one in the enumerated array,
  not the one in pSetup. This way, QueryPrintDlgProc()
  can massage any/all of the queues in the array before dismissing the dialog.
  */
  memcpy( pqi->pDriverData, pSetup->pDriverData, pSetup->pDriverData->cb );


  // Save the name of the preferred queue because the dialogs can change it.
  strcpy( szSavedQueue, pSetup->szPreferredQueue );

  if( fShowDialog )
  {
    // Pass pSetup to dialog procedure.
    ul = WinDlgBox( HWND_DESKTOP, pSetup->hwndFrame, (PFNWP)QueryPrintDlgProc, pSetup->hmod, ID_PICKQ, (PVOID)pSetup );
    assert( ul != DID_ERROR );
  }


  // QueryPrintDlgProc() may have modified pSetup->szPreferredQueue.
  pqi = FindQueue( pSetup );
  assert( pqi );

  if( 0 != strcmp( szSavedQueue, pSetup->szPreferredQueue ))
  {
    // The user picked a different queue during dialog processing.
    assert( pSetup->pDriverData );
    free( pSetup->pDriverData );

    pSetup->pDriverData = malloc( pqi->pDriverData->cb );
    assert( pSetup->pDriverData );
    if( ! pSetup->pDriverData )
    {
      ulrc = SETUP_NOMEMORY;
      goto depart;
    }
    pSetup->pDriverData->cb = pqi->pDriverData->cb;
  }

  // Copy data from the array back to PRINTERSETUP structure.
  assert( pSetup->pDriverData );
  assert( pSetup->pDriverData->cb == pqi->pDriverData->cb );
  memcpy( pSetup->pDriverData, pqi->pDriverData, pqi->pDriverData->cb );



  /* Prepare a DEVOPENSTRUC for DevOpenDC(). Use it here to open an OD_INFO
  DC. Caller may use the same DEVOPENSTRUC to open an OD_QUEUED DC when it is
  time to print. There are 9 pointers in the DEVOPENSTRUC. This code
  prepares the first 4. The others should be NULL.
  */

  // Prepare logical address which is preferred queue name.
  if( pSetup->fToFile )
  {
    pSetup->lDCType = OD_DIRECT;
    pSetup->devopenstruc.pszLogAddress = pSetup->szFileName;
  }
  else
  {
    pSetup->lDCType = OD_QUEUED;
    pSetup->devopenstruc.pszLogAddress = pSetup->szPreferredQueue;
  }


  // Prepare .DRV file name. Truncate after the period.
  strcpy( szWork, pqi->pszDriverName );
  pch = strchr( szWork, '.' );
  if( pch ) {
    *pch = 0;
  }
  if( pSetup->devopenstruc.pszDriverName )
  {
    free( pSetup->devopenstruc.pszDriverName );
  }
  pSetup->devopenstruc.pszDriverName = malloc( 1 + strlen( szWork ));
  assert( pSetup->devopenstruc.pszDriverName );
  if( ! pSetup->devopenstruc.pszDriverName )
  {
    ulrc = SETUP_NOMEMORY;
    goto depart;
  }
  strcpy( pSetup->devopenstruc.pszDriverName, szWork );

  // Prepare pointer to driver data.
  pSetup->devopenstruc.pdriv = pSetup->pDriverData;

  // Prepare data type. Standard is the preferred way to go.
  pSetup->devopenstruc.pszDataType = "PM_Q_STD";

  // Open an OD_INFO DC.
  pSetup->hdcPrinterInfo = DevOpenDC( pSetup->hab,
                                      OD_INFO,
                                      "*",
                                      4,
                                      (PDEVOPENDATA)&pSetup->devopenstruc,
                                      (HDC)0 );
  assert(  pSetup->hdcPrinterInfo  );
  if( !pSetup->hdcPrinterInfo ) {
    // Unable to open info DC. WinGetLastError() can provide diagnostics.
    ulrc = SETUP_NODC;
    goto depart;
  }

  // Create PS associated with OD_INFO DC.
  sizel.cx = 0;
  sizel.cy = 0;
  pSetup->hpsPrinterInfo = GpiCreatePS( pSetup->hab,
                                        pSetup->hdcPrinterInfo,
                                        &sizel,
                                        pSetup->lWorldCoordinates | GPIA_ASSOC );

  assert( pSetup->hpsPrinterInfo != GPI_ERROR );
  if( GPI_ERROR ==  pSetup->hpsPrinterInfo ) {
    // Problem with this setup.
    DevCloseDC( pSetup->hdcPrinterInfo );
    pSetup->hdcPrinterInfo = (HDC)0;
    pSetup->hpsPrinterInfo = (HPS)0;
    ulrc = SETUP_NODC;
    goto depart;
  }


  // OK to use.
  pSetup->pDevOpenData = (PDEVOPENDATA)&pSetup->devopenstruc;

  /* Suggestion: add a PFONTMETRICS pointer to the PRINTERSETUP
  structure and call GpiQueryFonts() to gather font information from
  the printer info PS.
  */

  // Success.
  ulrc = 0;

depart:
  return ulrc;
}






// ---------------------------------------------------------------------------------------------------------
// FindQueue finds the preferred queue name in the PRQINFO3 array.
// Returns the index if found; returns -1 if not found.

PPRQINFO3 APIENTRY FindQueue( PPRINTERSETUP pSetup )
{
  LONG   i;

  for( i = 0; i < pSetup->cQueues; i++ )
  {
    if( 0 == strcmp( pSetup->szPreferredQueue, pSetup->pQueueInfo[ i ].pszName ) )
    {
      return &pSetup->pQueueInfo[ i ];
    }
  }
  return NULL;
}





// ------------------------------------------------------------------------------------------------------

typedef struct _BACKUP {
  PRINTERSETUP         Setup;
  PPRINTERSETUP        pSaved;
} BACKUP;
typedef BACKUP *PBACKUP;


MRESULT APIENTRY QueryPrintDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  BOOL                fOK;
  CHAR                szDeviceName[ 48 ];
  CHAR                szDriverName[ 64 ];
  HWND                hwndListbox;
  HWND                hwndEntry;
  LONG                i;
  LONG                index;
  PCHAR               pch;
  PBACKUP             pBackup;
  PPRINTERSETUP       pSetup;
  PPRQINFO3           pqi;
  PSZ                 psz;


  switch (msg) {
  case WM_INITDLG:
    pBackup = malloc( sizeof( BACKUP ));
    assert( pBackup );

    WinSetWindowPtr( hwnd, QWL_USER, pBackup );

    memset( pBackup, 0, sizeof( BACKUP ));
    pBackup->pSaved = (PPRINTERSETUP)mp2;
    memcpy( &pBackup->Setup, pBackup->pSaved, sizeof( PRINTERSETUP ) );
    pSetup =  &pBackup->Setup;


    // fill listbox with print queues
    hwndListbox = WinWindowFromID( hwnd, IDC_LISTBOX );
    assert( hwndListbox );

    for( i = 0; i < pSetup->cQueues; i++ )
    {
      // Display printer comment if possible, else use queue name for display.
      psz = *pSetup->pQueueInfo[i].pszComment ? pSetup->pQueueInfo[i].pszComment : pSetup->pQueueInfo[i].pszName;
      assert( psz );

      index = (LONG) WinSendMsg( hwndListbox, LM_INSERTITEM, (MPARAM)LIT_END, (MPARAM)psz );

      if( 0 == strcmp( pSetup->pQueueInfo[i].pszName, pSetup->szPreferredQueue ))
      {
        // Hilight preferred queue.
        WinSendMsg( hwndListbox, LM_SELECTITEM, (MPARAM)index, (MPARAM)TRUE );
      }
    }

    // Ensure that one queue is selected.
    index = (LONG) WinSendMsg( hwndListbox, LM_QUERYSELECTION, (MPARAM)LIT_FIRST, 0 );
    assert( index != LIT_NONE );

    // check print-to-file button
    WinCheckButton( hwnd, IDC_TOFILE, pSetup->fToFile );
    if(! pSetup->fToFile )
    {
      hwndEntry = WinWindowFromID( hwnd, IDC_ENTRY );
      WinShowWindow( hwndEntry, FALSE );
    }

    if( 0 == strlen( pSetup->szFileName ))
    {
      // put in a default
      strcpy( pSetup->szFileName, "PRINTER.OUT" );
    }
    fOK = WinSetDlgItemText( hwnd, IDC_ENTRY, pSetup->szFileName  );
    assert( fOK );

    return 0;


  case WM_COMMAND:
    pBackup = WinQueryWindowPtr( hwnd, QWL_USER );
    pSetup =  &pBackup->Setup;
    assert( pSetup );

    hwndListbox = WinWindowFromID( hwnd, IDC_LISTBOX );
    assert( hwndListbox );

    index = (LONG) WinSendMsg( hwndListbox, LM_QUERYSELECTION, (MPARAM)LIT_FIRST, 0 );
    assert( index != LIT_NONE );
    pqi = &pSetup->pQueueInfo[ index ];


    switch(SHORT1FROMMP(mp1)) {
    case IDC_JOBPROP:
      // Call DevPostDeviceModes() to present the job setup dialog of the printer driver.
      // pqi->pszDriverName is DRIVER.Device format. Separate them.

      assert( sizeof( szDriverName ) > strlen(  pqi->pszDriverName ));
      strcpy( szDriverName, pqi->pszDriverName );

      pch = strchr( szDriverName, '.' );
      assert( pch );
      if( pch ) {
        strcpy( szDeviceName, pch+1 );
        *pch = 0;
      }
      else
      {
        *szDeviceName = 0;
      }


      // There may be more than one printer on this print queue
      pch = strchr( pqi->pszPrinters, ',' );
      if( pch ) {
        *pch = 0;
      }

      // Present the job properties dialog to the user.
      i = DevPostDeviceModes( pSetup->hab,
                              pqi->pDriverData,
                              szDriverName,
                              szDeviceName,
                              pqi->pszPrinters,
                              DPDM_POSTJOBPROP );
      assert( i == DEV_OK );
      break;

    case DID_OK:
      // New preferred queue. Modify the one in the PRINTERSETUP structure.
      strcpy( pSetup->szPreferredQueue, pqi->pszName );

      // query filename
      fOK = WinQueryDlgItemText( hwnd, IDC_ENTRY, sizeof( pSetup->szFileName ), pSetup->szFileName );
      if( ! fOK )
      {
        pSetup->fToFile = FALSE;
        *pSetup->szFileName = 0;
      }

      // save it
      memcpy( pBackup->pSaved, pSetup, sizeof( PRINTERSETUP ));
      break;

    case DID_CANCEL:
      break;
    }


    switch(SHORT1FROMMP(mp1)) {
    case DID_OK:
    case DID_CANCEL:
      free( pBackup );
      WinDismissDlg( hwnd, SHORT1FROMMP(mp1));
      break;
    }
    return 0;



  case WM_CONTROL:
    pBackup = WinQueryWindowPtr( hwnd, QWL_USER );
    pSetup =  &pBackup->Setup;

    switch( SHORT1FROMMP( mp1 )) {
    case IDC_TOFILE:
      hwndEntry = WinWindowFromID( hwnd, IDC_ENTRY );

      fOK = WinQueryButtonCheckstate( hwnd, IDC_TOFILE );
      WinShowWindow( hwndEntry, fOK );
      pSetup->fToFile = fOK;
      break;

    case IDC_LISTBOX:
      switch( SHORT2FROMMP( mp1 )) {
      case LN_ENTER:
        // User double-clicked or hit enter on a print queue in the list box.
        WinPostMsg( hwnd, WM_COMMAND, (MPARAM)DID_OK, 0 );
        break;
      }
      break;
    }
    return 0;
  }

  return( WinDefDlgProc( hwnd, msg, mp1, mp2 ));
}



//-----------------------------------------------------------------------------------------------------------
// close the PS's and DC's if they are open
// free any memory allocations

ULONG APIENTRY CleanupPrinter( PPRINTERSETUP pSetup )
{

  // Close DC's and PS's.

  assert( pSetup->hpsPrinterInfo ? pSetup->hdcPrinterInfo : TRUE );

  if( pSetup->hpsPrinterInfo )
  {
    GpiAssociate( pSetup->hpsPrinterInfo, (HDC)0 );
    GpiDestroyPS( pSetup->hpsPrinterInfo );
    pSetup->hpsPrinterInfo = (HPS)0;
  }

  if( pSetup->hdcPrinterInfo )
  {
    DevCloseDC( pSetup->hdcPrinterInfo );
    pSetup->hdcPrinterInfo = (HDC)0;
  }


  if( pSetup->pQueueInfo )
  {
    free( pSetup->pQueueInfo );
    pSetup->pQueueInfo = NULL;
  }

  if( pSetup->pDriverData )
  {
    free( pSetup->pDriverData );
    pSetup->pDriverData = NULL;
  }

  if( pSetup->devopenstruc.pszDriverName )
  {
    free( pSetup->devopenstruc.pszDriverName );
    pSetup->devopenstruc.pszDriverName = NULL;
  }

  return 0;
}


