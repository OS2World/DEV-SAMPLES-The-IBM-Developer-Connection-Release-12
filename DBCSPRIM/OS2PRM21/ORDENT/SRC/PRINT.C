/************   OS/2 Application Primer Sample   **********************/
/*                                                                    */
/* Module Name : PRINT.C                                              */
/* Version : 2.1 (12/24/93)                                           */
/*                                                                    */
/*    << This sample program is only for OS/2 V2.x. >>                */
/*                                                                    */
/* A sample program for "OS/2 DBCS Application Primer".               */
/* A dialog procedure to print a sales report, and the subroutines    */
/* related to it.                                                     */
/*                                                                    */
/**********************************************************************/
/*--- Include Dos and Win API call definitions ---------------*/
#define    INCL_NOCOMMON
#define    INCL_WINWINDOWMGR
#define    INCL_DOSMISC
#define    INCL_DOSFILEMGR
#define    INCL_WIN
#define    INCL_GPI
#define    INCL_DOS
#define    INCL_DOSFILEMGR
#define    INCL_NLS

#define    INCL_TEST_DLG
#define    INCL_PRTR_DLG

#include    <os2.h>

/*--- Include C run-time function definitions ----------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/*--- Include application-specific header files --------------*/
#define INCL_MAIN_WND
#include "ordent.h"
#include "dbcs.h"

/*--- Include program-specific header file -------------------*/
#include "printer.h"

/*--- Global variables ---------------------------------------*/
FILE   *printer;
SHORT  cPrn = 0;

/*--- The main function --------------------------------------*/
SHORT PrintReport(CHAR *start_date, CHAR *end_date)
{
   UCHAR   prod_no[20];
   UCHAR   prod_name[20];
   UCHAR   ord_qty[20];
   UCHAR   ord_amt[20];
   UCHAR   total[20];
   UCHAR   rpt_ttl[40];
   UCHAR   crt_date[20];
   UCHAR   tgt_date[20];
   UCHAR   period[40];
   UCHAR   sdate[10], edate[10];

   PUCHAR  pTxt[2];
   UCHAR   date[40];

   SHORT   i, j;
   ULONG   leng;
   SHORT   total_column, space;
   SHORT   wd_pno, wd_pna, wd_oqt, wd_oam;
   SHORT   col_pno, col_pna, col_oqt, col_oam;

   UCHAR   max_record_text[20];
   USHORT  max_prod, max_ordr;
   USHORT  *prod_qty;
   LONG    total_amount;

   UCHAR   quantity[20];
   UCHAR   amount[20];

   struct ordrRec op_rec;
   struct prodRec  pr_rec;


   strcpy(sdate, start_date);
   strcpy(edate, end_date);
   FormatDate(edate, sizeof(edate));
   FormatDate(sdate, sizeof(sdate));

   /*--- Loading text data from the specified resource -------*/
   WinLoadString(hab, hmodRsrc, IDD_MAIN_TTL_PN, sizeof(prod_no), prod_no);
   WinLoadString(hab, hmodRsrc, IDD_MAIN_TTL_NM, sizeof(prod_name), prod_name);
   WinLoadString(hab, hmodRsrc, IDD_MAIN_TTL_OQ, sizeof(ord_qty), ord_qty);
   WinLoadString(hab, hmodRsrc, IDD_MAIN_TTL_OP, sizeof(ord_amt), ord_amt);
   WinLoadString(hab, hmodRsrc, IDD_MAIN_TTL_TL, sizeof(total), total);
   WinLoadString(hab, hmodRsrc, IDD_MAIN_TTL_RT, sizeof(rpt_ttl), rpt_ttl);
   WinLoadString(hab, hmodRsrc, IDD_MAIN_TTL_CD, sizeof(crt_date), crt_date);
   WinLoadString(hab, hmodRsrc, IDD_MAIN_TTL_TD, sizeof(tgt_date), tgt_date);
   WinLoadString(hab, hmodRsrc, IDD_RPRT_MSG_TD, sizeof(period), period);

   /*--- Initialize a printer --------------------------------*/
   init_prn();

   /*--- Get today's date ------------------------------------*/
   GetDate(date, sizeof(date));

   draw_text(date, RIGHT, 0);
   draw_text(crt_date, RIGHT | CRLF, WIDTH - strlen(date) - 1);
   draw_text("", CRLF, 0);
   draw_text(rpt_ttl,  CENTER | BOX | DOUBLE | CRLF, 0);
   draw_text("", CRLF, 0);
   draw_text(tgt_date, LEFT, 0);

   pTxt[0] = sdate;
   pTxt[1] = edate;
   DosInsertMessage(pTxt, 2, period, strlen(period), date, sizeof(date), &leng);
   date[leng] = '\0';

   draw_text(date, CRLF, 2);
   draw_text("", CRLF, 0);

   wd_pno = max(strlen(prod_no), PROD_NO_COL);
   wd_pna = max(strlen(prod_name), PROD_NAME_COL);
   wd_oqt =  max(strlen(ord_qty), ORD_QTY_COL);
   wd_oam =  max(strlen(ord_amt), ORD_AMT_COL);
   total_column = wd_pno + wd_pna + wd_oqt + wd_oam;
   (space = WIDTH - total_column) > 0 ? (space /= 3) : (space = 0);
   col_pno = 0;
   col_pna = col_pno + wd_pno + space;
   col_oqt = col_pna + wd_pna + space;
   col_oam = col_oqt + wd_oqt + space;

   draw_text(prod_no, LEFT | EMPH | UNDER, col_pno);
   draw_text(prod_name, LEFT | EMPH | UNDER, col_pna);
   draw_text(ord_qty, RIGHT | EMPH | UNDER, col_oqt + wd_oqt);
   draw_text(ord_amt, RIGHT | EMPH | UNDER | CRLF, col_oam + wd_oam);
   draw_text("", CRLF, 0);

   GetNewNumber(PRODFILE, max_record_text);
   max_prod = atoi(max_record_text) - 1;
   prod_qty = malloc(max_prod * sizeof(USHORT));
   for (i = 0; i < max_prod; ++i) {
      *(prod_qty + i) = 0;
   }

   GetNewNumber(ORDRFILE, max_record_text);
   max_ordr = atoi(max_record_text) - 1;
   for (i = 1; i <= max_ordr; ++i) {
      sprintf(op_rec.ordrNo, "%6.6d", i);
      GetOrdrInfo(&op_rec);
      if (check_date(op_rec.ordrDate, start_date, end_date)) {
         j = 0;
         while (op_rec.prod[j].qty && j < 3) {
            *(prod_qty+atoi(op_rec.prod[j].prodNum)-1) += op_rec.prod[j].qty;
            ++j;
         }
      }
   }

   total_amount = 0;
   for (i = 1; i <= max_prod; ++i) {
      if (*(prod_qty + i - 1) != 0) {
         sprintf(pr_rec.prodNo, "%6.6d", i);
         GetProdInfo(&pr_rec);
         draw_text(pr_rec.prodNo, CENTER, col_pno + wd_pno);
         draw_text(pr_rec.prodName, LEFT, col_pna);
         _itoa(*(prod_qty + i - 1), quantity, 10);
         FormatInteger(quantity);
         draw_text(quantity, RIGHT, col_oqt + wd_oqt);
         total_amount += *(prod_qty + i - 1) * pr_rec.unitPrice;
         sprintf(amount, "%ld", *(prod_qty + i - 1) * pr_rec.unitPrice);
         FormatPrice(amount);
         draw_text(amount, RIGHT, col_oam + wd_oam);
         draw_text("", CRLF, 0);
      }
   }
   draw_text("", CRLF, 0);
   sprintf(amount, "%ld", total_amount);
   FormatPrice(amount);
   draw_text(amount, RIGHT, col_oam + wd_oam);
   draw_text(total, RIGHT, col_oam + wd_oam - strlen(amount) - 2);

   /*--- Close the printer -----------------------------------*/
   end_prn();

   return 0;
}

/**********************************************************************/
/*   Check if "date" is between "sdate" and "edate"                   */
/**********************************************************************/
BOOL check_date(CHAR *date, CHAR *sdate, CHAR *edate)
{
   struct tm tm_date;
   time_t time, stime, etime;

   tm_date.tm_sec = 0;
   tm_date.tm_min = 0;
   tm_date.tm_hour = 0;
   tm_date.tm_isdst = 0;

   sscanf(date, "%4d%2d%2d", &tm_date.tm_year, &tm_date.tm_mon,
          &tm_date.tm_mday);
   tm_date.tm_year -= 1900;
   tm_date.tm_mon  -= 1;
   time = mktime(&tm_date);

   sscanf(sdate, "%4d%2d%2d", &tm_date.tm_year, &tm_date.tm_mon,
          &tm_date.tm_mday);
   tm_date.tm_year -= 1900;
   tm_date.tm_mon  -= 1;
   stime = mktime(&tm_date);

   sscanf(edate, "%4d%2d%2d", &tm_date.tm_year, &tm_date.tm_mon,
          &tm_date.tm_mday);
   tm_date.tm_year -= 1900;
   tm_date.tm_mon  -= 1;
   etime = mktime(&tm_date);

   if ((stime <= time) && (time <= etime)) {
      return TRUE;
   } else {
      return FALSE;
   }
}

/**************************************************************/
/* Draw "text", with various attributes specified by "flags", */
/* from the column position specified by "margin"             */
/*                                                            */
/* Flags:                                                     */
/*                                                            */
/*      RIGHT    Right-justify the text                       */
/*      CENTER   Center the text                              */
/*      LEFT     Left-justify the text                        */
/*                                                            */
/*      BOX      Draw a box around the text                   */
/*      DOUBLE   Print the text in double width               */
/*      EMPH     Emphasize the text                           */
/*      UNDER    Underline the text                           */
/*                                                            */
/*      CR       Carriage return after printing the text      */
/*      CRLF     Carriage return and line feed after          */
/*               printing the text                            */
/*                                                            */
/**************************************************************/
VOID draw_text(UCHAR *text, USHORT flags, USHORT margin)
{
   USHORT tlen, i;

   tlen = strlen(text);
   if (flags & DOUBLE) tlen *= 2;
   if (flags & RIGHT) {
      cr();
      if (margin != 0) {
         for (i = 0; i < margin - tlen; ++i) {
            fputc(' ', printer);
         }
      } else {
         for (i = 0; i < WIDTH - tlen; ++i) {
            fputc(' ', printer);
         }
      }
   } else {
      if (flags & CENTER) {
         cr();
         if (margin != 0) {
            for (i = 0; i < (margin - tlen) / 2; ++i) {
               fputc(' ', printer);
            }
         } else {
            for (i = 0; i < (WIDTH - tlen) / 2; ++i) {
               fputc(' ', printer);
            }
         }
      } else {
         if (flags & LEFT) cr();
         for (i = 0; i < margin; ++i) {
            fputc(' ', printer);
         }
      }
   }

   if (flags & DOUBLE) dwon();
   if (flags & EMPH)   emon();
   if (flags & UNDER)  ulon();

   fputs(text, printer);

   if (flags & DOUBLE) dwoff();
   if (flags & EMPH)   emoff();
   if (flags & UNDER)  uloff();

   if (flags & BOX) {
      for (i = 0; i < tlen; ++i) bs();
      boxl();
      for (i = 0; i < tlen - 2; ++i) boxm();
      boxr();
   }
   if (flags & CR) cr();
   if (flags & CRLF) {
      cr(); lf();
   }

}

/**********************************************************************/
/*   Initialize a printer and load the control code set of the        */
/*   specified printer                                                */
/**********************************************************************/
USHORT init_prn()
{
   SHORT   i;
   USHORT  prn_selected;

   /*--- Check if the current code page is a combined one ----*/
   if (flDbcsCp == DBCS_CP) {
      prn_selected = IBM5575;
   } else {
      prn_selected = PROPRINTER;
   }

   /*--- Load the control set --------------------------------*/
   while (TRUE) {
      if (prn_selected == ccode[cPrn].ptype) {
         break;
      }
      if (++cPrn == MAX_SUPPORTED_PRINTERS) {
         return PRINTER_NOT_SUPPORTED;
      }
   }

   /*--- Open the printer device file ------------------------*/
   printer = fopen("PRN", "wb");

   /*- Send a control code for initialization to the printer -*/
   for (i = 0; i < ccode[cPrn].init.len; ++i) {
      fputc(ccode[cPrn].init.str[i], printer);
   }
   return 0;
}

/*--- Close the printer device file ----------------------------------*/
VOID end_prn()
{
   fclose(printer);
}

/*--- Send a control code for back space to the printer --------------*/
VOID bs()
{
   SHORT i;

   for (i = 0; i < ccode[cPrn].bs.len; ++i)
      fputc(ccode[cPrn].bs.str[i], printer);
}

/*--- Send a control code for line feed to the printer ---------------*/
VOID lf()
{
   SHORT i;

   for (i = 0; i < ccode[cPrn].lf.len; ++i)
      fputc(ccode[cPrn].lf.str[i], printer);
}

/*--- Send a control code for carriage return to the printer ---------*/
VOID cr()
{
   SHORT i;

   for (i = 0; i < ccode[cPrn].cr.len; ++i)
      fputc(ccode[cPrn].cr.str[i], printer);
}

/*--- Send a control code for underlining-on to the printer ----------*/
VOID ulon()
{
   SHORT i;

   for (i = 0; i < ccode[cPrn].ulon.len; ++i)
      fputc(ccode[cPrn].ulon.str[i], printer);
}

/*--- Send a control code for underlining-off to the printer ---------*/
VOID uloff()
{
   SHORT i;

   for (i = 0; i < ccode[cPrn].uloff.len; ++i)
      fputc(ccode[cPrn].uloff.str[i], printer);
}

/*--- Send a control code for double-width-on to the printer ---------*/
VOID dwon()
{
   SHORT i;

   for (i = 0; i < ccode[cPrn].dwon.len; ++i)
      fputc(ccode[cPrn].dwon.str[i], printer);
}

/*--- Send a control code for double-width-off to the printer --------*/
VOID dwoff()
{
   SHORT i;

   for (i = 0; i < ccode[cPrn].dwoff.len; ++i)
      fputc(ccode[cPrn].dwoff.str[i], printer);
}

/*--- Send a control code for emphasize-on to the printer ------------*/
VOID emon()
{
   SHORT i;

   for (i = 0; i < ccode[cPrn].emon.len; ++i)
      fputc(ccode[cPrn].emon.str[i], printer);
}

/*--- Send a control code for emphasize-off to the printer -----------*/
VOID emoff()
{
   SHORT i;

   for (i = 0; i < ccode[cPrn].emoff.len; ++i)
      fputc(ccode[cPrn].emoff.str[i], printer);
}

/*--- Send a control code for box-left to the printer ----------------*/
VOID boxl()
{
   SHORT i;

   for (i = 0; i < ccode[cPrn].boxl.len; ++i)
      fputc(ccode[cPrn].boxl.str[i], printer);
}

/*--- Send a control code for box-middle to the printer --------------*/
VOID boxm()
{
   SHORT i;

   for (i = 0; i < ccode[cPrn].boxm.len; ++i)
      fputc(ccode[cPrn].boxm.str[i], printer);
}

/*--- Send a control code for box-right to the printer ---------------*/
VOID boxr()
{
   SHORT i;

   for (i = 0; i < ccode[cPrn].boxr.len; ++i)
      fputc(ccode[cPrn].boxr.str[i], printer);
}


SHORT CheckInpDate(CHAR szDate[])
{
  CHAR szBuf[9];
  SHORT sDate;

  if (8 < strlen(szDate))
    return FAIL;
  strcpy(szBuf, szDate);
  sDate = (SHORT)strtol(&szBuf[6], (char **)NULL, 10);
  if ((sDate < 1) || (31 < sDate) )
    return FAIL;
  szBuf[6] = '\0';
  sDate = (SHORT)strtol(&szBuf[4], (char **)NULL, 10);
  if ((sDate < 1) || (12 < sDate))
    return FAIL;
  szBuf[ 4 ] = '\0';
  sDate = (SHORT)strtol(szBuf, (char **)NULL, 10);
  if ((sDate < 1980) || (2074 < sDate))
    return FAIL;
  return SUCCESS;
}


MRESULT EXPENTRY ReptPrtDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  CHAR szDateFrom[20], szDateTo[20];

  switch(msg)
  {
    case WM_COMMAND:
      switch(COMMANDMSG(&msg)->cmd)
      {
        case IDD_PRNT_BTN_ENT:
          WinQueryWindowText(WinWindowFromID(hwnd, IDD_PRNT_SLE_FRM),
                             sizeof(szDateFrom), szDateFrom );
          if (CheckInpDate(szDateFrom) == FAIL){
            DosBeep(1380, 100);
            return 0;
          }
          WinQueryWindowText(WinWindowFromID(hwnd, IDD_PRNT_SLE_TO),
                             sizeof(szDateTo), szDateTo);
          if (CheckInpDate(szDateTo) == FAIL){
            DosBeep(1380, 100);
            return 0;
          }
          PrintReport(szDateFrom, szDateTo);
          WinDismissDlg(hwnd, TRUE);
          return 0;
        case IDD_PRNT_BTN_CAN:
          WinDismissDlg(hwnd, TRUE);
          return 0;
      } break;
   }
   return(WinDefDlgProc(hwnd, msg, mp1, mp2));
}
