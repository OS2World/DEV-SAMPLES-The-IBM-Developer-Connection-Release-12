#ifndef DBGBUF_HPP_INCLUDED
#define DBGBUF_HPP_INCLUDED

#define INCL_DOS
#define INCL_ERRORS
#include <os2.h>

//-----------------------------------------------------------------------------
// DebugBuffer
//-----------------------------------------------------------------------------

struct DebugBuffer : public uDB_t   // Defined in BSEDOS.H
{
          DebugBuffer() ;

   void   dump       () const ;

   APIRET submit     () ;

   APIRET readMem    ( PID     pid           ,
                       ULONG   pSource       ,
                       ULONG   len           ,
                       ULONG   pRetrievedBuf ) ;

   APIRET writeMem   ( PID     pid      ,
                       ULONG   pTarget  ,
                       ULONG   len      ,
                       ULONG   pSendBuf ) ;

   APIRET setBreakpt ( PID    pid          ,
                       ULONG  pWhere       ,
                       char * pSavedOpcode ) ;

   APIRET getRegs    () ;

} ;

//-----------------------------------------------------------------------------
// AliasedAddress
//-----------------------------------------------------------------------------

class AliasedAddress
{
  public  :

  void           activate       () ;

  void           deactivate     () ;

                 AliasedAddress ( PID   pid      ,
                                  ULONG addr     ,
                                  ULONG numBytes ) ;

                ~AliasedAddress ()
                 {
                    if ( fActive )
                      deactivate() ;
                 }

  ULONG          myAddress      () const { return debuggerBase + offset ; }
  ULONG          hisAddress     () const { return debuggeeAddress ; }

  private :

  PID            debuggeePid      ;
  ULONG          debuggeeAddress  ;
  ULONG          debuggeeBase     ; // Page-aligned
  ULONG          offset           ;
  ULONG          debuggeeNumBytes ;

  ULONG          debuggerBase     ; // Debugger uses this address to access
                                    //  debuggee's storage

  int            fActive ;

  // Not allowed
                 AliasedAddress ( const AliasedAddress& a ) ;
  AliasedAddress operator=      ( const AliasedAddress& a ) ;
} ;

//-----------------------------------------------------------------------------
// WatchPoints
//-----------------------------------------------------------------------------

class WatchPoints
{
  public  :

                 WatchPoints    () ;

                ~WatchPoints    () ;

  void           set            ( PID   pid  ,
                                  ULONG addr ,
                                  ULONG type ) ;

  void           reset           ( int iWhich,
                                   PID pid   ) ;

//void           examine         ( DebugBuffer& db ) ;

  private :

  int            nextUnused      () const ;

  int            indexMatch      ( ULONG index ) const ;

  ULONG          aIndex [ 4 ] ;
  ULONG          aAddr  [ 4 ] ;
  int            aUsed  [ 4 ] ;
  int            aType  [ 4 ] ;

  // Not allowed
                 WatchPoints    ( const WatchPoints& w ) ;
  WatchPoints&   operator=      ( const WatchPoints& w ) ;

} ;

#endif /* DBGBUF_HPP_INCLUDED */
