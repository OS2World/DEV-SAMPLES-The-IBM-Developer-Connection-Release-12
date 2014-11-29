/*---------------------------------------------------------------------*/
signal on ERROR
signal on FAILURE
signal on HALT
trace off
call RxFuncAdd 'SysLoadFuncs', 'REXXUTIL', 'SysLoadFuncs'
call SysLoadFuncs
Funct1='RXLISTOBJECTS'
call RxFuncDrop Funct1
if RxFuncQuery( Funct1 ) Then Call RxFuncAdd Funct1, 'DMPWIN', Funct1
StdClass. =""
call stdclassset
say 'Now dumping Active window and childs for data sorted by position '
say 'relative to active window sorted left to right top to bottom '
num=RxListObjects()
do i=0 to num
    Say "------------------------"
    tmp=Class.i
    Say "Handle     "   Handle.i
    Say "Model      "   Model.i
    Say "Class      "   Class.i  StdClass.tmp
    Say "Text       "   Text.i
    Say "CtlData    "   CtlData.i
    Say "x          "   x.i
    Say "y          "   y.i
    Say "cx         "   cx.i
    Say "cy         "   cy.i
    Say "Id         "   Id.i
    Say "Style      "   Style.i StyleText.i
    Say "Subclassed "   Subclassed.i
    Say "IsFrame    "   IsFrame.i
    if IsFrame.i then do
       Say "Flags      "   Flags.i
       Say "xRestore   "   xRestore.i
       Say "yRestore   "   yRestore.i
       Say "cxRestore  "   cxRestore.i
       Say "cyRestore  "   cyRestore.i
       Say "xMinimize  "   xMinimize.i
       Say "yMinimize  "   yMinimize.i
    end
    do j=1 to MenuItems.i
       Say 'Item parent id' ItemParentId.i.j
       Say 'Item id'        Itemid.i.j
       Say 'Item Text'      ItemText.i.j
    end
    do j=1 to ListItems.i
       Say 'List Item Text' ListItemText.i.j
    end
end
Call RxFuncDrop Funct1
exit rc
FAILURE:
ERROR:
HALT:
Call RxFuncDrop Funct1
say "Error "rc  line sigl":"sourceline(sigl)
say errortext(rc)
Exit rc
stdclassset:
WC_FRAME      ="#1"
WC_COMBOBOX   ="#2"
WC_BUTTON     ="#3"
WC_MENU       ="#4"
WC_STATIC     ="#5"
WC_ENTRYFIELD ="#6"
WC_LISTBOX    ="#7"
WC_SCROLLBAR  ="#8"
WC_TITLEBAR   ="#9"
WC_MLE        ="#10"
WC_APPSTAT    ="#16"
WC_KBDSTAT    ="#17"
WC_PECIC      ="#18"
WC_DBE_KKPOPUP="#19"
WC_SPINBUTTON ="#32"
WC_CONTAINER  ="#37"
WC_SLIDER     ="#38"
WC_VALUESET   ="#39"
WC_NOTEBOOK   ="#40"
StdClass.WC_FRAME      ="WC_FRAME"
StdClass.WC_COMBOBOX   ="WC_COMBOBOX"
StdClass.WC_BUTTON     ="WC_BUTTON"
StdClass.WC_MENU       ="WC_MENU"
StdClass.WC_STATIC     ="WC_STATIC"
StdClass.WC_ENTRYFIELD ="WC_ENTRYFIELD"
StdClass.WC_LISTBOX    ="WC_LISTBOX"
StdClass.WC_SCROLLBAR  ="WC_SCROLLBAR"
StdClass.WC_TITLEBAR   ="WC_TITLEBAR"
StdClass.WC_MLE        ="WC_MLE"
StdClass.WC_APPSTAT    ="WC_APPSTAT"
StdClass.WC_KBDSTAT    ="WC_KBDSTAT"
StdClass.WC_PECIC      ="WC_PECIC"
StdClass.WC_DBE_KKPOPUP="WC_DBE_KKPOPUP"
StdClass.WC_SPINBUTTON ="WC_SPINBUTTON"
StdClass.WC_CONTAINER  ="WC_CONTAINER"
StdClass.WC_SLIDER     ="WC_SLIDER"
StdClass.WC_VALUESET   ="WC_VALUESET"
StdClass.WC_NOTEBOOK   ="WC_NOTEBOOK"
return
