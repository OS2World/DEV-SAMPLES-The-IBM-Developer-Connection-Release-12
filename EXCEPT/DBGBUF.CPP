#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INCL_DOS
#define INCL_ERRORS
#include <os2.h>

#include "dbgbuf.hpp"

//-----------------------------------------------------------------------------
// DebugBuffer
//-----------------------------------------------------------------------------

DebugBuffer::DebugBuffer()
{
  memset( this, 0x00, sizeof( DebugBuffer ) ) ;
}

void DebugBuffer::dump() const
{
   fprintf( stderr,
     "DosDebug Parameter Buffer:"                                 "\n"
     "  Pid %08lX Tid   %08lX"                                    "\n"
     "  Cmd %08lX Value %08lX Addr %08lX Buffer %08lX"            "\n"
     "  Len %08lX Index %08lX MTE  %08lX"                         "\n",
     Pid, Tid, Cmd, Value, Addr, Buffer, Len, Index, MTE
         ) ;
   fprintf( stderr,                                               "\n"
     "  EAX %08lX EBX %08lX ECX %08lX EDX %08lX"                  "\n"
     "  ESP %08lX EBP %08lX EDI %08lX ESI %08lX"                  "\n"
     "  EIP %08lX EFlags %08lX"                                   "\n",
     EAX, EBX, ECX, EDX, ESP, EBP, EDI, ESI, EIP, EFlags
         ) ;
   fprintf( stderr,                                               "\n"
     "  CSLim %08lX CSBase %08lX CSAcc %02X CSAtr %02X CS %04hX"  "\n"
     "  DSLim %08lX DSBase %08lX DSAcc %02X DSAtr %02X DS %04hX"  "\n"
     "  ESLim %08lX ESBase %08lX ESAcc %02X ESAtr %02X ES %04hX"  "\n"
     "  FSLim %08lX FSBase %08lX FSAcc %02X FSAtr %02X FS %04hX"  "\n"
     "  GSLim %08lX GSBase %08lX GSAcc %02X GSAtr %02X GS %04hX"  "\n"
     "  SSLim %08lX SSBase %08lX SSAcc %02X SSAtr %02X SS %04hX"  "\n",
     CSLim, CSBase, CSAcc, CSAtr, CS,
     DSLim, DSBase, DSAcc, DSAtr, DS,
     ESLim, ESBase, ESAcc, ESAtr, ES,
     FSLim, FSBase, FSAcc, FSAtr, FS,
     GSLim, GSBase, GSAcc, GSAtr, GS,
     SSLim, SSBase, SSAcc, SSAtr, SS
         ) ;
}

APIRET DebugBuffer::submit()
{
  ULONG  oldCmd = Cmd ;
  APIRET rc     = DosDebug( this ) ;
  if (    rc
       || Cmd == DBG_N_Error
     )
  {
    fprintf( stderr,  "DosDebug cmd %d error %d: Cmd %d, Value 0x%08lX (%d)\n",
            oldCmd, rc, Cmd, Value, Value
          ) ;

    dump() ;

    if ( rc == 0 )
      rc = Value ;
  }

  return rc ;
}

APIRET DebugBuffer::readMem( PID     pid           ,
                             ULONG   pSource       ,
                             ULONG   len           ,
                             ULONG   pRetrievedBuf )
{
  Cmd    = DBG_C_ReadMemBuf ;
  Pid    = pid ;
  Addr   = pSource ;
  Buffer = pRetrievedBuf ;
  Len    = len ;
  return submit() ;
}

APIRET DebugBuffer::writeMem( PID     pid      ,
                              ULONG   pTarget  ,
                              ULONG   len      ,
                              ULONG   pSendBuf )
{
  Cmd    = DBG_C_WriteMemBuf ;
  Pid    = pid ;
  Addr   = pTarget ;
  Buffer = pSendBuf ;
  Len    = len ;
  return submit() ;
}

static char gInt3 = 0xCC ; // Intel "int 3" instruction

APIRET DebugBuffer::setBreakpt ( PID    pid          ,
                                 ULONG  pWhere       ,
                                 char * pSavedOpcode )
{
  APIRET rc = readMem( pid, pWhere, 1, (ULONG)pSavedOpcode ) ;

  if ( rc == 0 )
    rc = writeMem( pid, pWhere, 1, (ULONG)&gInt3 ) ;

  return rc ;
}

APIRET DebugBuffer::getRegs()
{
  Cmd    = DBG_C_ReadReg ;
  Tid    = 0 ;
  return submit() ;
}

//-----------------------------------------------------------------------------
// AliasedAddress
//-----------------------------------------------------------------------------

AliasedAddress::AliasedAddress ( PID   pid      ,
                                 ULONG addr     ,
                                 ULONG numBytes )
  : debuggeePid      ( pid )
  , debuggeeAddress  ( addr )
  , debuggeeBase     ( debuggeeAddress & 0xFFFFF000 )
  , offset           ( debuggeeAddress & 0x00000FFF )
  , debuggeeNumBytes ( numBytes )
  , debuggerBase     ( NULL )
  , fActive          ( 0 )
{
  activate() ;
}

void AliasedAddress::activate()
{
  DebugBuffer db ;
  db.Pid    = debuggeePid ;
  db.Cmd    = DBG_C_MapRWAlias ;
  db.Buffer = 0 ;
  db.Addr   = debuggeeBase ;
  db.Len    = ( ( debuggeeNumBytes + offset + 0x0FFF )/0x1000 ) * 0x1000 ;
  if ( db.submit() == 0 )
  {
    debuggerBase = db.Buffer ;
    fActive = 1 ;
  }
}

void AliasedAddress::deactivate()
{
   DebugBuffer db ;
   db.Pid    = debuggeePid ;
   db.Cmd    = DBG_C_UnMapAlias ;
   db.Buffer = debuggerBase ;
   db.submit() ;
   fActive = 0 ;
}

//-----------------------------------------------------------------------------
// WatchPoints
//-----------------------------------------------------------------------------

WatchPoints::WatchPoints()
{
  for ( int i = 0 ; i < 4 ; ++i )
    aIndex[i] = aAddr[i] = aUsed[i] = aType[i] = 0 ;
}

WatchPoints::~WatchPoints()
{
  // ??? Should we clear them?
}

void WatchPoints::set( PID   pid  ,
                       ULONG addr ,
                       ULONG type )
{
  int iWhich = nextUnused() ;
  if ( iWhich < 0 )
    return ;

  DebugBuffer db ;
  db.Cmd   = DBG_C_SetWatch ;
  db.Pid   = pid ;
  db.Tid   = 0 ;
  db.Addr  = addr ;

  if ( type & DBG_W_Execute )
    db.Len = 1 ;
  else
    switch ( addr & 0x03 )
    {
      case 0 :
        db.Len = 4 ;
        break ;
      case 1 :
      case 3 :
        db.Len = 1 ;
        break ;
      case 2 :
        db.Len = 2 ;
        break ;
    }

  db.Value = type ; // DBG_W_Write + DBG_W_Global ;
  db.Index = 0 ;

  if ( db.submit() == 0 )
  {
    aUsed  [ iWhich ] = 1 ;
    aAddr  [ iWhich ] = addr ;
    aIndex [ iWhich ] = db.Index ;
    aType  [ iWhich ] = type ;
  }
}

void WatchPoints::reset( int iWhich,
                         PID pid   )
{
  DebugBuffer db ;
  db.Cmd   = DBG_C_SetWatch ;
  db.Pid   = pid ;
  db.Tid   = 0 ;
  db.Addr  = aAddr[ iWhich ] ;

  if ( aType[ iWhich ] & DBG_W_Execute )
    db.Len = 1 ;
  else
    switch ( aAddr[ iWhich ] & 0x03 )
    {
      case 0 :
        db.Len = 4 ;
        break ;
      case 1 :
      case 3 :
        db.Len = 1 ;
        break ;
      case 2 :
        db.Len = 2 ;
        break ;
    }

  db.Value = aType[ iWhich ] ;
  db.Index = 0 ;

  if ( db.submit() == 0 )
  {
    aUsed  [ iWhich ] = 1 ;
    aIndex [ iWhich ] = db.Index ;
  }
}

#if 0
void WatchPoints::examine( DebugBuffer& db )
{
  int iWhich = indexMatch( db.Index ) ;
  if ( iWhich < 0 )
    return ;

  // fprintf( stderr,  "Write to 0x%08lX by instruction at 0x%08lX\n",
  //         aAddr[iWhich], db.Addr ) ;

  PID   pid = db.Pid ;
  ULONG LinEbp ;
  ULONG RetIp ;

  if ( db.EBP )
  {
    if ( db.EBP > 0x10000 )
       LinEbp = db.EBP ;
    else
    {
       db.Cmd   = DBG_C_SelToLin ;
       db.Pid   = pid ;
       db.Value = db.SS ;
       db.Index = db.EBP;
       db.submit() ;

       LinEbp = db.Addr ;
    }

    db.readMem( pid, LinEbp+4, sizeof(RetIp), (ULONG)&RetIp ) ;
  }

  // At this point RetIp contains the return address
  // of the function that called the watched function
  // fprintf( stderr,  "  called from instruction just before 0x%08lX.\n", RetIp ) ;

  reset( iWhich, pid ) ;
}
#endif


int WatchPoints::nextUnused() const
{
  for ( int i = 0 ; i < 4 ; ++i )
    if ( ! aUsed[i] )
      return i ;
  return -1 ;
}

int WatchPoints::indexMatch( ULONG index ) const
{
  for ( int i = 0 ; i < 4 ; ++i )
    if ( aUsed[i] && aIndex[i] == index )
      return i ;
  return -1 ;
}


