
/*
 * This file was generated by the SOM Compiler.
 * FileName: myfolder.h.
 * Generated using:
 *     SOM Precompiler somipc: 2.2
 *     SOM Emitter emith: 2.7
 */

/*
 * 
 * 
 *    Module Name: MYFOLDER
 * 
 *    OS/2 Work Place Shell Sample Program - SOM 2.0 / IDL Version
 * 
 *    Copyright (C) 1992, 1993 IBM Corporation
 * 
 *        DISCLAIMER OF WARRANTIES.  The following [enclosed] code is
 *        sample code created by IBM Corporation. This sample code is not
 *        part of any standard or IBM product and is provided to you solely
 *        for  the purpose of assisting you in the development of your
 *        applications.  The code is provided "AS IS", without
 *        warranty of any kind.  IBM shall not be liable for any damages
 *        arising out of your use of the sample code, even if they have been
 *        advised of the possibility of such damages.
 * 
 */


#ifndef SOM_MYFOLDER_h
#define SOM_MYFOLDER_h


#include <som.h>

/*
 * -- This emitter treats Method Tokens as Thunks by default
 * -- Use the sc modifier "nothunks" to change this default
 */
#undef somresolve_
#define somresolve_(obj,mToken) ((somMethodProc*)((void)obj, mToken))

/*
 * Define the class name as an object type
 */
#ifndef MYFOLDER
#define MYFOLDER SOMObject
#endif
#include <wpfolder.h>

/*
 * Start of user-defined types:
 */

/*
 * End of user-defined types.
 */

#define MYFOLDER_MajorVersion 1
#define MYFOLDER_MinorVersion 2

/*
 * External name shortening if required
 */
#ifdef SOM_USE_SHORT_EXTERNAL_NAMES
#define MYFOLDERNewClass myfoldc
#pragma linkage(myfoldc, system)
#define MYFOLDERClassData myfoldd
#define MYFOLDERCClassData myfoldx
#endif /* SOM_USE_SHORT_EXTERNAL_NAMES */
/*--------------Migration------------*/
#define MYFOLDER_classObj MYFOLDERClassData.classObject
#define _MYFOLDER MYFOLDER_classObj

/*
 * Declare the class creation procedure
 */
SOMEXTERN SOMClass * SOMLINK MYFOLDERNewClass(integer4 somtmajorVersion,
		integer4 somtminorVersion);
#pragma linkage(MYFOLDERNewClass, system)

/*
 * Declare the class data structure
 */
SOMEXTERN struct MYFOLDERClassDataStructure {
	SOMAny *classObject;
	somMToken CloseViews;
} MYFOLDERClassData;

/*
 * Declare the C specific class data structure
 */
SOMEXTERN struct MYFOLDERCClassDataStructure {
	somMethodTabs parentMtab;
	somDToken instanceDataToken;
} MYFOLDERCClassData;

/*
 * New and Renew macros for MYFOLDER
 */
#define MYFOLDERNew() \
	( MYFOLDERClassData.classObject \
		? (_somNew(MYFOLDERClassData.classObject)) \
		: ( MYFOLDERNewClass(MYFOLDER_MajorVersion, MYFOLDER_MinorVersion), (_somNew(MYFOLDERClassData.classObject)) ) )
#define MYFOLDERRenew(buf) \
	( _somRenew(MYFOLDERClassData.classObject, buf) )

/*
 * Override method: wpOpen
 */
#define MYFOLDER_wpOpen(somSelf,hwndCnr,ulView,param) \
	WPObject_wpOpen(somSelf,hwndCnr,ulView,param)

/*
 * Override method: wpModifyPopupMenu
 */
#define MYFOLDER_wpModifyPopupMenu(somSelf,hwndMenu,hwndCnr,iPosition) \
	WPObject_wpModifyPopupMenu(somSelf,hwndMenu,hwndCnr,iPosition)

/*
 * Override method: wpMenuItemSelected
 */
#define MYFOLDER_wpMenuItemSelected(somSelf,hwndFrame,ulMenuId) \
	WPObject_wpMenuItemSelected(somSelf,hwndFrame,ulMenuId)

/*
 * New Method: CloseViews
 */
typedef void   SOMLINK somTP_MYFOLDER_CloseViews(MYFOLDER *somSelf);
#pragma linkage(somTP_MYFOLDER_CloseViews, system)
typedef somTP_MYFOLDER_CloseViews *somTD_MYFOLDER_CloseViews;
#define somMD_MYFOLDER_CloseViews "::MYFOLDER::CloseViews"
#define MYFOLDER_CloseViews(somSelf) \
    (SOM_Resolve(somSelf, MYFOLDER, CloseViews) \
	(somSelf))
#ifndef SOMGD_CloseViews
    #if (defined(_CloseViews) || defined(__CloseViews))
        #undef _CloseViews
        #undef __CloseViews
        #define SOMGD_CloseViews 1
    #else
        #define _CloseViews MYFOLDER_CloseViews
    #endif /* _CloseViews */
#endif /* SOMGD_CloseViews */
#ifndef SOM_DONT_USE_INHERITED_MACROS
#define MYFOLDER_wpSetFldrFlags WPFolder_wpSetFldrFlags
#define MYFOLDER_wpQueryFldrFlags WPFolder_wpQueryFldrFlags
#define MYFOLDER_wpSetFldrFont WPFolder_wpSetFldrFont
#define MYFOLDER_wpQueryFldrFont WPFolder_wpQueryFldrFont
#define MYFOLDER_wpSetFldrAttr WPFolder_wpSetFldrAttr
#define MYFOLDER_wpQueryFldrAttr WPFolder_wpQueryFldrAttr
#define MYFOLDER_wpSetNextIconPos WPFolder_wpSetNextIconPos
#define MYFOLDER_wpQueryNextIconPos WPFolder_wpQueryNextIconPos
#define MYFOLDER_wpPopulate WPFolder_wpPopulate
#define MYFOLDER_wpQueryContent WPFolder_wpQueryContent
#define MYFOLDER_wpAddFolderView1Page WPFolder_wpAddFolderView1Page
#define MYFOLDER_wpAddFolderView2Page WPFolder_wpAddFolderView2Page
#define MYFOLDER_wpAddFolderView3Page WPFolder_wpAddFolderView3Page
#define MYFOLDER_wpAddFolderIncludePage WPFolder_wpAddFolderIncludePage
#define MYFOLDER_wpAddFolderSortPage WPFolder_wpAddFolderSortPage
#define MYFOLDER_wpAddFolderBackgroundPage WPFolder_wpAddFolderBackgroundPage
#define MYFOLDER_wpInitIconPosData WPFolder_wpInitIconPosData
#define MYFOLDER_wpFreeIconPosData WPFolder_wpFreeIconPosData
#define MYFOLDER_wpStoreIconPosData WPFolder_wpStoreIconPosData
#define MYFOLDER_wpQueryIconPosition WPFolder_wpQueryIconPosition
#define MYFOLDER_wpSetFldrSort WPFolder_wpSetFldrSort
#define MYFOLDER_wpQueryFldrSort WPFolder_wpQueryFldrSort
#define MYFOLDER_wpRestoreFldrRunObjs WPFolder_wpRestoreFldrRunObjs
#define MYFOLDER_wpStoreFldrRunObjs WPFolder_wpStoreFldrRunObjs
#define MYFOLDER_wpHideFldrRunObjs WPFolder_wpHideFldrRunObjs
#define MYFOLDER_wpDeleteContents WPFolder_wpDeleteContents
#define MYFOLDER_wpSetFldrDetailsClass WPFolder_wpSetFldrDetailsClass
#define MYFOLDER_wpQueryFldrDetailsClass WPFolder_wpQueryFldrDetailsClass
#define MYFOLDER_wpSearchFolder WPFolder_wpSearchFolder
#define MYFOLDER_wpContainsFolders WPFolder_wpContainsFolders
#define MYFOLDER_wpQueryOpenFolders WPFolder_wpQueryOpenFolders
#define MYFOLDER_wpInitData WPObject_wpInitData
#define MYFOLDER_wpUnInitData WPObject_wpUnInitData
#define MYFOLDER_wpFree WPObject_wpFree
#define MYFOLDER_wpDelete WPObject_wpDelete
#define MYFOLDER_wpConfirmDelete WPObject_wpConfirmDelete
#define MYFOLDER_wpSaveState WPObject_wpSaveState
#define MYFOLDER_wpRestoreState WPObject_wpRestoreState
#define MYFOLDER_wpAddSettingsPages WPObject_wpAddSettingsPages
#define MYFOLDER_wpSetup WPObject_wpSetup
#define MYFOLDER_wpMoveObject WPObject_wpMoveObject
#define MYFOLDER_wpDrop WPObject_wpDrop
#define MYFOLDER_wpDragOver WPObject_wpDragOver
#define MYFOLDER_wpMenuItemHelpSelected WPObject_wpMenuItemHelpSelected
#define MYFOLDER_wpAddFile3Page WPFileSystem_wpAddFile3Page
#define MYFOLDER_wpAddFile2Page WPFileSystem_wpAddFile2Page
#define MYFOLDER_wpFormatDragItem WPObject_wpFormatDragItem
#define MYFOLDER_wpRender WPObject_wpRender
#define MYFOLDER_wpRefresh WPFileSystem_wpRefresh
#define MYFOLDER_wpFilterPopupMenu WPObject_wpFilterPopupMenu
#define MYFOLDER_wpQueryDefaultView WPObject_wpQueryDefaultView
#define MYFOLDER_wpRenderComplete WPObject_wpRenderComplete
#define MYFOLDER_wpQueryDefaultHelp WPObject_wpQueryDefaultHelp
#define MYFOLDER_wpSetRealName WPFileSystem_wpSetRealName
#define MYFOLDER_wpSetType WPFileSystem_wpSetType
#define MYFOLDER_wpQueryType WPFileSystem_wpQueryType
#define MYFOLDER_wpSetAttr WPFileSystem_wpSetAttr
#define MYFOLDER_wpQueryAttr WPFileSystem_wpQueryAttr
#define MYFOLDER_wpAddFile1Page WPFileSystem_wpAddFile1Page
#define MYFOLDER_wpAddFile2Page WPFileSystem_wpAddFile2Page
#define MYFOLDER_wpAddFile3Page WPFileSystem_wpAddFile3Page
#define MYFOLDER_wpAddFileMenuPage WPFileSystem_wpAddFileMenuPage
#define MYFOLDER_wpQueryCreation WPFileSystem_wpQueryCreation
#define MYFOLDER_wpQueryLastAccess WPFileSystem_wpQueryLastAccess
#define MYFOLDER_wpQueryLastWrite WPFileSystem_wpQueryLastWrite
#define MYFOLDER_wpQueryFileSize WPFileSystem_wpQueryFileSize
#define MYFOLDER_wpQueryEASize WPFileSystem_wpQueryEASize
#define MYFOLDER_wpSetDateInfo WPFileSystem_wpSetDateInfo
#define MYFOLDER_wpSetFileSizeInfo WPFileSystem_wpSetFileSizeInfo
#define MYFOLDER_wpRefresh WPFileSystem_wpRefresh
#define MYFOLDER_wpQueryRefreshFlags WPFileSystem_wpQueryRefreshFlags
#define MYFOLDER_wpSetRefreshFlags WPFileSystem_wpSetRefreshFlags
#define MYFOLDER_wpPrintPlainTextFile WPFileSystem_wpPrintPlainTextFile
#define MYFOLDER_wpSetTitleAndRenameFile WPFileSystem_wpSetTitleAndRenameFile
#define MYFOLDER_wpConfirmRenameFileWithExt WPFileSystem_wpConfirmRenameFileWithExt
#define MYFOLDER_wpVerifyUpdateAccess WPFileSystem_wpVerifyUpdateAccess
#define MYFOLDER_wpAddUserItemsToPopupMenu WPFileSystem_wpAddUserItemsToPopupMenu
#define MYFOLDER_wpQueryRealName WPFileSystem_wpQueryRealName
#define MYFOLDER_wpUnInitData WPObject_wpUnInitData
#define MYFOLDER_wpQueryHandle WPObject_wpQueryHandle
#define MYFOLDER_wpSaveImmediate WPObject_wpSaveImmediate
#define MYFOLDER_wpFilterPopupMenu WPObject_wpFilterPopupMenu
#define MYFOLDER_wpSetTitle WPObject_wpSetTitle
#define MYFOLDER_wpViewObject WPObject_wpViewObject
#define MYFOLDER_wpSwitchTo WPObject_wpSwitchTo
#define MYFOLDER_wpAddSettingsPages WPObject_wpAddSettingsPages
#define MYFOLDER_wpCopyObject WPObject_wpCopyObject
#define MYFOLDER_wpCreateFromTemplate WPObject_wpCreateFromTemplate
#define MYFOLDER_wpMoveObject WPObject_wpMoveObject
#define MYFOLDER_wpQueryIconData WPObject_wpQueryIconData
#define MYFOLDER_wpSetIconData WPObject_wpSetIconData
#define MYFOLDER_wpQueryDetailsData WPObject_wpQueryDetailsData
#define MYFOLDER_wpSaveState WPObject_wpSaveState
#define MYFOLDER_wpRestoreState WPObject_wpRestoreState
#define MYFOLDER_wpDraggedOverObject WPObject_wpDraggedOverObject
#define MYFOLDER_wpDroppedOnObject WPObject_wpDroppedOnObject
#define MYFOLDER_wpFormatDragItem WPObject_wpFormatDragItem
#define MYFOLDER_wpQueryNameClashOptions WPObject_wpQueryNameClashOptions
#define MYFOLDER_wpAppendObject WPObject_wpAppendObject
#define MYFOLDER_wpReplaceObject WPObject_wpReplaceObject
#define MYFOLDER_wpQueryStyle WPObject_wpQueryStyle
#define MYFOLDER_wpSetStyle WPObject_wpSetStyle
#define MYFOLDER_wpDoesObjectMatch WPObject_wpDoesObjectMatch
#define MYFOLDER_wpAddObjectGeneralPage WPObject_wpAddObjectGeneralPage
#define MYFOLDER_wpAddObjectWindowPage WPObject_wpAddObjectWindowPage
#define MYFOLDER_wpAddSettingsPages WPObject_wpAddSettingsPages
#define MYFOLDER_wpAddToObjUseList WPObject_wpAddToObjUseList
#define MYFOLDER_wpAllocMem WPObject_wpAllocMem
#define MYFOLDER_wpAppendObject WPObject_wpAppendObject
#define MYFOLDER_wpAssertObjectMutexSem WPObject_wpAssertObjectMutexSem
#define MYFOLDER_wpClose WPObject_wpClose
#define MYFOLDER_wpCnrInsertObject WPObject_wpCnrInsertObject
#define MYFOLDER_wpCnrRemoveObject WPObject_wpCnrRemoveObject
#define MYFOLDER_wpCnrSetEmphasis WPObject_wpCnrSetEmphasis
#define MYFOLDER_wpConfirmDelete WPObject_wpConfirmDelete
#define MYFOLDER_wpConfirmObjectTitle WPObject_wpConfirmObjectTitle
#define MYFOLDER_wpCopiedFromTemplate WPObject_wpCopiedFromTemplate
#define MYFOLDER_wpCopyObject WPObject_wpCopyObject
#define MYFOLDER_wpCreateAnother WPObject_wpCreateAnother
#define MYFOLDER_wpCreateFromTemplate WPObject_wpCreateFromTemplate
#define MYFOLDER_wpCreateShadowObject WPObject_wpCreateShadowObject
#define MYFOLDER_wpDelete WPObject_wpDelete
#define MYFOLDER_wpDeleteFromObjUseList WPObject_wpDeleteFromObjUseList
#define MYFOLDER_wpDisplayHelp WPObject_wpDisplayHelp
#define MYFOLDER_wpDoesObjectMatch WPObject_wpDoesObjectMatch
#define MYFOLDER_wpDraggedOverObject WPObject_wpDraggedOverObject
#define MYFOLDER_wpDragOver WPObject_wpDragOver
#define MYFOLDER_wpDrop WPObject_wpDrop
#define MYFOLDER_wpDroppedOnObject WPObject_wpDroppedOnObject
#define MYFOLDER_wpEndConversation WPObject_wpEndConversation
#define MYFOLDER_wpFilterPopupMenu WPObject_wpFilterPopupMenu
#define MYFOLDER_wpFindUseItem WPObject_wpFindUseItem
#define MYFOLDER_wpFormatDragItem WPObject_wpFormatDragItem
#define MYFOLDER_wpFree WPObject_wpFree
#define MYFOLDER_wpFreeMem WPObject_wpFreeMem
#define MYFOLDER_wpHide WPObject_wpHide
#define MYFOLDER_wpInitData WPObject_wpInitData
#define MYFOLDER_wpInsertPopupMenuItems WPObject_wpInsertPopupMenuItems
#define MYFOLDER_wpInsertSettingsPage WPObject_wpInsertSettingsPage
#define MYFOLDER_wpMenuItemHelpSelected WPObject_wpMenuItemHelpSelected
#define MYFOLDER_wpMoveObject WPObject_wpMoveObject
#define MYFOLDER_wpPrintObject WPObject_wpPrintObject
#define MYFOLDER_wpQueryConcurrentView WPObject_wpQueryConcurrentView
#define MYFOLDER_wpQueryButtonAppearance WPObject_wpQueryButtonAppearance
#define MYFOLDER_wpQueryConfirmations WPObject_wpQueryConfirmations
#define MYFOLDER_wpQueryDefaultHelp WPObject_wpQueryDefaultHelp
#define MYFOLDER_wpQueryDefaultView WPObject_wpQueryDefaultView
#define MYFOLDER_wpQueryDetailsData WPObject_wpQueryDetailsData
#define MYFOLDER_wpQueryError WPObject_wpQueryError
#define MYFOLDER_wpQueryIcon WPObject_wpQueryIcon
#define MYFOLDER_wpQueryIconData WPObject_wpQueryIconData
#define MYFOLDER_wpQueryMinWindow WPObject_wpQueryMinWindow
#define MYFOLDER_wpQueryNameClashOptions WPObject_wpQueryNameClashOptions
#define MYFOLDER_wpQueryStyle WPObject_wpQueryStyle
#define MYFOLDER_wpSetTaskRec WPObject_wpSetTaskRec
#define MYFOLDER_wpFindTaskRec WPObject_wpFindTaskRec
#define MYFOLDER_wpQueryTitle WPObject_wpQueryTitle
#define MYFOLDER_wpRegisterView WPObject_wpRegisterView
#define MYFOLDER_wpReleaseObjectMutexSem WPObject_wpReleaseObjectMutexSem
#define MYFOLDER_wpRender WPObject_wpRender
#define MYFOLDER_wpRenderComplete WPObject_wpRenderComplete
#define MYFOLDER_wpReplaceObject WPObject_wpReplaceObject
#define MYFOLDER_wpRequestObjectMutexSem WPObject_wpRequestObjectMutexSem
#define MYFOLDER_wpRestore WPObject_wpRestore
#define MYFOLDER_wpRestoreData WPObject_wpRestoreData
#define MYFOLDER_wpRestoreLong WPObject_wpRestoreLong
#define MYFOLDER_wpRestoreState WPObject_wpRestoreState
#define MYFOLDER_wpRestoreString WPObject_wpRestoreString
#define MYFOLDER_wpSaveData WPObject_wpSaveData
#define MYFOLDER_wpSaveImmediate WPObject_wpSaveImmediate
#define MYFOLDER_wpSaveDeferred WPObject_wpSaveDeferred
#define MYFOLDER_wpSaveLong WPObject_wpSaveLong
#define MYFOLDER_wpSaveState WPObject_wpSaveState
#define MYFOLDER_wpSaveString WPObject_wpSaveString
#define MYFOLDER_wpScanSetupString WPObject_wpScanSetupString
#define MYFOLDER_wpSetConcurrentView WPObject_wpSetConcurrentView
#define MYFOLDER_wpSetButtonAppearance WPObject_wpSetButtonAppearance
#define MYFOLDER_wpSetDefaultHelp WPObject_wpSetDefaultHelp
#define MYFOLDER_wpSetDefaultView WPObject_wpSetDefaultView
#define MYFOLDER_wpSetError WPObject_wpSetError
#define MYFOLDER_wpSetIcon WPObject_wpSetIcon
#define MYFOLDER_wpSetIconData WPObject_wpSetIconData
#define MYFOLDER_wpSetMinWindow WPObject_wpSetMinWindow
#define MYFOLDER_wpSetStyle WPObject_wpSetStyle
#define MYFOLDER_wpModifyStyle WPObject_wpModifyStyle
#define MYFOLDER_wpSetTitle WPObject_wpSetTitle
#define MYFOLDER_wpSetup WPObject_wpSetup
#define MYFOLDER_wpSwitchTo WPObject_wpSwitchTo
#define MYFOLDER_wpUnInitData WPObject_wpUnInitData
#define MYFOLDER_wpViewObject WPObject_wpViewObject
#define MYFOLDER_wpQueryTrueStyle WPObject_wpQueryTrueStyle
#define MYFOLDER_wpQueryHandle WPObject_wpQueryHandle
#define MYFOLDER_wpUnlockObject WPObject_wpUnlockObject
#define MYFOLDER_somInit SOMObject_somInit
#define MYFOLDER_somUninit SOMObject_somUninit
#define MYFOLDER_somFree SOMObject_somFree
#define MYFOLDER_somDefaultInit SOMObject_somDefaultInit
#define MYFOLDER_somDestruct SOMObject_somDestruct
#define MYFOLDER_somInit SOMObject_somInit
#define MYFOLDER_somUninit SOMObject_somUninit
#define MYFOLDER_somGetClass SOMObject_somGetClass
#define MYFOLDER_somGetClassName SOMObject_somGetClassName
#define MYFOLDER_somGetSize SOMObject_somGetSize
#define MYFOLDER_somIsA SOMObject_somIsA
#define MYFOLDER_somIsInstanceOf SOMObject_somIsInstanceOf
#define MYFOLDER_somRespondsTo SOMObject_somRespondsTo
#define MYFOLDER_somComputeForwardVisitMask SOMObject_somComputeForwardVisitMask
#define MYFOLDER_somComputeReverseVisitMask SOMObject_somComputeReverseVisitMask
#define MYFOLDER_somDispatchV SOMObject_somDispatchV
#define MYFOLDER_somDispatchL SOMObject_somDispatchL
#define MYFOLDER_somDispatchA SOMObject_somDispatchA
#define MYFOLDER_somDispatchD SOMObject_somDispatchD
#define MYFOLDER_somDispatch SOMObject_somDispatch
#define MYFOLDER_somClassDispatch SOMObject_somClassDispatch
#define MYFOLDER_somCastObj SOMObject_somCastObj
#define MYFOLDER_somResetObj SOMObject_somResetObj
#define MYFOLDER_somPrintSelf SOMObject_somPrintSelf
#define MYFOLDER_somDumpSelf SOMObject_somDumpSelf
#define MYFOLDER_somDumpSelfInt SOMObject_somDumpSelfInt
#endif /* SOM_DONT_USE_INHERITED_MACROS */

#endif       /* SOM_MYFOLDER_h */
