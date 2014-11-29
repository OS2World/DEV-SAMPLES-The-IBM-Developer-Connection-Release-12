#define  INCL_BASE
#define  INCL_AVIO
#define  INCL_VIO
#define  INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#define ID_EXITPROG          1000
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
#define ID_WINDOW             1
#define ID_SESSIONSTART       1000
#define ID_SESSION0           1000
#define ID_SESSION1           1001
#define ID_SESSION2           1002
#define ID_SESSION3           1003
#define ID_SESSION4           1004
#define ID_SESSION5           1005
#define ID_SESSION6           1006
#define ID_SESSION7           1007
#define ID_SESSION8           1008
#define ID_SESSION9           1009
#define ID_SESSION10          1010
#define ID_SESSION11          1011
#define ID_SESSION12          1012
#define ID_SESSION13          1013
#define ID_SESSION14          1014
#define ID_SESSION15          1015
