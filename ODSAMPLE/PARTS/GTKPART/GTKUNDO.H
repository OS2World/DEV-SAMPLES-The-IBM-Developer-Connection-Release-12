
#ifndef _GTKUNDO_
#define _GTKUNDO_

#define  UNDOCLS_BGNDCOLOR        1
#define  UNDOCLS_LOCK           100
#define  UNDOCLS_UNLOCK         101
#define  UNDOCLS_ATTRIBS        102

#define  UNDOCLS_TOTOP          103
#define  UNDOCLS_TOBOTTOM       104
#define  UNDOCLS_CREATE         105
#define  UNDOCLS_MOVE           106
//#define  UNDOCLS_COPY           107
#define  UNDOCLS_DELETE         108
#define  UNDOCLS_ROTATE         109
#define  UNDOCLS_RESIZE         110

// base struct for extracting class from all undo structs
struct UndoBase {
   ODULong     uclass;
};

// struct for undoing background color change (UNDOCLS_BGNDCOLOR)
struct UndoBgndColor {
   ODULong     uclass;
   ODFrame     *frame;
   RGBColor    oldColor;
   RGBColor    newColor;
   UndoBgndColor(ODFrame* fr, RGBColor oc, RGBColor nc)
     {uclass=UNDOCLS_BGNDCOLOR; frame=fr; oldColor=oc; newColor=nc;}
};

// struct for undoing Objects LOCK state (UNDOCLS_LOCK/UNLOCK)
struct UndoLockstuff {
   ODULong     uclass;
   ODULong     count;
   ODULong*    ids;
   UndoLockstuff(ODULong uc)
     {uclass = uc; count = 0; ids = 0;}
};

// struct for undoing Object attribute change (UNDOCLS_ATTRIBS)
struct UndoAttributes {
   ODULong     uclass;
   ODULong     aAttribType;
   ODULong     count;
   ODULong*    ids;
   long*       oldAttribs;
   long        newAttrib;
   UndoAttributes(ODULong atype, long aval)
     {uclass = UNDOCLS_ATTRIBS; aAttribType=atype; newAttrib=aval; count=0; ids=0; oldAttribs=0;}
};

// struct for undoing Objects MOVE op
struct UndoMove {
   ODULong     uclass;
   ODULong     count;
   ODULong*    ids;
   long        dx;
   long        dy;
   UndoMove(long delx, long dely)
     {uclass=UNDOCLS_MOVE; dx=delx; dy=dely; count=0; ids=0;}
};

// struct for undoing Objects CREATE op
struct UndoCreate {
   ODULong     uclass;
   ODBoolean*  undone;
   ODULong     count;
   ODULong*    ids;
   GtkObject** objs;
   UndoCreate() {uclass=UNDOCLS_CREATE; undone=kODFalse; ids=0; objs=0;}
};

#endif
