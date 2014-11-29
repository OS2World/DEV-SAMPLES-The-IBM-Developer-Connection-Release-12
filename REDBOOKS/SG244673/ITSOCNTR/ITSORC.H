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
#ifndef ITSORC_H
#define ITSORC_H

#ifndef __ODMENUID__
#include <odmenuid.h>
#endif

#define IconBarID       0x00000001
#define HORZ_SCROLL_ID  0x00000002
#define VERT_SCROLL_ID  0x00000003
#define BLANK_CORNER_ID 0x00000004
#define ICON_VIEW_ID    0x00000005
#define TREE_VIEW_ID    0x00000006
#define DETAILS_VIEW_ID 0x00000007

#define IDB_BITMAP      0x00000001

#define IDI_ICON        0x00000005

#define IDS_TITLE       0x00000005
#define IDS_MESSAGE_TITLE  0x00000006
#define IDS_MESSAGE_STRING 0x00000007

#define IDM_MENU        0x1001
#define IDM_HELP        0x1002
#define IDM_HELP_ABOUT  0x1003
#define IDM_HELP_MESSAGE 0x1004
#define VIEW_REFRESH 0x1006
#define SELECTED_LOCK   0x1009
#define SELECTED_UNLOCK 0x1010
#define SELECTED_GROUP  0x1011
#define SELECTED_UNGROUP 0x1012
#define SELECTED_MOVETOFRONT 0x1014
#define SELECTED_MOVETOBACK  0x1015
#define SELECTED_ALTMARK     0x1016

#define IDM_CONTEXTMENU 0x1005

#define IDMA_MOVETOFRONT    0x2000
#define IDMA_MOVETOBACK     0x2001
#define IDMA_MOVEFORWARD    0x2002
#define IDMA_MOVEBACKWARD   0x2003
#define IDMA_PARTINFO       0x2004
#define IDMA_ICLUIWINDOW    0x2005
#define IDM_ITSOCONTAINER   0x2006
#define IDM_ARRANGE         0x2007

#define IDD_ABOUT       0x3001
#define IDD_ABOUT_OK    0x3002

#define IDS_OPENWINDOW  MENUID_SELECTEDPART_USER + 1
#define IDS_BASE        MENUID_SELECTEDPART_USER + 3

#endif

