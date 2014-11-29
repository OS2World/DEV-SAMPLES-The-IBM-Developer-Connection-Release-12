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
#define INCL_ODSTORAGE
#define INCL_ODSTORAGEUNIT
#define INCL_ODTYPELIST
#define INCL_ODAPI

#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <builtin.h>
#include <string.h>

void writeSU(ODStorageUnit *su, char *text)
{
   long           dummydata=1;                      // Temporary hack for sutrace.log
   int            propCount,valCount,dataSize;
   FILE          *stream;
   ODPropertyName propName;
   ODValueType    valType;

   int i,j;
   Environment   *ev = somGetGlobalEnvironment();

   if (( stream = fopen("sutrace.log", "a"))== NULL ) {
       DosBeep(200,200);
       return;
   }

   fprintf(stream,"\n\n *** StorageUnit Description: %s\n\n",text);
   su->Focus(ev,kODNULL,kODPosAll,kODNULL,0,kODPosAll);      // Focus on Storage Unit
   propCount = su->CountProperties(ev);                      // Get # of prop's
   fprintf(stream,"PropertyCount ==> %d\n",propCount);

   for (i=1;i<=propCount;i++) {                              // For each Property
       su->Focus(ev,kODNULL,kODPosNextSib,kODNULL,0,kODPosAll); // Set focus
       su->AddValue(ev,"dummy");                             // Required because of bug
       StorageUnitSetValue(su,ev,sizeof (long),&dummydata);            // Required because of bug

       propName = su->GetProperty(ev);                       // Get prpoerty name
       fprintf(stream,"Property Name ==> %s\n",propName);

       valCount = su->CountValues(ev);                       // Get number of values for prop
       fprintf(stream,"\t ValuesCount ==> %d\n",valCount-1);   //** was -1

       su->Focus(ev,kODNULL,kODPosSame,kODNULL,0,kODPosFirstSib);  // Focus on first value

       for (j=1;j<=valCount-1;j++) {                          // For each value (except last)
           valType = su->GetType(ev);                         // get Type (string)
           dataSize = su->GetSize(ev);
           fprintf(stream,"\t\tType ==> %s\n",valType);
           fprintf(stream,"\t\tSize ==> %d\n",dataSize);  // Print Size of value
           su->Focus(ev,kODNULL,kODPosSame,kODNULL,0,kODPosNextSib); //Focus to next value
       } /* endfor */
   } /* endfor */
   fclose(stream);
}
