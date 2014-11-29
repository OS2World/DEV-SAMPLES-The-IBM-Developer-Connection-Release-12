// pmassert.h

#ifndef NDEBUG
#define assert(exp)\
{ if(!(exp))\
  {\
    char szAssertMsg[ 64 ];\
    ULONG errorid;\
    ULONG rc000;\
    errorid = WinGetLastError( globals.hab );\
    sprintf( szAssertMsg, "Line %d\nFile %s\nLast Error %p\nExpression %s\n", __LINE__, __FILE__, errorid, #exp );\
    rc000 = WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, szAssertMsg, "Assertion failed. Continue?", 0, MB_YESNO  );\
    if( rc000 == MBID_NO ) exit( 1 );\
  }\
}
#else
  #define assert(exp)
#endif

#define pmassert(e) assert(e)
