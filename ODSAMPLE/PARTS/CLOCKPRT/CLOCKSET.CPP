/*
 *  This file was generated by the SOM Compiler and Emitter Framework.
 *  Generated using:
 *      SOM Emitter emitxtm: 2.41
 */

#ifndef SOM_Module_clockset_Source
#define SOM_Module_clockset_Source
#endif
#define ClockSet_Class_Source
#define VARIABLE_MACROS

#define INCL_GPIPOLYGON





#define INCL_ODAPI
#define INCL_ODSTORAGESYSTEM
#define INCL_ODSTORAGEUNIT
#define INCL_ODTYPELIST
#define INCL_ODPART
#define INCL_ODSESSION

#include <os2.h>

#ifndef _ODUTILS_
#include <ODUtils.h>
#endif

#include "clockset.xih"

SOM_Scope ODULong  SOMLINK ClockSetAddNotebookPages(ClockSet *somSelf,
                                                    Environment *ev,
                                                   HWND hwndNotebook, ODULong flPageFlag,ODULong ulReserved)
{

     ODPart * part = (ODPart *)somSelf->GetBase(ev);
     ODTypeList * prefs = part->GetStorageUnit(ev)->GetSession(ev)->
                 GetStorageSystem(ev)->CreateTypeList(ev,(ODTypeList*)kODNULL);
    prefs->AddLast(ev, "Analog");
    prefs->AddLast(ev, "Digital");

    somSelf->InsertPresentationPage(ev, hwndNotebook, prefs);
    ClockSet_parent_ODSettingsExtension_AddNotebookPages(somSelf, ev,
                                                  hwndNotebook, flPageFlag,ulReserved);
    ODDeleteObject(prefs);
    return flPageFlag;
}


