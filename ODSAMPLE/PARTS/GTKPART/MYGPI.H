
HPS    CreateHps( ULONG cx, ULONG cy, ULONG hType );
void   ReleaseHps( HPS hps );
VOID   SetScaling( HPS hps, PRECTL prclSource, PRECTL prclTarget, BOOL fStretch );
void   FillRect( HPS hps, RECTL *rcl, RGBColor color );
POINTL CursorPosition( Environment* ev, ODFacet* facet );

#define HPS_BITMAP    1
#define HPS_METAFILE  2

#define IT_LARGEICON  1
#define IT_SMALLICON  2

class GtkPointer {
  public:
    GtkPointer() { _hptr = NULL; _hbm = NULL; _cx = 0; _cy = 0; }
    ~GtkPointer() { if (_hptr) WinDestroyPointer( _hptr ); }
    ODBoolean Init( HMODULE hmod, ULONG ptrId );
    void Select( ULONG viewType );
    void Draw( HPS hps, PPOINTL ptlOffset ) {
               WinDrawBitmap(hps,_hbm,0,ptlOffset,0,0,DBM_NORMAL); }
    ODBoolean SetCurrent();
    ODULong Cx() { return _cx; }
    ODULong Cy() { return _cy; }
  protected:
    HPOINTER  _hptr;
    HBITMAP   _hbm;
    ULONG     _cx;
    ULONG     _cy;
};

