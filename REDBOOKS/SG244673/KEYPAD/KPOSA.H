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
#ifndef _KEYPADPRP_
#define _KEYPADPRP_

void GetDirectParam(ODSession* session, OSAEvent* message, AEDesc* evtDp);
void CreatePropObjSpec(KeyPadPart* somSelf, Environment* ev, ODFrame* frame,
                       DescType form, DescType prop, AEDesc& objSpec);
void SendSetDataEvent(KeyPadPart *somSelf, Environment* ev, AEDesc& objSpec, AEDesc& data);
static ODError APIENTRY GetPropertyFromNULL(  ODPart*        part,
                      DescType      desiredClass,
                      ODOSLToken*      keypad,
                      DescType      keypadClass,
                      DescType      form,
                      ODDesc*        selectionData,
                      ODOSLToken*      value,
                      ODSLong        refCon);

static ODError APIENTRY HandleSetData(ODPart*      part,
                  ODOSAEvent*  message,
                  ODOSAEvent*  reply,
                  ODSLong      handlerRefcon);

class KeyPadPropAccessor;
class KeyPadPart;

class KeyPadPropAccessor
{
  public:
    KeyPadPropAccessor(DescType property, KeyPadPart* part);
    virtual void SetData(AEDesc* data);
//    virtual void GetData(AEDesc* result);

  private:
    KeyPadPart*   fPart;
    DescType  fProperty;
};

#endif

