/***************************************************************************/
/* XPRMDATA.C    XPG4 Primer for OS/2 WARP - V1.0       11/15/95           */
/*               Data handling functions including file I/O.               */
/***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys\stat.h>
#include <wchar.h>
#include <wcstr.h>
#include <locale.h>
#include <nl_types.h>
#include <langinfo.h>
#include <limits.h>

typedef unsigned short USHORT;
#include "xprmdata.h"

#define TRUE  1
#define FALSE 0

/*******************************/
/* Error code used in file I/O */
/*******************************/
#define ERR_FILE_OPEN          0x0001L
#define ERR_FILE_INVALID       0x0002L
#define ERR_FILE_IO            0x0003L
#define ERR_LOAD_RESOURCE      0x0004L

static const char* pszFnameProd = "PRODREC.DAT";
static const char* pszFnameCust = "CUSTREC.DAT";
static const char* pszFnameOrder = "ORDRREC.DAT";

static RECORDS prodRecs, custRecs;
static CUSTREC* pLastCust;
static RECORD* getRecordBlock( RECORD *pRec, size_t size, RECORD** ppTop);
static void freeRecordChain( RECORD *pTopRec );

typedef void *MPARAM;                  /* mp                               */
#define MPFROMP(p)                 ((MPARAM)((unsigned long)(p)))
extern void logError( ulong ulCode, MPARAM mp );

/*****************************************************************************/
/* readProdFile()                                                            */
/*      Reads the product records file and store the data into a chain of    */
/*      PRODREC pointed to by prodRecs.pTopRecord.  The locale name that     */
/*      the file was described in is stored in prodRecs.acLocale.            */
/*                                                                           */
/*      Returns a string of the locale name.                                 */
/*****************************************************************************/
char* readProdFile()
{
FILE* fh;
PRODREC *pProd;
Boolean flLocale=TRUE;
char acCharLocale[LOCALE_LEN];
wchar_t awcBuf[PROD_REC_MAXLEN+2];     /* 2 for '\n'+'\0'                  */
wchar_t *pToken, *ptr;
void getCharLocale( char *pszCharLocale );

   getCharLocale( acCharLocale );                                  /*110795*/

   if( (fh = fopen( pszFnameProd, "r" )) == NULL )
   {
      logError( ERR_FILE_OPEN, MPFROMP(pszFnameProd) );
      return NULL;
   }

   /* Read file locale name at first */
   if( fscanf( fh, "%s\n", prodRecs.acLocale ) != 1 )
   {
      logError( ERR_FILE_INVALID, MPFROMP(pszFnameProd) );
      return NULL;
   }

   /* check if the file locale is the same with the runtime locale */
   if( strcmpi( acCharLocale, prodRecs.acLocale ) != 0 )
   {
      flLocale = FALSE;
   }

   /* Initialize prodRecs members */
   pProd = NULL;
   /* start reading product records */
   while( fgetws( awcBuf, PROD_REC_MAXLEN+2, fh ) != NULL )
   {
      pProd = (PRODREC*)getRecordBlock( (RECORD*)pProd,
                                        sizeof(PRODREC),
                                        (RECORD**)&prodRecs.pTopRecord );

      pToken = wcstok( awcBuf, L"/", &ptr );
      if( pToken != NULL )  wcstombs( pProd->acNum, pToken, PROD_NUM_LEN );

      pToken = wcstok( NULL, L"/", &ptr );
      if( pToken != NULL && flLocale == TRUE )
      {
         wcsncpy( pProd->awcName, pToken, PROD_NAME_LEN-1 );      /* 110795*/
      }
      pToken = wcstok( NULL, L"/", &ptr );
      if( pToken != NULL && flLocale == FALSE )
      {
         wcsncpy( pProd->awcName, pToken, PROD_NAME_LEN-1 );      /* 110795*/
      }
      pToken = wcstok( NULL, L"\n", &ptr );
      if( pToken != NULL )  pProd->dUnitPrice = wcstod( pToken, NULL );

   }
   fclose( fh );
   return( prodRecs.acLocale );
}

/*****************************************************************************/
/* getProdRec()                                                              */
/*      Searches the desired product record(s).                              */
/*      If the pszKey is NULL, a pointer which points the top of a chain     */
/*      of whole product records is returned.  Otherwise, it searches the    */
/*      product(s) with the specified category and key word.  In this case,  */
/*      a pointer to a chain of matched product records is returned.  If no  */
/*      record is found, NULL is returned.                                   */
/*****************************************************************************/
PRODREC* getProdRec( char* pszKey, enum proditem e_category )
{
PRODREC *pSrcProd;
static PRODREC* pProdFound;

   if( pszKey == NULL )                    /* requested all product records*/
   {
     return (PRODREC*)prodRecs.pTopRecord;
   }
   else                                  /* query specified product records*/
   {
      if( pProdFound != NULL )               /* free previous query results*/
      {
          freeRecordChain( (RECORD*)pProdFound );
          pProdFound = NULL;
      }

      pSrcProd = (PRODREC*)prodRecs.pTopRecord;
      switch( e_category )
      {
         case prodnum:                                       /* exact match*/
           while( pSrcProd != NULL )
           {
              if( strcmp( pSrcProd->acNum, pszKey ) == 0 )  return pSrcProd;
              pSrcProd = pSrcProd->next;
           }
           return NULL;

         case prodname:                                   /* multiple match*/
         {
         wchar_t wcsKey[32];
         PRODREC *pProd = NULL;

           mbstowcs( wcsKey, pszKey, 32 );
           while( pSrcProd != NULL )
           {
              if( wcswcs( pSrcProd->awcName, wcsKey ) != NULL )
              {
                 pProd = (PRODREC*)getRecordBlock( (RECORD*)pProd,
                                                   sizeof(PRODREC),
                                                   (RECORD**)&pProdFound );
                 memcpy( pProd, pSrcProd, sizeof(PRODREC) );
                 pProd->next = NULL;              /* reset the next pointer*/
              }
              pSrcProd = pSrcProd->next;
           }
           return pProdFound;
         }
      }
      return NULL;
   }
}

/*****************************************************************************/
/* readCustFile()                                                            */
/*      Reads the customer records file and store the data into a chain of   */
/*      PRODREC pointed to by custRecs.pTopRecord.  The locale name that     */
/*      the file was described in is stored in custRecs.acLocale.            */
/*                                                                           */
/*      Returns a string of the locale name.                                 */
/*****************************************************************************/
char* readCustFile()
{
FILE* fh, *fhAlt;
CUSTREC *pCust;
Boolean flLocale=TRUE;
size_t len;
wchar_t wcs[CUST_ADDR_LEN+2];          /* +2= '\n' & '\0'                  */
char acCharLocale[LOCALE_LEN];
int iLineCnt;

   getCharLocale( acCharLocale );                                 /* 110795*/

   if( (fh = fopen( pszFnameCust, "r" )) == NULL )
   {
      logError( ERR_FILE_OPEN, MPFROMP(pszFnameCust) );
      return NULL;
   }

   /* Read file locale name at first */
   if( fscanf( fh, "%s\n", custRecs.acLocale ) != 1 )
   {
      logError( ERR_FILE_INVALID, MPFROMP(pszFnameCust) );
      return NULL;
   }

   /* check if the file locale is the same with the runtime locale */
   if( strcmpi( acCharLocale, custRecs.acLocale ) != 0 )
   {
      flLocale = FALSE;
   }

   /* Reopen the file for wide-oriented I/O */
   if( (fhAlt = freopen( "", "r", fh ))== NULL )
   {
      logError( ERR_FILE_OPEN, MPFROMP(pszFnameCust) );
      return NULL;
   }
   fh = fhAlt;
   fgetws( wcs, CUST_ADDR_LEN, fh );               /* ignore the first line*/

   iLineCnt = 0;
   pCust = NULL;
   while( fgetws( wcs, CUST_ADDR_LEN+2, fh ) != NULL )
   {
      len = wcslen( wcs );
      wcs[len-1] = L'\0';                      /* remove new line character*/

      iLineCnt++;
      switch( iLineCnt )
      {
         case 1:                                         /* customer number*/
           pCust = (CUSTREC*)getRecordBlock( (RECORD*)pCust,
                                             sizeof(CUSTREC),
                                             (RECORD**)&custRecs.pTopRecord );
           wcscpy( pCust->awcNum, wcs );
           break;

         case 2:                         /* customer name in national lang.*/
           if( flLocale == TRUE )   wcscpy( pCust->awcName, wcs );
           break;

         case 3:                                /* customer name in English*/
           if( flLocale == FALSE )  wcscpy( pCust->awcName, wcs );
           break;

         case 4:                     /* cusmer address in national language*/
           if( flLocale == TRUE )   wcscpy( pCust->awcAddr, wcs );
           break;

         case 5:                             /* customer address in English*/
           if( flLocale == FALSE )  wcscpy( pCust->awcAddr, wcs );
           break;

         case 6:
           wcscpy( pCust->awcPhone, wcs );
           iLineCnt = 0;
           break;
      }
   }
   pLastCust = pCust;               /* Store the pointer to the last record*/
   fclose( fh );
   return( custRecs.acLocale );
}

/*****************************************************************************/
/* getCustRec()                                                              */
/*      Searches the desired customer record(s).                             */
/*      If the pszKey is NULL, a pointer which points the top of a chain     */
/*      of whole customer records is returned.  Otherwise, it searches the   */
/*      customer(s) with the specified category and key word.  In this case, */
/*      a pointer to a chain of matched customer records is returned.  If    */
/*      no record is found, NULL is returned.                                */
/*****************************************************************************/
CUSTREC* getCustRec( char* pszKey, enum custitem e_category )
{
CUSTREC *pSrcCust, *pFoundCust=NULL;
wchar_t wcsKey[CUST_NAME_LEN];
static CUSTREC* pCustList;

   if( pszKey == NULL )                   /* requested all customer records*/
   {
     return (CUSTREC*)custRecs.pTopRecord;
   }
   else                                 /* query specified customer records*/
   {
      mbstowcs( wcsKey, pszKey, CUST_NAME_LEN );
      if( pCustList != NULL )                /* free previous query results*/
      {
          freeRecordChain( (RECORD*)pCustList );
          pCustList = NULL;
      }

      pSrcCust = (CUSTREC*)custRecs.pTopRecord;
      switch( e_category )
      {
         case custnum:                                       /* exact match*/
           while( pSrcCust != NULL )
           {
              if( wcscmp( pSrcCust->awcNum, wcsKey ) == 0 )
              {
                 pFoundCust = pSrcCust;
                 break;
              }
              pSrcCust = pSrcCust->next;
           }
           break;

         case phone:                                         /* exact match*/
           while( pSrcCust != NULL )
           {
              if( wcscmp( pSrcCust->awcPhone, wcsKey ) == 0 )
              {
                 pFoundCust = pSrcCust;
                 break;
              }
              pSrcCust = pSrcCust->next;
           }
           break;

         case custname:                                   /* multiple match*/
         {
         CUSTREC *pCust = NULL;

           while( pSrcCust != NULL )
           {
              if( wcswcs( pSrcCust->awcName, wcsKey ) != NULL )
              {
                 pCust = (CUSTREC*)getRecordBlock( (RECORD*)pCust,
                                                   sizeof(CUSTREC),
                                                   (RECORD**)&pCustList );
                 memcpy( pCust, pSrcCust, sizeof(CUSTREC) );
                 pCust->next = NULL;              /* reset the next pointer*/
              }
              pSrcCust = pSrcCust->next;
           }
           pFoundCust = pCustList;
           break;
         }

      }                                                    /* end of switch*/
      return pFoundCust;
   }                                                           /* wnd of if*/
}

/*****************************************************************************/
/* addNewCust()                                                              */
/*      Adds the new CUSTREC record to the customer records chain and update */
/*      the pLastCust pointer.  Appends the new record to the data file.     */
/*                                                                           */
/*      Returns TRUE if the operation ends without error.  Otherwise,        */
/*      returns FALSE.                                                       */
/*****************************************************************************/
Boolean addNewCust( CUSTREC* pCust )
{
FILE *fh;
Boolean flLocale = TRUE;
char acCharLocale[LOCALE_LEN];

   getCharLocale( acCharLocale );                                 /* 110795*/
   if( strcmpi( acCharLocale, custRecs.acLocale ) != 0 )
   {
      flLocale = FALSE;
   }

   pLastCust = (CUSTREC*)getRecordBlock( (RECORD*)pLastCust,
                                         sizeof(CUSTREC),
                                         NULL );
   memcpy( pLastCust, pCust, sizeof(CUSTREC) );

   if( (fh = fopen( pszFnameCust, "a" )) == NULL )
   {
      logError( ERR_FILE_OPEN, MPFROMP(pszFnameCust) );
      return FALSE;
   }

   fprintf( fh, "%ls\n", pCust->awcNum );
   if( flLocale == TRUE )                      /* Data in national language*/
   {
       fprintf( fh, "%ls\n", pCust->awcName );
       fprintf( fh, "\n" );
       fprintf( fh, "%ls\n", pCust->awcAddr );
       fprintf( fh, "\n" );
   }
   else                                                  /* data in English*/
   {
       fprintf( fh, "\n" );
       fprintf( fh, "%ls\n", pCust->awcName );
       fprintf( fh, "\n" );
       fprintf( fh, "%ls\n", pCust->awcAddr );
   }
   fprintf( fh, "%ls\n", pCust->awcPhone );
   fclose( fh );
   return TRUE;
}

/*****************************************************************************/
/* getNewCustNum()                                                           */
/*      Gets the last custmer record stored in pLastCust.  Calculates the    */
/*      next customer number and stores it to the buffer pointed to by       */
/*      wcsNewNumber as a wide string.                                       */
/*****************************************************************************/
void getNewCustNum( wchar_t* wcsNewNumber )
{
int iLastNum;
char acBuf[CUST_NUM_LEN];

   wcstombs( acBuf, pLastCust->awcNum, 8 );
   iLastNum = atoi( acBuf );
   sprintf( acBuf, "%06d", ++iLastNum );
   mbstowcs( wcsNewNumber, acBuf, 8 );
   return;
}

/*****************************************************************************/
/* getOrderRec()                                                             */
/*      Searches the desired order record(s).                                */
/*      If the pszKey is NULL, a pointer which points the top of a chain     */
/*      of whole order records is returned.  Otherwise, it searches the      */
/*      order(s) with the specified category and key word.  In this case,    */
/*      a pointer to a chain of matched order records is returned.  If no    */
/*      recod is found, NULL is returned.                                    */
/*****************************************************************************/
ORDERREC* getOrderRec( char *pszKey, enum orderitem e_category )
{
 int i;
 FILE* fh;
 time_t timeFrom, timeTo;
 struct tm tmFrom, tmTo;
 Boolean flStart = FALSE;
 ORDERREC *pOrderRec, *pFoundRec, *pCurrentRec;
 ANORDER anOrder;

   if( (fh = fopen( pszFnameOrder, "rb")) == NULL )
   {
      logError( ERR_FILE_OPEN, MPFROMP(pszFnameOrder) );
      return NULL;
   }

   if( e_category == orderdate )
   {
      /*create time_t data for difftime; */
      /* the pszKey must have yymmtt-yymmtt format */
      memset( &tmFrom, 0, sizeof(tmFrom) );
      memset( &tmTo, 0, sizeof(tmTo) );
      sscanf( pszKey, "%2d%2d%2d-%2d%2d%2d",
                      &tmFrom.tm_year, &tmFrom.tm_mon, &tmFrom.tm_mday,
                      &tmTo.tm_year, &tmTo.tm_mon, &tmTo.tm_mday );

      tmFrom.tm_mon -=1;                       /* to make 0-based numbering*/
      tmTo.tm_mon -=1;
      tmTo.tm_hour = 23;                                  /* until midnight*/
      tmTo.tm_min = 59;
      tmTo.tm_sec = 59;

      timeFrom = mktime( &tmFrom );
      timeTo = mktime( &tmTo );
   }

   pOrderRec = pFoundRec = pCurrentRec = NULL;
   while( fread( &anOrder, sizeof(anOrder), 1, fh ) == 1 )
   {
      if( e_category == ordernum &&
         strcmp( anOrder.acOrderNum, pszKey ) != 0 )  continue;
      else if( e_category == orderdate )
      {
         if( difftime( anOrder.orderDate, timeFrom ) < 0 ||
             difftime( timeTo, anOrder.orderDate ) < 0 )   continue;
      }

      pOrderRec = (ORDERREC*)getRecordBlock( (RECORD*)pOrderRec,
                                             sizeof(ORDERREC),
                                             (RECORD**)&pFoundRec );
      pOrderRec->prev = pCurrentRec;                  /* set backward chain*/
      pCurrentRec = pOrderRec;

      strcpy( pOrderRec->acOrderNum, anOrder.acOrderNum );
      pOrderRec->pCustRec = getCustRec( anOrder.acCustNum, custnum );
      for( i=0; i<MAX_ORDER_PROD; i++ )
      {
         pOrderRec->prods[i].pProd =
                getProdRec( anOrder.prods[i].acProdNum, prodnum );
         pOrderRec->prods[i].sQty =anOrder.prods[i].usQty;
      }
      pOrderRec->usTotalQty = anOrder.usTotalQty;
      pOrderRec->dTotalPrice = anOrder.dTotalPrice;
      strcpy( pOrderRec->acEmpNum, anOrder.acEmpNum );
      memcpy( &pOrderRec->orderDate, &anOrder.orderDate, sizeof(time_t));
      memcpy( &pOrderRec->shipDate, &anOrder.shipDate, sizeof(time_t));
   }

   /* test reason of loop's end */
   if( ferror( fh ) != 0 )
   {
      logError( ERR_FILE_IO, MPFROMP(pszFnameOrder) );
      pOrderRec = NULL;
   }

   fclose( fh );
   return pFoundRec;
}

/*****************************************************************************/
/* writeAnOrder()                                                            */
/*      Converts ORDERREC to ANORDER form so that pointers to product and    */
/*      customer records are converted to each record number.  Appends       */
/*      the normarized record to the end of the order record file.           */
/*      Returns TRUE if the operation ends without error.  Otherwise,        */
/*      returns FALSE.                                                       */
/*****************************************************************************/
Boolean writeAnOrder( ORDERREC *pOrder )
{
 int i;
 FILE* fh;
 ANORDER anOrder;

   if( (fh = fopen( pszFnameOrder, "ab")) == NULL )
   {
      logError( ERR_FILE_OPEN, MPFROMP(pszFnameOrder) );
      return FALSE;
   }

   /* clear buffer */
   memset( &anOrder, '\0', sizeof(anOrder) );
   /* generate ANORDER data */
   anOrder.usTotalQty = pOrder->usTotalQty;
   anOrder.dTotalPrice = pOrder->dTotalPrice;
   strcpy( anOrder.acOrderNum, pOrder->acOrderNum );
   wcstombs( anOrder.acCustNum, pOrder->pCustRec->awcNum, 8 );
   for( i=0; i<MAX_ORDER_PROD; i++ )
   {
     if( pOrder->prods[i].pProd != NULL )
     {
        strcpy( anOrder.prods[i].acProdNum, pOrder->prods[i].pProd->acNum );
        anOrder.prods[i].usQty = pOrder->prods[i].sQty;
     }
   }
   strcpy( anOrder.acEmpNum, pOrder->acEmpNum );
   memcpy( &anOrder.orderDate, &pOrder->orderDate, sizeof(time_t) );
   memcpy( &anOrder.shipDate, &pOrder->shipDate, sizeof(time_t) );

   if( fwrite( &anOrder, sizeof(ANORDER), 1,  fh ) < 1 )
   {
      logError( ERR_FILE_IO, MPFROMP(pszFnameOrder) );
      fclose( fh );
      return FALSE;
   }

   fclose( fh );
   return TRUE;
}

/*****************************************************************************/
/* getNewOrderNum()                                                          */
/*      Reads the last order record and get its order number.  Calculate the */
/*      next number and stores it to the buffer pointed to by pszNewNumber   */
/*      as a string.                                                         */
/*      If the file does not exist, the order number is always 000001.       */
/*      Returns TRUE if the operation ends withour error.  Otherwise,        */
/*      returns FALSE.                                                       */
/*****************************************************************************/
Boolean getNewOrderNum( char* pszNewNumber )
{
int iNewNum;
FILE *fh;
ANORDER anOrder;
struct stat buf;

   /* If file does not exist, order number 1 is returned. */
   if( stat( pszFnameOrder, &buf ) == -1 )
   {
      sprintf( pszNewNumber, "%06d", 1 );
      return TRUE;
   }

   if( (fh = fopen( pszFnameOrder, "rb")) == NULL )
   {
      logError( ERR_FILE_OPEN, MPFROMP(pszFnameOrder) );
      return FALSE;
   }

   /* get the file size */
   if( fstat( fileno(fh), &buf ) != 0 )
   {
      logError( ERR_FILE_IO, MPFROMP(pszFnameOrder) );
      fclose( fh );
      return FALSE;
   }

   /* seek to the top of the last order record */
   if( fseek( fh, (long)(buf.st_size-sizeof(anOrder)), SEEK_SET ) != 0 )
   {
      logError( ERR_FILE_IO, MPFROMP(pszFnameOrder) );
      fclose( fh );
      return FALSE;
   }

   if( fread( &anOrder, sizeof(anOrder), 1, fh ) < 1 )
   {
      logError( ERR_FILE_IO, MPFROMP(pszFnameOrder) );
      fclose( fh );
      return FALSE;
   }

   iNewNum = atoi( anOrder.acOrderNum );
   sprintf( pszNewNumber, "%06d", iNewNum+1 );

   fclose( fh );
   return TRUE;
}

/****************************************************************************/
/* getCharLocale()                                                          */
/*      Queries the current locale for LC_CTYPE category with the code set  */
/*      (e.g. "ja_jp.IBM-942") if needed.  The locale name is stored into   */
/*      the buffer pointed to by pszCurrentCode.                            */
/****************************************************************************/
static void getCharLocale( char *pszCharLocale )       /* 110795           */
{
char *pszLocale, *pszCodeSet;

   pszLocale = setlocale( LC_CTYPE, NULL );               /* runtime locale*/
   /* if code set is not specified */
   if( strchr( pszLocale, '.' ) == NULL )                         /* 110795*/
   {
      pszCodeSet = nl_langinfo( CODESET );         /* runtime code set name*/
      strcpy( pszCharLocale, pszLocale );
      strcat( pszCharLocale, "." );
      strcat( pszCharLocale, pszCodeSet );
   }
   else                                                           /* 110795*/
      strcpy( pszCharLocale, pszLocale );                         /* 110795*/

   return;
}

/*****************************************************************************/
/* getRecordBlock()                                                          */
/*      Allocates a memory object of specified size and chaines the object   */
/*      after an object pointed to by pRec.  If pRec is a NULL pointer, it   */
/*      stores pointer to the allocated object to *ppTop.                    */
/*      Returns the pointer to the new memory object.                        */
/*****************************************************************************/
static RECORD* getRecordBlock( RECORD *pRec, size_t size, RECORD** ppTop)
{
   if( pRec == NULL )                                  /* if the first path*/
   {
      pRec = calloc( 1, size );                                   /* 110795*/
      *ppTop = pRec;
   }
   else
   {
     pRec->next = calloc( 1, size );                              /* 110795*/
     pRec = pRec->next;
   }
   pRec->next = NULL;
   return pRec;
}

/*****************************************************************************/
/* freeRecordChain()                                                         */
/*      Free all memory objects in a chain pointed to by pTopRec.            */
/*****************************************************************************/
static void freeRecordChain( RECORD *pTopRec )
{
RECORD *pRec, *pNextRec;

   pRec = pTopRec;
   while( pRec != NULL )
   {
     pNextRec = pRec->next;
     free( pRec );
     pRec = pNextRec;
   }
}

/*****************************************************************************/
/* freeEverything()                                                          */
/*      Free all records.  Used at application termination.                  */
/*****************************************************************************/
void freeEverything()
{
    freeRecordChain( prodRecs.pTopRecord );
    freeRecordChain( custRecs.pTopRecord );
    return;
}

