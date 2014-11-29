#define ID_PRINT                         300
#define IDD_PRINT_QUEUE                  301
#define IDD_PRINT_OPTIONS                302
#define IDD_PRINT_HELP                   303


  //--------------------------------------------------------------------------
  // :type res=&IDT_PRNINFO. name='PRNINFO' text='Print Information'.
  // :p.
  // This structure contains printer :hp1.hdc:ehp1. and :hp1.hps:ehp1. a s
  // used by the ViewPrint* functions. :hp1.bounds:ehp1. defines the valid
  // printing area, output should be scaled to fit this area. :hp1.somMaster:ehp
  // is the top-level object in the print heirarchy, and :hp1.pagenum:ehp1.
  // is a count of pages printed thus far.
  // :pcxmp.
     #define PRNINFO_STRING_LENGTH   80
     //typedef struct {
     class PRNINFO {
        public:
        HDC       hdc;
        HPS       hps;
        RECTL     bounds;
        //SOMObject somMaster;
        SHORT     pagenum;
     };
     typedef PRNINFO* PPRNINFO;
  // :epcxmp.
  // :etype.


