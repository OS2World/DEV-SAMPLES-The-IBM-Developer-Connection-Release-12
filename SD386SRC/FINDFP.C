/*****************************************************************************/
/* File:                                                                     */
/*                                                                           */
/*   findfp.c                                                                */
/*                                                                           */
/* Description:                                                              */
/*                                                                           */
/*   Find/build a view.                                                      */
/*                                                                           */
/* History:                                                                  */
/*                                                                           */
/*****************************************************************************/

#include "all.h"

/**Externs********************************************************************/

extern uint        LinesPer;
extern uint        ExprAddr;            /* Set by ParseExpr               101*/
extern uint        ProcessID;
extern CmdParms    cmd;                 /* pointer to CmdParms structure  701*/
extern PROCESS_NODE *pnode;                                             /*827*/

AFILE *allfps;
/*****************************************************************************/
/* findfp                                                                    */
/*                                                                           */
/* Description:                                                              */
/*   Find the AFILE structure containing an address.                         */
/*   If it doesn't exist then make one.                                      */
/*                                                                           */
/* Parameters:                                                               */
/*   instaddr   instruction address.                                         */
/*                                                                           */
/* Return:                                                                   */
/*   fp         pointer to the AFILE structure.                              */
/*   NULL       did not find a pointer.                                      */
/*                                                                           */
/*****************************************************************************/
AFILE *findfp( ULONG instaddr )
{
 AFILE   *fp;
 AFILE   *fplast;
 ULONG    mid;
 ULONG    lno;
 DEBFILE *pdf;
 LNOTAB  *pLnoTabEntry;
 int      sfi;

 /****************************************************************************/
 /* - define the context where we landed.                                    */
 /****************************************************************************/
 pdf = FindExeOrDllWithAddr(instaddr);
 if( pdf == NULL )
  return(NULL);

 lno = sfi = 0;
 mid = DBMapInstAddr( instaddr, &pLnoTabEntry, pdf);

 /****************************************************************************/
 /* - if the instruction address maps to hyperspace, then make a fake        */
 /*   view.                                                                  */
 /****************************************************************************/
 if( mid == 0 )
 {
  ULONG segbase = 0;
  ULONG segspan = 0;

  segbase = DBLsegInfo( instaddr, &segspan, pdf );
  if( (segbase==0) || (segspan==0) )
   return(NULL);

  for( fplast=&allfps, fp=allfps; fp != NULL; fplast=fp, fp=fp->next ){;}

  fplast->next = fp = fakefp( segbase, segspan, pdf);
 }

 if( pLnoTabEntry )
 {
  lno = pLnoTabEntry->lno;
  sfi = pLnoTabEntry->sfi;
 }
 /****************************************************************************/
 /* - scan the views and see if we already have a view for this module.      */
 /****************************************************************************/
 for( fp = allfps; fp != NULL; fp = fp-> next )
 {
  if( (fp->mid == mid) && (fp->sfi == sfi) )
  {
   pagefp( fp, lno );

   fp->hotline = lno - fp->Nbias;
   fp->hotaddr = instaddr;
   return( fp );
  }
 }

 /****************************************************************************/
 /* - if we don't have a view, then make one and add it to the ring.         */
 /****************************************************************************/
 if( fp == NULL )
 {
  for( fplast=&allfps, fp=allfps; fp != NULL; fplast=fp, fp=fp->next ){;}

  fplast->next = fp = makefp( pdf, mid, instaddr, NULL );

  pagefp( fp, lno );
  fp->hotline = lno - fp->Nbias;
  fp->hotaddr = instaddr;
  return(fp);
 }

#if 0
/*****************************************************************************/
/* The first thing we want to do is establish pdf,mid, and lno. The instaddr */
/* was probably generated by a csip from a single-step or go. In this case,  */
/* we can use the exec values to establish pdf, mid, and lno. If this is not */
/* the case, then we have to find pdf, mid, and lno.                         */
/*                                                                           */
/*****************************************************************************/

    pdf=FindExeOrDllWithAddr(instaddr);                                 /*901*/
    if( instaddr==GetExecAddr())
    {
     mid = GetExecMid();
     lno = GetExecLno();
    }
    else                                /* else,                             */
    {
     if(pdf != NULL )                   /*                                101*/
      mid=DBMapInstAddr(instaddr,&lno,pdf); /* find the  mid and lno.        */
    }

    if(pdf)
     segbase = DBLsegInfo(instaddr, &segspan , pdf);
    if( pdf && (DEBFILE*)segbase != NULL )                              /*107*/
    {
        if( mid ){
            /* Note:  The "next" field must be the 1st field in
             * the AFILE struct for the following code to work.
             */

            fp      = allfps ;
            fplast  = (AFILE *) &allfps ;
            for( ; fp != NULL; fp = (fplast = fp) -> next )
            {
              if( fp->mid == mid )
              {
                DBMapInstAddr(instaddr,&junk,pdf); /* remove x=.          521*/
                                        /* a dummy call just to find the  115*/
                                        /* value of flag_src_or_asm       115*/
                if ( flag_asm_or_src == 1 )
                {
                  /***********************************************************/
                  /* if line not found change to asm view.                115*/
                  /***********************************************************/
                  fp->shower = showA;
                }
                goto ReturnFp;
              }
            }
            fplast->next = fp = makefp( mid, NULL );                    /*505*/

        ReturnFp:
            pagefp( fp, lno );
            fp->hotline = lno - fp->Nbias;
        ReturnAny:
            fp->hotaddr = instaddr;          /*                            101*/
            return( fp );
        }
        /* Note:  The "next" field must be the 1st field in
         * the AFILE struct for the following code to work.
         */
        fp      = allfps ;
        fplast  = (AFILE *) &allfps ;
        for( ; fp != NULL; fp = (fplast = fp) -> next )
            if( !fp->mid )
                goto ReturnAny;
        fplast->next = fp = fakefp(segbase, segspan, pdf);              /*107*/
        goto ReturnAny;
    }
    return(NULL);
#endif
}                                       /* end findfp().                     */


/*****************************************************************************/
/* DropOrMakeFakefp                                                       216*/
/*                                                                        216*/
/* Description:                                                           216*/
/*   try to find a suitable afile for the given address and span.         216*/
/*   If not found create one and return.                                  216*/
/*                                                                        216*/
/* Parameters:                                                            216*/
/*   base       module base address.                                      216*/
/*   span       length of the mdoule.                                     216*/
/*   pdf        pointer to debug file.                                    216*/
/*                                                                        216*/
/* Return:                                                                216*/
/*                                                                        216*/
/*****************************************************************************/
 void
DropOrMakeFakefp(uint base, uint span, DEBFILE *pdf)                                               /*107*/

{
  AFILE        *fp;                     /* pointer to an afile            216*/
  AFILE        *fplast;                 /* temp pointer to an afile       216*/
  AFILE        *fpold;                  /* -> old afile held for disposal 216*/
                                        /*                                216*/
/*****************************************************************************/
/* First check to see if this afile exists from a prior event. If it does 216*/
/* not then we build one.  If it does then we check to see if it's still  216*/
/* valid then return.  If it is no longer valid then build a new one and  216*/
/* dispose of the old one.                                                216*/
/*****************************************************************************/

  fplast = ( AFILE *)&allfps;           /* pointer to the head node       216*/
  for( fp = allfps;                     /* scan for afile with mid=FAKEMID216*/
       fp != NULL &&                    /* scan if the ring is established216*/
       fp->mid !=FAKEMID;               /* and  while mid != FAKEMID and  216*/
       fp = (fplast = fp)->next         /* next afile in ring             216*/
     ){;}                               /*                                216*/
                                        /*                                216*/
  if ((fp != NULL) && (fp->mid == FAKEMID)) /* found afile with fake mid? 216*/
  {                                     /*                                216*/
   if ( (GetExecAddr() >= fp->mseg) && (GetExecAddr() <= span) )
                                        /* still ok from last time?       216*/
    return;                             /* return                         216*/
   fpold = fp;                          /* if it's not good then hold ->to it*/
   fp = fakefp(base,span,pdf);          /* build a new one                216*/
   fp->mid  = FAKEMID;                  /* give it mid id                 216*/
   fp->next = fpold->next;              /* put it in the ring             216*/
   fplast->next = fp;                   /*                                216*/
   Tfree((void*)fpold);                  /* release the old memory     521 216*/
   fp->hotaddr = GetExecAddr();         /* set current address            216*/
   fp->sview  = NOSRC;                  /* assume no source disassembler view*/
  }                                     /*                                   */
/*****************************************************************************/
/* At this point we have not found an afile with a fake mid.                 */
/*****************************************************************************/
  fp = fakefp(base,span,pdf);            /* build a new one               216*/
  fp->next = NULL;                      /* and put it in the ring         216*/
  fp->mid  = FAKEMID;                   /* give it mid id                 216*/
  if ( fplast )                         /* if the ring is not empty then  216*/
  {                                     /*                                216*/
   fplast->next = fp;                   /* put it in the ring             216*/
  }                                     /*                                216*/
  else                                  /*                                216*/
  {                                     /*                                216*/
   allfps = fp;                         /* start the ring                 216*/
  }                                     /*                                216*/
  fp->hotaddr = GetExecAddr();          /* set current address            216*/
  fp->topoff = base;                    /* set top offset of display      107*/
  fp->flags=AF_FAKE|AF_ZOOM;            /* set appropriate flags          216*/
  fp->sview  = NOSRC;                   /* assume no source disassembler view*/
 }                                      /* end of dropormakefakefp()         */




/*****************************************************************************/
/* FindFuncOrAddr                                                            */
/*                                                                           */
/* Description:                                                              */
/*                                                                           */
/*   Find the AFILE structure containing a function name or address.         */
/*                                                                           */
/* Parameters:                                                               */
/*                                                                           */
/*   pFunctionNameOrAddr  -> to function name or address string.             */
/*   IsAddr               TRUE => Address.                                   */
/*                        FALSE=> Function name.                             */
/*                                                                           */
/* Return:                                                                   */
/*                                                                           */
/*   fp         pointer to the AFILE structure.                              */
/*   NULL       did not find a pointer.                                      */
/*                                                                           */
/*****************************************************************************/
AFILE *FindFuncOrAddr( uchar *pFunctionNameOrAddr , int IsAddr )
{
 AFILE   *fp;
 ULONG    line = 0;
 uint     n;
 uint     addr;
 uchar   *cp;
 uchar    uname[MAXSYM+2];
 DEBFILE *pdf;
 LNOTAB  *pLnoTabEntry;

 cp   = NULL;
 addr = NULL;
 if( IsAddr )
 {
  /***************************************************************************/
  /* Handle Addresses.                                                       */
  /***************************************************************************/
  cp = ParseExpr(pFunctionNameOrAddr,2,0,0,0);
  if( cp && ( *cp=='\0') )
  {
   addr = ExprAddr;
   pdf  = FindExeOrDllWithAddr(addr);
  }
 }
 else
 {
  /***************************************************************************/
  /* Handle function names.                                                  */
  /*                                                                         */
  /* - FindFuncOrAddr will receive the name without the "_". So build the    */
  /*   name with the "_" and make in an ASCII Z string.                      */
  /* - scan the list of pdfs looking through the public sections.            */
  /* - look for the "_"+name followed by a search for the name.              */
  /*                                                                         */
  /***************************************************************************/
  n = strlen(pFunctionNameOrAddr);
  if( n > sizeof(uname)-2 )
    n = sizeof(uname)-2;

  memcpy( uname+1, pFunctionNameOrAddr, n );
  uname[0] = '_';
  uname[n+1] = 0;

  pdf = pnode->ExeStruct;
  for( ; pdf; pdf=pdf->next )
  {
   {
    addr=DBPub(uname,pdf);
    if( addr == NULL)
    {
     addr=DBPub(pFunctionNameOrAddr,pdf);
    }
    if( addr != NULL)
     break;
   }
  }
 }

 /****************************************************************************/
 /* If can't find addr and pdf, then can't build a view.                     */
 /****************************************************************************/
 if( (addr == NULL) || (pdf == NULL) )
  return(NULL);

/*****************************************************************************/
/* At this point, we have an addr and a pdf. Now build a view.               */
/*****************************************************************************/
 fp = findfp( addr );
 if( fp )
 {
  if( fp->source)
  {
   DBMapInstAddr( addr, &pLnoTabEntry, pdf);
   if( pLnoTabEntry ) line = pLnoTabEntry->lno;
   line -= fp->Nbias;                   /* removed a '+1'.                234*/
   if( (line  >  0) &&                  /* removed an '='.                234*/
       (line <= (int)(fp->Nlines))      /* add '=' to make '<='           234*/
     )                                  /*                                   */
   {                                    /*                                   */
    fp->csrline = line;                 /* put the cursor on the line and    */
    if( (int)(fp->topline=line-LinesPer/2) < 1 ) /* make it show up in the304*/
     fp->topline = 1;                            /* middle of the page.   234*/
   }                                    /* end "if we have source info"      */
  }                                     /*                                   */
  else
   fp->topoff = addr;
 }
 return( fp );
}


    AFILE *
mid2fp(uint  mid )
{
    AFILE *fp;                          /* was register.                  112*/

    /* Note:  The "next" field must be the 1st field in
     * the AFILE struct for the following code to work.
     */

    for(fp = allfps; fp != NULL; fp = fp->next )
        if( fp->mid == mid )
            break;
    return( fp );
}
