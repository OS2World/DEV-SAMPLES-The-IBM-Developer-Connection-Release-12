/* Prototypes and definitions for UCMUtils functions */

#define UCMUTILS_PLACE_TOP    1    /* Toolbar placement options                */
#define UCMUTILS_PLACE_LEFT   2
#define UCMUTILS_PLACE_RIGHT  4
#define UCMUTILS_PLACE_BOTTOM 8

void APIENTRY    UCMUtilsAddToFrame(        /* Add a UCMenu to a frame or dialog window */
            HWND   TargetHwnd,     /* Handle of frame/dialog window            */
            HWND   ToolHwnd,       /* Handle of toolbar menu                   */
            ULONG  Place);         /* UCMUTILS_PLACE_xxx position flag         */

void APIENTRY    UCMUtilsRemoveFromFrame(   /* Remove a UCMenu from a frame or dialog   */
            HWND   TargetHwnd,     /* Frame handle                             */
            HWND   ToolHwnd);      /* UC menu handle                           */

void APIENTRY    UCMUtilsAdjustDlgSize(     /* Adjust dialog size with control movement */
            HWND   DlgHwnd,        /* Handle of dialog                         */
            LONG   Cx,             /* X adjustment (+/-)                       */
            LONG   Cy,             /* Y adjustment (+/-)                       */
            USHORT Where);         /* Edge to adjust: UCMUTILS_PLACE_* flag    */

void APIENTRY    UCMUtilsSetMinDlgSize(     /* Set minimum size of dialog to current    */
            HWND   DlgHwnd);       /* Handle of dialog                         */

void APIENTRY    UCMUtilsFreeUCMInfoStrings(/* Free all pszXXXX strings in UCMINFO      */
            UCMINFO *UCMData);     /* Ptr to UCMINFO structure                 */

MRESULT APIENTRY  UCMUtilsGetNextMenu(       /* Get next/prev menu for WM_NEXTMENU       */
            HWND Parent,           /* Parent of all menus in ring              */
            HWND CurrHwnd,         /* Handle of menu with current selection    */
            BOOL StartOfMenu,      /* Direction to traverse ring               */
            ...);                  /* Ordered list of menu IDs, 0 terminated   */
