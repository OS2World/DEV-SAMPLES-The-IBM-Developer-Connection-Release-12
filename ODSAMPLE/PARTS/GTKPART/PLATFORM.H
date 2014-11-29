
#define RGB_HPS(hps)           GpiCreateLogColorTable(hps,0,LCOLF_RGB,0,0,NULL)
#define DestroyWindow(hwnd)    WinDestroyWindow(hwnd)
#define InvalidateWindow(hwnd) WinInvalidateRect( hwnd, NULL, TRUE )
#define InvalidatePalette()    InvalidateWindow(_hwndTools)
#define ALARM()                WinAlarm( HWND_DESKTOP, WA_WARNING )

#define LoadPointer(module,id) WinLoadPointer( HWND_DESKTOP, module, id )
#define SetSysPointer(id)      WinSetPointer( HWND_DESKTOP,                   \
                                     WinQuerySysPointer(HWND_DESKTOP,id,0))
#define SetPointer(id)         WinSetPointer( HWND_DESKTOP, id )

