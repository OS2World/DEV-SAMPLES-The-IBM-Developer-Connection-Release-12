//-----------------------------------------------------------------------------
//  STACKNFO -- Report the stack utilization for a child process
//
//  Syntax:
//    STACKNFO <command-line-of-process-to-be-monitored>
//
//  Example:
//    STACKNFO view cmdref
//
//  Written by: Eric Diamond  EJDIAMO at NYCVMIC1                  10 July 1996
//                            ejdiamo@vnet.ibm.com
//
//  Based on: TRAPTRAP.C from EXCEPT PACKAGE by Mark Fiammante
//-----------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h> // isalnum

#include <io.h> // isatty

#include <fstream.h>

#define INCL_DOS
#define INCL_ERRORS
#include <os2.h>

#include <newexe.h>  // struct exe_hdr
#undef   NDISCARD    // Double-defined in both
#undef   NSDISCARD   //   newexe.h & exe386.h
#undef   NSMOVE      // "
#undef   NSSHARED    // "
#undef   NSPRELOAD   // "
#undef   NSEXRD      // "
#undef   NSCONFORM   // "
#define  DWORD unsigned long  int
#define  WORD  unsigned short int
#include <exe386.h>  // struct e32_exe

#include <iksset.h>

#include "thread.hpp"
#include "dbgbuf.hpp"

//-----------------------------------------------------------------------------
// Typedefs for collections
//-----------------------------------------------------------------------------

typedef IKeySortedSet< ThreadInfo, TID > ThreadInfoSet ;

//-----------------------------------------------------------------------------
// Global constants
//-----------------------------------------------------------------------------

static const char gszQueueName [] = "\\QUEUES\\STACKNFO\\STACKNFO.QUE" ;

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

DebugBuffer   gDbgBuf ;

RESULTCODES   gResultCode ;

ThreadInfoSet gThreadInfo ;

ULONG         gulStackSize   = 0 ;  // From EXE hdr: thread 1 stack size
ULONG         gulStackTop    = 0 ;  // From EXE hdr: initial ESP (top of stack)
ULONG         gulStartLinear = 0 ;  // DosDebug xform of object:offset to linear addr

ULONG         gulT1Bot       = 0 ;  // Set at thread 1 creation, from TIB data
ULONG         gulT1Top       = 0 ;  // Set at thread 1 creation, from TIB data

//-----------------------------------------------------------------------------
// Utility functions
//-----------------------------------------------------------------------------

void Pause()
{
  puts( "Press Enter to continue..." ) ;
  if ( isatty( _fileno(stdout) ) )
  {
    static  char pausebuf [50];
    gets( pausebuf ) ;
  }
}

void GetExeInfo( HMODULE hmod )
{
  static char exename [ CCHMAXPATH ] = "" ;
  DosQueryModuleName( hmod, sizeof(exename), exename ) ;

  fstream exefile( exename, ios::binary | ios::in ) ;
  if ( !exefile )
    return ;

  struct exe_hdr dosHeader ;
  struct e32_exe lxHeader ;

  exefile.read( (char*)&dosHeader, sizeof(dosHeader) ) ;
  exefile.seekg( dosHeader.e_lfanew, ios::beg )
         .read( (char*)&lxHeader, sizeof( lxHeader ) ) ;

  exefile.close() ;

  if ( E32_MAGIC1(lxHeader) != E32MAGIC1 ||
       E32_MAGIC2(lxHeader) != E32MAGIC2
     )
    return ;

  gulStackSize = lxHeader.e32_stacksize ;
  gulStackTop  = lxHeader.e32_esp ;

  DebugBuffer db ;
  db.Pid   = gResultCode.codeTerminate ;
  db.Cmd   = DBG_C_NumToAddr ;
  db.Value = lxHeader.e32_startobj ;
  db.MTE   = hmod ;
  if ( db.submit() == 0 )
  {
    gulStartLinear = db.Addr + lxHeader.e32_eip ;
  }

/*
  fprintf( stderr,  "Module %s: stacksize %08lX stackinit %08lX\n"
          " startobj %d starteip %08lX starteipbase %08lX.\n",
          exename, gulStackSize, gulStackTop,
          lxHeader.e32_startobj, lxHeader.e32_eip, gulStartLinear
        ) ;
*/

}

void DumpException( EXCEPTIONREPORTRECORD * pERepRec,
                    CONTEXTRECORD         * pCtxRec )
{
  fprintf( stderr,
          "Exception Report Record:"                                      "\n"
          "  XcptNum  %08lX Flags    %08lX pNested %08lX"                 "\n"
          "  XcptAddr %08lX InfoSize %08lX"                               "\n"
          "  Info[0]  %08lX Info[1]  %08lX Info[2] %08lX Info[3] %08lX"   "\n"
          , pERepRec->ExceptionNum
          , pERepRec->fHandlerFlags
          , pERepRec->NestedExceptionReportRecord
          , pERepRec->ExceptionAddress
          , pERepRec->cParameters
          , pERepRec->ExceptionInfo[0]
          , pERepRec->ExceptionInfo[1]
          , pERepRec->ExceptionInfo[2]
          , pERepRec->ExceptionInfo[3]
        ) ;

  fprintf( stderr,
          "Exception Context Record:"                                     "\n"
          "  CtxFlags %08lX"                                              "\n"
          "  Gs       %08lX Fs  %08lX Es %08lX Ds %08lX"                  "\n"
          "  Edi      %08lX Esi %08lX"                                    "\n"
          "  Eax      %08lX Ebx %08lX Ecx %08lX Edx %08lX"                "\n"
          "  Ebp      %08lX Eip %08lX Cs  %08lX EFlags %08lX"             "\n"
          "  Esp      %08lX Ss  %08lX"                                    "\n"
          , pCtxRec->ContextFlags
          , pCtxRec->ctx_SegGs
          , pCtxRec->ctx_SegFs
          , pCtxRec->ctx_SegEs
          , pCtxRec->ctx_SegDs
          , pCtxRec->ctx_RegEdi
          , pCtxRec->ctx_RegEsi
          , pCtxRec->ctx_RegEax
          , pCtxRec->ctx_RegEbx
          , pCtxRec->ctx_RegEcx
          , pCtxRec->ctx_RegEdx
          , pCtxRec->ctx_RegEbp
          , pCtxRec->ctx_RegEip
          , pCtxRec->ctx_SegCs
          , pCtxRec->ctx_EFlags
          , pCtxRec->ctx_RegEsp
          , pCtxRec->ctx_SegSs
        ) ;
}

//-----------------------------------------------------------------------------
// StackInfo class
//-----------------------------------------------------------------------------

struct StackInfo
{
  PID              ivPid          ;
  TID              ivTid          ;
  ULONG            ivpMyBot       ;
  ULONG            ivpHisTop      ;
  ULONG            ivpHisBot      ;
  ULONG            ivpMyTop       ;
  ULONG            ivAmountUnused ;
  ULONG            ivLowestPage   ; // Addr of lowest committed stack page
  AliasedAddress * pAlias         ;

                 StackInfo      ( PID pid, TID tid, ULONG bot, ULONG top, ULONG start ) ;
                ~StackInfo      () ;
  void           startMarking   ( ULONG start ) ;

  private :
  // Not allowed
                 StackInfo      ( const StackInfo& s ) ;
  StackInfo&     operator=      ( const StackInfo& s ) ;
} ;

StackInfo::StackInfo( PID pid, TID tid, ULONG bot, ULONG top, ULONG start )
  : ivPid          ( pid )
  , ivTid          ( tid )
  , ivpHisTop      ( top )
  , ivpHisBot      ( bot )
  , ivAmountUnused ( 0   )
  , ivLowestPage   ( 0   )
{
  pAlias    = new AliasedAddress( pid, bot, (ivpHisTop-ivpHisBot) ) ;
  ivpMyBot  = pAlias->myAddress() ;
  ivpMyTop  = ivpMyBot + (ivpHisTop-ivpHisBot) ;

  /*
  fprintf( stderr,  "Stack  : pid %d tid %d top %08lX bot %08lX (alias %08lX)\n",
          pid, tid, top, bot, ivpMyBot
        ) ;
  */

  startMarking( start ) ;
}

int PageIsOk( ULONG pageAddress )
{
  ULONG  ulNumPages   = 1 ;
  ULONG  ulAllocFlags ;
  APIRET rc  = DosQueryMem( (PVOID)pageAddress, &ulNumPages, &ulAllocFlags ) ;
  int    fOk =    rc == 0
               && ( ulAllocFlags & PAG_COMMIT )
               && ( ulAllocFlags & PAG_WRITE  )
               && ( ! ( ulAllocFlags & PAG_GUARD ) ) ;

  return fOk ;
}

void StackInfo::startMarking( ULONG start )
{
  ULONG * pMyCurrent  = (ULONG*)( ivpMyTop - (ivpHisTop-start) - sizeof(void*) ) ;
  ULONG * pHisCurrent = (ULONG*)(                       start  - sizeof(void*) ) ;

  /*
  fprintf( stderr,  "Marking: pid %d tid %d start %08lX (alias %08lX)\n",
          ivPid, ivTid, pHisCurrent, pMyCurrent
        ) ;
  */

  /* Put a known value in every location on the stack.
     This routine writes from the current stack address
     down to the bottom of the stack.
  */

  ULONG ulCurrentPage = (ULONG)pMyCurrent & 0xFFFFF000UL ;
  ULONG ulPrevPage    = ulCurrentPage ;
  ivLowestPage        = ulCurrentPage ;

  while ( pMyCurrent >= (ULONG*)ivpMyBot )
  {
    ulCurrentPage = (ULONG)pMyCurrent & 0xFFFFF000UL ;
    if ( ulCurrentPage != ulPrevPage )
    {
      if ( ! PageIsOk( ulCurrentPage ) )
        break ;
      ivLowestPage = ulPrevPage = ulCurrentPage ;
    }

    *pMyCurrent = (unsigned long)pHisCurrent ;
    --pMyCurrent ;
    --pHisCurrent ;
  }
}

StackInfo::~StackInfo()
{
  ULONG * pMyBot  = (ULONG*)ivpMyBot ;
  ULONG * pHisBot = (ULONG*)ivpHisBot ;
  ULONG   ulCount = 0 ;

  // Move lowest page downward if necessary
  // (This would only be necessary if we missed
  // a guard-page exception).
  while ( ivLowestPage > ivpMyBot )
  {
    ULONG testPage = ivLowestPage - 0x1000 ;
    if ( ! PageIsOk( testPage ) )
      break ;
    ivLowestPage -= 0x1000 ;
  }

  if ( ivpMyBot < ivLowestPage )
  {
    ULONG ulDelta = ivLowestPage - ivpMyBot ;
    pMyBot  = (ULONG*)ivLowestPage ;
    pHisBot = (ULONG*)(ivpHisBot + ulDelta ) ;
    ulCount = ulDelta / sizeof(ULONG) ;
  }

  while ( *pMyBot == (ULONG)pHisBot )
  {
    ++ulCount ;
    ++pMyBot ;
    ++pHisBot ;
  }

  ivAmountUnused = ulCount * sizeof(ULONG) ;

  fprintf( stderr,  "Thread %2d: total %8d = used %8d + unused %8d.\n",
          ivTid,
          (ivpMyTop-ivpMyBot),
          (ivpMyTop-ivpMyBot) - ivAmountUnused,
          ivAmountUnused
        ) ;

  delete pAlias ;
  pAlias = NULL ;
}

void MarkStack( PID pid, TID tid, ULONG bot, ULONG top, ULONG start )
{
  ThreadInfo t ;
  StackInfo * p = new StackInfo( pid, tid, bot, top, start ) ;
  t.ivPid = pid ;
  t.ivTid = tid ;
  t.ivpStackInfo = p ;
  gThreadInfo.add( t ) ;
}

void CheckStack( TID tid )
{
  ThreadInfo& t = gThreadInfo.elementWithKey( tid ) ;
  delete t.ivpStackInfo ;
  t.ivpStackInfo = NULL ;
}

// Cmdline: STACKNFO\0client\0\0
//          STACKNFO\0client parms\0\0
char * GetClientParms()
{
  PPIB   ppib;
  PTIB   ptib;

  DosGetInfoBlocks ( &ptib, &ppib ) ;

  // Skip over command name "STACKNFO"
  char * p = ppib->pib_pchcmd;
  while ( *p++ )
    ; // Empty

  // Skip over blank(s)
  while ( *p++ == ' ' )
    ; // Empty

  // Skip over CLIENT command name
  while ( *p && *p++ != ' ' )
    ; // Empty

  if ( *p == 0x00 ) // Nothing follows command name
    return strdup( "" ) ;

  // Skip over blank(s)
  while ( *p && *p++ == ' ' )
    ; // Empty

  return strdup( --p ) ;
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------

void PrintHelp()
{
  fputs(                                                                   "\n"
         "STACKNFO -- Report the stack utilization for a child process"    "\n"
                                                                           "\n"
         "Syntax:"                                                         "\n"
         "  STACKNFO <command-line-of-process-to-be-monitored>"            "\n"
                                                                           "\n"
         "Example:"                                                        "\n"
         "  STACKNFO view cmdref"                                          "\n"
                                                                           "\n"
         "Written by: Eric Diamond  EJDIAMO at NYCVMIC1  -- 10 July 1996"  "\n"
         "                          ejdiamo@vnet.ibm.com"                  "\n"
                                                                           "\n"
         "Based on: TRAPTRAP.C from EXCEPT PACKAGE by Mark Fiammante"      "\n"
         , stderr
       ) ;
}

int main( int argc, char * argv[] )
{
   APIRET        rc;
   char        * Dot;
   char        * Slash;
   ULONG         SessId;
   STARTDATA     StartData;
   ULONG         AppType;
   char        * CmdBuf ;
   static char   ObjectBuffer[ CCHMAXPATH ] ;
   static char   ProcessName [ CCHMAXPATH ] ;
   int           iWhichWatch = 0 ;

   if ( argc < 2 || !isalnum( argv[1][0] ) )
   {
      PrintHelp() ;
      return ERROR_INVALID_PARAMETER ;
   }

   /*--------------------------------------------------*/
   /*- Build new command line    ----------------------*/

   --argc ; ++argv ;
   strcpy( ProcessName, argv[0] ) ;

   Dot = strrchr( ProcessName, '.' ) ;
   /*-- look for .EXE */
   if ( Dot == NULL )
      strcat( ProcessName, ".EXE" ) ;
   else
   {
      Slash = strrchr( ProcessName, '\\' ) ;
      /* Maybe '.' for current directory but no EXE extension */
      if ( Slash > Dot )
         strcat( ProcessName, ".EXE" ) ;
   }
   strupr( ProcessName ) ;

   CmdBuf = GetClientParms() ; // Memory leak

   /*--------------------------------------------------*/
   HQUEUE hQueue = NULLHANDLE ;
   rc = DosCreateQueue( &hQueue, QUE_FIFO, gszQueueName ) ;
   if ( rc )
   {
      fprintf( stderr,  "Create queue rc %d\n", rc ) ;
      exit( rc ) ;
   }

   memset( &StartData, 0x00, sizeof(STARTDATA) ) ;
   DosQueryAppType( ProcessName, &AppType ) ;
   AppType = 0x0003 & AppType ;

   StartData.Length        = sizeof(STARTDATA) ;    /* length of data structure    */
   StartData.Related       = SSF_RELATED_CHILD ;    /* 0 = independent session,    */
                                                    /* 1 = child session           */
   StartData.FgBg          = SSF_FGBG_BACK ;        /* 0 = start in foreground,    */
                                                    /* 1 = start in background     */
   StartData.TraceOpt      = SSF_TRACEOPT_TRACE ;   /* 0 = no trace, 1 = trace     */
   StartData.PgmTitle      = ProcessName ;          /* address of program title    */
   StartData.PgmName       = ProcessName ;          /* address of program name     */
   StartData.PgmInputs     = CmdBuf ;               /* input arguments             */
   StartData.TermQ         = (BYTE*)gszQueueName ;  /*address of program queue name*/
   StartData.Environment   = NULL ;                 /* environment string          */
   StartData.InheritOpt    = SSF_INHERTOPT_PARENT ; /* where are handles and       */
                                                    /*  environment inherited from */
                                                    /*     0 = inherit from shell, */
                                                    /*     1 = inherit from caller */
   StartData.SessionType   = AppType ;              /* session type                */
   StartData.IconFile      = 0 ;                    /* address of icon definition  */
   StartData.PgmHandle     = 0 ;                    /* program handle              */
   StartData.PgmControl    = SSF_CONTROL_VISIBLE ;  /*initial state of windowed app*/
   StartData.InitXPos      = 0 ;                    /*x coor of init session window*/
   StartData.InitYPos      = 0 ;                    /*y coor of init session window*/
   StartData.InitXSize     = 0 ;                    /* initial size of x           */
   StartData.InitYSize     = 0 ;                    /* initial size of y           */
   StartData.Reserved      = 0 ;
   StartData.ObjectBuffer  = ObjectBuffer ;
   StartData.ObjectBuffLen = sizeof(ObjectBuffer) ;

   DosSelectSession( 0 ) ;

   rc = DosStartSession(
     &StartData,
     &SessId,
     &gResultCode.codeTerminate);

/*
   fprintf( stderr,  "Process %s, Id=%d, Session=%d,arguments=\"%s\"\n",
             ProcessName,
             gResultCode.codeTerminate,
             SessId,
             CmdBuf);
*/

   if ((rc != NO_ERROR)&&(rc!=ERROR_SMG_START_IN_BACKGROUND))
   {
       fprintf( stderr, "StartSession rc %d %s\n", rc, ObjectBuffer ) ;
       PrintHelp() ;
       return 0 ;
   }

   gDbgBuf.Cmd = DBG_C_Connect ;
   gDbgBuf.Pid = gResultCode.codeTerminate;
   gDbgBuf.Tid = 0;
   gDbgBuf.Value = DBG_L_386;
   rc = gDbgBuf.submit() ;

   WatchPoints * pWatches = new WatchPoints() ;  // ??? Mem leak

   while ( gDbgBuf.Cmd != DBG_N_ProcTerm )
   {
     DosSleep( 50L ) ;  // WHY??? EJD

     if ( gDbgBuf.Cmd == DBG_N_Exception )
     {
       gDbgBuf.Cmd = DBG_C_Continue ;
       gDbgBuf.Pid = gResultCode.codeTerminate;
       gDbgBuf.Value = XCPT_CONTINUE_SEARCH ;
       rc = gDbgBuf.submit() ;
     }
     else
     {
       gDbgBuf.Cmd = DBG_C_Go ;
       gDbgBuf.Pid = gResultCode.codeTerminate ; // Pid of debuggee
       rc = gDbgBuf.submit() ;
     }

     switch ( gDbgBuf.Cmd )
     {
         case DBG_N_Success:
            break;

         case DBG_N_Error :
            fprintf( stderr,  "DosDebug error 0x%08lX (%d)... exiting.\n", gDbgBuf.Value, gDbgBuf.Value ) ;
            exit( rc ) ;
            break;

         case DBG_N_ProcTerm:
            fprintf( stderr,  "\nProcess ended with rc %d\n", gDbgBuf.Value ) ;
            Pause() ;
            break;

         case DBG_N_Exception:
            /*
            fprintf( stderr,  "Exception occurred on thread %d\n"
                    " Value %08lX Addr %08lX Buffer %08lX Len %08lX\n",
                    gDbgBuf.Tid,
                    gDbgBuf.Value, gDbgBuf.Addr,
                    gDbgBuf.Buffer, gDbgBuf.Len
                  ) ;
            */
            break;

         case DBG_N_ModuleLoad:
            break;

         case DBG_N_CoError:
            fprintf( stderr,  "Coprocessor error\n" ) ;
            break;

         case DBG_N_ThreadTerm:
            fprintf( stderr,  "Thread %2d terminated with rc %d\n", gDbgBuf.Tid, gDbgBuf.Value ) ;
            CheckStack( gDbgBuf.Tid ) ;
            gThreadInfo.removeElementWithKey( gDbgBuf.Tid ) ;
            break;

         case DBG_N_AsyncStop:
            fprintf( stderr,  "Asynchronous stop\n" ) ;
            break;

         case DBG_N_NewProc:
            break;

         case DBG_N_AliasFree:
            fprintf( stderr,  "Alias freed\n" ) ;
            break;

         case DBG_N_Watchpoint:
            /*
            fprintf( stderr,  "Watchpoint hit\n" ) ;
            gDbgBuf.dump() ;
            */
            // Mark stack here for thread 1
            if ( gDbgBuf.Tid == 1 )
            {
              if ( iWhichWatch == 1 )
              {
                MarkStack( gDbgBuf.Pid, gDbgBuf.Tid, gulT1Bot, gulT1Top, gDbgBuf.ESP ) ;
                pWatches->set( gDbgBuf.Pid, gulT1Bot, DBG_W_Write + DBG_W_Local  ) ;
                iWhichWatch = 2 ;
              }
              else if ( iWhichWatch == 2 )
              {
                fputs(                                                      "\n"
                        "\a\a\a\a\a"                                        "\n"
                        "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"  "\n"
                        "Bottom of thread 1 stack has just been written!"   "\n"
                        "Further stack use will destroy static data area!"  "\n"
                        "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"  "\n"
                        , stderr
                     ) ;
                Pause() ;
                iWhichWatch = 0 ;
              }
            }
            break;

         case DBG_N_ThreadCreate:
            {
              fprintf( stderr,  "Thread %2d created\n", gDbgBuf.Tid ) ;
              if ( gDbgBuf.Tid == 1 )
              {
                GetExeInfo( gDbgBuf.MTE ) ;
                pWatches->set( gDbgBuf.Pid, gulStartLinear, DBG_W_Execute + DBG_W_Local  ) ;
                iWhichWatch = 1 ;
              }

              AliasedAddress tib( gDbgBuf.Pid, gDbgBuf.FSBase, sizeof(TIB) ) ;
              /*
              fprintf( stderr,  "  Thread stack bot %08lX top %08lX\n",
                      ((TIB*)tib.myAddress())->tib_pstack,
                      ((TIB*)tib.myAddress())->tib_pstacklimit
                    ) ;
              */
              // Mark stack here for thread 2-n -- from ESP-4 on down
              if ( gDbgBuf.Tid != 1 )
              {
                MarkStack( gDbgBuf.Pid,
                           gDbgBuf.Tid,
                           (ULONG)((TIB*)tib.myAddress())->tib_pstack,
                           (ULONG)((TIB*)tib.myAddress())->tib_pstacklimit,
                           gDbgBuf.ESP
                         ) ;
              }
              else
              {
                gulT1Bot = (ULONG)((TIB*)tib.myAddress())->tib_pstacklimit - gulStackSize ;
                gulT1Top = (ULONG)((TIB*)tib.myAddress())->tib_pstacklimit ;
              }
            }
            break;

         case DBG_N_ModuleFree:
            break;

         case DBG_N_RangeStep:
            fprintf( stderr,  "RangeStep fault\n" ) ;
            break;

     } // switch

     if ( gDbgBuf.Cmd == DBG_N_Exception )
     {
       ULONG Value     = gDbgBuf.Value ;
       ULONG Exception = gDbgBuf.Buffer ;

       gDbgBuf.Cmd = DBG_C_Continue ;
       gDbgBuf.Pid = gResultCode.codeTerminate ;
       gDbgBuf.Value = XCPT_CONTINUE_STOP ;
       rc = gDbgBuf.submit() ;

       if (    Value == DBG_X_FIRST_CHANCE
            || Value == DBG_X_STACK_INVALID
          )
       {
         EXCEPTIONREPORTRECORD ERepRec ;
         CONTEXTRECORD         CtxRec ;
         ULONG                 pERepRec = gDbgBuf.Buffer ;
         ULONG                 pCtxRec  = gDbgBuf.Len ;

         if (    0 == gDbgBuf.readMem( gResultCode.codeTerminate,
                                       pERepRec,
                                       sizeof(EXCEPTIONREPORTRECORD),
                                       (ULONG)&ERepRec
                                     )
              && 0 == gDbgBuf.readMem( gResultCode.codeTerminate,
                                       pCtxRec,
                                       sizeof(CONTEXTRECORD),
                                       (ULONG)&CtxRec
                                     )
            )
         {
           if (    ERepRec.ExceptionNum     == XCPT_GUARD_PAGE_VIOLATION
                && ERepRec.ExceptionInfo[0] == XCPT_WRITE_ACCESS
              )
           {
             /*
             fprintf( stderr,  "Guard page written: page %08lX cs:eip %04hX:%08lX\n",
                     ERepRec.ExceptionInfo[1], CtxRec.ctx_SegCs, CtxRec.ctx_RegEip
                   ) ;
             */

             ThreadInfo & t = gThreadInfo.elementWithKey( gDbgBuf.Tid ) ;
             t.ivBreakEip = CtxRec.ctx_RegEip ;
             t.ivBreakP1  = ERepRec.ExceptionInfo[1] ;

             gDbgBuf.setBreakpt( gResultCode.codeTerminate,
                                 CtxRec.ctx_RegEip,
                                 &t.ivBreakPrevInstr
                               ) ;
           }
           else if (    ERepRec.ExceptionNum != XCPT_PROCESS_TERMINATE
                     && ERepRec.ExceptionNum != XCPT_ASYNC_PROCESS_TERMINATE
                     && ERepRec.ExceptionNum != XCPT_SIGNAL
                   )
           {
             DumpException( &ERepRec, &CtxRec ) ;
             if ( ERepRec.ExceptionNum == XCPT_ACCESS_VIOLATION )
             {
               if ( CtxRec.ctx_RegEsp < gulT1Bot )
               {
                 fputs(                                                      "\n"
                         "\a\a\a\a\a"                                        "\n"
                         "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"  "\n"
                         "Thread 1 stack pointer points to static data!"     "\n"
                         "Static data has probably been overwritten!"        "\n"
                         "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"  "\n"
                         , stderr
                      ) ;
               }
             }
             Pause() ;
             exit( 99 ) ;
           }
         }
       }
       else if (    Value     == DBG_X_PRE_FIRST_CHANCE
                 && Exception == XCPT_BREAKPOINT
               )
       {
           ThreadInfo & t = gThreadInfo.elementWithKey( gDbgBuf.Tid ) ;

           // Remove breakpoint
           gDbgBuf.writeMem( gResultCode.codeTerminate,
                             t.ivBreakEip,
                             1,
                             (ULONG)&t.ivBreakPrevInstr
                           ) ;

           t.ivpStackInfo->startMarking( min( t.ivBreakP1 + 0xFFC, gDbgBuf.ESP ) ) ;
       }

       gDbgBuf.Cmd == DBG_N_Exception ; /* restore for top of loop */

     } // if gDbgBuf.Cmd == DBG_N_Exception

   } // while gDbgBuf.Cmd != DBG_N_ProcTerm

   return 0 ;
}

