/****************************************************************************
 * OS/2 OSA Test OSATEST
 *
 * File name: osaprof.c
 *
 * Description:  In this module are all the functions needed to save and
 *               restore the application defaults to/from the OSATEST.INI.
 *
 *               This source file contains the following functions:
 *
 *               GetProfile(pmp)
 *               SaveProfile(pmp)
 *               SaveVersion(pmp)
 *               SaveProfileOnly(pmp)
 *               PutProfileInfo(pmp)
 *               GetProfileInfo(pmp)
 *
 *    Files    :  OS2.H, OSATEST.H, osadlg.h, PMASSERT.H
 *
 ****************************************************************************/

/* os2 includes */
#define INCL_WINSTDFILE
#define INCL_WINSTDFONT
#define INCL_WINWINDOWMGR
#define INCL_WINSHELLDATA
#define INCL_OSAAPI
#define INCL_OSA
#include <os2.h>

/* c language includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <process.h>
#include <memory.h>
#include <sys\types.h>
#include <sys\stat.h>

/* application includes */
#include "osatest.h"
#include "osadlg.h"
#include "pmassert.h"

static const LONG lProgramVersion = 2100L;

static const PSZ pszApplication = "OSATEST";
static const PSZ pszProfileKey = "Profile";
static const PSZ pszVersionKey = "Version";

VOID GetProfileInfo(PMAIN_PARM pmp);
VOID PutProfileInfo(PMAIN_PARM pmp);
BOOL SaveProfileOnly(PMAIN_PARM pmp);
BOOL SaveVersion(PMAIN_PARM pmp);

/***************************************************************************
 * Name    : GetProfile
 *
 * Description:  Get the application defaults from OS2.INI (HINI_USERPROFILE)
 *
 * Parameters:  pmp = a pointer to the application main data structure
 *
 * Return:   [none]
 *
 ***************************************************************************/
VOID GetProfile(PMAIN_PARM pmp)
{
   LONG    lCount, lVersion;
   BOOL    fNeedDefaults;

   fNeedDefaults = TRUE;
   lVersion = 0;

   /*
    * First determine if the profile information in OS2.INI matches
    * the version number of this program.  If so, we will understand
    * the format of the data.  Otherwise, just use default values.
    */
   lCount = sizeof(LONG);

   if (!PrfQueryProfileData(
           HINI_USERPROFILE,
           pszApplication,
           pszVersionKey,
           (PVOID)&lVersion,
           (PULONG)&lCount) ||
       lCount != sizeof(LONG))
   {
       /*
        * The profile data for the version does not exist, so clean up
        * and other data. This deletes all keys under the application name.
        */
       PrfWriteProfileData(
           HINI_USERPROFILE,
           pszApplication,
           (PSZ)NULL,
           (PVOID)NULL,
           0L);
   }
   else if (lVersion != lProgramVersion)
   {
       /*
        * The profile data is the wrong version, so we need to clean
        * it up. This deletes all keys under the application name.
        */
       PrfWriteProfileData(
           HINI_USERPROFILE,
           pszApplication,
           (PSZ)NULL,
           (PVOID)NULL,
           0L);
   }
   else
   {
       /*
        * We have a matching version number, so we can proceed to
        * read the rest of the profile information.
        */

       /*
        * Look for the "Profile" information. We know the size of this
        * data so we're going to read as many bytes as we need. If for
        * some reason there's more info than we need that's all right
        * because we're relying on the lVersion to tell us we understand
        * the data.
        */
       lCount = sizeof(OSATEST_PROFILE);

       if (PrfQueryProfileData(
               HINI_USERPROFILE,
               pszApplication,
               pszProfileKey,
               (PVOID)&pmp->Profile,
               (PULONG)&lCount) &&
           lCount == sizeof(OSATEST_PROFILE))
       {
           fNeedDefaults = FALSE;
       }

   } /* end handle correct profile version info */

   if (fNeedDefaults)
   {
       memset((PVOID)&pmp->Profile, 0, sizeof(OSATEST_PROFILE));
   }
   else
   {
       GetProfileInfo(pmp);
   }
}  /*  end of GetProfile() */

/**************************************************************************
 * Name:     SaveProfile
 *
 * Description: saves profile data OSATEST_PROFILE Application defaults
 *
 * Parameters:  pmp = a pointer to the application main data structure
 *
 * Return:  [none]
 *
 ***************************************************************************/
VOID SaveProfile(PMAIN_PARM pmp)
{
    PutProfileInfo(pmp);
    SaveVersion(pmp);
    SaveProfileOnly(pmp);
    return;
} /* end of SaveProfile() */

/**************************************************************************
 * Name:     SaveVersion
 *
 * Description: saves program version to user profile
 *
 * Parameters:  pmp = a pointer to the application main data structure
 *
 * Return:  value from call to PrfWriteProfileData
 *
 ***************************************************************************/
BOOL SaveVersion(PMAIN_PARM pmp)
{
    return PrfWriteProfileData(
        HINI_USERPROFILE,
        pszApplication,
        pszVersionKey,
        (PVOID)&lProgramVersion,
        (ULONG)sizeof(lProgramVersion));
} /*  end of SaveVersion()  */

/**************************************************************************
 * Name :    SaveProfileOnly
 *
 * Description: saves program profile to user profile
 *
 * Parameters:  pmp = a pointer to the application main data structure
 *
 * Return:  value from call to PrfWriteProfileData
 *
 ***************************************************************************/
BOOL SaveProfileOnly(PMAIN_PARM pmp)
{
    return PrfWriteProfileData(
        HINI_USERPROFILE,
        pszApplication,
        pszProfileKey,
        (PVOID)&pmp->Profile,
        (ULONG)sizeof(pmp->Profile));
} /* end of SaveProfileOnly()  */

/**************************************************************************
 * Name :    PutProfileInfo
 *
 * Description:  writes data to the profile file
 *
 * API's:  WinQueryWindowPos
 *         WinQueryWindowUShort
 *
 * Parameters:  pmp = a pointer to the application main data structure
 *
 * Return:  [none]
 *
 ***************************************************************************/
VOID PutProfileInfo(PMAIN_PARM pmp)
{
   SWP     swpPos;

   /*
    * Query window position and size.
    */
   WinQueryWindowPos(pmp->hwndFrame, &swpPos);

   pmp->Profile.flOptions = 0;
   pmp->Profile.cy = swpPos.cy;
   pmp->Profile.cx = swpPos.cx;
   pmp->Profile.y = swpPos.y;
   pmp->Profile.x = swpPos.x;

   return;
}  /*  end of PutProfileInfo()  */

/**************************************************************************
 * Function: GetProfileInfo
 *
 * Description:  reads data which had been saved in the profile
 *
 * API's:  WinSetWindowPos
 *
 * Parameters:  pmp = a pointer to the application main data structure
 *
 * Result: [none]
 *
 ***************************************************************************/
VOID GetProfileInfo(PMAIN_PARM pmp)
{
   /* restore Window position */
   WinSetWindowPos( pmp->hwndFrame, HWND_TOP, pmp->Profile.x, pmp->Profile.y,
                    pmp->Profile.cx, pmp->Profile.cy,
                    pmp->Profile.flOptions |
                              SWP_ACTIVATE | SWP_MOVE | SWP_SIZE | SWP_SHOW );
   return;
}  /*  end of GetProfileInfo()  */
/***************************  End of osaprof.c ****************************/
