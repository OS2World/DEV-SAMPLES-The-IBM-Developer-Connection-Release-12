// dlg.c  - dialog procs

// os2 includes
#define INCL_PM
#define INCL_SPL
#define INCL_SPLDOSPRINT
#include <os2.h>

// c includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// app includes
#include "def.h"
#include "mshell.h"
#include "pmassert.h"








// ----------------------------------------------------------------------
// at pch, put a J for job, Q for queue
// return the index of the item selected or LIT_NONE if none


#define LEN_SHORTWORK  32
#define IS_NONE         0
#define IS_QUEUE        1
#define IS_JOB          2

LONG WhichSelected( HWND hwndListbox, PULONG pulWhich, PULONG pulJob, PSZ pszQueue )
{
  LONG  iSel;
  int   i;
  char  szWork[ LEN_SHORTWORK ];
  PCHAR pch;


  // firewalls
  *pszQueue = 0;


  iSel = (LONG) WinSendMsg( hwndListbox, LM_QUERYSELECTION, (MPARAM)LIT_FIRST, 0 );
  if( LIT_NONE == iSel ) {
    *pulWhich = IS_NONE;
  } else {
    WinSendMsg( hwndListbox, LM_QUERYITEMTEXT, MPFROM2SHORT( (SHORT)iSel, LEN_SHORTWORK-1 ), (MPARAM)szWork );
    switch( *szWork ) {
    case ' ':
      // jobs have a blank as the first char on the line;
      // figure out job number
      // figure out what queue this job is in
      *pulWhich = IS_JOB;
      szWork[ LEN_SHORTWORK-1 ] = 0;
      *pulJob = atol( szWork );
      pmassert( (HAB)0, *pulJob );
      // walk back up the job until I find the queue for it
      for( i = iSel-1; i > -1; i-- ) {
        WinSendMsg( hwndListbox, LM_QUERYITEMTEXT, MPFROM2SHORT( (SHORT)i, LEN_SHORTWORK-1 ), (MPARAM)szWork );
        if( *szWork != ' ' ) {
          // found the queue name
          pch = strchr( szWork, ' ' );
          *pch = 0;
          strcpy( pszQueue, szWork );
          break;
        }
      }
      pmassert( (HAB)0,  *pszQueue );
      break;
    default:          // otherwise it is a queue; figure out queue name
      *pulWhich = IS_QUEUE;
      pch = strchr( szWork, ' ' );
      *pch = 0;
      strcpy( pszQueue, szWork );
      break;
    }
  }
  return iSel;
}


// ----------------------------------------------------------------------

MRESULT EXPENTRY SpoolerDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  CHAR           szWork[ LEN_WORKSTRING ];
  PGLOBALS       pg;
  PPRJINFO2      pj2;
  PPRQINFO3      pq3;
  PSZ            pszStatus;
  PVOID          pvBuffer;
  ULONG          ulJob;
  ULONG          cReturned;
  ULONG          cTotal;
  ULONG          cbNeeded;
  ULONG          rc;
  ULONG          ul;
  int            i, j;
  static PSZ     pszJobPaused   = "Paused";
  static PSZ     pszJobPrinting = "Printing";
  static PSZ     pszJobSpooling = "Job spooling";
  static PSZ     pszQueueError  = "Queue Error";
  static PSZ     pszQueuePaused = "Queue Paused";
  static PSZ     pszReady       = "";
  static HWND    hwndListbox;

  switch( msg ) {
  case WM_INITDLG:
    // store the pointer to globals that is coming in in mp2 in window words
    pg = (PGLOBALS) mp2;
    WinSetWindowULong( hwnd, QWL_USER, (ULONG) pg );
    // for simplicity, use a mono-spaced font in listbox
    hwndListbox = WinWindowFromID( hwnd, IDC_LISTBOX );
    pmassert( pg->hab, hwndListbox );
    strcpy(  szWork, "10.Courier"  );
    WinSetPresParam( hwndListbox, PP_FONTNAMESIZE, strlen( szWork )+1, szWork );
    break;


  case WM_COMMAND:
    switch( SHORT1FROMMP( mp1 )) {
    case IDC_REFRESH:
      // need to enumerate queues; call to get size needed
      rc = SplEnumQueue( NULL,          // local machine
                         4,             // info level
                         NULL,          // pvBuffer in next call
                         0,             // count of bytes in pvBuffer
                         &cReturned,    // number of queues returned
                         &cTotal,       // total number of queues
                         &cbNeeded,     // number bytes needed to store
                         NULL );        // reserved

      if( 0 == cTotal ) {
        // this system has no printers defined
        WinMessageBox( HWND_DESKTOP, hwnd, "You have no printers defined.", CAPTION, 0, MB_CANCEL );
        return FALSE;
      }
      // allocate memory to store the enumerated queue information
      pvBuffer = malloc( cbNeeded ) ;
      pmassert( pg->hab, pvBuffer );
      // call to get data for real
      rc = SplEnumQueue( NULL, 4, pvBuffer, cbNeeded, &cReturned, &cTotal, &cbNeeded, NULL );
      pmassert( pg->hab, 0 == rc );
      // disable listbox painting
      WinEnableWindowUpdate( hwndListbox, FALSE );
      // delete all items currently in the listbox
      WinSendMsg( hwndListbox, LM_DELETEALL, 0, 0 );
      // walk the buffer returned; see page 7-36 PM Prog Ref Vol 1 for OS2 2.0
      pq3 = (PPRQINFO3)pvBuffer;
      for( i=0; i < cReturned; i++ ) {
        // figure out queue status
        switch( PRQ_STATUS_MASK & pq3->fsStatus ) {
        case  PRQ_ACTIVE:
          pszStatus = pszReady;
          break;
        case  PRQ_PAUSED:
          pszStatus = pszQueuePaused;
          break;
        case  PRQ_ERROR:
        case  PRQ_PENDING:
          pszStatus = pszQueueError;
          break;
        }
        sprintf( szWork, "%-12s %4d job(s) %s", pq3->pszName, pq3->cJobs, pszStatus  );
        WinSendMsg( hwndListbox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) szWork );
        // snag a local copy of job count
        ulJob = pq3->cJobs;
        // bump queue pointer up one; this makes it point to the first job info for this queue
        // except in the case where cJobs is zero, then it points to next queue
        pq3++;
        pj2 = (PPRJINFO2)pq3;
        for( j=0; j < ulJob; j++ ) {
          // figure out job status
          switch( 0x0003 & pj2->fsStatus ) {
          case 1:
            pszStatus = pszJobPaused;
            break;
          case 2:
            pszStatus = pszJobSpooling;
            break;
          case 3:
            pszStatus = pszJobPrinting;
            break;
          default:
            pszStatus = pszReady;
            break;
          }
          sprintf( szWork, "%5d %-10s %-10s %s",  pj2->uJobId, pj2->pszDocument, pj2->pszComment, pszStatus );
          WinSendMsg( hwndListbox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) szWork );
          pj2++;
        }
        // job pointer pj2 now points past jobs and at the next queue
        pq3 = (PPRQINFO3)pj2;
      }
      // enable listbox painting
      WinEnableWindowUpdate( hwndListbox, TRUE  );
      // done with this memory for now
      free( pvBuffer );
      return (MRESULT) 0;


    case IDC_HOLD:
      WhichSelected( hwndListbox, &ul, &ulJob, szWork );
      switch( ul ) {
      case IS_NONE:
        break;
      case IS_QUEUE:
        SplHoldQueue( NULL, szWork );
        break;
      case IS_JOB:
        SplHoldJob( NULL, szWork, ulJob );
        break;
      }
      WinPostMsg( hwnd, WM_COMMAND, (MPARAM)IDC_REFRESH, 0 );
      return (MRESULT) 0;



    case IDC_RELEASE:
      WhichSelected( hwndListbox, &ul, &ulJob, szWork );
      switch( ul ) {
      case IS_NONE:
        break;
      case IS_QUEUE:
        SplReleaseQueue( NULL, szWork );
        break;
      case IS_JOB:
        SplReleaseJob( NULL, szWork, ulJob );
        break;
      }
      WinPostMsg( hwnd, WM_COMMAND, (MPARAM)IDC_REFRESH, 0 );
      return (MRESULT) 0;



    case IDC_DELETE:
      WhichSelected( hwndListbox, &ul, &ulJob, szWork );
      switch( ul ) {
      case IS_NONE:
      case IS_QUEUE:
        break;
      case IS_JOB:
        SplDeleteJob( NULL, szWork, ulJob );
        break;
      }
      WinPostMsg( hwnd, WM_COMMAND, (MPARAM)IDC_REFRESH, 0 );
      return (MRESULT) 0;



    case DID_OK:
    case DID_CANCEL:
      pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );
      WinSetWindowPos( hwnd, (HWND)0, 0,0,0,0, SWP_HIDE | SWP_DEACTIVATE );
      break;
    }
    return (MRESULT) 0;
  }
  return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}


