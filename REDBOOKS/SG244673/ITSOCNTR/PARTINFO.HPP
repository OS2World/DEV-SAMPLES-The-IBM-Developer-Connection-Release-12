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
#ifndef PARTINFO_HPP
#define PARTINFO_HPP

#define INCL_ODFRAME
#define INCL_ODPART

#include <od.h>

class PartInfoRec
{
  public:
    PartInfoRec();
    virtual ~PartInfoRec();

    ODBoolean fIsActive;             // Does this frame have its focus set?
    ODBoolean fNeedsActivating;      // should this be activated when its window is activated?
    ODBoolean fHasSelectedPart;      //an embedded part is selected
    ODPart*   fSelectedPart;
    ODBoolean fRestoreSelMenu;
    ODFrame*  fSelPartFrame;
    ODBoolean fSelPopupAttached;
};

#endif

