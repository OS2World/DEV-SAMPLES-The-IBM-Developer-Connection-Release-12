/********************************************************************/
/*  Licensed Materials - Property of IBM                            */
/*                                                                  */
/*                                                                  */
/* Copyright (C) International Business Machines Corp., 1994.       */
/* Copyright (C) Apple Computer, Inc., 1994                         */
/*                                                                  */
/*  US Government Users Restricted Rights -                         */
/*  Use, duplication, or disclosure restricted                      */
/*  by GSA ADP Schedule Contract with IBM Corp.                     */
/*                                                                  */
/*  Change History:                                                 */
/*  122244 05/13/95  rlt  added fGridOn                             */
/*                                                                  */
/********************************************************************/

/* PartInfo.h - header file for PartInfoRec class */

#include <od.h>
#include <ODTypesB.xh>
#include <odtypesm.xh>
#include <part.xh>

typedef long RGBColor;

class PartInfoRec
  {
  public:
   PartInfoRec() { fIsActive = kODFalse; fNeedsActivating = kODFalse;
                   partwindowID = 0;     fHasSelectedPart = kODFalse;
                   fSelectedPart = kODNULL;  fRestoreSelMenu = kODFalse; 
                   fSelectedMenuAdjusted = kODFalse; fSelPartFrame =kODNULL;}
    virtual ~PartInfoRec() { }
//    RGBColor bgColor;
//    ODBoolean  fGridOn;  // Is the grid on?
    ODBoolean  fIsActive;  // Does this frame have its focus set?
    ODBoolean  fNeedsActivating; // should this be activated when its window is activated?
    ODID partwindowID;
    ODBoolean  fHasSelectedPart;     //an embedded part is selected
    ODPart*    fSelectedPart;
    ODBoolean  fRestoreSelMenu;
    ODBoolean  fSelectedMenuAdjusted;
    ODFrame*   fSelPartFrame;
  };
