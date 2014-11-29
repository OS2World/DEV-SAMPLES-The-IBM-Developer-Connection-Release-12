#ifndef THREAD_HPP_INCLUDED
#define THREAD_HPP_INCLUDED

#define INCL_DOS
#define INCL_ERRORS
#include <os2.h>

class AliasedAddress ;
class StackInfo      ;

struct ThreadInfo
{
  PID              ivPid ;
  TID              ivTid ;
  StackInfo      * ivpStackInfo ;

  ULONG            ivBreakEip ;
  ULONG            ivBreakP1  ;
  char             ivBreakPrevInstr ;
} ;

TID const& key( ThreadInfo const& t ) { return t.ivTid ; }

#endif /* THREAD_HPP_INCLUDED */
