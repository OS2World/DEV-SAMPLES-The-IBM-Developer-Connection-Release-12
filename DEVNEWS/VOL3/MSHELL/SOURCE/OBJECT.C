// object.c

// the object window procedure on thread 2;
// tasks posted to the object window are not bound by 1/10 second rule;
// post an acknowledgement message back when finished




// os2 includes
#define INCL_DOS
#define INCL_DOSERRORS
#define INCL_PM
#include <os2.h>

// crt includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// app includes
#include "def.h"
#include "mshell.h"
#include "pmassert.h"



// my own header for this undocumented spooler-start API
BOOL APIENTRY Spl32QmInitialize( PULONG pulError);



// ----------------------------------------------------------------
// thread 2 code; gets and dispatches object window messages


void _Optlink threadmain( void * pv  )
{
  BOOL       bSuccess;
  HAB        hab;
  HMQ        hmq;
  QMSG       qmsg;
  PGLOBALS   pg;


  // cast and set the void pointer coming in
  pg = (PGLOBALS) pv;

  // thread initialization
  hab = WinInitialize( 0 );
  hmq = WinCreateMsgQueue( hab, 0 );

  bSuccess = WinRegisterClass( hab, OBJECTCLASSNAME, ObjectWinProc, 0, sizeof( PGLOBALS ) );
  pmassert( hab, bSuccess );

  // create a window where its parent is the PM object window
  pg->hwndObject = WinCreateWindow( HWND_OBJECT,       // parent
                                    OBJECTCLASSNAME,   // class name
                                    "",                // name
                                    0,                 // style
                                    0,                 // x,y
                                    0,
                                    0,                 // cx,cy
                                    0,
                                    HWND_OBJECT,       // owner
                                    HWND_BOTTOM,       // position behind this window (nop)
                                    0,                 // id
                                    (PVOID)pg,         // globals pointer as control data
                                    NULL );            // presparams
  pmassert( hab, pg->hwndObject );


  // WM_CREATE processing completed; application has completely initialized
  WinPostMsg( pg->hwndClient, WM_USER_ACK, (MPARAM)WM_CREATE, 0 );


  // dispatch messages; these messages will be mostly user-defined messages
  while( WinGetMsg ( hab, &qmsg, 0, 0, 0 ))  {
    WinDispatchMsg ( hab, &qmsg );
  }

  // wrap up
  WinDestroyWindow ( pg->hwndObject );
  WinDestroyMsgQueue ( hmq );
  WinTerminate ( hab );

  return;
}





// -----------------------------------------------------------------------
// object window operated by thread 2
// post wm_user messages to this window to do lengthy tasks
// mp1 is the hwnd to ack when task completes


MRESULT EXPENTRY ObjectWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  BOOL              bAutoStart;
  BOOL              bOK;
  CHAR              szTitle[ LEN_WORKSTRING ];
  CHAR              szWork[ LEN_WORKSTRING ];
  FILE              *f;
  HAB               hab;
  HWND              hwndToAck;
  PCHAR             pch;
  PGLOBALS          pg;
  LONG              i;
  LONG              cLines;
  STARTDATA         sd;
  ULONG             rc;
  ULONG             ulProcID;
  ULONG             ulSessionID;



  // store the handle of the window to ack upon task completion;
  hwndToAck = (HWND)mp1;
  hab = WinQueryAnchorBlock( hwnd );


  switch( msg ) {


  case WM_CREATE:
    // mp1 is pointer to globals; save it in object window words
    pg = (PGLOBALS)mp1;
    WinSetWindowULong( hwnd, QWL_USER, (ULONG) mp1 );


    // start the spooler ?
    PrfQueryProfileString( HINI_PROFILE, "PM_SPOOLER", "SPOOL", "1;", szWork, LEN_WORKSTRING );
    if( *szWork == '1' ) {

      // assume a bad spooler start
      pg->fSpooler = FALSE;

      memset( &sd, 0, sizeof( STARTDATA ) );
      sd.Length          = sizeof( STARTDATA );
      sd.FgBg            = 1;                            // 1=background
      sd.PgmTitle        = "Print Manager";
      sd.PgmName         = "PMSPOOL.EXE";
      sd.PgmInputs       = szWork;
      sd.SessionType     = 3;                            // 3=pm
      rc = DosStartSession( &sd, &ulSessionID, &ulProcID );
      switch( rc ) {
      case NO_ERROR:
        // pmspool.exe from 1.3 started ok
        pg->fSpooler = TRUE;
        break;
      default:
        Spl32QmInitialize( &rc );
        pmassert( pg->hab, 0 == rc );
        pg->fSpooler = ( 0 == rc );
        break;
      }

      if( ! pg->fSpooler )  {
        WinEnableMenuItem( pg->hwndMenubar,  IDM_SPOOLER,  FALSE );
        WinPostMsg( pg->hwndClient, WM_NACK_NO_SPOOLER, 0, 0 );
      }
    }


    // gather program data by parsing the INI file
    if( WinSendMsg( hwnd, WM_USER_ADD_PROGRAMS, (MPARAM)hwnd, 0 )) {
      // start the autostarts
      for( i = 0; i < pg->cStartem; i++ ) {
        if( pg->aStartem[ i ].bAutoStart ) {
          WinSendMsg( hwnd, WM_USER_START, (MPARAM)hwnd, (MPARAM) i );
        }
      }
    } else {
      // no ini file
      WinPostMsg( pg->hwndClient, WM_NACK_NO_INI, 0, 0 );
    }

    return (MRESULT) 0;





  case WM_USER_ACK:
    // nop for object window
    return (MRESULT) 0;



  case WM_USER_ADD_PROGRAMS:
    pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );

    // free allocations from the last time
    for( i = 0; i < pg->cStartem; i++ ) {
      if( pg->aStartem[ i ].pszTitle ) {
        free( pg->aStartem[ i ].pszTitle );
        pg->aStartem[ i ].pszTitle = NULL;
      }
      if( pg->aStartem[ i ].pszCMD ) {
        free( pg->aStartem[ i ].pszCMD );
        pg->aStartem[ i ].pszCMD = NULL;
      }
    }

    // reset how many items are startable
    pg->cStartem = 0;


    // open the text ini
    f = fopen( "\\MSHELL.INI" , "r" );
    if( !f ) {
      // no file
      WinPostMsg( hwndToAck, WM_NACK_NO_INI, (MPARAM)msg, (MPARAM) FALSE );
      return (MPARAM) FALSE;
    }

    // disable listbox painting
    WinEnableWindowUpdate( pg->hwndListbox, FALSE );

    // delete all in listbox
    WinSendMsg( pg->hwndListbox, LM_DELETEALL, 0, 0 );


    cLines = 0;
    i = 0;
    while( i < LEN_STARTLIST && fgets( szWork, LEN_WORKSTRING, f )) {
      cLines++;
      ltrim( trim( szWork ));
      memset( &(pg->aStartem[ i ]),  0, sizeof( STARTEM ));

      // comment lines begin with *
      if( NULL == strchr( "*", *szWork )  && strlen( szWork ) ) {

        // not a comment and not an empty line. parse it
        bOK = FALSE;

        // check for autostart symbol
        bAutoStart = ('!' == *szWork);

        if( bAutoStart ) {
          pch = strchr( szWork, '!' );
          pmassert( pg->hab, pch );
          *pch = ' ';
          ltrim( szWork );
        }

        // parse title up to ;
        pch = strtok( szWork, ";" );
        if( pch ) {
          strcpy( szTitle, pch );

          // parse the rest as a CMD start command
          pch = strtok( NULL, "\n" );
          bOK = (BOOL) pch;
        }

        // test result of parsing
        if( bOK ) {
          pg->aStartem[ i ].bAutoStart = bAutoStart;
          pg->aStartem[ i ].pszTitle   = strdup( szTitle );
          pg->aStartem[ i ].pszCMD     = strdup( pch );
          WinSendMsg( pg->hwndListbox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) pg->aStartem[i].pszTitle );
          // bump count of startable items
          i++;
        } else {
          // fire off a nack to elicit a message box; provide line number
          WinPostMsg( hwndToAck, WM_NACK_SYNTAX_ERROR, (MPARAM)msg, (MPARAM)cLines );
        }
      }
    }
    pg->cStartem = i;
    fclose( f );

    if( pg->cStartem > 0 ) {
      // select the first item
      WinSendMsg( pg->hwndListbox, LM_SELECTITEM, (MPARAM) 0, (MPARAM) TRUE );
    }

    // let listbox paint itself
    WinEnableWindowUpdate( pg->hwndListbox, TRUE );

    // success
    WinPostMsg( hwndToAck, WM_USER_ACK, (MPARAM)msg, (MPARAM) TRUE );
    return (MRESULT) TRUE;




  case WM_USER_START:
    // index of program to start is in mp2

    // start an invisible, transient CMD session to handle the stored start command
    pg = (PGLOBALS) WinQueryWindowULong( hwnd, QWL_USER );

    // make, modify a copy of the CMD start command
    sprintf( szWork, "/c %s",  pg->aStartem[ (SHORT)mp2 ].pszCMD );

    // a startdata structure for DosStartSession that will start CMD.EXE;
    // the parameter to CMD is the start command in szWork
    memset( &sd, 0, sizeof( STARTDATA ));
    sd.Length          = sizeof( STARTDATA );
    sd.PgmInputs       = szWork;
    sd.PgmControl      = SSF_CONTROL_INVISIBLE;

    rc = DosStartSession( &sd, &ulSessionID, &ulProcID );
    pmassert( hab, rc == 0 ||  rc == ERROR_SMG_START_IN_BACKGROUND   );

    WinPostMsg( hwndToAck, WM_USER_ACK, (MPARAM)msg, 0 );
    break;




  case WM_USER_START_CMD:
    // start CMD in a window; this is default for DosStartSession
    memset( &sd, 0, sizeof( STARTDATA ));
    sd.Length = sizeof( STARTDATA );
    rc = DosStartSession( &sd, &ulSessionID, &ulProcID );
    pmassert( hab, rc == 0 );
    WinPostMsg( hwndToAck, WM_USER_ACK, 0, 0 );
    break;

  }

  // default:
  return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}






// ----------------------------------------------------------------------------------------------------------

char *trim( char *s )
{
  char *p;
  p=s;
  while( *s ) s++;
  s--;
  while( *s == '\r' || *s == ' ' || *s == '\n' || *s == '\t' ) *s-- = 0;
  return p;
}



// -----------------------------------------------------------------------

char *ltrim( char *s )
{
  char *p, *keep;
  keep=p=s;
  while( (*p) && (*p == ' ') ) p++;
  if( p > s ) while( *s++ = *p++ );
  return keep;
}




