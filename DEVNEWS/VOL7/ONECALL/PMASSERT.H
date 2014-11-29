// pmassert.h
// Monte Copeland for DevCon 7

#ifndef NDEBUG

#define assert(exp)\
{\
if(!(exp)) {\
  char ebuff[ 64 ];\
  ULONG errorid;\
  ULONG rc000;\
  errorid = WinGetLastError( 0 ); \
  sprintf( ebuff, "Line %d\nFile %s\nLast Error %p\nExpression %s\n", __LINE__, __FILE__, errorid, #exp );\
  rc000 = (ULONG)WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, ebuff, "Assertion failed. Continue?", 0, MB_YESNO  );\
  if( rc000 == MBID_NO ) exit( 1 );\
}\
}


#else
  #define assert(exp)
#endif
