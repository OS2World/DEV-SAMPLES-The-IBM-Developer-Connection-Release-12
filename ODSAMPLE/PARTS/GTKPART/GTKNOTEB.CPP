
#ifndef SOM_Module_gtknoteb_Source
#define SOM_Module_gtknoteb_Source
#endif
#define GtkNotebook_Class_Source
#define VARIABLE_MACROS

#define INCL_WINHELP
#define INCL_DOS
#define INCL_WINPOINTERS
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WIN
#define INCL_GPI
#define INCL_WPCLASS
#define INCL_ODPART
#define INCL_ODAPI
#include <os2.h>

#include "GtkNoteb.xih"

// ------------------------------------------------------------------------------
SOM_Scope ODULong SOMLINK GtkNotebookAddNotebookPages( GtkNotebook *somSelf,
                                                       Environment *ev,
                                                       HWND hwndNotebook,
                                                       ODULong flPageFlag,
                                                       ODULong ulReserve )
{
//  GtkNotebookData *somThis = GtkNotebookGetData(somSelf);
  GtkNotebookMethodDebug( "GtkNotebook", "AddNotebookPages" );

  // add to the default flags those for LinkSource, LinkTarget,
  // and the 3 additional container views!

  GtkNotebook_parent_ODSettingsExtension_AddNotebookPages( somSelf, ev,
                                                           hwndNotebook,
                                                           flPageFlag |
                                                           kODLinkSourcePage |
                                                           kODLinkTargetPage |
                                                           kODFolderViewPages,
                                                           ulReserve );

  // shouldn't we be returning the AGGREGATE flag set???
  return flPageFlag;
}

