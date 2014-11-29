/************   OS/2 Application Primer Sample   **********************/
/*                                                                    */
/* Module Name : NLSSUB.C                                             */
/* Version : 2.1 (12/24/93)                                           */
/*                                                                    */
/*    << This sample program is only for OS/2 V2.x. >>                */
/*                                                                    */
/* A sample program for "OS/2 DBCS Application Primer".               */
/* Subroutines for DBCS and NLS functions.                            */
/*                                                                    */
/**********************************************************************/
#define    INCL_NOCOMMON
#define    INCL_DOSNLS
#define    INCL_DOS
#define    INCL_NLS

#define    INCL_NLS_INFO
#define    INCL_SHLERRORS
#define    INCL_WINSHELLDATA

#include   <os2.h>
#include   <stdlib.h>
#include   <stdio.h>
#include   <string.h>              // STRING.H now includes MemXXX
#include   <time.h>
#include   "ordent.h"
#include   "dbcs.h"

COUNTRYINFO   ctryInfo;
USHORT flDbcsCp;

/*--------------------------------------------------------------------*/
/*  Table to identify if one byte data is in the range of             */
/*  DBCS 1st byte of the current environment.                         */
/*    If c is in the 1st byte range dbcsTable[c] == DBCS_1ST (== 1)   */
/*    else                          dbcsTable[c] == SBCS     (== 0)   */
/*--------------------------------------------------------------------*/
static USHORT dbcsTable[256];

USHORT InitDBCSTable( void );
USHORT isDBCS1st( UCHAR );
USHORT getCharType( UCHAR *, USHORT );
USHORT dbcsStrValidate( UCHAR *, USHORT *, USHORT );


/**********************************************************************/
/*    Set up a DBCS table with the DBCS environmental vector.         */
/**********************************************************************/
USHORT InitDBCSTable()
{
  UCHAR  dbcsEv[12];
  USHORT i, j;
  COUNTRYCODE ctryCode;

  /*--- Initialization of variables --------------------------*/
  ctryCode.country = 0;
  ctryCode.codepage = 0;
  for (i = 0; i < 256; i++) dbcsTable[i] = SBCS;

  /*--- System call to obtain the DBCS environmental vector --*/
  DosQueryDBCSEnv(12, &ctryCode, dbcsEv);

  for (j = 0; dbcsEv[j] | dbcsEv[j + 1]; j += 2)
    for (i = dbcsEv[j]; i <= dbcsEv[j + 1]; i++)
      dbcsTable[i] = DBCS_1ST;
  /*----------------------------------------------------------*/
  /*   Returns DBCS_CP if the DBCS 1st byte range is defined, */
  /*   else returns NON_DBCS_CP.                              */
  /*----------------------------------------------------------*/
  return flDbcsCp = (dbcsEv[0] | dbcsEv[1] ? DBCS_CP : NON_DBCS_CP);
}


/**********************************************************************/
/*    Test if a given byte is in the range of the DBCS 1st byte.      */
/*    If DBCS_1ST is returned, the byte is either a DBCS 1st byte or  */
/*    a DBCS 2nd byte. If SBCS is returned, the byte is either SBCS   */
/*    or a DBCS 2nd byte.                                             */
/*    In order to identify characteristics of a byte in a string,     */
/*    need to parse the string from the beginning.                    */
/**********************************************************************/
USHORT isDBCS1st(UCHAR aCode)
{
  return dbcsTable[aCode];
}


/**********************************************************************/
/*    Identify characteristics of the N-th byte of the given string   */
/*    by parsing the string from the beginning.                       */
/*    The characteristics (SBCS, DBCS 1st, DBCS 2nd) depends on the   */
/*    characteristics of the previous byte. If the previous byte is   */
/*    DBCS_1st, the byte examined now is DBCS_2nd.                    */
/**********************************************************************/
USHORT getCharType(UCHAR string[], USHORT n)
{
  USHORT i, typeCurrent;

  typeCurrent = SBCS;
  for (i = 0; i <= n; i++) {
    switch(typeCurrent) {
      case SBCS:
      case DBCS_2ND:
        typeCurrent = dbcsTable[string[i]];
        break;

      case DBCS_1ST:
        typeCurrent = DBCS_2ND;
        break;
    }
  }
  return typeCurrent;
}


/**********************************************************************/
/*    Check if a string terminator(0x00) is placed after a DBCS       */
/*    1st byte. If so, the string should be handled as an invalid     */
/*    mixed string and the DBCS 1st byte before the terminator should */
/*    be removed when the MODIFY flag(flagMod) is TRUE.               */
/**********************************************************************/
USHORT dbcsStrValidate(UCHAR string[], USHORT *pIndex, USHORT flagMod)
{
  USHORT i;

  /*--- Go unil string terminator ----------------------------*/
  for (i = 0; string[i] != 0x00; i++);

  /*--- Check if a string terminator is the 2nd byte of DBCS -*/
  if (getCharType(string, i) != DBCS_2ND) return TRUE;
  else {
    memcpy(pIndex, &i, sizeof(USHORT));
    /*- If the modify flag is TRUE, remove the last one byte -*/
    if (flagMod == TRUE) string[i - 1] = 0x00;
    return FALSE;
  }
}


/**********************************************************************/
/*   Fill out a structure "ctryInfo" with items of country information*/
/*   specified in a profile and/or returned from a system call        */
/*   DosGetCtryInfo. When the same item is available from both        */
/*   sources, the profile takes the precedence.                       */
/**********************************************************************/
SHORT InitCtryInfo(void)
{
  COUNTRYCODE ctryCode;
  ULONG       ulCInfLength;
  CHAR        strBuf[20];
  LONG        intBuf;

  /*------ Initialize variables for a system call --------------*/
  ctryCode.country = 0;
  ctryCode.codepage = 0;

  /*------ System call to get country information ------------*/
  DosQueryCtryInfo(sizeof(ctryInfo), &ctryCode, &ctryInfo, &ulCInfLength);

  /*------------------------------------------------------------*/
  /*  Query a profile about a country code, a date format,      */
  /*  a date/time separator, a currecy format/symbol, a decimal */
  /*  place/charcter, a thousand separator and a data separator,*/
  /*  and if available, those override DosGetCtryInfo data.     */
  /*------------------------------------------------------------*/
  if (0L != (intBuf=PrfQueryProfileInt(HINI_PROFILE, "PM_National",
                                       "iCountry", 0L )))
     ctryInfo.country = (ULONG)intBuf;
  if (0L != (intBuf=PrfQueryProfileInt(HINI_PROFILE, "PM_National",
                                       "iDate", 0L)))
     ctryInfo.fsDateFmt = (ULONG)intBuf;
  if (0L != (intBuf=PrfQueryProfileInt(HINI_PROFILE, "PM_National",
                                       "iCurrency", 0L)))
     ctryInfo.fsCurrencyFmt = (UCHAR)((ctryInfo.fsCurrencyFmt && 0x00FE) ||
                                      (intBuf && 0x0001));
  if (0L != (intBuf=PrfQueryProfileInt(HINI_PROFILE, "PM_National",
                                       "iDigits", 0L)))
     ctryInfo.cDecimalPlace = (UCHAR)intBuf;
  PrfQueryProfileString(HINI_PROFILE, "PM_National", "sCurrency",
                        NULL, strBuf, (ULONG)sizeof(strBuf));
  if (strBuf[0] != '\0') strcpy(ctryInfo.szCurrency, strBuf);
  PrfQueryProfileString(HINI_PROFILE, "PM_National", "sThousand",
                        NULL, strBuf, (ULONG)sizeof(strBuf));
  if (strBuf[0] != '\0') strcpy(ctryInfo.szThousandsSeparator, strBuf);
  PrfQueryProfileString(HINI_PROFILE, "PM_National", "sDecimal",
                        NULL, strBuf, (ULONG)sizeof(strBuf));
  if (strBuf[0] != '\0') strcpy(ctryInfo.szDecimal, strBuf);
  PrfQueryProfileString(HINI_PROFILE, "PM_National", "sDate",
                        NULL, strBuf, (ULONG)sizeof(strBuf));
  if (strBuf[0] != '\0') strcpy(ctryInfo.szDateSeparator, strBuf);
  PrfQueryProfileString(HINI_PROFILE, "PM_National", "sTime",
                        NULL, strBuf, (ULONG)sizeof(strBuf));
  if (strBuf[0] != '\0') strcpy(ctryInfo.szTimeSeparator, strBuf);
  PrfQueryProfileString(HINI_PROFILE, "PM_National", "sList",
                        NULL, strBuf, (ULONG)sizeof(strBuf));
  if (strBuf[0] != '\0') strcpy(ctryInfo.szDataSeparator, strBuf);

  return SUCCESS;
}


/***********************************************************************/
/*   Retrieve a currency symbol from the country information structure */
/***********************************************************************/
SHORT GetCurrency(CHAR *pChar, SHORT sLength)
{
  if (strlen(ctryInfo.szCurrency) < sLength) {
    strcpy(pChar, ctryInfo.szCurrency);
    return SUCCESS;
  } else return FAIL;
}


/**********************************************************************/
/*   Insert characters "aCahr" to the head of the string "strDat" and */
/*   make length of string as "cols"                                  */
/**********************************************************************/
SHORT PadCharacter(CHAR strDat[], CHAR aChar, SHORT cols)
{
  SHORT i;

  if (cols < (i = strlen(strDat)))
    return FAIL;
  strDat[cols--] = '\0';
  for (i--; 0 <= i;)
    strDat[cols--] = strDat[i--];
  for (; 0 <= cols;)
    strDat[cols--] = aChar;
  return SUCCESS;
}


/**********************************************************************/
/*   Format an integer string with the country inforamtion data.      */
/**********************************************************************/
SHORT FormatInteger(CHAR strBuf[])
{
  USHORT  i, j;
  CHAR    wkArea[20];

  StrReverse(strBuf);
  /*----------------------------------------------------------*/
  /*   Inesert a thousands separator every three characters   */
  /*----------------------------------------------------------*/
  for (i=0, j=0; i < strlen(strBuf);){
    wkArea[j++] = strBuf[i++];

    /*-----------------------------------------------------------*/
    /*  Conditions to insert a separator are...                  */
    /*   the current position is followed by a block of 3 chars, */
    /*   and is not the head of the entire string,               */
    /*   and is not preceded by a '-' character.                 */
    /*-----------------------------------------------------------*/
    if ( ((j + 1) % 4  == 0) &
         (strBuf[i] != '-')  &
         (i < strlen(strBuf)) )
      wkArea[j++] = ctryInfo.szThousandsSeparator[0];
  }
  wkArea[j] = '\0';
  StrReverse(wkArea);
  strcpy(strBuf, wkArea);
  return j;
}


/**********************************************************************/
/*   Insert a date separator to the formated date                     */
/**********************************************************************/
static SHORT InsertDateSep(CHAR* strBuf)
{
  CHAR *pw, *ps, *pwork;
  SHORT i;

  pw = pwork = malloc(strlen(strBuf) + 3);
  ps = strBuf;

  for (i = 1; i <= 3; i++, pw++, ps += 2){
    memcpy(pw, ps, 2);
    pw += 2;
    if (i == 3)
      break;
    *pw = ctryInfo.szDateSeparator[0];
  }

  *pw = '\0';
  strcpy(strBuf, pwork);

  return SUCCESS;
}


/**********************************************************************/
/*   Format a string of date. Change the order of year, month, date   */
/*   of the string to YYMMDD, following the system information.       */
/*   Then insert an appropriate date separator. If the result is too  */
/*   long, return FAIL.                                               */
/**********************************************************************/
SHORT FormatDate( CHAR strDate[], SHORT sLength )
{
  CHAR strBuf[20], wkArea[2];
  CHAR *ptr;

  strcpy(strBuf, &strDate[2]);
  ptr = strBuf;
  switch (ctryInfo.fsDateFmt) {
    case YYMMDD:
      break;

    case DDMMYY:
      ptr += 4;
      memcpy(wkArea, strBuf, 2);
      memcpy(strBuf, ptr, 2);
      memcpy(ptr, wkArea, 2);
      break;

    case MMDDYY:
      ptr += 2;
      memcpy(wkArea, strBuf, 2);
      memcpy(strBuf, ptr, 4);
      ptr += 2;
      memcpy(ptr, wkArea, 2);
      break;

    default:
      return FAIL;
  }
  InsertDateSep(strBuf);
  if (strlen(strBuf) < sLength) {
    strcpy(strDate, strBuf);
    return SUCCESS;
  } else return FAIL;
}


/**********************************************************************/
/*   Get date in the form YYMMDD by a system call                     */
/**********************************************************************/
void GetDateYYYYMMDD(CHAR strYYYYMMDD[], SHORT maxlength)
{
  time_t now;
  struct tm *pnowtime;

  time(&now);
  pnowtime = localtime(&now);
  strftime(strYYYYMMDD, maxlength, "%Y%m%d", pnowtime);
  strYYYYMMDD[8] = 0x00;
}


/**********************************************************************/
/*   Get the date, following the system information                   */
/**********************************************************************/
SHORT GetDate(CHAR *szDate, SHORT length)
{
  CHAR  szBuf[20];
  struct tm *pnowtime;

  GetDateYYYYMMDD(szBuf, sizeof(szBuf));

  FormatDate(szBuf, sizeof(szBuf));
  if (strlen(szBuf) < length){
    strcpy(szDate, szBuf);
    return SUCCESS;
  } else return FAIL;
}


/**********************************************************************/
/*   Format a price, following the system information                 */
/**********************************************************************/
SHORT FormatPrice(CHAR szBuf[])
{
  int i, j;
  char wkArea[20];

  for (i=0; i<strlen(szBuf); i++)
    if ((szBuf[i] < '0') || ('9' < szBuf[i]))
      return FAIL;
  if (i == 0) return FAIL;
  if (strlen(szBuf) < (ctryInfo.cDecimalPlace + 1))
    PadCharacter(szBuf, '0', ctryInfo.cDecimalPlace + 1);
  for (i=0, j=0; i < strlen(szBuf) - ctryInfo.cDecimalPlace; i++, j++)
    wkArea[j] = szBuf[i];
  wkArea[j] = '\0';
  FormatInteger(wkArea);
  j = strlen(wkArea);
  if (0 < ctryInfo.cDecimalPlace){
    wkArea[j++] = ctryInfo.szDecimal[0];
    for (; i < strlen(szBuf); i++, j++)
      wkArea[j] = szBuf[i];
  }
  wkArea[j] = '\0';
  if ((ctryInfo.fsCurrencyFmt & CUR_PLACE) == CUR_BEFORE) {
    StrReverse(wkArea);
    if ((ctryInfo.fsCurrencyFmt & CUR_SPACE) == CUR_SP_YES)
      strcat(wkArea, " ");
    strcat(wkArea, ctryInfo.szCurrency);
    StrReverse(wkArea);
  } else {
    if (ctryInfo.fsCurrencyFmt & CUR_SPACE == CUR_SP_YES)
      strcat(wkArea, " ");
    strcat(wkArea, ctryInfo.szCurrency);
  }
  strcpy(szBuf, wkArea);
  return j;
}


/**********************************************************************/
/*   Get a country code from the country information                  */
/**********************************************************************/
USHORT GetCtryCode(void)
{
  return ctryInfo.country;
}


/**********************************************************************/
/*   Get Language ID matched with a country code from "DefLang.TBL"   */
/*   Format of DefLang.TBL is  xxx,yyy // any comment                 */
/*      where xxx is a 3-digit number of a country code and           */
/*            yyy is a 3-char string of a language ID                 */
/**********************************************************************/
USHORT GetDefLang(CHAR LangID[])
{
  USHORT c, ctryCode;
  CHAR   DefCtryCode[4];
  FILE   *stream;

  ctryCode = GetCtryCode();
  stream = fopen("deflang.tbl","r");
  while (fscanf(stream, "%3s,", DefCtryCode) != EOF) {
    if (ctryCode == atoi(DefCtryCode)) {
      /*--- If a code page matches, read/put the lang. ID ---------*/
      fscanf(stream, "%3s", LangID);
      fclose(stream);
      return SUCCESS;
    } else {
      while ((c = fgetc(stream)) != '\n');
    }
  }
  fclose(stream);
  return FAIL;
}


/**********************************************************************/
/*   Check the language ID with the current code page                 */
/**********************************************************************/
USHORT CheckCodepage(CHAR LangID[])
{
  USHORT CpList[20], i, c;
  ULONG  dataLength, CurrentCpList[4];
  CHAR   strCp[6];
  FILE   *stream;

  /*--- Get CPs available for the Language ID ----------------*/
  GetCpList(LangID, CpList);
  DosQueryCp(sizeof(CurrentCpList), CurrentCpList, &dataLength);
  i=0;
  while (CpList[i] != 0) {
    if (CpList[i] == (USHORT)CurrentCpList[0])
       /*-------- If the current CP matches ------------------*/
       return SUCCESS;
    i++;
  }
  /*----------------------------------------------------------*/
  /*   If the current CP does not match any CPs available     */
  /*   for the Language ID, try to determin Language ID       */
  /*   based on the current code page.                        */
  /*   The format of DefCp.TBL is xxxxx,yyy // any comment    */
  /*      where xxxxx is a 5-digit number of code page ID and */
  /*            yyy is a 3-char string of Language ID         */
  /*----------------------------------------------------------*/
  stream = fopen("DefCp.TBL","r");
  while (fscanf(stream, "%5s,", strCp) != EOF) {
    if (CurrentCpList[0] == atoi(strCp)) {
      fscanf(stream, "%3s", LangID);
      fclose(stream);
      return SUCCESS;
      break;
    } else {
      while (c = fgetc(stream) != '\n');
    }
  }
  fclose(stream);
  return FAIL;
}


/**********************************************************************/
/*   Get CPs availabe to a language ID from the "LangCp.TBL"          */
/*   The format of LangCp.TBL is xxx(aaaaa,bbbbb, .....,ccccc)        */
/*      where xxx is a 3-char string of a Language ID and             */
/*            aaaaa, bbbbb,..., ccccc are 5-digit numbers of          */
/*            code page IDs available to the language ID xxxxx        */
/**********************************************************************/
USHORT GetCpList(CHAR LangID[], USHORT CpList[])
{
  FILE   *stream;
  CHAR   strCp[6], Lang[4];
  USHORT i, c;

  stream = fopen("LangCp.TBL","r");
  while(fscanf(stream, "%3s", Lang) != EOF) {
    if (strcmp(Lang, LangID) == 0) {
      i = 0;
      while (fgetc(stream) != ')') {
        fscanf(stream, "%5s", strCp);
        CpList[i] = atoi(strCp);
        i++;
      }
      CpList[i] = 0;
      fclose(stream);
      return SUCCESS;
    }
    while (c = fgetc(stream) != '\n');
  }
  fclose(stream);
  return FAIL;
}


/**********************************************************************/
/*   Reverse a given string as same as strrev does. strrev function   */
/*   is no longer included in ANSI C, SAA C Level 2 and C Set/2       */
/*   Standard libraries. C Set/2 supports strrev only in Migration    */
/*   library.                                                         */
/**********************************************************************/
USHORT StrReverse(CHAR szBuf[])
{
  CHAR *ptr1, *ptr2, c;

  ptr1 = szBuf;
  ptr2 = &szBuf[strlen(szBuf)-1];

  while (ptr1 < ptr2){
        c       = *ptr1;
        *ptr1++ = *ptr2;
        *ptr2-- = c;
  }
  return SUCCESS;
}
