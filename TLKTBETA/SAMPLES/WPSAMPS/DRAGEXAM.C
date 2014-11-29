/******************************************************************************
*
*  Module Name: DRAGEXAM.C
*
*  OS/2 Work Place Shell IDL File BETA Sample Program
*
*  Copyright (C) 1993 IBM Corporation
*
*      DISCLAIMER OF WARRANTIES.  The following [enclosed] code is
*      sample code created by IBM Corporation. This sample code is not
*      part of any standard or IBM product and is provided to you solely
*      for  the purpose of assisting you in the development of your
*      applications.  The code is provided "AS IS", without
*      warranty of any kind.  IBM shall not be liable for any damages
*      arising out of your use of the sample code, even if they have been
*      advised of the possibility of such damages.                                                    *
*
******************************************************************************/

#define  INCL_PM
#define  INCL_DOS
#define  INCL_DOSERRORS
#include <os2.h>
#include <string.h>

#define DRAGEXAM_Class_Source
#include "dragexam.ih"

/* The following section is for the definition of methods and method overrides
 * for the DRAGEXAM class.
 */

/*
 * SOM_Scope BOOL   SOMLINK drag_wpFormatDragItem(DRAGEXAM *somSelf,
 *                 PDRAGITEM pdrgItem)
 *
 * an object of class DRAGEXAM can only be dropped on an object of
 * class DRAGFOLD.  To set this up, we can use a rendering mechanism
 * and format that is foreign to the WPSH: <DRM_OUROWNSPECIAL,DRF_OBJECT>
 *
 * This RMF will be understood only by objects of class DRAGFOLD.
 *
 * So, we want to load up the DRAGITEM with the proper information at
 * the point where the drag begins: wpFormatDragItem
 *
 */

SOM_Scope BOOL  SOMLINK drag_wpFormatDragItem(DRAGEXAM *somSelf,
                                              PDRAGITEM pdrgItem)
{
/*    DRAGEXAMData *somThis = DRAGEXAMGetData(somSelf); */
    DRAGEXAMMethodDebug("DRAGEXAM","drag_wpFormatDragItem");

    parent_wpFormatDragItem(somSelf,pdrgItem);

    /* We do NOT want to really let the workplace shell render
     * our object, so change the rendering mechanism and format
     * to be ours.
     */
    DrgDeleteStrHandle(pdrgItem->hstrRMF);

    pdrgItem->hstrRMF = DrgAddStrHandle("<DRM_OUROWNSPECIAL,DRF_OBJECT>");

    return TRUE;
}


/* The following section is reserved for class method definitions and overrides
 */
#undef SOM_CurrentClass
#define SOM_CurrentClass SOMMeta

/*
 * SOM_Scope PSZ   SOMLINK dragM_wpclsQueryTitle(M_DRAGEXAM *somSelf)
 */

SOM_Scope PSZ  SOMLINK dragM_wpclsQueryTitle(M_DRAGEXAM *somSelf)
{
    /* M_DRAGEXAMData *somThis = M_DRAGEXAMGetData(somSelf); */
    M_DRAGEXAMMethodDebug("M_DRAGEXAM","dragM_wpclsQueryTitle");

    return "Drag Example";
}


