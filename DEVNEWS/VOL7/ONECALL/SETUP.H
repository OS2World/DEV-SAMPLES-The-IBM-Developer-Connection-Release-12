// setup.h
// Monte Copeland for DevCon 7

#if ! defined INCL_SPLDOSPRINT && !defined RC_INVOKED
#error Define INCL_SPLDOSPRINT and recompile.
#endif

#define LEN_WORKSTRING    196

typedef struct _PRINTERSETUP {

  // INPUT PARAMETERS: assign these before calling SetupPrinter()
  LONG           lWorldCoordinates; // PU_TWIPS or whatever
  HAB            hab;               // application's anchor block
  HWND           hwndFrame;         // application frame window
  HMODULE        hmod;              // zero for exe's, hmod if built into DLL

  // PROFILED PARAMETERS: store these two items on app close.
  // Retrieve them on app open.
  // Note pDriverData->cb is length of DRIVDATA structure.
  CHAR           szPreferredQueue[ 64 ];  // name of user's preferred queue
  PDRIVDATA      pDriverData;             // driver's data

  // OUTPUT PARAMETERS: for use by SetupPrinter() and the application
  HDC            hdcPrinterInfo;    // printer info DC
  HPS            hpsPrinterInfo;    // printer info PS
  LONG           lDCType;           // DC type suitable for DevOpenDC() param 2
  PDEVOPENDATA   pDevOpenData;      // suitable for DevOpenDC() parameter 5
  DEVOPENSTRUC   devopenstruc;      // pdevopendata points to this.
  LONG           cQueues;           // count of queues in PRQINFO3 array
  PPRQINFO3      pQueueInfo;        // pointer to array of PRQINFO3
  BOOL           fToFile;           // printing to file case
  CHAR           szFileName[ CCHMAXPATH ];  // file name for print-to-file
} PRINTERSETUP;
typedef PRINTERSETUP *PPRINTERSETUP;

// Error result codes from SetupPrinter(). (Returns zero for OK.)
#define SETUP_NOQUEUES       1
#define SETUP_NODC           2
#define SETUP_NOMEMORY       3

// Input defines for SetupPrinter() parameter 2.
#define SETUP_SHOWDIALOG     (BOOL)1
#define SETUP_NODIALOG       (BOOL)0

// Defines for resource compiler.
#define ID_PICKQ            22000
#define IDC_LISTBOX         22001
#define IDC_JOBPROP         22002
#define IDC_ENTRY           22003
#define IDC_TOFILE          22004

ULONG APIENTRY SetupPrinter( PPRINTERSETUP pSetup, BOOL fShowDialog );
ULONG APIENTRY CleanupPrinter( PPRINTERSETUP pSetup );
