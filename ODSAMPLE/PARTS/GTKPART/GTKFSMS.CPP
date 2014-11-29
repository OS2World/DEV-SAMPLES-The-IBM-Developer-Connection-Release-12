
#define VARIABLE_MACROS

#ifndef _EXCEPT_
#include "Except.h"
#endif

#ifndef GraphicsPartMethodDebug
   #define GraphicsPartMethodDebug(c,m) SOMMethodDebug(c,m)
#endif

#define INCL_DOSMODULEMGR
#define INCL_DOSRESOURCES
#define INCL_DOSPROCESS
#define INCL_GPIBITMAPS
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES
#define INCL_GPIREGIONS
#define INCL_GPITRANSFORMS
#define INCL_WINACCELERATORS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINMESSAGEMGR
#define INCL_WINPOINTERS
#define INCL_WINSTDDRAG
#define INCL_WINTRACKRECT
#define INCL_WINWINDOWMGR
#define INCL_WINERRORS
#define INCL_DEV

#define INCL_ODAPI
#define INCL_ODARBITRATOR
#define INCL_ODCANVAS
#define INCL_ODDRAGANDDROP
#define INCL_ODDRAFT
#define INCL_ODERRORS
#define INCL_ODFACET
#define INCL_ODFOCUSSET
#define INCL_ODFRAME
#define INCL_ODLINK
#define INCL_ODLINKSOURCE
#define INCL_ODLINKSPEC
#define INCL_ODMENUBAR
#define INCL_ODSESSION
#define INCL_ODSEMANTICINTERFACE
#define INCL_ODSHAPE
#define INCL_ODSTORAGEUNIT
#define INCL_ODSTORAGEUNITVIEW
#define INCL_ODTRANSFORM
#define INCL_ODUI
#define INCL_ODUNDO
#define INCL_ODWINDOW
#define INCL_ODWINDOWSTATE
#include <os2.h>

#include "gtkpart.xih"

#ifndef SOM_XEmbeddedFramesIterator_xh
   #include <XFrmIter.xh>
#endif

#ifndef _ORDCOLL_
#include "OrdColl.h"
#endif

#include "debug.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <builtin.h>
#include <string.h>

#include "gtkbase.h"
#include "gtkpic.h"
#include "gtkres.h"
#include "actions.h"
#include "platform.h"


// ------------------------------------------------------------------
//   some useful utilities...                                     aaa
// ------------------------------------------------------------------

#define CAPTURE_MOUSE                                                          \
   _fSession->GetArbitrator(ev)->RequestFocus(ev, _fMouseFocus, _fCurFrame);   \
   WinSetCapture(HWND_DESKTOP,_hwndObj)

#define FREE_MOUSE                                                             \
   WinSetCapture(HWND_DESKTOP,NULLHANDLE);                                     \
   _fSession->GetArbitrator(ev)->RelinquishFocus(ev, _fMouseFocus, _fCurFrame)

//---------------------------------------------------------------------------
SHORT  msg2action( ULONG msg)
{
  SHORT value;

  switch (msg) {
    case WM_MOUSEMOVE:
      value = FSA_MOVE;
      break;
    case WM_BUTTON1CLICK:
      value = FSA_MB1CLICK;
      break;
    case WM_BEGINSELECT :
      value = FSA_BEGINSELECT;
      break;
    case WM_ENDSELECT :
      value = FSA_ENDSELECT;
      break;
    case WM_SINGLESELECT :
//      value = FSA_SINGLESELECT;
      value = FSA_MB1CLICK;
      break;
    case WM_BEGINDRAG :
      value = FSA_BEGINDRAG;
      break;
    case WM_ENDDRAG :
      value = FSA_ENDDRAG;
      break;
    case WM_BUTTON2DOWN:
      value = FSA_MB2DOWN;
      break;
    case WM_BUTTON1DBLCLK:
      value = FSA_MB1DBLCLK;
      break;
#if 0
    case WM_BUTTON1DOWN:
      value = FSA_MB1DOWN;
      break;
    case WM_BUTTON1UP:
      value = FSA_MB1UP;
      break;
    case WM_BUTTON2UP:
      value = FSA_MB2UP;
      break;
    case WM_BUTTON2DBLCLK:
      value = FSA_MB2DBLCLK;
      break;
#endif
    default:
      value = 0;
      break;
  } /* endswitch */
  return value;
}

// --------------------------------------------------------------------------
void shadow_hps( HPS hps )
{
  GpiSetLineWidth( hps, LINEWIDTH_NORMAL );
  GpiSetMix( hps, FM_XOR );
  GpiSetColor( hps, RGB_WHITE );
}

// --------------------------------------------------------------------------
void SetSizingPointer( LONG handle )
{
  SHORT ptr = 0;
  switch ( handle ) {
    case 0:
    case 7:
      ptr = SPTR_SIZENESW;
      break;
    case 2:
    case 5:
      ptr = SPTR_SIZENWSE;
      break;
    case 1:
    case 6:
      ptr = SPTR_SIZENS;
      break;
    case 3:
    case 4:
      ptr = SPTR_SIZEWE;
      break;
  }
  if (ptr) SetSysPointer( ptr );
}

// --------------------------------------------------------------------------
VOID adjust_line_pointer ( PPOINTL pptl0, PPOINTL pptl1 )
{
   LONG dx, dy;
   dx = labs(pptl1->x - pptl0->x);
   dy = labs(pptl1->y - pptl0->y);
   if ( dx >= dy ) pptl1->y = pptl0->y;
   else pptl1->x = pptl0->x;
   }

//---------------------------------------------------------------------------
BOOL draw_box( HPS hps, PPOINTL ptl1, PPOINTL ptl2 )
{
   GpiMove( hps, ptl1 );
   return GpiBox( hps, DRO_OUTLINE, ptl2, 0L, 0L ) != GPI_ERROR;
   }

// ------------------------------------------------------------------
//   STATE MACHINES                                               aaa
// ------------------------------------------------------------------

typedef struct {
        POINTL   anchorpt, lastpt;
        BOOL     fMode;
        BOOL     fDrawn;
        } FSMLINEDATA, *PFSMLINEDATA;

// ------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartfsm_select(GraphicsPart *somSelf,
                                                     Environment *ev,
                                                    short action,
                                                    POINTL* pptl,
                                                    ODULong flags)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_select");

  PFSMLINEDATA   pLD = (PFSMLINEDATA)_fsmData;
  ODBoolean      handled = FALSE;

  SOM_TRY

  switch ( action ) {
    case FSA_MOVE:
      if ( _fsmState &&
       (!pLD->fDrawn || pLD->lastpt.x != pptl->x || pLD->lastpt.y != pptl->y) ) {
        if ( pLD->fDrawn ) {
          if ( !draw_box( _fsmHps, &pLD->anchorpt, &pLD->lastpt ) ) break;
          pLD->fDrawn = FALSE;
        }
        if ( draw_box( _fsmHps, &pLD->anchorpt, pptl ) ) {
          pLD->lastpt = *pptl;
          pLD->fDrawn = TRUE;
        }
      }

      if (!_fReadOnly) {
        SetPointer( _ptrXhair );
        handled = TRUE;
      }
      break;

    case FSA_INIT:
      handled = TRUE;
      break;

    case FSA_ENTER :
//    case FSA_SINGLESELECT :
    case FSA_MB1CLICK :
      if (_picture->ObjectCount()) {
        ULONG mode;
        if (FLAGS_CONTROL)  mode = SELECT_TOGGLEONE;
        else if (FLAGS_SHIFT) mode = SELECT_NEXTONE;
        else mode = SELECT_ONLYONE;

        somSelf->HighlightSelection(ev, 0, 0);
        _picture->Select(_fsmHps, pptl, mode);
        somSelf->ClipEmbeds( ev, 0, 0 );
        _fAltMark = FALSE;   // FLAGS_ALT;
        somSelf->HighlightSelection(ev, 0, 0);

        handled = TRUE;
      }
      break;
    case FSA_MB2DOWN :
      if (_fsmState) {
        somSelf->fsm_select( ev, FSA_RESET, pptl, flags );
        _fMB2handled = handled = TRUE;
      }
      break;
    case FSA_BEGINSELECT :
      if (_picture->ObjectCount()) {
        switch (_fsmState) {
          case 0:
            _fsmData = pLD = (PFSMLINEDATA)malloc( sizeof(FSMLINEDATA) );
            pLD->anchorpt = *pptl;
            pLD->fDrawn = FALSE;
            _fsmState = 1;
            shadow_hps( _fsmHps );
            GpiSetLineType( _fsmHps, LINETYPE_SHORTDASH );
            CAPTURE_MOUSE;
            break;
          default:
            somSelf->fsm_select( ev, FSA_RESET, pptl, flags );
            break;
          }
        handled = TRUE;
      }
      break;

    case FSA_ENDSELECT :
      if (_fsmState) {
        if ( pLD->fDrawn ) {
          draw_box( _fsmHps, &pLD->anchorpt, &pLD->lastpt );
          pLD->fDrawn = FALSE;
          }
        if ( pLD->anchorpt.x != pptl->x && pLD->anchorpt.y != pptl->y ) {
          ULONG mode;
          if (FLAGS_CONTROL)  mode = SELECT_TOGGLEONE;
          else mode = SELECT_ONLYONE;

          somSelf->HighlightSelection(ev, 0, 0);

          _picture->Select(_fsmHps, &pLD->anchorpt, pptl, mode);

          somSelf->ClipEmbeds( ev, 0, 0 );
          _fAltMark = FALSE;   // FLAGS_ALT;
          somSelf->HighlightSelection(ev, 0, 0);
        }
        else somSelf->fsm_select( ev, FSA_MB1CLICK, pptl, flags );

        somSelf->fsm_select( ev, FSA_RESET, pptl, flags );
        handled = TRUE;
      }
      break;

    case FSA_RESET:
      if ( _fsmState ) {
        FREE_MOUSE;
        if ( pLD->fDrawn )
          draw_box( _fsmHps, &pLD->anchorpt, &pLD->lastpt );
        _fsmState = 0;
        _fsmData = NULL;
        free(pLD);
        somSelf->DisplayResourceText(ev, 0, _fCurFrame);
        handled = TRUE;
        }
      break;

    default: break;
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return handled;
}

//---------------------------------------------------------------------------
// leaves LINESTYLE up to calling routine!
//---------------------------------------------------------------------------
BOOL draw_line( HPS hps, PPOINTL ptl1, PPOINTL ptl2 )
{
   GpiMove( hps, ptl1 );
   return GpiLine( hps, ptl2 ) != GPI_ERROR;
   }

//---------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartfsm_line(GraphicsPart *somSelf,
                                                   Environment *ev,
                                                  short action,
                                                  POINTL* pptl,
                                                  ODULong flags)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_line");

  PFSMLINEDATA   pLD = (PFSMLINEDATA)_fsmData;
  BOOL           handled = FALSE;

  SOM_TRY

  // horz/vert constraint
  if ( _fsmState && pptl && FLAGS_SHIFT)
//  if ( _fsmState && pptl && FLAGS_SHIFT && !fCreatingLink)
    adjust_line_pointer ( &pLD->anchorpt, pptl );

  switch ( action ) {
    case FSA_MOVE:
      if ( _fsmState &&
      (!pLD->fDrawn || pLD->lastpt.x != pptl->x || pLD->lastpt.y != pptl->y) ) {
        if ( pLD->fDrawn ) {
          if ( !draw_line( _fsmHps, &pLD->anchorpt, &pLD->lastpt ) )
            break;
          pLD->fDrawn = FALSE;
          }
        if ( draw_line( _fsmHps, &pLD->anchorpt, pptl ) ) {
          pLD->lastpt = *pptl;
          pLD->fDrawn = TRUE;
          }
        }
      SetPointer( _ptrLine );
      handled = TRUE;
      break;

    case FSA_INIT:
      if (!_ptrLine) _ptrLine = LoadPointer( _hmod, ID_LINEPTR );
      handled = TRUE;
      break;

    case FSA_PAINT:
      if ( _fsmState && pLD->fDrawn ) {
         // redraw rubber box
         draw_line( _fsmHps, &pLD->anchorpt, &pLD->lastpt );
         handled = TRUE;
         }
      break;

    case FSA_ENTER :
    case FSA_MB1CLICK :
      {
      switch (_fsmState) {
        case 0:
          _fsmData = pLD = (PFSMLINEDATA)malloc( sizeof(FSMLINEDATA) );
          pLD->anchorpt = *pptl;
          pLD->fDrawn = FALSE;

          _fsmState = 1;
          shadow_hps( _fsmHps );
          somSelf->DisplayResourceText(ev, 5, _fCurFrame); // 124281
          GpiSetLineType( _fsmHps, LINETYPE_SOLID );
          CAPTURE_MOUSE;
          break;
        case 1:
          {
          if ( pLD->anchorpt.x == pptl->x && pLD->anchorpt.y == pptl->y ) {
            DosBeep(200,100);
            break;
          }

          GtkLine *obj = new GtkLine(pLD->anchorpt,*pptl);
          somSelf->AddObject( ev, obj );

          somSelf->fsm_line( ev, FSA_RESET, pptl, flags );
          }
          break;
        default: break;
        } /* endswitch */
      handled = TRUE;
      }
      break;

    case FSA_RESET:
    case FSA_MB2DOWN:
      if ( _fsmState ) {
        FREE_MOUSE;
        if ( pLD->fDrawn )
          draw_line( _fsmHps, &pLD->anchorpt, &pLD->lastpt );
        _fsmState = 0;
        _fsmData = NULL;
        free(pLD);
        somSelf->DisplayResourceText(ev, 0, _fCurFrame);
        _fMB2handled = TRUE;                                 // disable POPUP!
        handled = TRUE;
      }

      #ifdef GTKLINKING
        if (fCreatingLink) {
          _fsmType = _fsmLastType;
          fCreatingLink = FALSE;
        }
      #endif

      break;

    default: break;
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return handled;
}

// --------------------------------------------------------------------------
VOID adjust_box_pointer( PPOINTL pptl0, PPOINTL pptl1 )
{
   LONG dx, dy;
   dx = pptl1->x - pptl0->x;
   dy = pptl1->y - pptl0->y;
   if ( labs(dx) >= labs(dy) )
        pptl1->x = pptl0->x + (dx>0 ? labs(dy) : -labs(dy));
   else pptl1->y = pptl0->y + (dy>0 ? labs(dx) : -labs(dx));
}

#define SET_DEF_ATTRIBUTE( obj, attrib )                                     \
            obj->SetAttribute( attrib, _picture->QueryDefAttribute(attrib) )

//---------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartfsm_box(GraphicsPart *somSelf,
                                                  Environment *ev,
                                                 short action,
                                                 POINTL* pptl,
                                                 ODULong flags)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_box");

  PFSMLINEDATA   pLD = (PFSMLINEDATA)_fsmData;
  BOOL           handled = FALSE;

  SOM_TRY

  // horz/vert constraint
  if ( _fsmState && pptl && FLAGS_SHIFT)
    adjust_box_pointer ( &pLD->anchorpt, pptl );

  switch ( action ) {
    case FSA_MOVE:
      if ( _fsmState &&
      (!pLD->fDrawn || pLD->lastpt.x != pptl->x || pLD->lastpt.y != pptl->y) ) {
        if ( pLD->fDrawn ) {
          if ( !draw_box( _fsmHps, &pLD->anchorpt, &pLD->lastpt ) )
            break;
          pLD->fDrawn = FALSE;
          }
        if ( draw_box( _fsmHps, &pLD->anchorpt, pptl ) ) {
          pLD->lastpt = *pptl;
          pLD->fDrawn = TRUE;
          }
        }
      SetPointer( _ptrBox );
      handled = TRUE;
      break;

    case FSA_INIT:
      if (!_ptrBox) _ptrBox = LoadPointer( _hmod, ID_BOXPTR );
      handled = TRUE;
      break;

    case FSA_PAINT:
      if ( _fsmState && pLD->fDrawn ) {
         // redraw rubber box
         draw_box( _fsmHps, &pLD->anchorpt, &pLD->lastpt );
         handled = TRUE;
         }
      break;

    case FSA_ENTER :
    case FSA_MB1CLICK :
      {
      switch (_fsmState) {
        case 0:
          _fsmData = pLD = (PFSMLINEDATA)malloc( sizeof(FSMLINEDATA) );
          pLD->anchorpt = *pptl;
          pLD->fDrawn = FALSE;
          _fsmState = 1;
          shadow_hps( _fsmHps );
          somSelf->DisplayResourceText(ev, _fsmType==ACTION_BOX?9:6, _fCurFrame);
          GpiSetLineType( _fsmHps, LINETYPE_SOLID );
          CAPTURE_MOUSE;
          break;
        case 1:
          {
          if ( pLD->anchorpt.x == pptl->x && pLD->anchorpt.y == pptl->y ) {
            DosBeep(200,100);
            break;
            }
          GtkBox *obj = new GtkBox(pLD->anchorpt,*pptl);
          if (_fsmType==ACTION_BOX) {
            obj->SetAttribute( LS_COLOR, RGB_BLACK );
            obj->SetAttribute( FS_COLOR, RGB_WHITE );
            obj->SetAttribute( FS_STYLE, PATSYM_SOLID );
            obj->SetAttribute( SHADOW_STYLE, PATSYM_HALFTONE );
            obj->SetRounding(20,20);
          } else {
            SET_DEF_ATTRIBUTE( obj, LS_COLOR );
            SET_DEF_ATTRIBUTE( obj, LS_STYLE );
            SET_DEF_ATTRIBUTE( obj, LS_WIDTH );
            SET_DEF_ATTRIBUTE( obj, FS_COLOR );
            SET_DEF_ATTRIBUTE( obj, FS_STYLE );
            SET_DEF_ATTRIBUTE( obj, SHADOW_STYLE );
          }
          somSelf->AddObject( ev, obj );

          somSelf->fsm_box( ev, FSA_RESET, pptl, flags );
          }
          break;
        default: break;
        } /* endswitch */
      handled = TRUE;
      }
      break;

    case FSA_RESET:
    case FSA_MB2DOWN:
      if ( _fsmState ) {
        FREE_MOUSE;
        if ( pLD->fDrawn )
          draw_box( _fsmHps, &pLD->anchorpt, &pLD->lastpt );
        _fsmState = 0;
        _fsmData = NULL;
        free(pLD);
        somSelf->DisplayResourceText(ev, 0, _fCurFrame);
        _fMB2handled = TRUE;                                 // disable POPUP!
        handled = TRUE;
        }
      break;

    default: break;
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return handled;
}

//---------------------------------------------------------------------------
// ELLIPSE fsm stuff
//---------------------------------------------------------------------------
BOOL draw_ellipse( HPS hps, PPOINTL ptl1, PPOINTL ptl2 )
{
  POINTL ptl;
  ptl.x = 2*ptl1->x - ptl2->x;
  ptl.y = 2*ptl1->y - ptl2->y;
  GpiMove( hps, &ptl );
  return GpiBox( hps, DRO_OUTLINE, ptl2,
                 abs(ptl2->x - ptl.x),
                 abs(ptl2->y - ptl.y) ) != GPI_ERROR;
   }

//---------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartfsm_ellipse(GraphicsPart *somSelf,
                                                      Environment *ev,
                                                     short action,
                                                     POINTL* pptl,
                                                     ODULong flags)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_ellipse");

  PFSMLINEDATA   pLD = (PFSMLINEDATA)_fsmData;
  BOOL           handled = FALSE;

  SOM_TRY

  // horz/vert constraint
  if ( _fsmState && pptl && FLAGS_SHIFT)
    adjust_box_pointer ( &pLD->anchorpt, pptl );

  switch ( action ) {
    case FSA_MOVE:
      if ( _fsmState &&
       (!pLD->fDrawn || pLD->lastpt.x != pptl->x || pLD->lastpt.y != pptl->y) ) {
        if ( pLD->fDrawn ) {
          if ( !draw_ellipse( _fsmHps, &pLD->anchorpt, &pLD->lastpt ) )
            break;
          pLD->fDrawn = FALSE;
          }
        if ( draw_ellipse( _fsmHps, &pLD->anchorpt, pptl ) ) {
          pLD->lastpt = *pptl;
          pLD->fDrawn = TRUE;
          }
        }
      SetPointer( _ptrEllipse );
      handled = TRUE;
      break;

    case FSA_INIT:
      if (!_ptrEllipse)
        _ptrEllipse = LoadPointer( _hmod, ID_ELLIPSEPTR );
      handled = TRUE;
      break;

    case FSA_PAINT:
      if ( _fsmState && pLD->fDrawn ) {
         // redraw rubber box
         draw_ellipse( _fsmHps, &pLD->anchorpt, &pLD->lastpt );
         handled = TRUE;
         }
      break;

    case FSA_ENTER :
    case FSA_MB1CLICK :
      {
      switch (_fsmState) {
        case 0:
          _fsmData = pLD = (PFSMLINEDATA)malloc( sizeof(FSMLINEDATA) );
          pLD->anchorpt = *pptl;
          pLD->fDrawn = FALSE;
          _fsmState = 1;
          shadow_hps( _fsmHps );
          GpiSetLineType( _fsmHps, LINETYPE_SOLID );
          CAPTURE_MOUSE;
          somSelf->DisplayResourceText(ev, 7, _fCurFrame);
          break;
        case 1:
          {
          if ( pLD->anchorpt.x == pptl->x && pLD->anchorpt.y == pptl->y ) {
            DosBeep(200,100);
            break;
            }

          GtkEllipse *obj = new GtkEllipse(pLD->anchorpt,*pptl);
          somSelf->AddObject( ev, obj );

//          pLD->fDrawn = FALSE;
          somSelf->fsm_ellipse( ev, FSA_RESET, pptl, flags );
          }
          break;
        default: break;
        } /* endswitch */
      handled = TRUE;
      }
      break;

    case FSA_RESET:
    case FSA_MB2DOWN:
      if ( _fsmState ) {
        FREE_MOUSE;
        if ( pLD->fDrawn )
          draw_ellipse( _fsmHps, &pLD->anchorpt, &pLD->lastpt );
        _fsmState = 0;
        _fsmData = NULL;
        free(pLD);
        somSelf->DisplayResourceText(ev, 0, _fCurFrame);
        _fMB2handled = TRUE;                                 // disable POPUP!
        handled = TRUE;
        }
      break;

    default: break;
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return handled;
}

//---------------------------------------------------------------------------
typedef struct {
        GtkPolyline *poly;
        PPOINTL      ptls;
        ULONG        n;
        BOOL         fMode;
        BOOL         fDrawn;
        } FSMPLINEDATA, *PFSMPLINEDATA;

//---------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartfsm_polyline(GraphicsPart *somSelf,
                                                       Environment *ev,
                                                      short action,
                                                      POINTL* pptl,
                                                      ODULong flags)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_polyline");

  #define MAX_PTS 100
  PFSMPLINEDATA   pPLD = (PFSMPLINEDATA)_fsmData;
  BOOL           handled = FALSE;
  ULONG          n;
  PPOINTL        ptls;

  SOM_TRY

  // horz/vert constraint
  if ( _fsmState ) {
    n = pPLD->n;
    ptls = pPLD->ptls;
    if ( pptl && FLAGS_SHIFT)
      adjust_line_pointer ( &ptls[n-1], pptl );
  }

  switch ( action ) {
    case FSA_MOVE:
      if ( _fsmState &&
      (!pPLD->fDrawn || ptls[n].x != pptl->x || ptls[n].y != pptl->y) ) {
        if ( pPLD->fDrawn ) {
          if ( !draw_line( _fsmHps, &ptls[n-1], &ptls[n] ) )   break;
          pPLD->fDrawn = FALSE;
          }
        if ( draw_line( _fsmHps, &ptls[n-1], pptl ) ) {
          ptls[n] = *pptl;
          pPLD->fDrawn = TRUE;
          }
        }
      SetPointer( _ptrPolyline );
      handled = TRUE;
      break;

    case FSA_INIT:
      if (!_ptrPolyline)
        _ptrPolyline = LoadPointer( _hmod, ID_POLYPTR );
      handled = TRUE;
      break;

    case FSA_PAINT:
      if ( _fsmState ) {
        if ( pPLD->fDrawn )
          draw_line( _fsmHps, &ptls[n-1], &ptls[n] );
        for (int i=1; i<n; i++)
          draw_line( _fsmHps, &ptls[i-1], &ptls[i] );
      }
      break;

    case FSA_ENTER :
    case FSA_MB1CLICK :
      switch (_fsmState) {
        case 0:
          _fsmData = pPLD = (PFSMPLINEDATA)malloc( sizeof(FSMPLINEDATA) );
          pPLD->ptls = new POINTL[MAX_PTS];
          pPLD->ptls[0] = *pptl;
          pPLD->n = 1;
          pPLD->fDrawn = FALSE;
          _fsmState = 1;

          pPLD->poly = new GtkPolyline(1,pPLD->ptls);                  // zzz

          somSelf->HighlightSelection(ev, 0, 0);

          _picture->AddObject( pPLD->poly );

          _picture->Select();
          somSelf->ClipEmbeds( ev, 0, 0 );
          _fAltMark = FALSE;

          shadow_hps( _fsmHps );
          GpiSetLineType( _fsmHps, LINETYPE_SOLID );
          CAPTURE_MOUSE;
          somSelf->DisplayResourceText(ev, 8, _fCurFrame);
          break;
        case 1:
          if ( ptls[n-1].x == pptl->x && ptls[n-1].y == pptl->y )
            somSelf->fsm_polyline( ev, FSA_MB1DBLCLK, pptl, flags );
          else {
            pPLD->ptls[n] = *pptl;
            pPLD->n += 1;
            pPLD->poly->SetPts(pPLD->n,pPLD->ptls);                    // zzz
            pPLD->fDrawn = FALSE;
          }
          break;
        }
      handled = TRUE;
      break;

    case FSA_MB1DBLCLK:
      if ( _fsmState ) {
        GtkObject* obj = (GtkObject*)pPLD->poly;
        RECTL rcl = obj->Bounds();
        _picture->Select( obj );

        UndoCreate* uc = new UndoCreate();
        _picture->CreateUndoStruct( UNDOCLS_CREATE, (PVOID*)&uc );
        LogUndoAction( somSelf, ev, uc, sizeof(*uc), _fSession, "create object" );

        somSelf->ClipEmbeds( ev, 0, &rcl );
        _fAltMark = FALSE;
        somSelf->InvalidateObject( ev, obj );
        somSelf->PartUpdated( ev, 0, 0 );

        pPLD->fDrawn = FALSE;
        pPLD->poly = 0;
        somSelf->fsm_polyline( ev, FSA_RESET, pptl, flags );
        handled = TRUE;
      }
      break;

    case FSA_MB2DOWN:
      if ( _fsmState ) {
        _fMB2handled = TRUE;                                 // disable POPUP!
        if ( pPLD->fDrawn )
          draw_line( _fsmHps, &pPLD->ptls[n-1], &pPLD->ptls[n] );
        if (n>1) {
          pPLD->n -= 1;
          pPLD->poly->SetPts(pPLD->n,pPLD->ptls);                    // zzz
          pPLD->fDrawn = TRUE;
        } else {
          pPLD->fDrawn = FALSE;
          somSelf->fsm_polyline( ev, FSA_RESET, pptl, flags );
        }
        handled = TRUE;
      }
      break;

    case FSA_RESET:
      if ( _fsmState ) {
        FREE_MOUSE;
        somSelf->fsm_polyline( ev, FSA_PAINT, pptl, flags );       // erases it
        _fsmState = 0;
        _fsmData = NULL;
        delete ptls;
        if (pPLD->poly)  _picture->Delete( pPLD->poly );
        free(pPLD);
        somSelf->DisplayResourceText(ev, 0, _fCurFrame);
        handled = TRUE;
      }
      break;

    default: break;
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return handled;
}

//---------------------------------------------------------------------------
// ARC fsm stuff
//---------------------------------------------------------------------------
BOOL draw_fullarc( HPS hps, PFSMARCDATA pAD, PPOINTL p1, BOOL discrete )
{
  FIXED  fxA;
  PPOINTL p0 = &pAD->center;

  if (!get_angle( pAD, p1, discrete ) ) return FALSE;
  if (!hps) return TRUE;

  pAD->n = 38;
  pAD->ptls[0] = *p0;
  fxA = pAD->fxA1;
  for (int i=1; i<37; i++) {
    pAD->ptls[i] = angle_point( p0, fxA, pAD->r );
    fxA += MAKEFIXED(10,0);
  }
  pAD->ptls[37] = pAD->ptls[1];

  GpiMove( hps, pAD->ptls );
  return (GpiPolyLine( hps, pAD->n-1, pAD->ptls+1 ) != GPI_ERROR);
}

//---------------------------------------------------------------------------
BOOL draw_partarc( HPS hps, PFSMARCDATA pAD, PPOINTL p2, BOOL discrete )
{
  FIXED   fxA;
  PPOINTL p0 = &pAD->center;

  if (!calc_sweep( pAD, p2, discrete ) ) return FALSE;

  pAD->n = min( (pAD->fxA2 - pAD->fxA1) / MAKEFIXED(5,0) + 2, 74 );
  fxA = pAD->fxA1;

  for (int i=0; i<pAD->n-2; i++) {
    pAD->ptls[i] = angle_point( p0, fxA, pAD->r );
    fxA += MAKEFIXED(5,0);
  }
  pAD->ptls[pAD->n-2] = angle_point( p0, pAD->fxA2, pAD->r );
  pAD->ptls[pAD->n-1] = *p0;

  GpiMove( hps, pAD->ptls );
  return (GpiPolyLine( hps, pAD->n-1, pAD->ptls+1 ) != GPI_ERROR);
}

//---------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartfsm_arc(GraphicsPart *somSelf,
                                                  Environment *ev,
                                                 short action,
                                                 POINTL* pptl,
                                                 ODULong flags)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_arc");

  PFSMARCDATA   pAD = (PFSMARCDATA)_fsmData;
  BOOL          handled = FALSE;

  SOM_TRY

  switch ( action ) {
    case FSA_MOVE:
      if ( _fsmState && pAD->fDrawn ) {
        GpiMove( _fsmHps, pAD->ptls );
        if (GpiPolyLine( _fsmHps, pAD->n-1, pAD->ptls+1 ) == GPI_ERROR) break;
        pAD->fDrawn = FALSE;
      }
      switch (_fsmState) {
        case 1:
          if ( draw_fullarc( _fsmHps, pAD, pptl, FLAGS_SHIFT ) ) {
//            pAD->firstpt = *pptl;
            pAD->fDrawn = TRUE;
          }
          break;
        case 2:
          if ( draw_partarc( _fsmHps, pAD, pptl, FLAGS_SHIFT ) ) {
//            pAD->lastpt = *pptl;
            pAD->fDrawn = TRUE;
          }
          break;
        default:
          break;
      }
      SetPointer( _ptrArc );
      handled = TRUE;
      break;
    case FSA_INIT:
      if (!_ptrArc) _ptrArc = LoadPointer( _hmod, ID_ARCPTR );
      handled = TRUE;
      break;
#if 0
    case FSA_PAINT:
      if ( _fsmState && pLD->fDrawn ) {
         // redraw rubber box
         draw_ellipse( _fsmHps, &pLD->anchorpt, &pLD->lastpt );
         handled = TRUE;
         }
      break;
#endif
    case FSA_ENTER :
    case FSA_MB1CLICK :
      {
      switch (_fsmState) {
        case 0:
          _fsmData = pAD = (PFSMARCDATA)malloc( sizeof(FSMARCDATA) );
          pAD->center = *pptl;
          pAD->fDrawn = FALSE;
          _fsmState = 1;
          shadow_hps( _fsmHps );
          GpiSetLineType( _fsmHps, LINETYPE_SOLID );
          CAPTURE_MOUSE;
          somSelf->DisplayResourceText(ev, 10, _fCurFrame);
          break;
        case 1:
          if ( pAD->center.x == pptl->x && pAD->center.y == pptl->y ) {
            DosBeep(200,100);
            break;
          }
          if ( pAD->fDrawn ) {
            GpiMove( _fsmHps, pAD->ptls );
            GpiPolyLine( _fsmHps, pAD->n-1, pAD->ptls+1 );
            pAD->fDrawn = FALSE;
          }

          if (FLAGS_SHIFT) get_angle( pAD, pptl, TRUE );

          pAD->firstpt = *pptl;
          GpiMove( _fsmHps, &pAD->center );
          GpiLine( _fsmHps, &pAD->firstpt );
          _fsmState = 2;
          somSelf->DisplayResourceText(ev, 11, _fCurFrame);
          break;
        case 2:
          {
          // abort end if degenerate
          if ( pAD->center.x == pptl->x && pAD->center.y == pptl->y ) {
            DosBeep(200,100);
            break;
          }

          // calc the angles
          calc_sweep( pAD, pptl, FLAGS_SHIFT );
//          FIXED fxR = (FIXED)((double)pAD->r * 65.536);

          // create the object!
          GtkArc *obj = new GtkArc( pAD->center, pAD->r, pAD->fxA1, pAD->fxA2 );
          somSelf->AddObject( ev, obj );

//          pAD->fDrawn = FALSE;
          somSelf->fsm_arc( ev, FSA_RESET, pptl, flags );
          }
          break;
        default: break;
        } /* endswitch */
      handled = TRUE;
      }
      break;

    case FSA_RESET:
    case FSA_MB2DOWN:
      if ( _fsmState ) {
        FREE_MOUSE;
        if ( pAD->fDrawn ) {
          GpiMove( _fsmHps, pAD->ptls );
          GpiPolyLine( _fsmHps, pAD->n-1, pAD->ptls+1 );
        }
        if ( _fsmState==2 ) {
          GpiMove( _fsmHps, &pAD->center );
          GpiLine( _fsmHps, &pAD->firstpt );
        }
        _fsmState = 0;
        _fsmData = NULL;
        free(pAD);
        somSelf->DisplayResourceText(ev, 0, _fCurFrame);
        _fMB2handled = TRUE;                                 // disable POPUP!
        handled = TRUE;
      }
      break;

    default: break;
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return handled;
}

//---------------------------------------------------------------------------
typedef struct      {
        POINTL      ptlBase;
        MATRIXLF    mx;
        BOOL        fDrawn;
        GtkObject*  obj;
        } MOVECOPYDATA, *PMOVECOPYDATA;

//-------------------------------------------------------------------------
// Finite State Machine
//   state 0:   basepoint.  doing NOTHING.  Use this to reset FSM in app...
//   state 1:   entry point for move OR copy
//   state 2:   dragging objects, waiting for end point.
//
//   VK_CTRL  determines move or copy
//---------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartfsm_movecopy(GraphicsPart *somSelf,
                                                       Environment *ev,
                                                      short action,
                                                      POINTL* pptl,
                                                      ODULong flags)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_movecopy");

  PMOVECOPYDATA  mcd  = (PMOVECOPYDATA)_fsmData;
  BOOL           handled = FALSE;

  SOM_TRY

  switch ( action ) {
    case FSA_MOVE:
      if (_fsmState == 2) {

        // convert to Drag/Drop???
        {
          RECTL rclPic;
          _picture->QueryBounds(&rclPic);
          if ( !PointInRect(pptl,&rclPic) ) {
            somSelf->fsm_execute( ev, FSA_RESET, NULL );
            handled = TRUE;

            somSelf->InitDrag(ev, kODNULL);
            break;
          }
        }

        if ( mcd->fDrawn ) {
          // erase old objects
          GpiSetModelTransformMatrix( _fsmHps, 9, &mcd->mx, TRANSFORM_REPLACE );
          _picture->DrawShadows( _fsmHps, mcd->obj );
          }

        if (FLAGS_SHIFT) adjust_line_pointer ( &mcd->ptlBase, pptl );
        mcd->mx.lM31 = pptl->x - mcd->ptlBase.x;
        mcd->mx.lM32 = pptl->y - mcd->ptlBase.y;

        GpiSetModelTransformMatrix( _fsmHps, 9, &mcd->mx, TRANSFORM_REPLACE );
        _picture->DrawShadows( _fsmHps, mcd->obj );
//        GpiSetModelTransformMatrix( _fsmHps, 0, NULL, TRANSFORM_REPLACE );
        GpiSetModelTransformMatrix( _fsmHps, 9, &mx0, TRANSFORM_REPLACE );  // 143738
        mcd->fDrawn = TRUE;

        // show the delta on the status line
        char string[80], fmtstring[80];
        // copy or move??
        WinLoadString( (HAB)0, _hmod,  (FLAGS_CONTROL?13:12), 79, fmtstring);
        sprintf( string, fmtstring, mcd->mx.lM31, mcd->mx.lM32 );
        somSelf->DisplayText(ev, string, _fCurFrame);

        SetPointer( _ptrXhair );
        handled = TRUE;
      }
      break;
    case FSA_PAINT:
      if (_fsmState == 2) {
        if (mcd->fDrawn) {
          GpiSetModelTransformMatrix( _fsmHps, 9, &mcd->mx, TRANSFORM_REPLACE );
          _picture->DrawShadows( _fsmHps, mcd->obj );
//          GpiSetModelTransformMatrix( _fsmHps, 0, NULL, TRANSFORM_REPLACE );
          GpiSetModelTransformMatrix( _fsmHps, 9, &mx0, TRANSFORM_REPLACE );  // 143738
        }
        handled = TRUE;
      }
      break;
    case FSA_BEGINDRAG :
      if (_fsmState == 1) {
        _fsmData = mcd = new MOVECOPYDATA;
        mcd->fDrawn  = FALSE;
        mcd->ptlBase = *pptl;
        mcd->obj     = _picture->HitObject();

        shadow_hps( _fsmHps );
        GpiSetLineType( _fsmHps, LINETYPE_SOLID );
//        GpiSetModelTransformMatrix( _fsmHps, 0, NULL, TRANSFORM_REPLACE );
        GpiSetModelTransformMatrix( _fsmHps, 9, &mx0, TRANSFORM_REPLACE );  // 143738
        GpiQueryModelTransformMatrix( _fsmHps, 9, &mcd->mx );
        _fsmState = 2;
        CAPTURE_MOUSE;
        handled = TRUE;
      }
      break;
    case FSA_ENTER :
    case FSA_ENDDRAG :
      if (_fsmState == 2) {

        if (mcd->fDrawn) {
          GpiSetModelTransformMatrix( _fsmHps, 9, &mcd->mx, TRANSFORM_REPLACE );
          _picture->DrawShadows( _fsmHps, mcd->obj );
//          GpiSetModelTransformMatrix( _fsmHps, 0, NULL, TRANSFORM_REPLACE );
          GpiSetModelTransformMatrix( _fsmHps, 9, &mx0, TRANSFORM_REPLACE );  // 143738
          mcd->fDrawn = FALSE;
        }

        LONG dx, dy;
        if (FLAGS_SHIFT) adjust_line_pointer ( &mcd->ptlBase, pptl );
        dx = (pptl->x - mcd->ptlBase.x);
        dy = (pptl->y - mcd->ptlBase.y);

        if (!FLAGS_CONTROL && !dx && !dy) {               // NOP?
//          _fMB2handled = FALSE;                            // reset so can POPUP!
        } else {
          if (FLAGS_CONTROL) {                            // COPY op

            somSelf->HighlightSelection(ev, 0, 0);
            _picture->Copy();                              // copy objects

            // NOW we set up the undo from the newly-created selected set
            // of objects.  we are only saving the set of id's at this point.
            // prep COPY undo
            UndoCreate* uc = new UndoCreate();
            if ( !_picture->CreateUndoStruct( UNDOCLS_CREATE, (PVOID*)&uc ) ) {
              delete uc;
              somSelf->fsm_execute( ev, FSA_RESET, NULL );
              handled = TRUE;
            }

            LogUndoAction( somSelf, ev, uc, sizeof(*uc), _fSession, "copy" );

          } else {                                        // MOVE op

            // prep MOVE undo
            UndoMove* um = new UndoMove( dx, dy );
            if ( !_picture->CreateUndoStruct( UNDOCLS_MOVE, (PVOID*)&um ) ) {
              delete um;
              somSelf->fsm_execute( ev, FSA_RESET, NULL );
              handled = TRUE;
              break;
            }

            LogUndoAction( somSelf, ev, um, sizeof(*um), _fSession, "move" );

            somSelf->InvalidateObject( ev, 0 );
          }

          _picture->Move( dx, dy );

          somSelf->PartUpdated( ev, _fCurFrame, PUF_CLIPEMBEDS | PUF_REPAINT | PUF_UPDATELINKS );
        }
        somSelf->fsm_execute( ev, FSA_RESET, NULL );
        handled = TRUE;
      }
      break;
    case FSA_RESET:
      FREE_MOUSE;
      if (_fsmState == 2) {
        if (mcd->fDrawn) {
          GpiSetModelTransformMatrix( _fsmHps, 9, &mcd->mx, TRANSFORM_REPLACE );
          _picture->DrawShadows( _fsmHps );
//          GpiSetModelTransformMatrix( _fsmHps, 0, NULL, TRANSFORM_REPLACE );
          GpiSetModelTransformMatrix( _fsmHps, 9, &mx0, TRANSFORM_REPLACE );  // 143738
        }
        _fsmData = NULL;
        delete mcd;
      }
      _fsmState = 0;
//      _fsmType = ACTION_SELECT;
      _fsmType = _fsmLastType;
      somSelf->DisplayResourceText(ev, 0, _fCurFrame);
      handled = TRUE;                        // ALWAYS return TRUE here
      break;                                 // unless forbidding RESET...
    default: break;
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return handled;
}

//---------------------------------------------------------------------------
typedef struct      {
        POINTL      originalPt;
        GtkObject*  originalObj;
        GtkObject*  newObj;
        LONG        handle;
        BOOL        fDrawn;
        } PTEDITDATA, *PPTEDITDATA;

//-------------------------------------------------------------------------
// Finite State Machine
//   state 0:   basepoint.  doing NOTHING.  Use this to reset FSM in app...
//   state 1:   initialize
//   state 2:   dragging handle
//---------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartfsm_pointedit(GraphicsPart *somSelf,
                                                        Environment *ev,
                                                       short action,
                                                       POINTL* pptl,
                                                       ODULong flags)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_pointedit");

  PPTEDITDATA  ped  = (PPTEDITDATA)_fsmData;
  BOOL           handled = FALSE;

  SOM_TRY

  switch ( action ) {
    case FSA_MOVE:
      if (_fsmState == 2) {
        if ( ped->fDrawn ) ped->newObj->DrawShadow( _fsmHps );

        if (FLAGS_SHIFT) adjust_line_pointer ( &ped->originalPt, pptl );

        ped->newObj->SetHandle( ped->handle, *pptl );
        ped->newObj->DrawShadow( _fsmHps );
        ped->fDrawn = TRUE;

        SetPointer( _ptrXhair );
        handled = TRUE;
      }
      break;
    case FSA_PAINT:
      if (_fsmState == 2) {
        if ( ped->fDrawn ) ped->newObj->DrawShadow( _fsmHps );
        handled = TRUE;
      }
      break;
    case FSA_BEGINDRAG :
      if (_fsmState == 1) {
        _fsmData = ped = new PTEDITDATA;

        ped->fDrawn = FALSE;
        ped->originalPt = *pptl;
        ped->originalObj = _picture->HitObject( &ped->handle );
        ped->newObj = ped->originalObj->Copy(TRUE);
        shadow_hps( _fsmHps );
        GpiSetLineType( _fsmHps, LINETYPE_SOLID );
        _fsmState = 2;
        CAPTURE_MOUSE;
        handled = TRUE;
      }
      break;
    case FSA_ENTER :
    case FSA_ENDDRAG :
      if (_fsmState == 2) {
        RECTL rcl1, rcl2;
#if 1
        if ( ped->fDrawn ) {
          ped->newObj->DrawShadow( _fsmHps );
          ped->fDrawn = FALSE;
        }
#endif
        rcl1 = ped->originalObj->Bounds();

        somSelf->HighlightSelection(ev, 0, 0);
        somSelf->InvalidateObject( ev, ped->originalObj );
        ped->originalObj->SetHandle( ped->handle, *pptl, TRUE );
        _picture->BoundSelected();
        somSelf->HighlightSelection(ev, 0, 0);

        somSelf->PartUpdated( ev, _fCurFrame, PUF_CLEARUNDO | PUF_UPDATELINKS );

        rcl2 = ped->originalObj->Bounds();
        AddRect( &rcl2, &rcl1 );

        somSelf->ClipEmbeds( ev, 0, &rcl2 );
        somSelf->InvalidateObject( ev, ped->originalObj );

        ped->fDrawn = FALSE;
        somSelf->fsm_execute( ev, FSA_RESET, NULL );
        handled = TRUE;
      }
      break;
    case FSA_RESET:
      FREE_MOUSE;
      if (_fsmState == 2) {
        if ( ped->fDrawn ) ped->newObj->DrawShadow( _fsmHps );
        _fsmData = NULL;
        delete ped->newObj;
        delete ped;
      }
      _fsmState = 0;
      _fsmType = _fsmLastType;
      somSelf->DisplayResourceText(ev, 0, _fCurFrame);
      handled = TRUE;                        // ALWAYS return TRUE here
      break;                                 // unless forbidding RESET...
    default: break;
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return handled;
}

//---------------------------------------------------------------------------
typedef struct    {
        RECTL     rclBase;
        RECTL     rclNew;
        LONG      handle;
        MATRIXLF  mx;
        BOOL      fDrawn;
        } RESIZEDATA, *PRESIZEDATA;

//---------------------------------------------------------------------------
// ugly kludge to fix resizing anchor point... YUCK
//---------------------------------------------------------------------------
void adjust_rclBase( PRESIZEDATA rd )
{
  switch (rd->handle) {
    case 0 :                                   // lower-left
      rd->rclBase.xRight -= BOUNDS_BORDER;
      rd->rclBase.yTop -= BOUNDS_BORDER;
      break;
    case 1 :                                   // bottom-center
      rd->rclBase.yTop -= BOUNDS_BORDER;
      break;
    case 2 :                                   // lower-right
      rd->rclBase.xLeft += BOUNDS_BORDER;
      rd->rclBase.yTop -= BOUNDS_BORDER;
      break;
    case 3 :                                   // left side
      rd->rclBase.xRight -= BOUNDS_BORDER;
      break;
    case 4 :                                   // right side
      rd->rclBase.xLeft += BOUNDS_BORDER;
      break;
    case 5 :                                   // upper-left
      rd->rclBase.xRight -= BOUNDS_BORDER;
      rd->rclBase.yBottom += BOUNDS_BORDER;
      break;
    case 6 :                                   // top-center
      rd->rclBase.yBottom += BOUNDS_BORDER;
      break;
    case 7 :                                   // upper-right
      rd->rclBase.xLeft += BOUNDS_BORDER;
      rd->rclBase.yBottom += BOUNDS_BORDER;
      break;
    default:
      break;
  }
}

//---------------------------------------------------------------------------
// calc the new bounding rect and req'd scaling matrix
//---------------------------------------------------------------------------
void calc_resize( PRESIZEDATA rd, PPOINTL ptl, ULONG flags )
{
  switch (rd->handle) {
    case 0 :                                   // lower-left
      rd->rclNew.xLeft = ptl->x;
      rd->rclNew.yBottom = ptl->y;
      break;
    case 1 :                                   // bottom-center
      rd->rclNew.yBottom = ptl->y;
      break;
    case 2 :                                   // lower-right
      rd->rclNew.xRight = ptl->x;
      rd->rclNew.yBottom = ptl->y;
      break;
    case 3 :                                   // left side
      rd->rclNew.xLeft = ptl->x;
      break;
    case 4 :                                   // right side
      rd->rclNew.xRight = ptl->x;
      break;
    case 5 :                                   // upper-left
      rd->rclNew.xLeft = ptl->x;
      rd->rclNew.yTop = ptl->y;
      break;
    case 6 :                                   // top-center
      rd->rclNew.yTop = ptl->y;
      break;
    case 7 :                                   // upper-right
      rd->rclNew.xRight = ptl->x;
      rd->rclNew.yTop = ptl->y;
      break;
    default:
      break;
  }

  calc_matrix( &rd->rclNew, &rd->rclBase, &rd->mx );
}

//-------------------------------------------------------------------------
// Finite State Machine
//   state 0:   basepoint.  doing NOTHING.  Use this to reset FSM in app...
//   state 1:   entry point for resize op
//   state 2:   resizing objects, waiting for end point.
//
//   VK_ ...
//---------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartfsm_resize(GraphicsPart *somSelf,
                                                     Environment *ev,
                                                    short action,
                                                    POINTL* pptl,
                                                    ODULong flags)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_resize");

  PRESIZEDATA  rd  = (PRESIZEDATA)_fsmData;
  BOOL         handled = FALSE;

  SOM_TRY

  switch ( action ) {
    case FSA_MOVE:
      if (_fsmState == 2) {
        if ( rd->fDrawn ) {
          // erase old objects
          GpiSetModelTransformMatrix( _fsmHps, 9, &rd->mx, TRANSFORM_REPLACE );
          _picture->DrawShadows( _fsmHps );
//          GpiSetModelTransformMatrix( _fsmHps, 0, NULL, TRANSFORM_REPLACE );
          GpiSetModelTransformMatrix( _fsmHps, 9, &mx0, TRANSFORM_REPLACE );  // 143738
//          draw_box( _fsmHps, (PPOINTL)&rd->rclNew.xLeft, (PPOINTL)&rd->rclNew.xRight );
          }

        calc_resize( rd, pptl, flags );

        GpiSetModelTransformMatrix( _fsmHps, 9, &rd->mx, TRANSFORM_REPLACE );
        _picture->DrawShadows( _fsmHps );
//        GpiSetModelTransformMatrix( _fsmHps, 0, NULL, TRANSFORM_REPLACE );
        GpiSetModelTransformMatrix( _fsmHps, 9, &mx0, TRANSFORM_REPLACE );  // 143738
//        draw_box( _fsmHps, (PPOINTL)&rd->rclNew.xLeft, (PPOINTL)&rd->rclNew.xRight );
        rd->fDrawn = TRUE;

        SetSizingPointer( rd->handle );
        handled = TRUE;
      }
      break;
    case FSA_PAINT:
      if (_fsmState == 2) {
        if (rd->fDrawn) {
          GpiSetModelTransformMatrix( _fsmHps, 9, &rd->mx, TRANSFORM_REPLACE );
          _picture->DrawShadows( _fsmHps );
//          GpiSetModelTransformMatrix( _fsmHps, 0, NULL, TRANSFORM_REPLACE );
          GpiSetModelTransformMatrix( _fsmHps, 9, &mx0, TRANSFORM_REPLACE );  // 143738
//          draw_box( _fsmHps, (PPOINTL)&rd->rclNew.xLeft, (PPOINTL)&rd->rclNew.xRight );
        }
        handled = TRUE;
      }
      break;
    case FSA_BEGINDRAG :
      if (_fsmState == 1) {
        _fsmData = rd = new RESIZEDATA;
        rd->fDrawn = FALSE;
        _picture->HitObject( &rd->handle );
        rd->rclBase = *_picture->SelectedBounds();
        adjust_rclBase( rd );
        rd->rclNew = rd->rclBase;

        shadow_hps( _fsmHps );
        GpiSetLineType( _fsmHps, LINETYPE_SOLID );
//        GpiSetModelTransformMatrix( _fsmHps, 0, NULL, TRANSFORM_REPLACE );
        GpiSetModelTransformMatrix( _fsmHps, 9, &mx0, TRANSFORM_REPLACE );  // 143738
        GpiQueryModelTransformMatrix( _fsmHps, 9, &rd->mx );
        _fsmState = 2;

        if (_picture->SelectedCount()==1) {
          ULONG ulObjFlags, ulStateFlags;
          _picture->ClassifySelection( &ulObjFlags, &ulStateFlags );

          if ((ulObjFlags&GOT_EMBED)!=0)
            somSelf->DisplayResourceText(ev, 266, _fCurFrame);
        }

        CAPTURE_MOUSE;
        handled = TRUE;
      }
      break;
    case FSA_ENTER :
    case FSA_ENDDRAG :
      if (_fsmState == 2) {
        somSelf->fsm_resize( ev, FSA_PAINT, pptl, flags );       // erases it
        rd->fDrawn = FALSE;

        calc_resize( rd, pptl, flags );

        somSelf->InvalidateObject( ev, 0 );
        _picture->Transform( &rd->mx );

        somSelf->PartUpdated( ev, _fCurFrame, PUF_CLEARUNDO | PUF_UPDATELINKS |
                                            PUF_REPAINT | PUF_CLIPEMBEDS | PUF_RESETFSM );
        handled = TRUE;
      }
      break;

    case FSA_RESET:

//  GpiQueryDefaultViewMatrix( _fsmHps, 9, &rd->mx );

      FREE_MOUSE;
      if (_fsmState == 2) {
        somSelf->fsm_resize( ev, FSA_PAINT, pptl, flags );       // erases it
        _fsmData = NULL;
        delete rd;
      }
      _fsmState = 0;
      _fsmType = _fsmLastType;
      somSelf->DisplayResourceText(ev, 0, _fCurFrame);
      handled = TRUE;                        // ALWAYS return TRUE here
      break;                                 // unless forbidding RESET...
    default: break;
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return handled;
}

//---------------------------------------------------------------------------
typedef struct    {
        POINTL    ptlCenter;
        POINTL    ptlLL;
        POINTL    ptlUR;
        POINTL    ptlPerim;
        LONG      radius;
        FIXED     angle;
        MATRIXLF  mx;
        BOOL      fDrawn;
        } ROTATEDATA, *PROTATEDATA;

#define BOXDX 4

//---------------------------------------------------------------------------
ODBoolean  draw_rotate_circle( HPS hps, PROTATEDATA rd )
{
  GpiSetLineType( hps, LINETYPE_SHORTDASH );
  GpiMove( hps, &rd->ptlLL );
  GpiBox( hps, DRO_OUTLINE, &rd->ptlUR, rd->radius*2, rd->radius*2 );

  GpiSetLineType( hps, LINETYPE_SOLID );
  GpiMove( hps, &rd->ptlCenter );
  GpiLine( hps, &rd->ptlPerim );

  POINTL ptl;
  ptl = rd->ptlCenter;
  ptl.x -= BOXDX; ptl.y -= BOXDX;
  GpiMove( hps, &ptl );
  ptl.x += 2*BOXDX; ptl.y += 2*BOXDX;
  GpiBox( hps, DRO_OUTLINE, &ptl, BOXDX*2, BOXDX*2 );
  ptl = rd->ptlPerim;
  ptl.x -= BOXDX; ptl.y -= BOXDX;
  GpiMove( hps, &ptl );
  ptl.x += 2*BOXDX; ptl.y += 2*BOXDX;
  GpiBox( hps, DRO_OUTLINE, &ptl, 0, 0 );
  return kODTrue;
}

#define   DISCRETE_ANGLE       MAKEFIXED(15,0)
//---------------------------------------------------------------------------
FIXED  calc_angle2( PPOINTL ptl0, PPOINTL ptl1, ODULong flags )
{
  FIXED angle;

  // calc the rotation angle
  calc_angle( ptl0, ptl1, &angle, NULL );

  // here's where we would allow discrete angles...
  if (FLAGS_SHIFT) {
    angle = ( (angle + DISCRETE_ANGLE/2) / DISCRETE_ANGLE ) * DISCRETE_ANGLE;
  }

  return angle;
}

//-------------------------------------------------------------------------
// Finite State Machine
//   state 0:   basepoint.  doing NOTHING.  Use this to reset FSM in app...
//   state 1:   entry point for rotate op
//   state 2:   rotating objects, waiting for end point.
//
//---------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartfsm_rotate( GraphicsPart *somSelf,
                                                     Environment *ev,
                                                     short action,
                                                     POINTL* pptl,
                                                     ODULong flags)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_resize");

  PROTATEDATA  rd  = (PROTATEDATA)_fsmData;
  BOOL         handled = FALSE;

  SOM_TRY

  switch ( action ) {
    case FSA_MOVE:
      if (_fsmState > 1) {
        FIXED newAngle;
        handled = TRUE;
        if (_fsmState == 2) {
          newAngle = calc_angle2( &rd->ptlCenter, pptl, flags );
          if (newAngle == rd->angle) break;
        }

        if ( rd->fDrawn ) {
          // erase old objects
          GpiSetModelTransformMatrix( _fsmHps, 9, &rd->mx, TRANSFORM_REPLACE );
          draw_rotate_circle( _fsmHps, rd );
          if (_fsmState == 2)
            _picture->DrawShadows( _fsmHps );
          }

        if (_fsmState == 2) {
          // build the transform
          rd->angle = newAngle;
          calc_rotate( newAngle, &rd->ptlCenter, &rd->mx );

          // show the angle on the status line
          double dAngle = newAngle/65536.0;
          char string[80], fmtstring[80];
          WinLoadString( (HAB)0, _hmod, 4, 79, fmtstring);
          sprintf( string, fmtstring, dAngle );
          somSelf->DisplayText(ev, string, _fCurFrame); // 124281
        } else {
          rd->ptlCenter = *pptl;
          rd->ptlLL.x = rd->ptlCenter.x - rd->radius;
          rd->ptlLL.y = rd->ptlCenter.y - rd->radius;
          rd->ptlUR.x = rd->ptlCenter.x + rd->radius;
          rd->ptlUR.y = rd->ptlCenter.y + rd->radius;
          rd->ptlPerim = rd->ptlCenter;
          rd->ptlPerim.x += rd->radius;

          calc_rotate( 0, &rd->ptlCenter, &rd->mx );
        }

        GpiSetModelTransformMatrix( _fsmHps, 9, &rd->mx, TRANSFORM_REPLACE );
        draw_rotate_circle( _fsmHps, rd );
        if (_fsmState == 2)
          _picture->DrawShadows( _fsmHps );
//        GpiSetModelTransformMatrix( _fsmHps, 0, NULL, TRANSFORM_REPLACE );
        GpiSetModelTransformMatrix( _fsmHps, 9, &mx0, TRANSFORM_REPLACE );  // 143738
        rd->fDrawn = TRUE;
      }
      break;
    case FSA_PAINT:
      if (_fsmState && rd->fDrawn) {
        GpiSetModelTransformMatrix( _fsmHps, 9, &rd->mx, TRANSFORM_REPLACE );
        draw_rotate_circle( _fsmHps, rd );
        if (_fsmState == 2)
          _picture->DrawShadows( _fsmHps );
//        GpiSetModelTransformMatrix( _fsmHps, 0, NULL, TRANSFORM_REPLACE );
        GpiSetModelTransformMatrix( _fsmHps, 9, &mx0, TRANSFORM_REPLACE );  // 143738
        handled = TRUE;
      }
      break;
    case FSA_ENTER :
      if (_fsmState == 0) {
        _fsmData = rd = new ROTATEDATA;
        GtkObject* obj = _picture->HitObject();
        RECTL rcl = obj ? obj->Bounds()
                        : *_picture->SelectedBounds();
        rd->ptlCenter.x = (rcl.xLeft + rcl.xRight)/2;
        rd->ptlCenter.y = (rcl.yBottom + rcl.yTop)/2;
        rd->radius = 72;                                        // DPI
        rd->angle = 0;
        rd->ptlLL.x = rd->ptlCenter.x - rd->radius;
        rd->ptlLL.y = rd->ptlCenter.y - rd->radius;
        rd->ptlUR.x = rd->ptlCenter.x + rd->radius;
        rd->ptlUR.y = rd->ptlCenter.y + rd->radius;
        rd->ptlPerim = rd->ptlCenter;
        rd->ptlPerim.x += rd->radius;

        // init the transform
        calc_rotate( 0, &rd->ptlCenter, &rd->mx );

        shadow_hps( _fsmHps );
//        GpiSetModelTransformMatrix( _fsmHps, 0, NULL, TRANSFORM_REPLACE );
        GpiSetModelTransformMatrix( _fsmHps, 9, &mx0, TRANSFORM_REPLACE );  // 143738

        draw_rotate_circle( _fsmHps, rd );
        rd->fDrawn = TRUE;
        _fsmState = 1;

        somSelf->DisplayResourceText(ev, 1, _fCurFrame); // 124281
        handled = TRUE;
      }
      break;
    case FSA_BEGINDRAG :
      if (_fsmState == 1) {
        // grabbing the rotation handle?
        if ( labs(pptl->x - rd->ptlPerim.x) <= BOXDX &&
             labs(pptl->y - rd->ptlPerim.y) <= BOXDX  ) {
          _fsmState = 2;
          _picture->DrawShadows( _fsmHps );
          CAPTURE_MOUSE;
          handled = TRUE;
        // grabbing the center handle??
        } else if ( labs(pptl->x - rd->ptlCenter.x) <= BOXDX &&
                    labs(pptl->y - rd->ptlCenter.y) <= BOXDX  ) {
          _fsmState = 3;
          CAPTURE_MOUSE;
          handled = TRUE;
        }
      }
      break;
    case FSA_ENDDRAG :
      if (_fsmState == 2) {
        somSelf->fsm_rotate( ev, FSA_PAINT, pptl, flags );
        rd->fDrawn = FALSE;

        somSelf->InvalidateObject( ev, 0 );
        _picture->Transform( &rd->mx );

        somSelf->PartUpdated( ev, _fCurFrame, PUF_CLIPEMBEDS | PUF_REPAINT | PUF_CLEARUNDO |
                                         PUF_UPDATELINKS | PUF_RESETFSM);
        handled = TRUE;
      } else if (_fsmState == 3) {
        _fsmState = 1;
        handled = TRUE;
      }
      break;
    case FSA_RESET:
      FREE_MOUSE;
      if (_fsmState) {
        if (rd->fDrawn)
          somSelf->fsm_rotate( ev, FSA_PAINT, pptl, flags );       // erases it
        _fsmData = NULL;
        delete rd;
      }
      _fsmState = 0;
      _fsmType = _fsmLastType;
      somSelf->DisplayResourceText(ev, 0, _fCurFrame);
      handled = TRUE;                        // ALWAYS return TRUE here
      break;                                 // unless forbidding RESET...
    default: break;
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return handled;
}

// ------------------------------------------------------------------
//   general control methods                                      aaa
// ------------------------------------------------------------------

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartAddObject( GraphicsPart *somSelf,
                                               Environment *ev, GtkObject* obj )
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","AddObject");

  SOM_TRY

  somSelf->HighlightSelection(ev, 0, 0);               // erase marks
  _fAltMark = FALSE;                                   // reset

  _picture->AddObject( obj );
  _picture->Select( obj );                             // (); for select-none

  UndoCreate* uc = new UndoCreate();
  _picture->CreateUndoStruct( UNDOCLS_CREATE, (PVOID*)&uc );
  LogUndoAction( somSelf, ev, uc, sizeof(*uc), _fSession, "create object" );

  RECTL rcl = obj->Bounds();
  somSelf->ClipEmbeds( ev, 0, &rcl );

  somSelf->PartUpdated( ev, 0, PUF_REPAINT );

  SOM_CATCH_ALL
  SOM_ENDTRY
}

//---------------------------------------------------------------------------
// return TRUE if handled, FALSE if not
// Ptl is in PAGE (WORLD) coords already
//------------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartfsm_execute(GraphicsPart *somSelf,
                                                      Environment *ev,
                                                     short action,
                                                     POINTL* pPtl)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_execute");

  SHORT         oldtype, oldstate;
  BOOL          handled = FALSE;
  ULONG         flgs;

  SOM_TRY

  oldtype  = _fsmType;
  oldstate = _fsmState;

  // here's where we check to see if a direct manipulation op is starting (and ok)

  if (!_fsmState && _picture->ObjectCount() ) {
    if (action == FSA_BEGINDRAG ) {
      _fsmLastType = _fsmType;
      _fsmType = 0;

      // are we grabbing the selected-bounds sizing handles?
      if (_picture->HitSizers(_fsmHps,pPtl) ) {
        _fsmType = ACTION_RESIZE;
        _picture->Correlate(0);                        // remove lastHit check

      // are we dragging an object handle?
      } else if (_fAltMark && _picture->HitHandle(_fsmHps,pPtl)) {
        _fsmType = ACTION_POINTEDIT;

      // are we just dragging an object or the selection?
      } else if (_picture->Correlate(_fsmHps,pPtl)) {

        // immediate start of embed D&D op?
        if (key_shift(VK_ALT)) {
          somSelf->InitDrag(ev, kODNULL);
          return TRUE;

        } else
          _fsmType = ACTION_MOVECOPY;
      }

      // if we selected a new type lets initiate it
      if (_fsmType) {
        _fsmState = 1;
        if ( somSelf->fsm_execute( ev, action, pPtl ) )
          return TRUE;
      }

      _fsmType = _fsmLastType;
      _fsmState = 0;
    } else if (action == FSA_MOVE && !_fReadOnly &&
               _picture->HitSizers(_fsmHps,pPtl)   ) {
      LONG handle;
      _picture->HitObject( &handle );
      SetSizingPointer( handle );
      return TRUE;
    }
  }

  flgs = 0x0000;
  if (key_shift(VK_SHIFT)) flgs |= FSAF_SHIFT;
  if (key_shift(VK_CTRL))  flgs |= FSAF_CONTROL;
  if (key_shift(VK_ALT))   flgs |= FSAF_ALT;

  switch ( _fsmType ) {
    case ACTION_LINE:
       handled = somSelf->fsm_line( ev, action, pPtl, flgs );
       break;
    case ACTION_POLYLINE:
       handled = somSelf->fsm_polyline( ev, action, pPtl, flgs );
       break;
    case ACTION_RECT:
    case ACTION_BOX:
       handled = somSelf->fsm_box( ev, action, pPtl, flgs );
       break;
    case ACTION_ELLIPSE:
       handled = somSelf->fsm_ellipse( ev, action, pPtl, flgs );
       break;
    case ACTION_ARC:
    case ACTION_PIE:
       handled = somSelf->fsm_arc( ev, action, pPtl, flgs );
       break;
    case ACTION_MOVECOPY:
       handled = somSelf->fsm_movecopy( ev, action, pPtl, flgs );
       break;
    case ACTION_POINTEDIT:
       handled = somSelf->fsm_pointedit( ev, action, pPtl, flgs );
       break;
    case ACTION_RESIZE:
       handled = somSelf->fsm_resize( ev, action, pPtl, flgs );
       break;
    case ACTION_ROTATE:
       handled = somSelf->fsm_rotate( ev, action, pPtl, flgs );
       break;
#if 0
    case ACTION_SKETCH:
       owd->fsmProc = somSelf->fsm_sketch;
       break;
    case ACTION_TEXT:
       owd->fsmProc = somSelf->fsm_text;
       break;
#endif
    case ACTION_SELECT:
    default:
//       owd->fsmProc = somSelf->fsm_select;
       handled = somSelf->fsm_select( ev, action, pPtl, flgs );
       break;
    }

  SOM_CATCH_ALL
  SOM_ENDTRY

  return handled;
}

//------------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartfsm_set_state(GraphicsPart *somSelf,
                                                        Environment *ev,
                                                       short sType,
                                                       short sState)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_set_state");

  SOM_TRY

  if (_fsmState)
    somSelf->fsm_execute( ev, FSA_RESET, NULL );

  _fsmType  = sType;
  _fsmState = sState;

  if ( !_fsmState ) somSelf->fsm_execute( ev, FSA_INIT, NULL );

  SOM_CATCH_ALL
  SOM_ENDTRY

  return TRUE;
}

//------------------------------------------------------------------------------
SOM_Scope void  SOMLINK GraphicsPartfsm_query_state(GraphicsPart *somSelf,
                                                     Environment *ev,
                                                    short* pType,
                                                    short* pState)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_query_state");

  *pType  = _fsmType;
  *pState = _fsmState;
}

//------------------------------------------------------------------------------
SOM_Scope ODBoolean  SOMLINK GraphicsPartfsm_set_buffer(GraphicsPart *somSelf,
                                                         Environment *ev,
                                                        void* pBuf)
{
  GraphicsPartData *somThis = GraphicsPartGetData(somSelf);
  GraphicsPartMethodDebug("GraphicsPart","fsm_set_buffer");

  _fsmData = pBuf;
  return TRUE;
}

