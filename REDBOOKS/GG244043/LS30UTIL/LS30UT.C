/*********************************************************************/
/* LS30UT.C                                                          */
/*                                                                   */
/* This DLL provides utility functions for the IBM LAN Server 3.0    */
/*                                                                   */
/* These functions provided are:                                     */
/*  GetDCName     Get Domain Controller Name                         */
/*  CopyDirAcls   Copy Directory Access Control Profiles specified   */
/*                in the source location to the destination location.*/
/*                The source directory will be created if it does    */
/*                not exist. Local support only                      */
/*  DumpAllUsers  Dump all users defined to a binary file.           */
/*  DumpUser      Dump user specified to a binary file.              */
/*  InsertAllUsers From dump file restablish all user definitions,   */
/*                Must supply new password.                          */
/*  QueryDirAliasPath Query the path of a directory alias            */
/*  MoveDirAlias  Move a directory alias to a new location           */
/*                                                                   */
/*  SetLogonAsn   Set logon assignment for a user                    */
/*  GetLogonAsn   Get logon assignments for a user                   */
/*                                                                   */
/* Modified functions: REXXUTIL OS/2 2.0 Developers Toolkit sample   */
/*  DropLs30utFuncs Drop all functions in LS30UT.DLL                 */
/*  LoadLs30utFuncs Register all functions in LS30UT.DLL             */
/*                                                                   */
/* Additional functions                                              */
/*  NetEnum       Enumerate some NET provided information            */
/*  GetLogonAsnAcp Get logon assignment access control profiles      */
/*                                                                   */
/* Author: Ingolf Lindberg, ITSC Austin 1993                         */
/*********************************************************************/
#define INCL_32

/* Include files */
#define  INCL_DOS
#define  INCL_DOSFILEMGR
#define  INCL_DOSMEMMGR
#define  INCL_DOSNMPIPES
#define  INCL_SPL
#define  INCL_SPLDOSPRINT
#define  INCL_ERRORS
#define  INCL_REXXSAA
#define  _DLL
#define  _MT
#include <os2.h>
#include <rexxsaa.h>
#include <memory.h>
#include <malloc.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

/*********************************************************************/
/* LAN Server specific header files                                  */
/*********************************************************************/
#include <netcons.h>
#include <access.h>
#include <neterr.h>
#include <audit.h>
#include <dcdb.h>
#include <server.h>

/*********************************************************************/
/*  Declare exported functions as REXX functions.                    */
/*********************************************************************/
RexxFunctionHandler LoadLs30utFuncs;
RexxFunctionHandler DropLs30utFuncs;
RexxFunctionHandler GetDCName;
RexxFunctionHandler CopyDirAcls;
RexxFunctionHandler DumpAllUsers;
RexxFunctionHandler DumpUser;
RexxFunctionHandler InsertAllUsers;
RexxFunctionHandler MoveDirAlias;
RexxFunctionHandler QueryDirAliasPath;
RexxFunctionHandler SetLogonAsn;
RexxFunctionHandler GetLogonAsn;
RexxFunctionHandler NetEnum;
RexxFunctionHandler GetLogonAsnAcp;

/*********************************************************************/
/* Definitions used                                                  */
/*********************************************************************/
#define  NO_UTIL_ERROR    "0"          /* No error occurred          */
#define  INVALID_ROUTINE  40           /* Raise REXX error           */
#define  VALID_ROUTINE     0           /* Successful completion      */
#define  MAX             256           /* Temporary buffer length    */
#define  IBUF_LEN       4096           /* Input buffer length        */

#define LEVEL_0   0                    /* Information levels         */
#define LEVEL_1   1
#define LEVEL_2   2
#define LEVEL_3   3

#define PUBDOS    1                    /* Public DOS applications    */
#define PUBOS2    2                    /* Public OS/2 applications   */
#define PRIVATE   4                    /* All private applications   */
#define ALLAPPS   7                    /* All application types      */

#define AN_APP  100                    /* For an application         */

#ifndef NERR_AppNotFound               /* DCDB errors are missing    */
#define NERR_AppNotFound 2793          /* in the header files for    */
#endif                                 /* OS/2 LAN Server 3.0        */
/*********************************************************************/
/* Structures used including typedefs                                */
/*********************************************************************/

/*********************************************************************/
/* RxStemData. Structure which describes as generic stem variable    */
/*********************************************************************/

typedef struct RxStemData1 {
    SHVBLOCK shvb;                   /* Request block for RxVar      */
    CHAR ibuf[IBUF_LEN];             /* Input buffer                 */
    CHAR varname[MAX];               /* Buffer for the variable name */
    CHAR stemname[MAX];              /* Buffer for the variable name */
    ULONG stemlen;                   /* Length of stem               */
    ULONG vlen;                      /* Length of variable value     */
    ULONG count;                     /* Number of elements processed */
} RXSTEMDATA1;

/*********************************************************************/
/* Some useful macros                                                */
/*********************************************************************/

#define BUILDRXSTRING(t, s) { \
  strcpy((t)->strptr,(s));\
  (t)->strlength = strlen((s)); \
}

/* Handle _Seg16 offset calculation */
#define GET16OFFSET(x, y) { \
 x = (CHAR * _Seg16) ((ULONG) x - (ULONG) y); \
}

/* Handle _Seg16 address calculation */
#define GET16ADDRS(x, y) { \
 x = (CHAR * _Seg16) ((ULONG) x + (ULONG) y); \
}

/*********************************************************************/
/* RxFncTable                                                        */
/*   Array of names of the LS30UT functions. Used for registration   */
/*   deregistration by SysLoadFuncs and SysDropFuncs                 */
/*********************************************************************/

static PSZ  RxFncTable[] =             /* Function package list      */
{
       "GetDCName",                    /* Get Domain Controller Name */
       "CopyDirAcls",                  /* Copy Access Controls       */
       "DumpAllUsers",                 /* Dump all user structures   */
       "DumpUser",                     /* Dump one user structure    */
       "InsertAllUsers",               /* Insert users from file     */
       "MoveDirAlias",                 /* Move directory alias       */
       "QueryDirAliasPath",            /* Query alias path definition*/
       "SetLogonAsn",                  /* Set Logon Assignment       */
       "GetLogonAsn",                  /* Get Logon Assignments      */
       "NetEnum",                      /* Enumerate NET information  */
       "GetLogonAsnAcp",               /* Check assignment access    */
       "DropLs30utFuncs",              /* Package drop function      */
       "LoadLs30utFuncs"               /* Package load function      */
};

/*********************************************************************/
/* Function definitions                                              */
/*********************************************************************/
ULONG insertStem(RXSTEMDATA1 * rxs, UCHAR * pszString);
ULONG lastInStem(RXSTEMDATA1 * rxs);
ULONG getSplEnumDevice(PULONG, PSZ, RXSTEMDATA1 *, ULONG);
ULONG getServerEnum2(PULONG, PSZ, RXSTEMDATA1 *, ULONG);
ULONG getAppEnum(PULONG, PSZ, RXSTEMDATA1 *, ULONG);
ULONG getAppEnumAcp(PULONG, PSZ, PSZ, RXSTEMDATA1 *);
ULONG getLasnAliasAcp(PULONG, PSZ, PSZ, RXSTEMDATA1 *);
VOID  setAcp(PSZ pszAcpBuf, USHORT usPermission);
ULONG getAliasAcp(PSZ, PSZ, PSZ, RXSTEMDATA1 *, struct alias_info_2 *, PSZ, ULONG);

/*********************************************************************/
/* Functions provided                                                */
/*********************************************************************/

/*********************************************************************/
/* Function:  LoadLs30utFuncs                                        */
/*                                                                   */
/* Syntax:    call SysLoadFuncs()                                    */
/* Params:    none                                                   */
/* Return:    null string                                            */
/*********************************************************************/
ULONG LoadLs30utFuncs(CHAR *name, ULONG numargs, RXSTRING args[],
                           CHAR *queuename, RXSTRING *retstr)
{
 INT    entries, j;                    /* # entries, counter         */

 if (numargs > 0)                      /* Check arguments            */
  return INVALID_ROUTINE;

 retstr->strlength = 0;                /* Set return value           */

 entries = sizeof(RxFncTable)/sizeof(PSZ);
 for (j = 0; j < entries; j++)
  RexxRegisterFunctionDll(RxFncTable[j], "LS30UT", RxFncTable[j]);

 return VALID_ROUTINE;

}

/*********************************************************************/
/* Function:  DropLs30utFuncs                                        */
/*                                                                   */
/* Syntax:    call SysDropFuncs()                                    */
/* Return:    NO_UTIL_ERROR - Successful.                            */
/*********************************************************************/
ULONG DropLs30utFuncs(CHAR *name, ULONG numargs, RXSTRING args[],
                          CHAR *queuename, RXSTRING *retstr)
{
 INT     entries, j;                   /* # entries, counter         */

 if (numargs > 0)                      /* No arguments for this call */
  return INVALID_ROUTINE;              /* Raise an error             */

 retstr->strlength = 0;                /* Return a null string result*/

 entries = sizeof(RxFncTable)/sizeof(PSZ);
 for (j = 0; j < entries; j++)
  RexxDeregisterFunction(RxFncTable[j]);

 return VALID_ROUTINE;                 /* No error on call           */
}

/*********************************************************************/
/* Function:  GetDCName                                              */
/*                                                                   */
/* Syntax:    call GetDCName()                                       */
/* Params:    none                                                   */
/* Return:    Combined RC and Domain Controller name if RC='0'       */
/*********************************************************************/
ULONG GetDCName(CHAR *name, ULONG numargs, RXSTRING args[],
                    CHAR *queuename, RXSTRING *retstr)
{
 struct server_info_0 *srv_info_buf;
 USHORT usEntriesread, usTotalentries;
 USHORT usRc;

 if (numargs > 0)                      /* Is argument specified      */
  return INVALID_ROUTINE;              /* Raise the error            */

 srv_info_buf = malloc ( sizeof(struct server_info_0) );
 if(usRc=NetServerEnum2("",                     /* Server = local    */
                        LEVEL_0,                /* level_0           */
                        (char *)srv_info_buf,   /* server_info_0     */
                        sizeof(struct server_info_0),
                        &usEntriesread,         /* entries returned  */
                        &usTotalentries,        /* total entries     */
                        (long)SV_TYPE_DOMAIN_CTRL,/*domain controller*/
                        "" ) )
 {
  sprintf(retstr->strptr, "%d", usRc);
  retstr->strlength = strlen(retstr->strptr);
  free(srv_info_buf);                  /* Drain used storage         */
  return VALID_ROUTINE;                /* no error on call           */
 }

 sprintf(retstr->strptr, "%d \\\\%s", usRc, srv_info_buf->sv0_name);
 retstr->strlength = strlen(retstr->strptr);
 free(srv_info_buf);

 return VALID_ROUTINE;                 /* No error on call           */

}

/*********************************************************************/
/* Function:  CopyDirAcls                                            */
/*                                                                   */
/* Syntax:    call CopyDirAcls(source, destination)                  */
/* Params:    source directory like C:\APPS                          */
/*            destination directory like D:\APPS                     */
/* Return:    Return code from any failing function. '0' if no error */
/*********************************************************************/
ULONG CopyDirAcls(CHAR *name, ULONG numargs, RXSTRING args[],
                    CHAR *queuename, RXSTRING *retstr)
{
 INT      i;
 USHORT   usBuflen     = 0;           /* Size of buffer area         */
 USHORT   usTotalavail = 0;           /* Total info available        */
 ULONG    ulRc;                       /* Return code                 */
 PEAOP2   EABuf;                      /* Extended attribute buffer   */
 BOOL     fMore;                      /* Loop flag                   */
 char     * tmpPtr;                   /* Directory string pointers   */
 char     * tmpPtrLast;               /*                             */
 SHORT    sParmnum;                   /* All or dedicated ACL        */
 struct access_info_1 *accinfo1Buf;   /* Resource info buffer        */

 if ( (numargs != 2) ||         /* Are arguments specified correctly */
      !RXVALIDSTRING(args[0]) ||
      !RXVALIDSTRING(args[1]) )
  return INVALID_ROUTINE;                  /* raise error            */

 /* Check the presence of the Source Directory and get work size */
 ulRc=NetAccessGetInfo("",              /* Local resource only */
                       args[0].strptr,  /* The resource name   */
                       LEVEL_1,
                       NULL,
                       usBuflen,
                       &usTotalavail);
 if( (ulRc != 0) && (ulRc != 2123) ) /* Ok or buffer to small */
 {
  sprintf(retstr->strptr, "%d", ulRc);
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;
 }

 /* There is information available, therefor continue */
 ulRc = 0; /* For test after this line if condition */
 /* Test for 2 chars in args[1] like E: and for 3 chars like E:\ */
 if( args[1].strlength > 3 )
 {
  /* Try to create directory. It is ok to create or if it exists */
  EABuf = 0;                   /* Indicate that no EAs are defined */
  ulRc = DosCreateDir(args[1].strptr, EABuf);/*This will create any  */
                                             /* first level directory*/
  if( (ulRc != 0) && (ulRc != 5 ) ) /* Directory already exists */
  {
   /* Create sub directories */
   tmpPtr = args[1].strptr + 3;  /* X:\  FIRST\SECOND */
   tmpPtrLast = args[1].strptr + (strlen(args[1].strptr) - 1);
   i = 3;
   fMore = TRUE;
   while (fMore == TRUE)
   {
    while ( *tmpPtr != '\\' && tmpPtr < tmpPtrLast )
    {
     tmpPtr++;
     i++;
    }
    /* The Last done? */
    if ( tmpPtr == tmpPtrLast)
    {
     ulRc = DosCreateDir(args[1].strptr, EABuf);/* Create directory*/
     fMore = FALSE;
    }
    else
    {
     if ( *tmpPtr == '\\')
     {
      args[1].strptr[i] = '\0';
      ulRc = DosCreateDir(args[1].strptr, EABuf);/* Create directory */
      args[1].strptr[i] = '\\';
      tmpPtr++;
      i++;
     } /* end if */
    } /* end else */
   } /* end while */
  } /* end if */

  if( (ulRc != 0) && (ulRc != 5) ) /* Could not create directories */
  {
   sprintf(retstr->strptr, "%d", ulRc);
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE;
  }

 } /* end if */

 /* Assume destination directory exist now, add Access Control Profiles */
 usBuflen = usTotalavail;
 accinfo1Buf = malloc(usBuflen);
 ulRc=NetAccessGetInfo("",
                       args[0].strptr,  /* The resource name   */
                       LEVEL_1,
                       (PCHAR) accinfo1Buf,
                       usBuflen,
                       &usTotalavail);
 if( ulRc != 0 ) /* Return on any error now */
 {
  sprintf(retstr->strptr, "%d", ulRc);
  free(accinfo1Buf);
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;
 }

 /* Set resource pointer in accinfo1Buf structure */
 accinfo1Buf->acc1_resource_name = args[1].strptr;

 /* Set all information at once */
 sParmnum = 0;
 ulRc=NetAccessSetInfo("",
                       args[1].strptr,  /* The destination resource name   */
                       LEVEL_1,
                       (PCHAR) accinfo1Buf,
                       usBuflen,
                       sParmnum);

 if(ulRc == 2222) /* No access profile exist */
 {
  ulRc=NetAccessAdd("",                  /* server name */
                    LEVEL_1,             /* level of detail */
                    (PCHAR) accinfo1Buf, /* ptr to access_info */
                    usBuflen);           /* size of data structure */
  }

 free(accinfo1Buf);
 sprintf(retstr->strptr, "%d", ulRc);
 retstr->strlength = strlen(retstr->strptr);

 return VALID_ROUTINE;                /* No error on call           */

}

/*********************************************************************/
/* Function:  DumpAllUsers                                           */
/*                                                                   */
/* Syntax:    call DumAllUsers(server, file_name)                    */
/* Params:    server from where users are uptained                   */
/*            dump file name                                         */
/* Return:    Return code from any failing function. '0' if no error */
/*********************************************************************/
ULONG DumpAllUsers(CHAR *name, ULONG numargs, RXSTRING args[],
                    CHAR *queuename, RXSTRING *retstr)
{

 FILE *stream;
 struct user_info_0 * pUserInfo0;   /* User information, level 0 */
 struct user_info_0 * pUserInfo0Org;/* User information, level 0 */
 struct user_info_2 * pUserInfo2;   /* User information, level 2 */
 USHORT usUserList, usBuflen;       /* Memory structure sizes    */
 USHORT uswrite, usRc;
 USHORT usEntriesRead, usTotalAvail; /* Number of entries read and */
                                     /* the total number */
 BOOL   fLoop;
 INT    i;
 CHAR * _Seg16 sz16Pointer;
 CHAR * _Seg16 sz16MakeThunk;
 ULONG  ulVal;

 if ( (numargs != 2) ||         /* Are arguments specified correctly */
     !RXVALIDSTRING(args[0]) ||
     !RXVALIDSTRING(args[1]) )
  return INVALID_ROUTINE;     /* raise error            */

 /* Validate that file can be opened in binary+write mode */
 if ( (stream = fopen(args[1].strptr, "wb+")) == NULL)
 {
  sprintf(retstr->strptr, "Could not open file");
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;
 }

 /* Prepare user information for dump action */
 usUserList = 3119 * sizeof(struct user_info_0); /* Get up to seg size */

 usBuflen = sizeof(struct user_info_2) + 3 * (MAXCOMMENTSZ+1) +
            2 * PATHLEN + (CNLEN+1) + MAXWORKSTATIONS * (CNLEN+1) + (21);
 pUserInfo0 = malloc(usUserList);
 pUserInfo2 = malloc(usBuflen);      /* For 1 user only information */
 pUserInfo0Org = pUserInfo0;         /* Save origin */

 memcpy(pUserInfo2, "USERLIST\0", 9);
 uswrite = fwrite(pUserInfo2, sizeof(char), 9, stream);
 /* Validate that file can be opened in binary+write mode */
 if ( uswrite != 9)
 {
  sprintf(retstr->strptr, "Could not write to file");
  retstr->strlength = strlen(retstr->strptr);
  free(pUserInfo0);      /* Free storage allocated */
  free(pUserInfo2);
  fclose(stream);
  return VALID_ROUTINE;
 }

 /* Enumerate users */
 fLoop = TRUE;
 while(fLoop == TRUE)
 {
  usRc = NetUserEnum(args[0].strptr,      /* The server name */
                     LEVEL_0,             /* Get just the names */
                     (char *) pUserInfo0, /* Place in buffer */
                     usUserList,          /* The buffer size */
                     &usEntriesRead,      /* Number of entries read */
                     &usTotalAvail);      /* Number of entries available */

  if( (usRc != 0) && (usRc != ERROR_MORE_DATA) )
  {
   sprintf(retstr->strptr, "Could not enumerate users. RC = %d", usRc);
   retstr->strlength = strlen(retstr->strptr);
   free(pUserInfo0);
   free(pUserInfo2);
   fclose(stream);
   return VALID_ROUTINE;
  }

  /* could not get ERROR_MORE_DATA to work */
  if( (usRc == 0) || (usRc == ERROR_MORE_DATA) )
   fLoop = FALSE;

  for (i = 0; i < usEntriesRead; i++)
  {
   usRc = NetUserGetInfo(args[0].strptr,        /* server name */
                         pUserInfo0->usri0_name,/* user  name  */
                         LEVEL_2,               /* level of detail */
                         (char *) pUserInfo2,   /* ptr to user_info_2
                                                /* data structure */
                         usBuflen,              /* size of structure */
                         &usTotalAvail);        /* number of bytes of */
                                                /* information available */
   if(usRc != 0)
   {
    sprintf(retstr->strptr, "Could not get user information. RC = %d", usRc);
    retstr->strlength = strlen(retstr->strptr);
    free(pUserInfo0);
    free(pUserInfo2);
    fclose(stream);
    return VALID_ROUTINE;
   }

   /* The pointers in user_info_2 structure must be aligned to offsets */
   /* They contain full address(16:16). Make it an offset and Thunk it!*/
   sz16Pointer = (CHAR *) pUserInfo2;
   /* Make offsets(16:16) within user_info_2 structure */
   GET16OFFSET(pUserInfo2->usri2_home_dir,sz16Pointer);
   GET16OFFSET(pUserInfo2->usri2_comment,sz16Pointer);
   GET16OFFSET(pUserInfo2->usri2_script_path,sz16Pointer);
   GET16OFFSET(pUserInfo2->usri2_full_name,sz16Pointer);
   GET16OFFSET(pUserInfo2->usri2_usr_comment,sz16Pointer);
   GET16OFFSET(pUserInfo2->usri2_parms,sz16Pointer);
   GET16OFFSET(pUserInfo2->usri2_workstations,sz16Pointer);
   GET16OFFSET(pUserInfo2->usri2_logon_hours,sz16Pointer);
   GET16OFFSET(pUserInfo2->usri2_logon_server,sz16Pointer);

   uswrite = fwrite(pUserInfo2, sizeof(char), usBuflen, stream);
   /* Validate that file can be opened in binary+write mode */
   if ( uswrite != usBuflen)
   {
    sprintf(retstr->strptr, "Could not write user data to file");
    retstr->strlength = strlen(retstr->strptr);
    free(pUserInfo0);
    free(pUserInfo2);
    fclose(stream);
    return VALID_ROUTINE;
   }

   pUserInfo0++;

  } /* end for */

  pUserInfo0 = pUserInfo0Org; /* Retore old condition */

 } /* end while(fLoop == TRUE) */

 /* Every thing is done now, cleanup */
 free(pUserInfo0);
 free(pUserInfo2);
 fclose(stream);
 sprintf(retstr->strptr, "0");
 retstr->strlength = strlen(retstr->strptr);

 return VALID_ROUTINE;                /* no error on call           */

}

/*********************************************************************/
/* Function:  DumpUser                                               */
/*                                                                   */
/* Syntax:    call DumUser(server, filename, userid)                 */
/* Params:    server from where users are obtained                   */
/*            dump file name                                         */
/*            userid to dump information about                       */
/* Return:    Return code from any failing function. '0' if no error */
/*********************************************************************/
ULONG DumpUser(CHAR *name, ULONG numargs, RXSTRING args[],
                    CHAR *queuename, RXSTRING *retstr)
{
 FILE *stream;
 struct user_info_2 * pUserInfo2;   /* User information, level 2 */
 USHORT usBuflen;                   /* Memory structure sizes    */
 USHORT usread, uswrite, usRc, usTotalAvail;
 ULONG  ulRc;
 CHAR * _Seg16 sz16Pointer;

 if ( (numargs != 3) ||         /* Are arguments specified correctly */
      !RXVALIDSTRING(args[0]) ||
      !RXVALIDSTRING(args[1]) ||
      !RXVALIDSTRING(args[2]) )
  return INVALID_ROUTINE;     /* raise error            */

 /* Make preparations */
 usBuflen = sizeof(struct user_info_2) + 3 * (MAXCOMMENTSZ+1) +
            2 * PATHLEN + (CNLEN+1) + MAXWORKSTATIONS * (CNLEN+1) + (21);
 pUserInfo2 = malloc(usBuflen);      /* For 1 user only information */

 /* Validate that user dump file can be opened in binary read/write mode */
 if ( (stream = fopen(args[1].strptr, "rb+")) == NULL)
 {
  /* Create the file */
  if ( (stream = fopen(args[1].strptr, "wb+")) == NULL)
  {
   sprintf(retstr->strptr, "Could not open file");
   retstr->strlength = strlen(retstr->strptr);
   free(pUserInfo2);
   return VALID_ROUTINE;
  }
  else
  {
   /* Set the file values and check against "USERLIST\0" */
   memcpy(pUserInfo2, "USERLIST\0", 9);
   uswrite = fwrite(pUserInfo2, sizeof(char), 9, stream);
   if ( uswrite != 9)
   {
    sprintf(retstr->strptr, "Could not write to file");
    retstr->strlength = strlen(retstr->strptr);
    free(pUserInfo2);
    fclose(stream);
    return VALID_ROUTINE;
   }
  }
 }

 ulRc = fseek(stream, 0L, SEEK_SET);  /* Set a beginning of file */
 if ( ulRc != 0)
 {
  sprintf(retstr->strptr, "Could not move seek pointer in user dump file");
  retstr->strlength = strlen(retstr->strptr);
  fclose(stream);
  free(pUserInfo2);
  return VALID_ROUTINE;
 }

 /* Read header */
 usread = fread( pUserInfo2, sizeof( char ), 9, stream );

 ulRc = memcmp(pUserInfo2, "USERLIST\0", 9);
 if(ulRc != 0)
 {
  sprintf(retstr->strptr, "Wrong user dump file type");
  retstr->strlength = strlen(retstr->strptr);
  fclose(stream);
  free(pUserInfo2);
  return VALID_ROUTINE;
 }

 /* Set seek pointer to end of file and append information */
 ulRc = fseek(stream, 0L, SEEK_END);  /* Set a beginning of file */
 if ( ulRc != 0)
 {
  sprintf(retstr->strptr, "Could not move seek pointer in user dump file");
  retstr->strlength = strlen(retstr->strptr);
  fclose(stream);
  free(pUserInfo2);
  return VALID_ROUTINE;
 }

 /* Get user information */
 usRc = NetUserGetInfo(args[0].strptr,        /* Server name */
                         args[2].strptr,      /* User name   */
                         LEVEL_2,             /* Level of detail */
                         (char *) pUserInfo2, /* ptr to user_info_2 */
                                              /* data structure */
                         usBuflen,            /* Size of data structure */
                         &usTotalAvail);      /* Number of bytes of
                                              /* information available */
 if(usRc != 0)
 {
  sprintf(retstr->strptr, "Could not get user information. RC = %d", usRc);
  retstr->strlength = strlen(retstr->strptr);
  free(pUserInfo2);
  fclose(stream);
  return VALID_ROUTINE;
 }

 /* The pointers in user_info_2 structure must be aligned to offsets */
 /* They contain full address(16:16). Why make it easy! */
 sz16Pointer = (CHAR *) pUserInfo2;
 /* Make offsets(16:16) within user_info_2 structure */
 GET16OFFSET(pUserInfo2->usri2_home_dir,sz16Pointer);
 GET16OFFSET(pUserInfo2->usri2_comment,sz16Pointer);
 GET16OFFSET(pUserInfo2->usri2_script_path,sz16Pointer);
 GET16OFFSET(pUserInfo2->usri2_full_name,sz16Pointer);
 GET16OFFSET(pUserInfo2->usri2_usr_comment,sz16Pointer);
 GET16OFFSET(pUserInfo2->usri2_parms,sz16Pointer);
 GET16OFFSET(pUserInfo2->usri2_workstations,sz16Pointer);
 GET16OFFSET(pUserInfo2->usri2_logon_hours,sz16Pointer);
 GET16OFFSET(pUserInfo2->usri2_logon_server,sz16Pointer);

 uswrite = fwrite(pUserInfo2, sizeof(char), usBuflen, stream);
 /* Validate that file write was ok */
 if ( uswrite != usBuflen)
 {
  sprintf(retstr->strptr, "Could not write user data to file");
  retstr->strlength = strlen(retstr->strptr);
  free(pUserInfo2);
  fclose(stream);
  return VALID_ROUTINE;
 }

 /* Every thing is done now, cleanup */
 free(pUserInfo2);
 fclose(stream);
 sprintf(retstr->strptr, "0");
 retstr->strlength = strlen(retstr->strptr);

 return VALID_ROUTINE;                /* No error on call           */

}

/*********************************************************************/
/* Function:  InsertAllUsers                                         */
/*                                                                   */
/* Syntax:    call InsertAllUsers(server, filename, logfile, newpw)  */
/* Params:    server from where users are obtained                   */
/*            user dump file name                                    */
/*            logfile name                                           */
/*            newpw is the new password that must be supplied        */
/*                                                                   */
/*            If user exist nothing will be added                    */
/*                                                                   */
/* Return:    Return code from any failing function. '0' if no error */
/*********************************************************************/
ULONG InsertAllUsers(CHAR *name, ULONG numargs, RXSTRING args[],
                    CHAR *queuename, RXSTRING *retstr)
{
 FILE *stream;
 FILE *logstream;
 struct user_info_2 * pUserInfo2;   /* User information, level 2 */
 USHORT usBuflen;                   /* Memory structure sizes    */
 USHORT usread, usRc;
 ULONG  ulRc;
 BOOL   fLoop;
 CHAR * _Seg16 sz16Pointer;
 CHAR   szBuf[80];

 if ( (numargs != 4) ||         /* Are arguments specified correctly */
     !RXVALIDSTRING(args[0]) ||
     !RXVALIDSTRING(args[1]) ||
     !RXVALIDSTRING(args[2]) ||
     !RXVALIDSTRING(args[3]) )
  return INVALID_ROUTINE;     /* raise error            */

 /* Validate that user dump file can be opened in binary+read mode */
 if ( (stream = fopen(args[1].strptr, "rb")) == NULL)
 {
  sprintf(retstr->strptr, "Could not open user file");
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;
 }

 /* Validate that log file can be opened in text mode */
 if ( (logstream = fopen(args[2].strptr, "a")) == NULL)
 {
  sprintf(retstr->strptr, "Could not open log file");
  retstr->strlength = strlen(retstr->strptr);
  fclose(stream);
  return VALID_ROUTINE;
 }

 usBuflen = sizeof(struct user_info_2) + 3 * (MAXCOMMENTSZ+1) +
            2 * PATHLEN + (CNLEN+1) + MAXWORKSTATIONS * (CNLEN+1) + (21);
 pUserInfo2 = malloc(usBuflen);      /* For 1 user only information */

 /* Read header and test it */
 usread = fread( pUserInfo2, sizeof( char ), 9, stream );
 ulRc = memcmp(pUserInfo2, "USERLIST\0", 9);
 if(ulRc != 0)
 {
  sprintf(retstr->strptr, "Wrong user dump file type");
  retstr->strlength = strlen(retstr->strptr);
  fclose(stream);
  fclose(logstream);
  free(pUserInfo2);
  return VALID_ROUTINE;
 }

 /* Ready to process user dump file content */
 /* Set correct addresses */
 sz16Pointer = (CHAR *) pUserInfo2; /* To get correct 16:16 address */
 fLoop = TRUE;
 while(fLoop == TRUE)
 {
  usread = fread( pUserInfo2, sizeof( char ), usBuflen, stream );
  if(usread == usBuflen)
  {
   /* Make address(16:16) within user_info_2 structure */
   GET16ADDRS(pUserInfo2->usri2_home_dir,sz16Pointer);
   GET16ADDRS(pUserInfo2->usri2_comment,sz16Pointer);
   GET16ADDRS(pUserInfo2->usri2_script_path,sz16Pointer);
   GET16ADDRS(pUserInfo2->usri2_full_name,sz16Pointer);
   GET16ADDRS(pUserInfo2->usri2_usr_comment,sz16Pointer);
   GET16ADDRS(pUserInfo2->usri2_parms,sz16Pointer);
   GET16ADDRS(pUserInfo2->usri2_workstations,sz16Pointer);
   GET16ADDRS(pUserInfo2->usri2_logon_hours,sz16Pointer);
   GET16ADDRS(pUserInfo2->usri2_logon_server,sz16Pointer);

   /* Set new password */
   strcpy(pUserInfo2->usri2_password, args[3].strptr);
   /* Add the user to the server specified. This must be a DC */
   usRc = NetUserAdd(args[0].strptr,         /* Servername */
                     LEVEL_2,                /* Info level 2 */
                     (PCHAR) pUserInfo2,     /* User info buffer */
                     usBuflen);              /* User infor buf size */
   if(usRc != 0)
   {
    if(usRc == NERR_UserExists)
    {
     sprintf( szBuf, "User %s already exists. No modification performed.\n", \
              pUserInfo2->usri2_name);
     fwrite( szBuf, sizeof(char), strlen(szBuf), logstream);
    }
    else
    {
     sprintf( szBuf, "Error adding user %s. rc = %d\n", \
              pUserInfo2->usri2_name, usRc);
     fwrite( szBuf, sizeof(char), strlen(szBuf), logstream);
    }
   }
  }
  else
  {
   /* Determine error and break loop */
   if ( ferror(stream) )
   {
    sprintf( szBuf,"Error reading user file\n");
    fwrite( szBuf, sizeof(char), strlen(szBuf), logstream);
   }
   else
   {
    if ( feof(stream) )
    {
     sprintf( szBuf,"End of user dump file reached\n");
     fwrite( szBuf, sizeof(char), strlen(szBuf), logstream);
    }
   }
   fLoop = FALSE; /* Done */

  } /* end if(usread == usBuflen) */
 } /* end while */

 fclose(stream);
 fclose(logstream);
 free(pUserInfo2);
 sprintf(retstr->strptr, "0");
 retstr->strlength = strlen(retstr->strptr);
 return VALID_ROUTINE;     /* done */

}

/*********************************************************************/
/* Function:  MoveDirAlias                                           */
/*                                                                   */
/* Syntax:    call MoveDirAlias(dcName, alias, newserver, newpath)   */
/* Params:    dcName The domain controller name                      */
/*            alias  The alias name of the alias to be moved         */
/*            newserver The new server name                          */
/*            newpath   The new d:\path specification                */
/*                                                                   */
/* Return:    Return code from any failing function. '0' if no error */
/*********************************************************************/
ULONG MoveDirAlias(CHAR *name, ULONG numargs, RXSTRING args[],
                    CHAR *queuename, RXSTRING *retstr)
{
 struct alias_info_2 * pAliasInfo2; /* Alias information, level 2 */
 USHORT usBuflen;                   /* Memory structure sizes    */
 USHORT usBytesAvail;
 USHORT usRc;
 CHAR   szOrgCompName[CNLEN + 1];   /* Origional computer name */
 CHAR   * pszOrgPath;               /* Original path location */

 if ( (numargs != 4) ||         /* Are arguments specified correctly */
     !RXVALIDSTRING(args[0]) ||
     !RXVALIDSTRING(args[1]) ||
     !RXVALIDSTRING(args[2]) ||
     !RXVALIDSTRING(args[3]) )
  return INVALID_ROUTINE;     /* raise error            */

 /* Prepare buffer structure */
 usBuflen = sizeof(struct alias_info_2) + (MAXCOMMENTSZ + 1) +  \
            PATHLEN + 18 * (DEVLEN + 1);
 pAliasInfo2 = malloc(usBuflen);      /* For 1 alias only information */
 if (usRc = NetAliasGetInfo( args[0].strptr, /* DC name */
                             args[1].strptr, /* Alias Name */
                             LEVEL_2,
                             (char *) pAliasInfo2,
                             usBuflen,
                             &usBytesAvail) )
 {
  /* Got an error */
  sprintf(retstr->strptr, "99 Could not obtain Alias information. rc = %d", usRc);
  free(pAliasInfo2);
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;     /* done */
 }

 if(pAliasInfo2->ai2_type != ALIAS_TYPE_FILE)
 {
  sprintf(retstr->strptr, "99 Not a directory alias type.");
  free(pAliasInfo2);
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;     /* done */
 }

 usBytesAvail = 0;
 /* Remove alias definition */
 if (usRc = NetAliasDel( args[0].strptr, /* DC name */
                         args[1].strptr, /* Alias Name */
                         usBytesAvail) )
 {
  /* Got an error. In big trouble now. Was it deleted?? or what?? */
  /* Try to add the origional again */
  usRc = NetAliasAdd ( args[0].strptr,
                       LEVEL_2,
                       (char *) pAliasInfo2,
                       usBuflen);
  sprintf(retstr->strptr, "99 Alias delete failed. Recreate returned rc = %d", usRc);
  retstr->strlength = strlen(retstr->strptr);
  free(pAliasInfo2);
  return VALID_ROUTINE;     /* done */
 }
 else
 { /* NetAliasDel() was ok, now create it again in new location. Make a copy */
  strcpy(szOrgCompName, pAliasInfo2->ai2_server);
  pszOrgPath = pAliasInfo2->ai2_path;
  /* Set new values */
  strcpy(pAliasInfo2->ai2_server, args[2].strptr);
  pAliasInfo2->ai2_path = args[3].strptr;
  if( usRc = NetAliasAdd ( args[0].strptr,  /* The same DC */
                           LEVEL_2,
                           (char *) pAliasInfo2,
                           usBuflen) )
  {
   /* Recreate origional if possible */
   strcpy(pAliasInfo2->ai2_server, szOrgCompName);
   pAliasInfo2->ai2_path = pszOrgPath;
   NetAliasAdd ( args[0].strptr,
                 LEVEL_2,
                 (char *) pAliasInfo2,
                 usBuflen);
   sprintf(retstr->strptr, "99 Alias move failed");
   free(pAliasInfo2);
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE;     /* done */
  }
 }

 free(pAliasInfo2);
 sprintf(retstr->strptr, "0");
 retstr->strlength = strlen(retstr->strptr);

 return VALID_ROUTINE;     /* done */

}

/*********************************************************************/
/* Function:  QueryDirAliasPath                                      */
/*                                                                   */
/* Syntax:    call QueryDirAliasPath(dcName, alias)                  */
/* Params:    dcName The domain controller name                      */
/*            alias  The alias name of the alias to be moved         */
/*                                                                   */
/* Return:    Return code from any failing function. '0' if no error */
/*            Need to do                                             */
/*            parse value QueryDirAliasPath() with rc path           */
/*********************************************************************/
ULONG QueryDirAliasPath(CHAR *name, ULONG numargs, RXSTRING args[],
                    CHAR *queuename, RXSTRING *retstr)
{
 struct alias_info_2 * pAliasInfo2; /* Alias information, level 2 */
 USHORT usBuflen;                   /* Memory structure sizes    */
 USHORT usBytesAvail;
 USHORT usRc;

 if ( (numargs != 2) ||         /* Are arguments specified correctly */
     !RXVALIDSTRING(args[0]) ||
     !RXVALIDSTRING(args[1]) )
  return INVALID_ROUTINE;     /* raise error            */

 usBuflen = sizeof(struct alias_info_2) + (MAXCOMMENTSZ + 1) +  \
            PATHLEN + (18 * (DEVLEN + 1));
 pAliasInfo2 = malloc(usBuflen);      /* For 1 user only information */
 if (usRc = NetAliasGetInfo( args[0].strptr, /* DC name */
                             args[1].strptr, /* Alias Name */
                             LEVEL_2,
                             (char *) pAliasInfo2,
                             usBuflen,
                             &usBytesAvail) )
 {
  /* Got an error */
  sprintf(retstr->strptr, "99 Could not obtain Alias information. rc = %d", usRc);
  retstr->strlength = strlen(retstr->strptr);
  free(pAliasInfo2);
  return VALID_ROUTINE;     /* done */
 }

 if(pAliasInfo2->ai2_type != ALIAS_TYPE_FILE)
 {
  sprintf(retstr->strptr, "99 Not a directory alias type.");
  retstr->strlength = strlen(retstr->strptr);
  free(pAliasInfo2);
  return VALID_ROUTINE;     /* done */
 }
 strcpy(retstr->strptr, "0 ");
 strcat(retstr->strptr,pAliasInfo2->ai2_path);

 retstr->strlength = strlen(retstr->strptr);
 free(pAliasInfo2);
 return VALID_ROUTINE;     /* done */

}

/*********************************************************************/
/* Function:  SetLogonAsn. Set Logon Assignment for a user           */
/*                                                                   */
/* Syntax:    call SetLogonAsn(userid, type, device, alias)          */
/* Params:    userid The userid                                      */
/*            type   Either a device assignment or application       */
/*                   Used values are 'D=' for device and 'A=' for    */
/*                   application assignment                          */
/*            device The used device. For example G:, LPT6, COM8     */
/*                   An empty specification is allowed               */
/*            alias  The alias for the device or application         */
/*                                                                   */
/* Return:    Return code from any failing function. '0' if no error */
/*********************************************************************/
ULONG SetLogonAsn(CHAR *name, ULONG numargs, RXSTRING args[],
                    CHAR *queuename, RXSTRING *retstr)
{
 struct alias_info_1 * pAliasInfo1; /* Alias information, level 1    */
 struct server_info_0 *srvInfo0;    /* Server Information 0 Buffer   */
 struct app_info_3    *pAppInfo3;   /* Appl info pointer             */
 USHORT usBuflen;                   /* Memory structure sizes        */
 USHORT usBytesAvail;
 USHORT usRc;
 UCHAR  szDcName[UNCLEN+1];         /* The Domain Controller Name    */
 INT    i;
 USHORT usEntriesread;
 USHORT usTotalentries;
 USHORT usLal_type;

 struct asn_info {
   struct logon_asn_info_1 *pLaiBuf;
   struct logon_asn_list *pLalBuf;
 } asnPointers;

 struct sel_info {
   struct app_sel_info_1 *pAsiBuf;
   struct app_sel_list *pAslBuf;
 } selPointers;

 if ( (numargs != 4) ||         /* Are arguments specified correctly */
     !RXVALIDSTRING(args[0]) ||
     !RXVALIDSTRING(args[1]) ||
     !RXVALIDSTRING(args[3]) )
  return INVALID_ROUTINE;       /* Raise error            */

 srvInfo0 = malloc(sizeof(struct server_info_0));
 if (usRc=NetServerEnum2("",                 /* server = local    */
                         LEVEL_0,            /* level_0           */
                         (char *)srvInfo0,   /* server_info_0     */
                         sizeof(struct server_info_0),
                         &usEntriesread,     /* entries returned  */
                         &usTotalentries,    /* total entries     */
                         (long)SV_TYPE_DOMAIN_CTRL , /* domain controller */
                         "" ) )
 {
  /* Error getting Domain Controller name */
  free(srvInfo0);
  sprintf(retstr->strptr, "%d Could not get Domain Controller Name", usRc);
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;     /* done */
 }
 /* Build correct DC name */
 strcpy(szDcName, "\\\\");
 strcat(szDcName, srvInfo0->sv0_name);
 free(srvInfo0); /* Done with server information structure */

 /* Determine values provided. args[1].strptr is the type parameter */
 if (strnicmp(strupr(args[1].strptr), "A=", 2) == NO_ERROR)
 {
  /* An appliciation. Get the application information required */
  usBuflen = sizeof(struct app_info_3)+MAXCOMMENTSZ+PATHLEN*3+4;
  pAppInfo3 = malloc(usBuflen);
  if (usRc=NetAppGetInfo(szDcName,           /* server             */
                         NULL,               /* public application */
                         args[3].strptr,     /* application id     */
                         LEVEL_3,            /* level 3            */
                         (char *) pAppInfo3, /* buffer             */
                         usBuflen,           /* length             */
                         &usTotalentries ) ) /* total size         */
  {
   sprintf(retstr->strptr, "%d Could not get Application Information", usRc);
   free(pAppInfo3);
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE;     /* done */
  }

  usBuflen = sizeof(struct app_sel_info_1);
  selPointers.pAsiBuf = malloc(usBuflen);      /* For 1 user only information */
  usLal_type = pAppInfo3->app3_apptype;

  free(pAppInfo3);
  usBytesAvail = 0;
  usRc = NetUserGetAppSel(szDcName,             /* The DC   */
                          args[0].strptr,       /* The Userid */
                          LEVEL_1,              /* level_1  */
                          APP_ALL,              /* The type */
                          (char *) (selPointers.pAsiBuf),  /* The buffer */
                          usBuflen,             /* Size of the structure */
                          &usBytesAvail);
  free(selPointers.pAsiBuf);
  if( (usRc == ERROR_FILE_NOT_FOUND) || (usRc == ERROR_PATH_NOT_FOUND) )
  {
   /* Initiate a structure */
   if (usRc=NetUserDCDBInit(szDcName,args[0].strptr,(ULONG)NULL))
   {
    sprintf(retstr->strptr, "%d Could not initiate user DCDB information", usRc);
    retstr->strlength = strlen(retstr->strptr);
    return VALID_ROUTINE;     /* done */
   }
  }
  else
  {
   if( usRc != NERR_BufTooSmall )
   {
    sprintf(retstr->strptr, "%d Could not get user defined Applications", usRc);
    retstr->strlength = strlen(retstr->strptr);
    return VALID_ROUTINE;     /* done */
   }
  }

  /* We got something. Allocate the correct size, obtain information */
  usBuflen = usBytesAvail + sizeof(struct app_sel_list);
  selPointers.pAsiBuf = malloc(usBuflen); /* For 1 additional   */
  usRc = NetUserGetAppSel(szDcName,             /* The DC     */
                          args[0].strptr,       /* The Userid */
                          LEVEL_1,              /* level_1    */
                          APP_ALL,              /* All types  */
                          (char *) (selPointers.pAsiBuf),  /* The buffer */
                          usBuflen,             /* Size of the structure */
                          &usBytesAvail);
  if( usRc != 0 )
  {
   free(selPointers.pAsiBuf);
   sprintf(retstr->strptr, "%d Could not get user defined Applications", usRc);
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE;     /* done */
  }

  /* Get starting point */
  selPointers.pAslBuf = (struct app_sel_list *) \
  ((struct app_sel_info_1 *) selPointers.pAsiBuf + 1);
  /* This is the new one */
  selPointers.pAslBuf += selPointers.pAsiBuf->asi1_count; /* Set offset */
  selPointers.pAsiBuf->asi1_count++; /* Add one more to add */
  strcpy(selPointers.pAslBuf->asl_appname, args[3].strptr);
  selPointers.pAslBuf->asl_apptype = usLal_type; /* The type */
  /* Set the new one including the old ones */
  usRc = NetUserSetAppSel(szDcName,             /* The DC   */
                          args[0].strptr,       /* The Userid */
                          LEVEL_1,              /* level_1  */
                          (char *) (selPointers.pAsiBuf), /* The buffer */
                          usBuflen);            /* Size of the structure */

  free(selPointers.pAsiBuf);
  if(usRc != 0)
  {
   sprintf(retstr->strptr, "%d Could not define Application for user", usRc);
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE;     /* done */
  }
 }
 else
 {
  if (strnicmp(strupr(args[1].strptr), "D=", 2) == NO_ERROR)
  {
   /* A device, validate that the alias exist */
   usBuflen = sizeof(struct alias_info_1)+MAXCOMMENTSZ+1;
   pAliasInfo1 = malloc(usBuflen);
   if(usRc=NetAliasGetInfo(szDcName,            /* The Domain Controller*/
                           args[3].strptr,      /* The alias           */
                           LEVEL_1,             /* The level           */
                           (char *)pAliasInfo1, /* buffer pointer      */
                           usBuflen,            /* The length          */
                           &usBytesAvail) )     /* The total size      */
   {
    sprintf(retstr->strptr, "%d Could not get Alias Information", usRc);
    free(pAliasInfo1);
    retstr->strlength = strlen(retstr->strptr);
    return VALID_ROUTINE;     /* done */
   }
   /* Have valid alias information */
   if(args[2].strlength != 0)
   {
    /* args[2].strptr is the device name */
    if(strchr(args[2].strptr, ':') != NULL)
    {
     /* Got a colon, delete it */
     for (i=0; i<strlen(args[2].strptr) ;i++ ) {
      if(args[2].strptr[i] == ':')
       args[2].strptr[i] = '\0';
     }
    }
   }
   /* Determine which type of resource and ask for current count */
   usBuflen = sizeof(struct logon_asn_info_1);
   asnPointers.pLaiBuf = malloc(usBuflen);      /* For 1 user only information */
   usLal_type = pAliasInfo1->ai1_type;  /* The type */
   free(pAliasInfo1);
   usBytesAvail = 0;
   usRc = NetUserGetLogonAsn(szDcName,             /* The DC   */
                             args[0].strptr,       /* The Userid */
                             LEVEL_1,              /* level_1  */
                             7,                    /* The type */
                             (char *) (asnPointers.pLaiBuf),  /* The buffer */
                             usBuflen,             /* Size of the structure */
                             &usBytesAvail);
   free(asnPointers.pLaiBuf);
   if( (usRc == ERROR_FILE_NOT_FOUND) || (usRc == ERROR_PATH_NOT_FOUND) )
   {
    /* Initiate a structure */
    if (usRc=NetUserDCDBInit(szDcName,args[0].strptr,(ULONG)NULL))
    {
     sprintf(retstr->strptr, "%d Could not initiate user DCDB information", usRc);
     retstr->strlength = strlen(retstr->strptr);
     return VALID_ROUTINE;     /* done */
    }
   }
   else
   {
    if( usRc != NERR_BufTooSmall )
    {
     sprintf(retstr->strptr, "%d Could not get user Logon Assignment", usRc);
     retstr->strlength = strlen(retstr->strptr);
     return VALID_ROUTINE;     /* done */
    }
   }

   /* We got something. Allocate the correct size, obtain information */
   usBuflen = usBytesAvail + sizeof(struct logon_asn_list);
   asnPointers.pLaiBuf = malloc(usBuflen); /* For 1 additional   */
   usRc = NetUserGetLogonAsn(szDcName,             /* The DC     */
                             args[0].strptr,       /* The Userid */
                             LEVEL_1,              /* level_1    */
                             7,                    /* All types  */
                             (char *) (asnPointers.pLaiBuf),  /* The buffer */
                             usBuflen,             /* Size of the structure */
                             &usBytesAvail);
   if( usRc != 0 )
   {
    free(asnPointers.pLaiBuf);
    sprintf(retstr->strptr, "%d Could not get user Logon Assignment", usRc);
    retstr->strlength = strlen(retstr->strptr);
    return VALID_ROUTINE;     /* done */
   }

   /* Get starting point */
   asnPointers.pLalBuf = (struct logon_asn_list *) \
   ((struct logon_asn_info_1 *) asnPointers.pLaiBuf + 1);
   /* This is the new one */
   asnPointers.pLalBuf += asnPointers.pLaiBuf->lai1_count;
   asnPointers.pLaiBuf->lai1_count++; /* Add one */
   strcpy(asnPointers.pLalBuf->lal_alias, args[3].strptr);
   asnPointers.pLalBuf->lal_type = usLal_type; /* The type */

   if(args[2].strlength != 0)
    strcpy(asnPointers.pLalBuf->lal_device, args[2].strptr);

   /* Add User Logn assignments */
   usRc = NetUserSetLogonAsn(szDcName,             /* The DC   */
                             args[0].strptr,       /* The Userid */
                             LEVEL_1,              /* level_1  */
                             (char *) (asnPointers.pLaiBuf), /* The buffer */
                             usBuflen);            /* Size of the structure */
   free(asnPointers.pLaiBuf);
   if(usRc != 0)
   {
    sprintf(retstr->strptr, "%d Could not set user Logon Assignment", usRc);
    retstr->strlength = strlen(retstr->strptr);
    return VALID_ROUTINE;     /* done */
   }
  }
  else
  {
   sprintf(retstr->strptr, "99 Invalid Type specification");
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE;     /* done */
  }
 }

 BUILDRXSTRING(retstr, "0");
 return VALID_ROUTINE;     /* no error  */

}

/*********************************************************************/
/* Function:  GetLogonAsn. Get Logon Assignments for a user          */
/*                                                                   */
/* Syntax:    call GetLogonAsn(userid, asnList)                      */
/* Params:    userid The userid                                      */
/*            asnList The stem variable to hold the logon assignments*/
/*                   The following values are used:                  */
/*                                                                   */
/*                   -none- No assignments found                     */
/*                                                                   */
/*                          or                                       */
/*                                                                   */
/*                   type   Either a device assignment or application*/
/*                          Used values are 'D=' for device and 'A=' */
/*                          for application assignment               */
/*                   device The used device. For example G:, LPT6,   */
/*                          COM8. An empty specification is set to   */
/*                          blanks                                   */
/*                   alias  The alias for the device or application  */
/*                                                                   */
/* Return:    Return code from any failing function. '0' if no error */
/*********************************************************************/
ULONG GetLogonAsn(CHAR *name, ULONG numargs, RXSTRING args[],
                    CHAR *queuename, RXSTRING *retstr)
{
 struct alias_info_1 * pAliasInfo1; /* Alias information, level 1    */
 struct server_info_0 *srvInfo0;    /* Server Information 0 Buffer   */
 struct app_info_3    *pAppInfo3;   /* Appl info pointer             */
 USHORT usBuflen;                   /* Memory structure sizes        */
 USHORT usBytesAvail;
 USHORT usRc;
 ULONG  ulRc;
 UCHAR  szDcName[UNCLEN+1];         /* The Domain Controller Name    */
 UCHAR  szWorkBuf[80];              /* Working charater buffer       */
 INT    i;
 USHORT usEntriesread;
 USHORT usTotalentries;
 USHORT usLal_type;
 BOOL   fAppAsn = FALSE;
 RXSTEMDATA1 lrxs;                   /* Local REXX stem data structure*/


 struct asn_info {
   struct logon_asn_info_1 *pLaiBuf;
   struct logon_asn_list *pLalBuf;
 } asnPointers;

 struct sel_info {
   struct app_sel_info_1 *pAsiBuf;
   struct app_sel_list *pAslBuf;
 } selPointers;

 if ( (numargs != 2) ||         /* Are arguments specified correctly */
     !RXVALIDSTRING(args[0]) ||
     !RXVALIDSTRING(args[1]) )
  return INVALID_ROUTINE;       /* Raise error            */

 srvInfo0 = malloc(sizeof(struct server_info_0));
 if (usRc=NetServerEnum2("",                 /* server = local    */
                         LEVEL_0,            /* level_0           */
                         (char *)srvInfo0,   /* server_info_0     */
                         sizeof(struct server_info_0),
                         &usEntriesread,     /* entries returned  */
                         &usTotalentries,    /* total entries     */
                         (long)SV_TYPE_DOMAIN_CTRL , /* domain controller */
                         "" ) )
 {
  /* Error getting Domain Controller name */
  free(srvInfo0);
  sprintf(retstr->strptr, "%d Could not get Domain Controller Name", usRc);
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;     /* done */
 }

 /* Build correct DC name */
 strcpy(szDcName, "\\\\");
 strcat(szDcName, srvInfo0->sv0_name);
 free(srvInfo0); /* Done with server information structure */

 /* Initialize stem data structure */
 lrxs.count = 0;
 strcpy(lrxs.varname, args[1].strptr);
 lrxs.stemlen = args[1].strlength;
 strupr(lrxs.varname);

 if(lrxs.varname[lrxs.stemlen - 1] != '.')
 {
  lrxs.varname[lrxs.stemlen] = '.';
  lrxs.varname[lrxs.stemlen + 1] = '\0';
  lrxs.stemlen++;
 }

 usBuflen = sizeof(struct app_sel_info_1);
 selPointers.pAsiBuf = malloc(usBuflen);
 usBytesAvail = 0;
 usRc = NetUserGetAppSel(szDcName,             /* The DC   */
                         args[0].strptr,       /* The Userid */
                         LEVEL_1,              /* level_1  */
                         APP_ALL,              /* The type */
                         (char *) (selPointers.pAsiBuf),  /* The buffer */
                         usBuflen,             /* Size of the structure */
                         &usBytesAvail);
 free(selPointers.pAsiBuf);

 if( (usRc == ERROR_FILE_NOT_FOUND) || (usRc == ERROR_PATH_NOT_FOUND) )
 {
  /* User has nothing defined, set ending values, and end this call */
  strcpy(szWorkBuf, "-none-");
  ulRc = insertStem(&lrxs, szWorkBuf);
  if( ulRc != VALID_ROUTINE)
    return INVALID_ROUTINE;

  ulRc = lastInStem(&lrxs);
  if( ulRc != VALID_ROUTINE)
    return INVALID_ROUTINE;

  BUILDRXSTRING(retstr, "0");
  return VALID_ROUTINE;     /* done */

 }

 /* There was information available */
 if( usRc == NERR_BufTooSmall )
 {
  /* We got something. Allocate the correct size, obtain information */
  usBuflen = usBytesAvail + sizeof(struct app_sel_list);
  selPointers.pAsiBuf = malloc(usBuflen); /* For 1 additional   */
  usRc = NetUserGetAppSel(szDcName,             /* The DC     */
                          args[0].strptr,       /* The Userid */
                          LEVEL_1,              /* level_1    */
                          APP_ALL,              /* All types  */
                          (char *) (selPointers.pAsiBuf),  /* The buffer */
                          usBuflen,             /* Size of the structure */
                          &usBytesAvail);
  if( usRc == 0 )
  {
   /* Get starting point */
   fAppAsn = TRUE;
   /* Point to the first */
   selPointers.pAslBuf = (struct app_sel_list *) \
   ((struct app_sel_info_1 *) selPointers.pAsiBuf + 1);

   for(i=0; i < selPointers.pAsiBuf->asi1_count; i++)
   {
    strcpy(szWorkBuf, "A= ");
    strcat(szWorkBuf, selPointers.pAslBuf->asl_appname);
    ulRc = insertStem(&lrxs, szWorkBuf);

    if( ulRc != VALID_ROUTINE)
    {
     free(selPointers.pAsiBuf);
     return INVALID_ROUTINE;
    }

    selPointers.pAslBuf++;
   }

   ulRc = lastInStem(&lrxs);
   if( ulRc != VALID_ROUTINE)
   {
    free(selPointers.pAsiBuf);
    return INVALID_ROUTINE;
   }

  }
  free(selPointers.pAsiBuf);
 }

 /* Logon assignments now */
 usBuflen = sizeof(struct logon_asn_info_1);
 asnPointers.pLaiBuf = malloc(usBuflen);
 usBytesAvail = 0;
 usRc = NetUserGetLogonAsn(szDcName,             /* The DC   */
                           args[0].strptr,       /* The Userid */
                           LEVEL_1,              /* level_1  */
                           7,                    /* The type */
                           (char *) (asnPointers.pLaiBuf),  /* The buffer */
                           usBuflen,             /* Size of the structure */
                           &usBytesAvail);
 free(asnPointers.pLaiBuf);

 /* There was information available */
 if( usRc == NERR_BufTooSmall )
 {
  /* We got something. Allocate the correct size, obtain information */
  usBuflen = usBytesAvail;
  asnPointers.pLaiBuf = malloc(usBuflen); /* For all information*/
  usRc = NetUserGetLogonAsn(szDcName,             /* The DC     */
                            args[0].strptr,       /* The Userid */
                            LEVEL_1,              /* level_1    */
                            7,                    /* All types  */
                            (char *) (asnPointers.pLaiBuf),  /* The buffer */
                            usBuflen,             /* Size of the structure */
                            &usBytesAvail);
  if( usRc != 0 )
  {
   free(asnPointers.pLaiBuf);
   if(fAppAsn == FALSE)
   {
    sprintf(retstr->strptr, "%d Could not get user Assignments", usRc);
    retstr->strlength = strlen(retstr->strptr);
    return VALID_ROUTINE;     /* done */
   }
  }
  else
  {
   /* Work on the information provided. Get starting point */
   asnPointers.pLalBuf = (struct logon_asn_list *) \
   ((struct logon_asn_info_1 *) asnPointers.pLaiBuf + 1);
   for(i=0; i < asnPointers.pLaiBuf->lai1_count; i++)
   {
    sprintf(szWorkBuf,"D=     %8s  %8s", \
                      asnPointers.pLalBuf->lal_device, \
                      asnPointers.pLalBuf->lal_alias);

    ulRc = insertStem(&lrxs, szWorkBuf);
    if( ulRc != VALID_ROUTINE)
    {
     free(asnPointers.pLaiBuf);
     return INVALID_ROUTINE;
    }

    asnPointers.pLalBuf++;
   }

   ulRc = lastInStem(&lrxs);
   if( ulRc != VALID_ROUTINE)
   {
    free(asnPointers.pLaiBuf);
    return INVALID_ROUTINE;
   }

  }
  free(selPointers.pAsiBuf);
 }
 else
 {
  if(usBytesAvail == sizeof(struct logon_asn_info_1) )
  {
   /* User has nothing defined this time, set ending values */
   if(fAppAsn == FALSE)  /* Check if application selector was set */
   {
    strcpy(szWorkBuf, "-none-");

    ulRc = insertStem(&lrxs, szWorkBuf);
    if( ulRc != VALID_ROUTINE)
     return INVALID_ROUTINE;

    ulRc = lastInStem(&lrxs);
    if( ulRc != VALID_ROUTINE)
     return INVALID_ROUTINE;
   }

  }
  else
  {
   if(fAppAsn == FALSE)
   {
     sprintf(retstr->strptr, "%d Could not get user Assignments", usRc);
     retstr->strlength = strlen(retstr->strptr);
     return VALID_ROUTINE;     /* done */
   }

  }

 }

 BUILDRXSTRING(retstr, "0");
 return VALID_ROUTINE;       /* Raise no error */

}

/*********************************************************************/
/* Function:  GetLogonAsnAcp  Get the logon assignment access control*/
/*                            profiles for applications or alias     */
/*                                                                   */
/*            External resources are not supported                   */
/*                                                                   */
/* Syntax:    GetLogonAsnAcp(user, type, stem)                       */
/* Params:    user    The userid to check                            */
/*            type    Alias type 'A=', 'D=', 'ALL'                   */
/*            stem    The REXX stem variable used to return the      */
/*                    alias specifications that do not have an       */
/*                    access control profile for the user. User and  */
/*                    groups are validated                           */
/*                                                                   */
/* Example call:  rc = GetLogonAsnAcp(USR, 'D=', 'stem.')            */
/*                                                                   */
/* Return value examples:                                            */
/*                                                                   */
/*               D=  ALIAS    RWC                                    */
/*               D=  ALIAS8   external resource                      */
/*               A=  ALIAS1   RWC                                    */
/*               A=  ALIAS2     N                                    */
/*               A=  ALIAS3     R                                    */
/*                                                                   */
/* The complete path is not displayed, only the alias name.          */
/*                                                                   */
/* Return:    Return code from any failing function. '0' if no error */
/*********************************************************************/
ULONG GetLogonAsnAcp(CHAR *name, ULONG numargs, RXSTRING args[],
                     CHAR *queuename, RXSTRING *retstr)
{

 RXSTEMDATA1 lrxs;                  /* Local REXX stem data structure*/
 struct server_info_0 *pSrvInfo0;   /* Pointer to server information */
 USHORT usEntriesread, usTotalentries;
 ULONG  ulRc;                       /* Return code                   */
 UCHAR  szDcName[UNCLEN+1];         /* The Domain Controller Name    */
 ULONG  ulFnRc;                     /* Function return code          */

 if ((numargs != 3) ||          /* Are arguments specified correctly */
     !RXVALIDSTRING(args[0]) ||
     !RXVALIDSTRING(args[1]) ||
     !RXVALIDSTRING(args[2]) )
  return INVALID_ROUTINE;       /* Raise error            */

 /* Initialize stem data structure */
 lrxs.count = 0;
 strcpy(lrxs.varname, args[2].strptr);
 lrxs.stemlen = args[2].strlength;
 strupr(lrxs.varname);

 if(lrxs.varname[lrxs.stemlen - 1] != '.')
 {
  lrxs.varname[lrxs.stemlen] = '.';
  lrxs.varname[lrxs.stemlen + 1] = '\0';
  lrxs.stemlen++;
 }

 /* Get domain controller machine ID */
 pSrvInfo0 = malloc( sizeof(struct server_info_0) );
 if(ulRc=NetServerEnum2("",                     /* Server name       */
                        LEVEL_0,                /* level_0           */
                        (char *)pSrvInfo0,      /* Pointer to buffer */
                        sizeof(struct server_info_0),
                        &usEntriesread,         /* Entries returned  */
                        &usTotalentries,        /* Total entries     */
                        (long)SV_TYPE_DOMAIN_CTRL,/*DC only          */
                        "" ) )
 {
  sprintf(retstr->strptr, "%d Could not obtain Domain Controller machine ID", ulRc);
  retstr->strlength = strlen(retstr->strptr);
  free(pSrvInfo0);                     /* Free used memory           */
  return VALID_ROUTINE;                /* No REXX error on call      */
 }

 sprintf(szDcName, "\\\\%s", pSrvInfo0->sv0_name);
 free(pSrvInfo0);

 /* Determine the request type */
 if (strnicmp(strupr(args[1].strptr), "ALL", 3) == NO_ERROR)
 {
  ulRc = getAppEnumAcp(&ulFnRc, szDcName, args[0].strptr, &lrxs);
  if(ulRc != VALID_ROUTINE)
   return INVALID_ROUTINE;            /* Raise error */

  if (ulFnRc != 0)
  {
   sprintf(retstr->strptr, "%d Could not get Application information", ulFnRc);
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE;       /* Raise no error */
  }

  ulRc = getLasnAliasAcp(&ulFnRc, szDcName, args[0].strptr, &lrxs);
  if(ulRc != VALID_ROUTINE)
   return INVALID_ROUTINE;            /* Raise error */

  if (ulFnRc != 0)
  {
   sprintf(retstr->strptr, "%d Could not get Application information", ulFnRc);
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE;       /* Raise no error */
  }

 }
 else
 if (strnicmp(strupr(args[1].strptr), "D=", 2) == NO_ERROR)
 {
  ulRc = getLasnAliasAcp(&ulFnRc, szDcName, args[0].strptr, &lrxs);
  if(ulRc != VALID_ROUTINE)
   return INVALID_ROUTINE;            /* Raise error */

  if (ulFnRc != 0)
  {
   sprintf(retstr->strptr, "%d Could not get Application information", ulFnRc);
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE;       /* Raise no error */
  }

 }
 else
 if (strnicmp(strupr(args[1].strptr), "A=", 2) == NO_ERROR)
 {
  ulRc = getAppEnumAcp(&ulFnRc, szDcName, args[0].strptr, &lrxs);
  if(ulRc != VALID_ROUTINE)
   return INVALID_ROUTINE;            /* Raise error */

  if (ulFnRc != 0)
  {
   sprintf(retstr->strptr, "%d Could not get Application information", ulFnRc);
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE;       /* Raise no error */
  }

 }

 BUILDRXSTRING(retstr, "0");
 return VALID_ROUTINE;       /* Raise no error */

}

/*********************************************************************/
/* Function:  NetEnum. Enumerate LAN Server 3.0 domain information   */
/*                                                                   */
/* Syntax:    call NetEnum(option, serverName, infoList, addInfo)    */
/* Params:    option     The item to enumerate                       */
/*                       The following options are supported         */
/*                                                                   */
/*                       option                   addInfo            */
/*                       ----------------         ---------------    */
/*                       "APPLICATIONS"           "PUBLICOS2"        */
/*                       "APPLICATIONS"           "PUBLICDOS"        */
/*                       "APPLICATIONS"           "PRIVATE"          */
/*                       "APPLICATIONS"           "ALL"              */
/*                       "PRINTDEV"               "STATUS"           */
/*                       "SERVERS"                srvType            */
/*                                                                   */
/*            serverName The server machine name. Normally this is   */
/*                       the domain controller machine id. The       */
/*                       machine name used must include \\           */
/*            infoList   The stem variable to hold the info. This    */
/*                       variable is only valid if NetEnum()         */
/*                       returns a "0" returncode.                   */
/*            addInfo    Variable or value for addtional enumerate   */
/*                       options. For default override.              */
/*                                                                   */
/* The srvType parameter must be supplied for "SERVERS"              */
/*                                                                   */
/* The addInfo parameter must be supplied for the "APPLICATIONS"     */
/* enumaration together with the domain controller machine ID.       */
/*                                                                   */
/*            The content of the infoList variable is as follows:    */
/*                                                                   */
/*            infoList.0 The number of information items             */
/*            infoList.n information item n                          */
/*                                                                   */
/* Examples:  rc= NetEnum('printdev','\\TheSrv','list.','status')    */
/*            rc= NetEnum('servers',,'srvList.', 2)                  */
/*            rc= NetEnum('applications','\\OURDC','appList.','all') */
/*                                                                   */
/* Return:    Return code from any failing function. '0' if no error */
/*********************************************************************/
ULONG NetEnum(CHAR *name, ULONG numargs, RXSTRING args[],
                    CHAR *queuename, RXSTRING *retstr)
{
 USHORT usBuflen;                   /* Memory structure sizes        */
 USHORT usBytesAvail;
 USHORT usRc;
 UCHAR  szDcName[UNCLEN+1];         /* The Domain Controller Name    */
 INT    i;
 USHORT usEntriesread;
 USHORT usTotalentries;
 USHORT usLal_type;
 ULONG  ulFnRc;                     /* Function return code          */
 ULONG  ulRc;

 RXSTEMDATA1 lrxs;                  /* Local REXX stem data structure*/
 ULONG       ulStatus;


 if ( !RXVALIDSTRING(args[0]) ||
     !RXVALIDSTRING(args[2]) )
  return INVALID_ROUTINE;                  /* Raise error            */

 /* Initialize stem data structure */
 lrxs.count = 0;
 strcpy(lrxs.varname, args[2].strptr);
 lrxs.stemlen = args[2].strlength;
 strupr(lrxs.varname);

 if(lrxs.varname[lrxs.stemlen - 1] != '.')
 {
  lrxs.varname[lrxs.stemlen] = '.';
  lrxs.varname[lrxs.stemlen + 1] = '\0';
  lrxs.stemlen++;
 }

 ulFnRc = 0;
 /* Determine what the request is and perform it */
 /* PRINTDEV ? */
 if (strnicmp(strupr(args[0].strptr), "PRINTDEV", 8) == NO_ERROR)
 {
  ulStatus = 0;
  if ( (numargs == 3) || (numargs == 4) )
  {
   if( (RXVALIDSTRING(args[3]) ) && (args[3].strlength == 6) )
   {
    if (strnicmp(strupr(args[3].strptr), "STATUS", 6) == NO_ERROR)
    {
     ulStatus = 1;  /* Do only name and status */
    }
   }
  }

  if( getSplEnumDevice(&ulFnRc, args[1].strptr, &lrxs, ulStatus) )
   return INVALID_ROUTINE;                 /* Raise error            */

  if (ulFnRc != 0)
  {
   sprintf(retstr->strptr, "%d Could not get PRINTDEV information", ulFnRc);
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE;       /* Raise no error */
  }
 }
 else
 /* SERVERS ? */
 if (strnicmp(strupr(args[0].strptr), "SERVERS", 7) == NO_ERROR)
 {
  /* The type parameter can be one or a combination of the following  */
  /* SV_TYPE_WORKSTATION     0x00000001 Workstation                   */
  /* SV_TYPE_SERVER          0x00000002 Server                        */
  /* SV_TYPE_SQLSERVER       0x00000004 SQL server                    */
  /* SV_TYPE_DOMAIN_CTRL     0x00000008 Domain controller             */
  /* SV_TYPE_DOMAIN_BAKCTRL  0x00000010 Backup domain controller      */
  /* SV_TYPE_TIME_SOURCE     0x00000020 Time server                   */
  /* SV_TYPE_AFP             0x00000040 Apple** File Protocol service */
  /* SV_TYPE_NOVELL          0x00000080 Novell** service              */
  /* SV_TYPE_ALL             0xFFFFFFFF All types of servers          */

  if ( !RXVALIDSTRING(args[3]))
   return INVALID_ROUTINE;                  /* Raise error            */

  ulRc = getServerEnum2(&ulFnRc, NULL, &lrxs, atoi(args[3].strptr) );
  if(ulRc != VALID_ROUTINE)
   return INVALID_ROUTINE;            /* Raise error */

  if (ulFnRc != 0)
  {
   sprintf(retstr->strptr, "%d Could not get SERVERS information", ulFnRc);
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE;       /* Raise no error */
  }

 }
 else
 /* APPLICATIONS */
 if (strnicmp(strupr(args[0].strptr), "APPLICATIONS", 12) == NO_ERROR)
 {
  ulStatus = ALLAPPS;
  if (numargs == 4)
  {
   if( (RXVALIDSTRING(args[1]) ) && (RXVALIDSTRING(args[3]) ) && \
        (args[1].strlength <= 17) && (args[3].strlength <= 9) )
   {
    if (strnicmp(strupr(args[3].strptr), "PUBLICDOS", 9) == NO_ERROR)
     ulStatus = PUBDOS;               /* Get public DOS applications */

    if (strnicmp(strupr(args[3].strptr), "PUBLICOS2", 9) == NO_ERROR)
     ulStatus = PUBOS2;               /* Get public OS/2 applications */

    if (strnicmp(strupr(args[3].strptr), "PRIVATE", 7) == NO_ERROR)
     ulStatus = PRIVATE;               /* Get all private applications */

    if (strnicmp(strupr(args[3].strptr), "ALL", 3) == NO_ERROR)
     ulStatus = ALLAPPS;               /* Get all applications */
   }
  }
  if(ulStatus == ALLAPPS)
  {
   if( getAppEnum(&ulFnRc, args[1].strptr, &lrxs, PUBDOS) )
    return INVALID_ROUTINE;            /* Raise error */
   if( getAppEnum(&ulFnRc, args[1].strptr, &lrxs, PUBOS2) )
    return INVALID_ROUTINE;            /* Raise error */
   if( getAppEnum(&ulFnRc, args[1].strptr, &lrxs, PRIVATE) )
    return INVALID_ROUTINE;            /* Raise error */
  }
  else
  {
   if( getAppEnum(&ulFnRc, args[1].strptr, &lrxs, ulStatus) )
    return INVALID_ROUTINE;            /* Raise error */
  }

  if (ulFnRc != 0)
  {
   sprintf(retstr->strptr, "%d Could not get APPLICATIONS information", ulFnRc);
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE;       /* Raise no error */
  }

 }

 BUILDRXSTRING(retstr, "0");
 return VALID_ROUTINE;       /* Raise no error */

}

/*********************************************************************/
/* Insert a string into available stem variable                      */
/*********************************************************************/
ULONG insertStem(RXSTEMDATA1 * rxs, UCHAR * pszString)
{

 if(pszString != '\0')           /* Make sure a valid string is used */
 {
  if( strlen(pszString) < IBUF_LEN)
  {
   strcpy(rxs->ibuf, pszString);
   rxs->vlen = strlen(pszString);
  }
  else
  {
   memcpy(rxs->ibuf, pszString, IBUF_LEN - 1);
   rxs->ibuf[IBUF_LEN] = '\0';
  }
 }
 else
 {
  strcpy(rxs->ibuf, "Invalid data");
  rxs->vlen = strlen("Invalid data");
 }

 rxs->count++;                                    /* n'th + 1 entry */
 sprintf(rxs->varname+rxs->stemlen, "%d", rxs->count);
 rxs->shvb.shvnext = NULL;          /* This is the last entry       */
 rxs->shvb.shvname.strptr = rxs->varname; /* This is the var name   */
 rxs->shvb.shvname.strlength = strlen(rxs->varname);
 rxs->shvb.shvnamelen        = strlen(rxs->varname);
 rxs->shvb.shvvalue.strptr   = rxs->ibuf; /* Set buffer address     */
 rxs->shvb.shvvalue.strlength= rxs->vlen;
 rxs->shvb.shvvaluelen       = rxs->vlen;
 rxs->shvb.shvcode           = RXSHV_SET; /* Set value request      */
 rxs->shvb.shvret            = 0;

 if (RexxVariablePool(&rxs->shvb) == RXSHV_BADN)
 {
  return INVALID_ROUTINE;                 /* error on non-zero      */
 }

 return VALID_ROUTINE;                     /* Raise no error         */

}


/*********************************************************************/
/* Set last stem variable                                            */
/*********************************************************************/
ULONG lastInStem(RXSTEMDATA1 * rxs)
{

 sprintf(rxs->ibuf, "%d", rxs->count);
 rxs->varname[rxs->stemlen] = '0';        /* 'varname.0'            */
 rxs->varname[rxs->stemlen + 1] = '\0';
 rxs->shvb.shvnext = NULL;          /* No further entries           */
 rxs->shvb.shvname.strptr = rxs->varname; /* This is the var name   */
 rxs->shvb.shvname.strlength = strlen(rxs->varname);
 rxs->shvb.shvnamelen        = strlen(rxs->varname);
 rxs->shvb.shvvalue.strptr   = rxs->ibuf; /* Set buffer address     */
 rxs->shvb.shvvalue.strlength= strlen(rxs->ibuf);
 rxs->shvb.shvvaluelen       = strlen(rxs->ibuf);
 rxs->shvb.shvcode           = RXSHV_SET; /* Set value request      */
 rxs->shvb.shvret            = 0;

 if (RexxVariablePool(&rxs->shvb) == RXSHV_BADN)
 {
  return INVALID_ROUTINE;                 /* error on non-zero      */
 }

 return VALID_ROUTINE;       /* Raise no error                      */

}

/*********************************************************************/
/* Get Spl device enumeration                                        */
/*********************************************************************/
ULONG getSplEnumDevice(PULONG ulRcode, PSZ pszSrvName,
                       RXSTEMDATA1 * rxs, ULONG ulStat)
{

 UCHAR  szWorkBuf[4096];            /* Working charater buffer       */
 UCHAR  szStatusBuf[80];            /* Working status buffer         */
 PBYTE  pbBuf;                      /* Pointer to buffer             */
 ULONG  ulReturned;                 /* Number of entries returned    */
 ULONG  ulTotal;                    /* Number of total entries       */
 ULONG  ulNeeded;                   /* Bytes needed to hold info     */
 ULONG  ulBufSize;                  /* Buffer size                   */
 ULONG  i;                          /* Loop variable                 */
 SPLERR splerr;                     /* rc from SplEnumDevice()       */
 PPRDINFO3 pprd3 ;                  /* Structure to hold print       */
                                    /* device information            */
 ULONG  ulRc;                       /* Temporary return code         */
 USHORT fstatus;

 /* Determine the size of the buffer to hold the information */
 splerr = SplEnumDevice(pszSrvName,
                        LEVEL_3,
                        pbBuf,
                        0L,                  /* Size of the buffer */
                        &ulReturned,
                        &ulTotal,
                        &ulNeeded,
                        NULL) ;              /* Reserved           */

 /* If ERROR_MORE_DATA or NERR_BufTooSmall continue */
 if( (splerr == ERROR_MORE_DATA) || (splerr == NERR_BufTooSmall) )
 {
  if(ulNeeded == 0)
  {
   /* Does not know the size required. Take max times ulTotal */
   ulBufSize = sizeof(PRDINFO3) + PRINTERNAME_SIZE + UNLEN + PDLEN + \
               2 * MAXCOMMENTSZ + 18 * DRIV_DEVICENAME_SIZE + 256;
   ulNeeded = ulTotal * ulBufSize;
  }

  pbBuf = malloc(ulNeeded);
  ulBufSize = ulNeeded ;

  /* Get the information available */
  splerr = SplEnumDevice(pszSrvName,
                         LEVEL_3,
                         pbBuf,
                         ulBufSize,
                         &ulReturned,
                         &ulTotal,
                         &ulNeeded,
                         NULL);

  /* If no errors, return the information available into stem variable */
  if(splerr == NO_ERROR)
  {
   for (i=0;i < ulReturned; i++)
   {
    pprd3 = (PPRDINFO3) pbBuf + i;      /* Point to the info structure */
    if(ulStat == 1)
    {
     sprintf(szStatusBuf, "No status available");

     fstatus = PRD_STATUS_MASK & pprd3->fsStatus;

     if( (fstatus & PRD_ACTIVE) == PRD_ACTIVE)
      sprintf(szStatusBuf, "Processing");
     if( (fstatus & PRD_PAUSED) == PRD_PAUSED)
      sprintf(szStatusBuf, "Not processing, or paused");

     fstatus = PRJ_DEVSTATUS & pprd3->fsStatus;

     if( (fstatus & PRJ_COMPLETE) == PRJ_COMPLETE)
      sprintf(szStatusBuf, "Job complete");
     if( (fstatus & PRJ_INTERV) == PRJ_INTERV)
      sprintf(szStatusBuf, "Intervention required");
     if( (fstatus & PRJ_ERROR) == PRJ_ERROR)
      sprintf(szStatusBuf, "Error occurred");
     if( (fstatus & PRJ_DESTOFFLINE) == PRJ_DESTOFFLINE)
      sprintf(szStatusBuf, "Print device offline");
     if( (fstatus & PRJ_DESTPAUSED) == PRJ_DESTPAUSED)
      sprintf(szStatusBuf, "Print device paused");
     if( (fstatus & PRJ_NOTIFY) == PRJ_NOTIFY)
      sprintf(szStatusBuf, "Raise alert");
     if( (fstatus & PRJ_DESTNOPAPER) == PRJ_DESTNOPAPER)
      sprintf(szStatusBuf, "Print device out of paper");
     if(strlen(pprd3->pszLogAddr) == 0)
     {
     sprintf(szWorkBuf, "Logical_Address: -none-   Status: %s", szStatusBuf);
     }
     else
     {
     sprintf(szWorkBuf, "Logical_Address: %8s Status: %s",
       pprd3->pszLogAddr, szStatusBuf);
     }
    }
    else
    {
     if(strlen(pprd3->pszLogAddr) == 0)
     {
     sprintf(szWorkBuf,
      "PrinterName: %s Logical_Address: -none-   Status: %X Drivers_supported: %s",
       pprd3->pszPrinterName,
       pprd3->fsStatus, pprd3->pszDrivers);
     }
     else
     {
     sprintf(szWorkBuf,
      "PrinterName: %s Logical_Address: %8s Status: %X Drivers_supported: %s",
       pprd3->pszPrinterName, pprd3->pszLogAddr,
       pprd3->fsStatus, pprd3->pszDrivers);
     }
    }

    ulRc = insertStem(rxs, szWorkBuf);

    if( ulRc != VALID_ROUTINE)
    {
     free(pbBuf);
     return INVALID_ROUTINE;
    }

   } /* end for */
  } /* if(splerr == NO_ERROR) */

  free(pbBuf);

 } /* end if( (splerr == ERROR_MORE_DATA) || */

 *ulRcode = splerr;

 ulRc = lastInStem(rxs);
 if( ulRc != VALID_ROUTINE)
  return INVALID_ROUTINE;

 return VALID_ROUTINE;                   /* OK!  */

}

/*********************************************************************/
/* Get Servers enumerated                                            */
/*********************************************************************/
ULONG getServerEnum2(PULONG ulRcode, PSZ pszDomain,
                     RXSTEMDATA1 * rxs, ULONG ulSrvType)
{

 ULONG  ulRc;                       /* Temporary return code         */
 PBYTE  pbBuf;                      /* Pointer to buffer             */
 USHORT usBufLen;                   /* Buffer length                 */
 USHORT usEntriesRead;              /* # entries returned in buffer  */
 USHORT usTotalEntries;             /* Total # entries               */
 ULONG  i;                          /* Loop variable                 */
 struct server_info_0 * pSrvInfo0;  /* Server info 0 pointer         */
 UCHAR  szBuf[64];                  /* Working buffer                */

 usBufLen = 0;
 ulRc = NetServerEnum2("", LEVEL_0, pbBuf, usBufLen, &usEntriesRead,
                       &usTotalEntries, ulSrvType, pszDomain);

 /* If ERROR_MORE_DATA or NERR_BufTooSmall continue */
 if( (ulRc == ERROR_MORE_DATA) || (ulRc == NERR_BufTooSmall) )
 {
  usBufLen = usTotalEntries * sizeof(struct server_info_0);
  pbBuf = malloc(usBufLen);
  ulRc = NetServerEnum2("", LEVEL_0, pbBuf, usBufLen, &usEntriesRead,
                        &usTotalEntries, ulSrvType, pszDomain);
  if(ulRc == NO_ERROR)
  {
   for (i=0;i < usEntriesRead; i++)
   {
    /* Point to the info structure */
    pSrvInfo0 = (struct server_info_0 *) pbBuf + i;
    sprintf(szBuf, "\\\\%s", pSrvInfo0->sv0_name);

    ulRc = insertStem(rxs, szBuf);
    if( ulRc != VALID_ROUTINE)
    {
     free(pbBuf);
     return INVALID_ROUTINE;
    }

   } /* end for( ;; ) */
  } /* end  if(ulRc == NO_ERROR) */

  free(pbBuf);

 }

 *ulRcode = ulRc;                        /* Set return value */

 ulRc = lastInStem(rxs);                 /* Make stem variable ready */
 if( ulRc != VALID_ROUTINE)
  return INVALID_ROUTINE;

 return VALID_ROUTINE;                   /* OK!  */

}

/*********************************************************************/
/* Get Applications enumerated                                       */
/*********************************************************************/
ULONG getAppEnum(PULONG ulRcode, PSZ pszDcName, RXSTEMDATA1 * rxs,
                 ULONG ulAppType)
{

 struct user_info_0 * pUserInfo0;   /* User information, level 0 */
 struct app_info_0  * pAppInfo0;    /* App information, level 0  */

 ULONG  ulRc;
 USHORT usUserList, usBuflen;       /* Memory structure sizes    */
 USHORT uswrite, usRc;
 USHORT usEntriesRead, usTotalAvail; /* Number of entries read and */
                                    /* the total number */
 PBYTE  pbBuf;                      /* Pointer to buffer             */
 INT    i, j;                       /* Loop counters */
 UCHAR  szBuf[64];                  /* Working buffer  */
 USHORT usUsrEntriesRead;           /* Number of user Ids read */

 switch(ulAppType)
 {
  case PUBDOS:
  case PUBOS2:
   /* Determine the number of entries */
   usBuflen = 0;
   ulRc = NetAppEnum (pszDcName, NULL, LEVEL_0, (USHORT) ulAppType,
                      pbBuf, usBuflen, &usEntriesRead, &usTotalAvail);

   /* If ERROR_MORE_DATA or NERR_BufTooSmall continue */
   if( (ulRc == ERROR_MORE_DATA) || (ulRc == NERR_BufTooSmall) )
   {
    /* Get the entries */
    usBuflen = usTotalAvail * sizeof(struct app_info_0);
    pbBuf = malloc(usBuflen);
    ulRc = NetAppEnum (pszDcName, NULL, LEVEL_0, (USHORT) ulAppType,
                       pbBuf, usBuflen, &usEntriesRead, &usTotalAvail);

    if(ulRc == NO_ERROR)
    {
     for (i=0;i < usEntriesRead; i++)
     {
      /* Point to the info structure */
      pAppInfo0 = (struct app_info_0 *) pbBuf + i;

      if (ulAppType == PUBDOS)
       sprintf(szBuf, "%10s  Public DOS", pAppInfo0->app0_name);
      else
       sprintf(szBuf, "%10s  Public OS/2", pAppInfo0->app0_name);

      ulRc = insertStem(rxs, szBuf);
      if( ulRc != VALID_ROUTINE)
      {
       free(pbBuf);
       return INVALID_ROUTINE;
      }

     } /* end for( ;; ) */
    } /* end  if(ulRc == NO_ERROR) */

    free(pbBuf);
   }

   break;

  case PRIVATE:
   /* Get a list of users */
   usUserList = 3119 * sizeof(struct user_info_0); /*Get up to seg size*/
   pUserInfo0 = malloc(usUserList);        /* Get memory */
   ulRc = NetUserEnum(pszDcName,          /* The server name */
                      LEVEL_0,             /* Get just the names */
                      (char *) pUserInfo0, /* Place in buffer */
                      usUserList,          /* The buffer size */
                      &usUsrEntriesRead,   /* Number of entries read */
                      &usTotalAvail);   /* Number of entries available */

   if(ulRc == 0)
   {
    /* Obtain information for each user ID found */
    usBuflen = 1024 * sizeof(struct app_info_0);
    pbBuf = malloc(usBuflen);

    for(i=0; i < usUsrEntriesRead; i++)
    {
     ulRc = NetAppEnum (pszDcName, pUserInfo0->usri0_name, LEVEL_0,
                        (USHORT) ulAppType, pbBuf, usBuflen,
                        &usEntriesRead, &usTotalAvail);

     if(ulRc == NO_ERROR)
     {
      for (j=0;j < usEntriesRead; j++)
      {
       /* Point to the info structure */
       pAppInfo0 = (struct app_info_0 *) pbBuf + j;

       sprintf(szBuf, "%10s  Private, UserID= %8s", \
                      pAppInfo0->app0_name, \
                      pUserInfo0->usri0_name);

       ulRc = insertStem(rxs, szBuf);
       if( ulRc != VALID_ROUTINE)
       {
        free(pbBuf);
        free(pUserInfo0);
        return INVALID_ROUTINE;
       }

      } /* end for(j ;; ) */
     } /* end  if(ulRc == NO_ERROR) */
     else
     {
      if( (ulRc != ERROR_FILE_NOT_FOUND) && (ulRc != ERROR_PATH_NOT_FOUND) )
      {
       sprintf(szBuf,"%d Error obtaining Applcation info for %8s", ulRc, pUserInfo0->usri0_name);
       ulRc = insertStem(rxs, szBuf);
       if( ulRc != VALID_ROUTINE)
       {
        free(pbBuf);
        free(pUserInfo0);
        return INVALID_ROUTINE;
       }
      }
     }
     pUserInfo0++;
    } /* end for(i=0; i < usEntriesRead; i++) */

    free(pUserInfo0);

   }
   free(pbBuf);
   break;

  default:
   *ulRcode = NERR_AppNotFound;
   return VALID_ROUTINE;
   break;
 }

 if( (ulRc == ERROR_FILE_NOT_FOUND) || (ulRc == ERROR_PATH_NOT_FOUND) )
  ulRc = 0;   /* Make a good return here */

 *ulRcode = ulRc;

 ulRc = lastInStem(rxs);                 /* Take care of stem variable */
 if( ulRc != VALID_ROUTINE)
  return INVALID_ROUTINE;

 return VALID_ROUTINE;                   /* OK!  */

}

/*********************************************************************/
/* Get Applications enumerated and get access control profile        */
/*********************************************************************/
ULONG getAppEnumAcp(PULONG ulRcode, PSZ pszDcName, PSZ pszUserId,
                    RXSTEMDATA1 * rxs)
{

 USHORT  usBuflen, usBytesAvail;
 UCHAR   szWorkBuf[80];              /* Working charater buffer       */
 UCHAR   szPathBuf[260];             /* Total path                    */
 ULONG   ulRc;
 struct  app_info_2 * pAppInfo2;
 struct  alias_info_2 * pAliasInfo2; /* Alias information, level 2 */
 INT     i;

 struct sel_info {
   struct app_sel_info_1 *pAsiBuf;
   struct app_sel_list *pAslBuf;
 } selPointers;

 usBuflen = sizeof(struct app_sel_info_1);
 selPointers.pAsiBuf = malloc(usBuflen);
 usBytesAvail = 0;

 ulRc = NetUserGetAppSel(pszDcName,            /* The DC   */
                         pszUserId,            /* The Userid */
                         LEVEL_1,              /* level_1  */
                         APP_ALL,              /* The type */
                         (char *) (selPointers.pAsiBuf),  /* The buffer */
                         usBuflen,             /* Size of the structure */
                         &usBytesAvail);
 free(selPointers.pAsiBuf);

 if( (ulRc == ERROR_FILE_NOT_FOUND) || (ulRc == ERROR_PATH_NOT_FOUND) )
 {
  /* User has nothing defined, set ending values, and end this call */
  *ulRcode = 0;
  strcpy(szWorkBuf, "A=     -none-");
  ulRc = insertStem(rxs, szWorkBuf);
  if( ulRc != VALID_ROUTINE)
    return INVALID_ROUTINE;

  ulRc = lastInStem(rxs);
  if( ulRc != VALID_ROUTINE)
    return INVALID_ROUTINE;

  return VALID_ROUTINE;     /* done */

 }

 /* There was information available */
 if( ulRc == NERR_BufTooSmall )
 {
  /* We got something. Allocate the correct size, obtain information */
  usBuflen = usBytesAvail + sizeof(struct app_sel_list);
  selPointers.pAsiBuf = malloc(usBuflen); /* For 1 additional   */
  ulRc = NetUserGetAppSel(pszDcName,             /* The DC     */
                          pszUserId,            /* The Userid */
                          LEVEL_1,              /* level_1    */
                          APP_ALL,              /* All types  */
                          (char *) (selPointers.pAsiBuf),  /* The buffer */
                          usBuflen,             /* Size of the structure */
                          &usBytesAvail);
  if( ulRc == 0 )
  {
   /* Point to the first */
   selPointers.pAslBuf = (struct app_sel_list *) \
   ((struct app_sel_info_1 *) selPointers.pAsiBuf + 1);

   usBuflen = sizeof(struct alias_info_2) + MAXCOMMENTSZ + \
              PATHLEN + (MAXDEVENTRIES * DEVLEN) + 128;
   pAliasInfo2 = malloc(usBuflen);

   usBuflen = sizeof(struct app_info_2) + MAXCOMMENTSZ + 4 * PATHLEN;
   pAppInfo2 = malloc(usBuflen);

   for(i=0; i < selPointers.pAsiBuf->asi1_count; i++)
   {
    /* Get each application defined and determine the ACPs */
    if(selPointers.pAslBuf->asl_apptype == APP_OS2_PRIVATE)
     ulRc = NetAppGetInfo(pszDcName, pszUserId,
                          selPointers.pAslBuf->asl_appname,
                          LEVEL_2, (char *) pAppInfo2,
                          usBuflen, &usBytesAvail);
    else
     ulRc = NetAppGetInfo(pszDcName, NULL,
                          selPointers.pAslBuf->asl_appname,
                          LEVEL_2, (char *) pAppInfo2,
                          usBuflen, &usBytesAvail);

    if(ulRc == 0)
    {
     /* Application path */
     if( strchr(pAppInfo2->app2_app_alias_or_drv, ':') == NULL)
     {
      /* Got an alias */
      if( pAppInfo2->app2_app_path_to_dir[0] != '\0')
       strcpy(szPathBuf, pAppInfo2->app2_app_path_to_dir);
      else
       szPathBuf[0] = '\0';

      ulRc=getAliasAcp(pszDcName, pAppInfo2->app2_app_alias_or_drv, \
                       pszUserId, rxs, pAliasInfo2, szPathBuf, AN_APP);

      if( ulRc != VALID_ROUTINE)
      {
       free(selPointers.pAsiBuf);
       free(pAliasInfo2);
       free(pAppInfo2);
       return INVALID_ROUTINE;
      }

     }
     else
     {
      /* Got a drive */
      sprintf(szWorkBuf,"A=     %8s Local drive", \
             selPointers.pAslBuf->asl_appname);

      ulRc = insertStem(rxs, szWorkBuf);

      if( ulRc != VALID_ROUTINE)
      {
       free(selPointers.pAsiBuf);
       free(pAliasInfo2);
       free(pAppInfo2);
       return INVALID_ROUTINE;
      }

     }
     /* Working directory path */
     if( strchr(pAppInfo2->app2_wrkdir_alias_or_drv, ':') == NULL)
     {
      /* Got an alias maybe */
      if ( (pAppInfo2->app2_wrkdir_alias_or_drv == NULL) || \
           (pAppInfo2->app2_wrkdir_alias_or_drv[0] == '\0') )
      {
       sprintf(szWorkBuf,"A=     %8s No working directory specified", \
              selPointers.pAslBuf->asl_appname);

       ulRc = insertStem(rxs, szWorkBuf);

       if( ulRc != VALID_ROUTINE)
       {
        free(selPointers.pAsiBuf);
        free(pAliasInfo2);
        free(pAppInfo2);
        return INVALID_ROUTINE;
       }

      }
      else
      {
       if( pAppInfo2->app2_wrkdir_path_to_dir[0] != '\0' )
        strcpy(szPathBuf, pAppInfo2->app2_wrkdir_path_to_dir);
       else
        szPathBuf[0] = '\0';

       ulRc=getAliasAcp(pszDcName, pAppInfo2->app2_wrkdir_alias_or_drv,
                        pszUserId, rxs, pAliasInfo2, szPathBuf, AN_APP);

       if( ulRc != VALID_ROUTINE)
       {
        free(selPointers.pAsiBuf);
        free(pAliasInfo2);
        free(pAppInfo2);
        return INVALID_ROUTINE;
       }

      }

     }
     else
     {
      /* Got a drive */
      sprintf(szWorkBuf,"A=     %8s Local drive", \
             selPointers.pAslBuf->asl_appname);

      ulRc = insertStem(rxs, szWorkBuf);

      if( ulRc != VALID_ROUTINE)
      {
       free(selPointers.pAsiBuf);
       free(pAliasInfo2);
       free(pAppInfo2);
       return INVALID_ROUTINE;
      }

     }

    } /* end if(ulRc == 0) */

    selPointers.pAslBuf++;

   } /* end for(i=0; i < selPointers.pAsiBuf->asi1_count; i++) */

   free(pAliasInfo2);
   free(pAppInfo2);

  } /* end if( usRc == 0 ) */

  free(selPointers.pAsiBuf);

 }

 if( (ulRc == ERROR_FILE_NOT_FOUND) || (ulRc == ERROR_PATH_NOT_FOUND) )
  ulRc = 0;   /* Make a good return here */

 *ulRcode = ulRc;

 ulRc = lastInStem(rxs);                 /* Take care of stem variable */
 if( ulRc != VALID_ROUTINE)
  return INVALID_ROUTINE;

 return VALID_ROUTINE;                   /* OK!  */

}

/*********************************************************************/
/* Get Alias access control profile                                  */
/*********************************************************************/
ULONG getLasnAliasAcp(PULONG ulRcode, PSZ pszDcName, PSZ pszUserId,
                      RXSTEMDATA1 * rxs)
{

/* struct alias_info_1 * pAliasInfo1;  Alias information, level 1    */
 struct alias_info_2 * pAliasInfo2; /* Alias information, level 2    */
 USHORT usBuflen;                   /* Memory structure sizes        */
 USHORT usBytesAvail;
 ULONG  ulRc;
 UCHAR  szWorkBuf[80];              /* Working charater buffer       */
 UCHAR  szDevBuf[128];              /* Working device buffer       */
 UCHAR  szAcpBuf[32];               /* Access control string buffer  */
 INT    i;
 USHORT usEntriesread;
 USHORT usTotalentries;

 struct asn_info {
   struct logon_asn_info_1 *pLaiBuf;
   struct logon_asn_list *pLalBuf;
 } asnPointers;

 /* Logon assignments */
 usBuflen = sizeof(struct logon_asn_info_1);
 asnPointers.pLaiBuf = malloc(usBuflen);
 usBytesAvail = 0;
 ulRc = NetUserGetLogonAsn(pszDcName,            /* The DC   */
                           pszUserId,            /* The Userid */
                           LEVEL_1,              /* level_1  */
                           7,                    /* The type */
                           (char *) (asnPointers.pLaiBuf),  /* The buffer */
                           usBuflen,             /* Size of the structure */
                           &usBytesAvail);
 free(asnPointers.pLaiBuf);

 /* There was information available */
 if( ulRc == NERR_BufTooSmall )
 {
  /* Allocate the correct size, obtain information */
  usBuflen = usBytesAvail;
  asnPointers.pLaiBuf = malloc(usBuflen); /* For all information*/
  ulRc = NetUserGetLogonAsn(pszDcName,             /* The DC     */
                            pszUserId,             /* The Userid */
                            LEVEL_1,              /* level_1    */
                            7,                    /* All types  */
                            (char *) (asnPointers.pLaiBuf),  /* The buffer */
                            usBuflen,             /* Size of the structure */
                            &usBytesAvail);
  if( ulRc != 0 )
  {
   free(asnPointers.pLaiBuf);
   *ulRcode = ulRc;
  }
  else
  {
   /* Work with the information provided. */
   asnPointers.pLalBuf = (struct logon_asn_list *) \
   ((struct logon_asn_info_1 *) asnPointers.pLaiBuf + 1);
   /* Get storage for alias_info_2 structure */
   usBuflen = sizeof(struct alias_info_2) + MAXCOMMENTSZ + \
              PATHLEN + (MAXDEVENTRIES * DEVLEN) + 128;
   pAliasInfo2 = malloc(usBuflen);

   for(i=0; i < asnPointers.pLaiBuf->lai1_count; i++)
   {
    ulRc=getAliasAcp(pszDcName, asnPointers.pLalBuf->lal_alias, \
                     pszUserId, rxs, pAliasInfo2, NULL, 0);

    if( ulRc != VALID_ROUTINE)
    {
     free(asnPointers.pLaiBuf);
     return INVALID_ROUTINE;
    }
    asnPointers.pLalBuf++;

   } /* end for(i=0; i < asnPointers.pLaiBuf->lai1_count; i++) */

   free(pAliasInfo2);

  } /* end if( ulRc != 0 ) */

 free(asnPointers.pLaiBuf);

 } /* end if( ulRc == NERR_BufTooSmall ) */

 if( (ulRc == ERROR_FILE_NOT_FOUND) || (ulRc == ERROR_PATH_NOT_FOUND) )
  ulRc = 0;   /* Make a good return here */

 *ulRcode = ulRc;

 ulRc = lastInStem(rxs);
 if( ulRc != VALID_ROUTINE)
  return INVALID_ROUTINE;

 return VALID_ROUTINE;

}

/*********************************************************************/
/* Set access control permission to a string                         */
/*********************************************************************/
VOID setAcp(PSZ pszAcpBuf, USHORT usPermission)
{
 INT i;

 i = 0;
 if(usPermission == ACCESS_NONE)
  pszAcpBuf[i++] = 'N';

 if( (usPermission & ACCESS_READ) == ACCESS_READ)
  pszAcpBuf[i++] = 'R';

 if( (usPermission & ACCESS_WRITE) == ACCESS_WRITE)
  pszAcpBuf[i++] = 'W';

 if( (usPermission & ACCESS_CREATE) == ACCESS_CREATE)
  pszAcpBuf[i++] = 'C';

 if( (usPermission & ACCESS_EXEC) == ACCESS_EXEC)
  pszAcpBuf[i++] = 'X';

 if( (usPermission & ACCESS_DELETE) == ACCESS_DELETE)
  pszAcpBuf[i++] = 'D';

 if( (usPermission & ACCESS_PERM) == ACCESS_PERM)
  pszAcpBuf[i++] = 'P';

 if (i == 0)
  strcpy(pszAcpBuf, "Not Defined");
 else
  pszAcpBuf[i] = '\0';

}

/*********************************************************************/
/* Get alias access control profile                                  */
/*********************************************************************/
ULONG getAliasAcp(PSZ pszDcName, PSZ pszAlias, PSZ pszUserId,
                  RXSTEMDATA1 * rxs, struct alias_info_2 * pAliasInfo2,
                  PSZ pszPath, ULONG ulType)

{

 USHORT usBuflen, usBytesAvail, usPermission;
 ULONG  ulRc;
 UCHAR  szWorkBuf[80];              /* Working charater buffer       */
 UCHAR  szDevBuf[128];              /* Working device buffer       */
 UCHAR  szAcpBuf[32];               /* Access control string buffer  */
 UCHAR  szPathBuf[260];             /* Path buffer                 */

 usBuflen = sizeof(struct alias_info_2) + MAXCOMMENTSZ + \
            PATHLEN + (MAXDEVENTRIES * DEVLEN) + 128;
 /* Obtain alias information */
 ulRc = NetAliasGetInfo(pszDcName,
                        pszAlias,
                        LEVEL_2,
                        (char *) pAliasInfo2,
                        usBuflen,
                        &usBytesAvail);
 if( ulRc != 0 )
 {
  sprintf(szWorkBuf,"D=     %8s  Error getting alias information", pszAlias);
  ulRc = insertStem(rxs, szWorkBuf);

  if( ulRc != VALID_ROUTINE)
   return INVALID_ROUTINE;

 }
 else
 {
  /* Get the location. Can be external or internal */
  if(pAliasInfo2->ai2_location != ALIAS_LOCATION_INTERNAL)
  {
   if(ulType == AN_APP)
    sprintf(szWorkBuf,"A=     %8s External resource", pszAlias);
   else
    sprintf(szWorkBuf,"D=     %8s External resource", pszAlias);

   ulRc = insertStem(rxs, szWorkBuf);
   if( ulRc != VALID_ROUTINE)
    return INVALID_ROUTINE;

  }
  else
  {
   /* Need to get the access control profile. Obtain type */
   usPermission = -1;
   if(pAliasInfo2->ai2_type == ALIAS_TYPE_FILE)
   {

    strcpy(szPathBuf, pAliasInfo2->ai2_path);
    if(pszPath != NULL)
     if( (pszPath[0] != '\0') && (pszPath[0] != '\\') )
      strcat(szPathBuf, pszPath);

    ulRc = NetAccessGetUserPerms(pAliasInfo2->ai2_server,
                                 pszUserId,             /* The Userid */
                                 szPathBuf,
                                 &usPermission);
    if(ulRc == 0)
    {
     setAcp(szAcpBuf, usPermission);
     if(ulType == AN_APP)
      sprintf(szWorkBuf,"A=     %8s  %16s", pszAlias, szAcpBuf);
     else
      sprintf(szWorkBuf,"D=     %8s  %16s", pszAlias, szAcpBuf);
    }
    else
     if(ulType == AN_APP)
      sprintf(szWorkBuf,"A=     %8s  Error obtaining ACP", pszAlias);
     else
      sprintf(szWorkBuf,"D=     %8s  Error obtaining ACP", pszAlias);

    ulRc = insertStem(rxs, szWorkBuf);
    if( ulRc != VALID_ROUTINE)
     return INVALID_ROUTINE;

   } /* end if(pAliasInfo2->ai2_type == ALIAS_TYPE_FILE) */

   if(pAliasInfo2->ai2_type == ALIAS_TYPE_PRINTER)
   {
    strcpy(szDevBuf, "\\PRINT\\");
    strcat(szDevBuf, pAliasInfo2->ai2_queue);

    ulRc = NetAccessGetUserPerms(pAliasInfo2->ai2_server,
                                 pszUserId,             /* The Userid */
                                 szDevBuf,
                                 &usPermission);
    if(ulRc == 0)
    {
     setAcp(szAcpBuf, usPermission);
     if(ulType == AN_APP)
      sprintf(szWorkBuf,"A=     %8s  %16s", pszAlias, szAcpBuf);
     else
      sprintf(szWorkBuf,"D=     %8s  %16s", pszAlias, szAcpBuf);
    }
    else
     if(ulType == AN_APP)
      sprintf(szWorkBuf,"A=     %8s  Error obtaining ACP", pszAlias);
     else
      sprintf(szWorkBuf,"D=     %8s  Error obtaining ACP", pszAlias);

    ulRc = insertStem(rxs, szWorkBuf);
    if( ulRc != VALID_ROUTINE)
     return INVALID_ROUTINE;

   } /* end if(pAliasInfo2->ai2_type == ALIAS_TYPE_PRINTER) */

   if(pAliasInfo2->ai2_type == ALIAS_TYPE_SERIAL)
   {
    strcpy(szDevBuf, "\\COMM\\");
    strcat(szDevBuf, pAliasInfo2->ai2_queue);

    ulRc = NetAccessGetUserPerms(pAliasInfo2->ai2_server,
                                 pszUserId,             /* The Userid */
                                 szDevBuf,
                                 &usPermission);

    if(ulRc == 0)
    {
     setAcp(szAcpBuf, usPermission);
     if(ulType == AN_APP)
      sprintf(szWorkBuf,"A=     %8s  %16s", pszAlias, szAcpBuf);
     else
      sprintf(szWorkBuf,"D=     %8s  %16s", pszAlias, szAcpBuf);
    }
    else
     if(ulType == AN_APP)
      sprintf(szWorkBuf,"A=     %8s  Error obtaining ACP", pszAlias);
     else
      sprintf(szWorkBuf,"D=     %8s  Error obtaining ACP", pszAlias);

    ulRc = insertStem(rxs, szWorkBuf);
    if( ulRc != VALID_ROUTINE)
     return INVALID_ROUTINE;

   } /* end if(pAliasInfo2->ai2_type == ALIAS_TYPE_SERIAL) */

  } /* end if(pAliasInfo2->ai2_location != ALIAS_LOCATION_INTERNAL) */

 } /* if( ulRc != 0 ) */

 return VALID_ROUTINE;

}
