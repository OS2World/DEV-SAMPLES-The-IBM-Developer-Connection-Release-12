#define  INCL_BASE
#define  INCL_AVIO
#define  INCL_VIO
#include <os2.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#define VIOGETPHYSBUF         00
#define VIOGETBUF             01
#define VIOSHOWBUF            02
#define VIOGETCURPOS          03
#define VIOCURTYPE            04
#define VIOGETMODE            05
#define VIOSETCURPOS          06
#define VIOSETCURTYPE         07
#define VIOSETMODE            08
#define VIOREADCHARSTR        09
#define VIOREADCELLSTR        10

#define VIOWRTNCHAR           11
#define VIOWRTNATTR           12
#define VIOWRTNCELL           13
#define VIOWRTCHARSTR         14
#define VIOWRTCHARSTRATT      15
#define VIOWRTCELLSTR         16
#define VIOWRTTTY             17
#define VIOSCROLLUP           18
#define VIOSCROLLDN           19
#define VIOSCROLLLF           20
#define VIOSCROLLRT           21

#define VIOSETANSI            22
#define VIOGETANSI            23
#define VIOPRTSC              24
#define VIOSCRLOCK            25
#define VIOSCRUNLOCK          26
#define VIOSAVREDRAWWAIT      27
#define VIOSAVEREDRAWUNDO     28
#define VIOPOPUP              29
#define VIOENDPOPUP           30
#define VIOPRTSCTOGGLE        31
#define VIOMODEWAIT           32
#define VIOMODEUNDO           33
#define VIOGETFONT            34
#define VIOGETCONFIG          35
#define VIOSETCP              36
#define VIOGETCP              37
#define VIOSETFONT            38
#define VIOGETSTATE           39
#define VIOSETSTATE           40
#define COMMANDSHELL          "C:\\OS2\\CMD.EXE"
