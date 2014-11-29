/****************************************************************************

   MODULE NAME:  GREHOOK

   DESCRIPTIVE NAME:  Graphic Engine Monitoring example


   Author   : Marc Fiammante

   FUNCTION:  Monitors Graphic Engine calls for wait-for-string
              Wait-For-String

   NOTES:

     DEPENDENCIES: OS/2 EE 2.0

     RESTRICTIONS: OS/2 PM only.


   ENTRY POINTS: OS2_PM_DRV_ENABLE

     PURPOSE:  See FUNCTION.

     LINKAGE:  Standard IBM C-SET/2

 *****************************************************************************/
#define  INCL_BASE
#include <os2.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#define  INCL_GRE_CLIP
#define  INCL_GRE_DCS
#define  INCL_GRE_STRINGS               /* include GRE strings        */
#include <pmddim.h>                     /* in Graphic engine include  */
#define  INCL_RXSHV                     /* include shared variable    */
#define  INCL_RXMACRO                   /* include macrospace info    */
#include <rexxsaa.h>                    /* REXXSAA header information */
#define  DEBUG
/*--------------------------------------------------------------------*/
CHAR OS2_PM_DRV_ENABLE_LEVELS[]={       /* Say we are running at Ring 3 */
   0x03,      /* 0  */
   0x03,      /* 1  */
   0x03,      /* 2  */
   0x03,      /* 3  */
   0x03,      /* 4  */
   0x03,      /* 5  */
   0x03,      /* 6  */
   0x03,      /* 7  */
   0x03,      /* 8  */
   0x03,      /* 9  */
   0x03,      /* A  */
   0x03,      /* B  */
   0x03,      /* C  */
   0x00
};
/*--------------------------------------------------------------------*/
/*- This is where we store previous copy of the Graphic engine       -*/
/*- Dispatch Table defined in                                        -*/
typedef  ULONG (APIENTRY _GRE32ENTRY)(HDC hDC, ...);
typedef  _GRE32ENTRY  *GRE32ENTRY;
#pragma data_seg(GLOBDATA)
GRE32ENTRY GreEntries[256];
#pragma data_seg()
/*--------------------------------------------------------------------*/
/*- PM_OS2_DRV_ENABLE function 0X0C Param2 points to following struct-*/
typedef struct _TABLES  {
   ULONG Reserved;
   GRE32ENTRY *GreEntryTable;
} TABLES;
typedef TABLES * PTABLES;
/*--------------------------------------------------------------------*/
ULONG APIENTRY HookedGre32Entry4(HDC hDC,...);
ULONG APIENTRY HookedGre32Entry5(HDC hDC,...);
ULONG APIENTRY HookedGre32Entry8(HDC hDC,...);
ULONG APIENTRY HookedGre32Entry10(HDC hDC,...);
/*--------------------------------------------------------------------*/
/*- PM_OS2_DRV_ENABLE is the entry point which is called we function--*/
/*  0x0C if the user OS2.INI file has been updated with the following:*/
/*                                                                    */
/* The Application Name looked for is -------- PM_ED_HOOKS            */
/* The Key_Name is  -------------------------- MODULENAME (FAJGREHK)  */
/* The Data Field is Fully Qualified Path----- C:\WITT\FAJGREHK.DLL   */
/* This will allow several Monitors to be installed consecutively.    */
/*                                                                    */
/* the receiving  dll will get a copy of the Display Driver and Engine*/
/* Dispatch tables.  These tables will be in there raw form - that is */
/* the engine dispatch will be a table of 32Bit Entry Points and the  */
/* Display Driver Dispatch Table will either 16 or 32 depending on the*/
/* display selected at install time.                                  */
/*                                                                    */
LONG APIENTRY OS2_PM_DRV_ENABLE(ULONG Subfunc,
                                PVOID   *Param1,
                                PTABLES  Param2
                                ) {
    APIRET rc;
    ULONG  Size;
    ULONG  Attr;
    USHORT Index;
    int i;
    /*----------------------------------------*/
    /* Check if the function is 0x0C          */
    if (Subfunc==0x0C) {
#ifdef DEBUG
       printf("GREHOOK Graphic engine hook sample\n");
       printf("PM_OS2_DRV_ENABLE called for function %X (Install Monitor)\n",Subfunc);
#endif
       Size=256*4;
       rc=DosQueryMem(Param2,&Size,&Attr);
#ifdef DEBUG
       printf("Param2 %p rc %d , Size %d ,Attr %8.8X \n",Param2,rc,Size,Attr);
#endif
       /*---------------------------------------------------*/
       /* Make sure there is enough room in the parameter 2 */
       if ((rc==0)&&(Size>=sizeof(TABLES))) {
          Size=0x1000;
          rc=DosQueryMem(Param2->GreEntryTable,&Size,&Attr);
#ifdef DEBUG
          printf("Table %p rc %d , Size %d ,Attr %8.8X \n",Param2->GreEntryTable,rc,Size,Attr);
#endif
          Size=min(Size,sizeof(GreEntries));
#ifdef DEBUG
          printf("Graphic table Entries Size=%d\n",Size);
#endif
          /* Save the original copy of the Table */
          memcpy(GreEntries,Param2->GreEntryTable,Size);
          Size=4;
          /*----------------------------------------------------*/
          /*- Set GreCharStringPos Interception ----------------*/
          Index=(USHORT)(NGreCharStringPos&0x000000FFL);
          rc=DosQueryMem((PVOID)Param2->GreEntryTable[Index],&Size,&Attr);
          if (rc==0) {
#ifdef DEBUG
             printf("At index %X Address was %p\n",Index,Param2->GreEntryTable[Index]);
             printf("At index %X Saved Address is  %p\n",Index,GreEntries[Index]);
#endif
             Param2->GreEntryTable[Index]=HookedGre32Entry10; /* 10 Parms (pmddim.h */
#ifdef DEBUG
             printf("At index %X Address is  %p\n",Index,Param2->GreEntryTable[Index]);
#endif
          } else {
             printf("function %X pointer Is not 32 bits (%p)\n",Index,Param2->GreEntryTable[Index]);
             DosSleep(5000L);
          } /* endif */
          /*----------------------------------------------------*/
          /*- Set GreCharString Interception -------------------*/
          Index=(USHORT)(NGreCharString&0x000000FFL);
          rc=DosQueryMem((PVOID)Param2->GreEntryTable[Index],&Size,&Attr);
          if (rc==0) {
#ifdef DEBUG
             printf("At index %X Address was %p\n",Index,Param2->GreEntryTable[Index]);
             printf("At index %X Saved Address is  %p\n",Index,GreEntries[Index]);
#endif
             Param2->GreEntryTable[Index]=HookedGre32Entry5;
#ifdef DEBUG
             printf("At index %X Address is  %p\n",Index,Param2->GreEntryTable[Index]);
#endif
          } else {
             printf("function %X pointer Is not 32 bits (%p)\n",Index,Param2->GreEntryTable[Index]);
             DosSleep(5000L);
          } /* endif */
          /*----------------------------------------------------*/
          /*- Set GreCharStr Interception  (VIO graphic)-------*/
          Index=(USHORT)(NGreCharStr&0x000000FFL);
          rc=DosQueryMem((PVOID)Param2->GreEntryTable[Index],&Size,&Attr);
          if (rc==0) {
#ifdef DEBUG
             printf("At index %X Address was %p\n",Index,Param2->GreEntryTable[Index]);
             printf("At index %X Saved Address is  %p\n",Index,GreEntries[Index]);
#endif
             Param2->GreEntryTable[Index]=HookedGre32Entry5;
#ifdef DEBUG
             printf("At index %X Address is  %p\n",Index,Param2->GreEntryTable[Index]);
#endif
          } else {
             printf("function %X pointer Is not 32 bits (%p)\n",Index,Param2->GreEntryTable[Index]);
             DosSleep(5000L);
          } /* endif */
          /*----------------------------------------------------*/
          /*- Set GreCharRect Interception  (VIO graphic) ------*/
          Index=(USHORT)(NGreCharRect&0x000000FFL);
          rc=DosQueryMem((PVOID)Param2->GreEntryTable[Index],&Size,&Attr);
          if (rc==0) {
#ifdef DEBUG
             printf("At index %X Address was %p\n",Index,Param2->GreEntryTable[Index]);
             printf("At index %X Saved Address is  %p\n",Index,GreEntries[Index]);
#endif
             Param2->GreEntryTable[Index]=HookedGre32Entry5;
#ifdef DEBUG
             printf("At index %X Address is  %p\n",Index,Param2->GreEntryTable[Index]);
#endif
          } else {
             printf("function %X pointer Is not 32 bits (%p)\n",Index,Param2->GreEntryTable[Index]);
             DosSleep(5000L);
          } /* endif */
          /*----------------------------------------------------*/
          /*- Set GreSetupDC  Interception  (Init graphic) -----*/
          Index=(USHORT)(NGreSetupDC&0x000000FFL);
          rc=DosQueryMem((PVOID)Param2->GreEntryTable[Index],&Size,&Attr);
          if (rc==0) {
#ifdef DEBUG
             printf("At index %X Address was %p\n",Index,Param2->GreEntryTable[Index]);
             printf("At index %X Saved Address is  %p\n",Index,GreEntries[Index]);
#endif
             Param2->GreEntryTable[Index]=HookedGre32Entry8;
#ifdef DEBUG
             printf("At index %X Address is  %p\n",Index,Param2->GreEntryTable[Index]);
#endif
          } else {
             printf("function %X pointer Is not 32 bits (%p)\n",Index,Param2->GreEntryTable[Index]);
             DosSleep(5000L);
          } /* endif */

       } else {
          printf("Received Size %d should be TABLES %d\n",Size,sizeof(TABLES));
          DosSleep(5000L);
       } /* endif */
#ifdef DEBUG
       DosSleep(5000L);   /* Allow some time to read messages at boot time */
#endif
       return 0;
    } else {
       return -1;
    } /* endif function 0x0C */
}

/*--------------------------------------------------------------------*/
/*- Define the Vio Presentation Space Structure which has been -------*/
/*- Forgotten in pmddim.H                                      -------*/
typedef struct _VioPS {
  ULONG  Lock[5];                     /* Unused                       */
  UCHAR *LVB;                         /* Logical video Buffer pointer */
  ULONG  Next[2];                     /* Unused                       */
  USHORT CellSize;                    /* Vio Cell Size                */
  USHORT Rows;                        /* Vio Buffer Rows              */
  USHORT Cols;                        /* Vio Buffer Cols              */
} VioPs;
/*  Used by GreCharRect VIO function                                  */
typedef struct _CHARRECT {
  USHORT x;
  USHORT y;
  USHORT Cx;
  USHORT Cy;
} CHARRECT;
/*  Used by GreCharStr  VIO function                                  */
typedef struct _CHARSTR {
  USHORT x;
  USHORT y;
  USHORT Len;
} CHARSTR;
/*--------------------------------------------------------------------*/
/*- Ids for spying Globally shared data ------------------------------*/
APIRET APIENTRY WaitForString(PID WinPid,PUCHAR String);
#pragma data_seg(GLOBDATA)
BOOL  Waiting   =FALSE;
PID   SpiedPid  =0;
#define STRINGSIZE 255
UCHAR WaitString[STRINGSIZE+1];
#pragma data_seg()
#define WAITSEM "\\sem32\\waitstr"
HEV    hWait=0;
/*--------------------------------------------------------------------*/
/*-                                                                ---*/
PID    HookedPid =0;
/*--------------------------------------------------------------------*/
/*- Check if we are spying for that process/thread                 ---*/
/*- TRUE Spying , FALSE Not Spying                                 ---*/
/*- I Allow only one at a time  !!!!!!!!!!!!!!!                    ---*/
/*- Change design a few for multiple Spy                           ---*/
/*- Queue name ,PID/TID arrays etc...                              ---*/
BOOL  CheckSpying() {
    APIRET rc;
    PTIB   ptib;
    PPIB   ppib;
    /*---------------------------------------------------------*/
    /*--If not waiting then return not spying     -------------*/
    if (Waiting==FALSE) return FALSE;
    /*---------------------------------------------------------*/
    /*--If Pid is 0 wait for string anywhere      -------------*/
    if (SpiedPid==0) return TRUE;
    /*---------------------------------------------------------*/
    /*--Now compare Pid     wanted and current   --------------*/
    /*--comparing Tid too does not work for Comm Mgr AVio------*/
    rc=DosGetInfoBlocks(&ptib,&ppib);
    if (rc==0) {
       if (ppib->pib_ulpid==SpiedPid) {
          return TRUE;
       } else {
          return FALSE;
       }
    } else {
       return FALSE;
    } /* endif */
}
VOID PmGpiMatch(PUCHAR  InData,ULONG Size) {
   PUCHAR Data;
   APIRET rc;
   rc=DosAllocMem((PVOID)&Data,Size+1,fALLOC);
   if (rc==0) {
      Data[Size]=0x00; /* NUll terminate string for strstr */
      memcpy(Data,InData,Size);
      if (strstr(Data,WaitString)) {
         Waiting   =FALSE;
         hWait=0;
         DosOpenEventSem(WAITSEM,&hWait);
         DosPostEventSem(hWait);
         DosCloseEventSem(hWait);
         hWait=0;
      } /* endif */
      DosFreeMem(Data);
   } /* endif */
}
VOID VioMatch(VioPs *pVio) {
   APIRET rc;
   ULONG  Size;
   ULONG  Attr;
   pVio->LVB;
   Size=0x2000;
   rc=DosQueryMem((PVOID)pVio->LVB,&Size,&Attr);
   if (rc==0) {
      PUCHAR Data;
      Size=pVio->Rows*pVio->Cols;
      rc=DosAllocMem((PVOID)&Data,Size+1,fALLOC);
      if (rc==0) {
         USHORT Cell,CellSize,Offset;
         CellSize= pVio->CellSize;
         Offset=0L;
         for (Cell=0;Cell<CellSize*pVio->Rows*pVio->Cols;Cell+=CellSize) {
            Data[Offset]=pVio->LVB[Cell];
            Offset++;
         } /* endfor */
         Data[Size]=0x00; /* NUll terminate string for strstr */
         if (strstr(Data,WaitString)) {
            Waiting   =FALSE;
            hWait=0;
            DosOpenEventSem(WAITSEM,&hWait);
            DosPostEventSem(hWait);
            DosCloseEventSem(hWait);
            hWait=0;
         } /* endif */
         DosFreeMem(Data);
      } /* endif */
   } /* endif */
}
/*--------------------------------------------------------------------*/
/*- Intercept Graphic engine Functions with 5 parameters           ---*/
ULONG APIENTRY HookedGre32Entry5(HDC hDC,...) {
   va_list   args;
   ULONG     b,c,d;
   ULONG     Fct;
   USHORT    Index;
   CHARRECT *pRect;
   CHARSTR  *pStr;
   int i;
   APIRET rc;
   ULONG  Size;
   ULONG  Attr;
   FILE    *Test;                           /* Just for debug purpose */
   /*-----------------------------------------------------------------*/
   /* Get all the parameters -----------------------------------------*/
   va_start(args,hDC);
   b  =va_arg(args,ULONG);
   c  =va_arg(args,ULONG);
   d  =va_arg(args,ULONG);
   Fct=va_arg(args,ULONG);
   va_end(args);
   /*-----------------------------------------------------------------*/
   /* Switch depending on Function -----------------------------------*/
   switch (Fct&0x0000FFFFL) {
      /*-----------------------------------------------------------------*/
      /* PM Gpi Char String operation -----------------------------------*/
      case NGreCharString:
         if (CheckSpying()) {
             PmGpiMatch((UCHAR *)c,b);
         } /* endif */
         break;
      /*-----------------------------------------------------------------*/
      /* VIO Char String operation    -----------------------------------*/
      case NGreCharStr:
         if (CheckSpying()) { /* If we are spying for that process/thread */
             VioMatch((VioPs *)b);
         } /* endif */
         break;
      /*-----------------------------------------------------------------*/
      /* VIO Char Rect operation      -----------------------------------*/
      case NGreCharRect:
         if (CheckSpying()) { /* If we are spying for that process/thread */
             VioMatch((VioPs *)b);
         } /* endif */
         break;
      default:
         break;
   } /* endswitch */
   /*-----------------------------------------------------------------*/
   /* Get Function index and call default handling -------------------*/
   Index=(USHORT)(Fct&0x000000FFL);
   return (*GreEntries[Index])(hDC,b,c,d,Fct);
}
/*--------------------------------------------------------------------*/
/*- Intercept Graphic engine Functions with 8 parameters          ---*/
ULONG APIENTRY HookedGre32Entry8(HDC hDC,...) {
   va_list args;
   ULONG b,c,d,e,f,g;
   ULONG Fct;
   USHORT Index;
   /*-----------------------------------------------------------------*/
   /* Get all the parameters -----------------------------------------*/
   va_start(args,hDC);
   b  =va_arg(args,ULONG);
   c  =va_arg(args,ULONG);
   d  =va_arg(args,ULONG);
   e  =va_arg(args,ULONG);
   f  =va_arg(args,ULONG);
   g  =va_arg(args,ULONG);
   Fct=va_arg(args,ULONG);
   va_end(args);
   /*-----------------------------------------------------------------*/
   /* Switch depending on Function -----------------------------------*/
   /* Would need SetupDC to correlate window with HDC clip rectangle--*/
   /* To be sure we are getting the right text                      --*/
// if ((Fct&0x0000FFFF)==NGreSetupDC) {
//    if (CheckSpying()) { /* If we are spying for that process/thread */
//    } /* endif */
// } /* endif */
   /*-----------------------------------------------------------------*/
   /* Get Function index and call default handling -------------------*/
   Index=(USHORT)(Fct&0x000000FFL);
   return (*GreEntries[Index])(hDC,b,c,d,e,f,g,Fct);
}
/*--------------------------------------------------------------------*/
/*- Intercept Graphic engine Functions with 10 parameters          ---*/
ULONG APIENTRY HookedGre32Entry10(HDC hDC,...) {
   va_list args;
   ULONG b,c,d,e,f,g,h,i;
   ULONG Fct;
   USHORT Index;
   /*-----------------------------------------------------------------*/
   /* Get all the parameters -----------------------------------------*/
   va_start(args,hDC);
   b  =va_arg(args,ULONG);
   c  =va_arg(args,ULONG);
   d  =va_arg(args,ULONG);
   e  =va_arg(args,ULONG);
   f  =va_arg(args,ULONG);
   g  =va_arg(args,ULONG);
   h  =va_arg(args,ULONG);
   i  =va_arg(args,ULONG);
   Fct=va_arg(args,ULONG);
   va_end(args);
   /*-----------------------------------------------------------------*/
   /* Switch depending on Function -----------------------------------*/
   if ((Fct&0x0000FFFF)==NGreCharStringPos) {
      if (CheckSpying()) { /* If we are spying for that process/thread */
         PmGpiMatch((UCHAR *)f,e);
      } /* endif */
   } /* endif */
   /*-----------------------------------------------------------------*/
   /* Get Function index and call default handling -------------------*/
   Index=(USHORT)(Fct&0x000000FFL);
   return (*GreEntries[Index])(hDC,b,c,d,e,f,g,h,i,Fct);
}
APIRET APIENTRY WaitForString(PID WinPid,PUCHAR String) {
   APIRET rc;
   hWait=0;
   strncpy(WaitString,String,STRINGSIZE);
   WaitString[STRINGSIZE]=0x00;
   rc=DosCreateEventSem(WAITSEM,&hWait,0,FALSE);
   if (rc!=NO_ERROR) {
      return rc;
   } /* endif */
   Waiting   =TRUE;
   SpiedPid  = WinPid;
   /* Now wait until the string appears */
   rc=DosWaitEventSem(hWait,SEM_INDEFINITE_WAIT);
   Waiting   =FALSE;
   DosCloseEventSem(hWait);
   hWait=0;
   return rc;
}
