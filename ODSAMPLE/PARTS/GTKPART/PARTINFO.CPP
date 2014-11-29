/* PartInfo.cpp - seperate file for PartInfoRec class */

#define INCL_ODAPI
#define INCL_ODPART
#include <os2.h>

typedef long RGBColor;

class PartInfoRec
 {
 public:
   PartInfoRec() { fIsActive = kODFalse; fNeedsActivating = kODFalse;
                   fHasSelectedPart = kODFalse; fRestoreSelMenu = kODFalse;
                   fSelectedMenuAdjusted = kODFalse; fSelPartFrame = kODNULL;}
   virtual ~PartInfoRec() { }
//   RGBColor bgColor;
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