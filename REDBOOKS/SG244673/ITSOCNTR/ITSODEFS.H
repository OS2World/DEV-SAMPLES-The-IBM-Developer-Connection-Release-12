/* ******************************************************************** *
 * Copyright International Business Machines Corporation 1995, 1996     *
 * DISCLAIMER OF WARRANTIES.                                            *
 * The following [enclosed] code is sample code created by IBM          *
 * Corporation. This sample code is not part of any standard or IBM     *
 * product and is provided to you solely for the purpose of assisting   *
 * you in the development of your applications.  The code is provided   *
 * "AS IS". IBM MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT  *
 * NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS *
 * FOR A PARTICULAR PURPOSE, REGARDING THE FUNCTION OR PERFORMANCE OF   *
 * THIS CODE.  IBM shall not be liable for any damages arising out of   *
 * your use of the sample code, even if they have been advised of the   *
 * possibility of such damages.                                         *
 *                                                                      *
 * DISTRIBUTION.                                                        *
 * This sample code can be freely distributed, copied, altered, and     *
 * incorporated into other software, provided that it bears the above   *
 * Copyright notice and DISCLAIMER intact.                              *
 * -------------------------------------------------------------------- *
 * This file is part of the source for OpenDoc parts document in:       *
 *  IBM International Technical Support Organization Bulletin (Redbook) *
 *  "OpenDoc : An Introduction to Part Development" (SG24-4673)         *
 *                                                                      *
 * ******************************************************************** *
 */

#ifndef ITSODEFS_H
#define ITSODEFS_H

#define ICON_SIZE 8

#define ENTIRE_FRAME 0x0001
#define LEFT_COLUMN  0x0002
#define RIGHT_COLUMN 0x0004
#define BOTTOM_ROW   0x0008
#define TOP_ROW      0x0010
#define PAGE_LEFT    0x0020
#define PAGE_RIGHT   0x0040
#define PAGE_DOWN    0x0080
#define PAGE_UP      0x0100

#define LINE_WIDTH   1
#define LINE_HEIGHT  1

#define VERT_SCROLL_WIDTH  10
#define HORZ_SCROLL_HEIGHT 10

#define MODULENAME "itsocntr.dll"

#define LARGEICONSIZE 32
#define SMALLICONSIZE 16
#define THUMBNAILSIZE 64

#define ITSOCNTR_HEIGHT 128
#define ITSOCNTR_WIDTH  128


/*
    The following text constants are used in the registration and binding
    operations to identify your part.
*/
const ODType kKind            = "ITSOContainer Part Kind";
const ODType kKindDisplayName = "ITSOContainerPart";
const ODType kPartHandlerName = "ITSOContainerPart";
const ODType kCategory        = "ITSOContainer Part";

// Drag storage unit properties
static const ODPropertyName kPropFrameInfo = "ITSOContainerPart:Property:FrameInfo";
static const ODPropertyName kPropMouseDownOffset = "ITSOContainerPart:Property:MouseDownOffset";
static const ODValueType    kODFrameRef = "OS2:Type:FrameReference";

const ODSLong kSelectionHandleWidth = MAKEFIXED(8, 0);
ODBoolean dmSelectRMF(PDRAGITEM pDragItem, PSZ pszSelectedRMF);

/*
    This debug section will produce a trace file named itsocntr.DBG
    if DEBUG is defined (uncomment the line).  The output will show the
    'class name : method name' as each method is called.
*/
#ifdef DEBUG
#undef ITSOContainerPartMethodDebug
#undef M_ITSOContainerPartMethodDebug

#define ITSOContainerPartMethodDebug(x, y) Log(x, y)
#define M_ITSOContainerPartMethodDebug(x, y) Log(x, y)

#include <stdio.h>
#include <string.h>

void Log(char *x, char *y)
{
  FILE *fp = fopen("itsocntr.dbg", "a");

  if (fp != NULL)
  {
    fprintf(fp, "%s:%s\n", x, y);
    fflush(fp);

    if (strcmp(y, "somUninit") == 0)
    {
      fclose(fp);
      fp = NULL;
    }

    fclose(fp);
  }
}
#endif


struct FacetClip
{
  ODFacet* facet;
  ODShape* clip;
};

ODFacet* FirstFacet( Environment* ev, ODFrame *frame )
{
  ODFrameFacetIterator* facets = frame->CreateFacetIterator(ev);
  ODFacet* facet = facets->First(ev);
  delete facets;
  return facet;
}

struct Proxy {
   ODFrame*     frame;
   ODTransform* transform;
   ODBoolean    transformChanged;
   Proxy(ODFrame* f, ODTransform* t) {frame = f; transform = t; transformChanged = kODTrue;}
};
#endif

