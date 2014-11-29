/****************************************************************************

    PROGRAM: DCEWin.h

    PURPOSE: Windows DCE user interface

****************************************************************************/

extern int DCEmain( int argc, char *argv[]);
extern void DCEexit( int rc);

#define exit( x)    DCEexit( x)
#define main( x, y) DCEmain( x, y)
