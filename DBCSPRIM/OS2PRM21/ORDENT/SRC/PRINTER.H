/************   OS/2 Application Primer Sample   **********************/
/*                                                                    */
/* Module Name : PRINTER.H                                            */
/* Version : 1.0 (No change for OS/2 V2.x)                            */
/*                                                                    */
/* Header file for PRINTER.C, print subroutines.                      */
/*                                                                    */
/**********************************************************************/

/* Error codes */
#define NOT_ENOUGH_MEMORY     1
#define PROFILE_NOT_FOUND     2
#define PRINTER_NOT_SUPPORTED 3
#define INVALID_DATA          4

/* Print out form size (72 x 40) */
#define WIDTH   72
#define LINES   40

/* Minimum width of each field */
#define PROD_NO_COL   12
#define PROD_NAME_COL 36
#define ORD_QTY_COL   7
#define ORD_AMT_COL   12
#define ORD_TTL_COL   13

/* Printer type definitions */
#define PROPRINTER 0
#define IBM5575    1

/* Structure for control codes and escape sequences */
typedef struct _SEQ {
   SHORT len;
   CHAR *str;
} SEQ;

/* Structure for control code sets for supported printers */
typedef struct _CCODE {
   USHORT ptype;
   SEQ init;
   SEQ bs;
   SEQ lf;
   SEQ cr;
   SEQ ulon;
   SEQ uloff;
   SEQ dwon;
   SEQ dwoff;
   SEQ emon;
   SEQ emoff;
   SEQ boxl;
   SEQ boxm;
   SEQ boxr;
} CCODE;

/* Number of printers currently supported */
#define MAX_SUPPORTED_PRINTERS 2

/* Specification of control codes for supported printers */
CCODE ccode[MAX_SUPPORTED_PRINTERS] =
{{
      PROPRINTER,          // Control codes for Proprinter
      { 4,  "\x12\x1b\x41\x0c"},           // Initialization
      { 1,  "\x08"},                       // Back Space
      { 1,  "\x0a"},                       // Line Feed
      { 1,  "\x0d"},                       // Carriage Return
      { 3,  "\x1b\x2d\x01"},               // Underlining On
      { 3,  "\x1b\x2d\x02"},               // Underlining Off
      { 1,  "\x0e"},                       // Double Width On
      { 1,  "\x14"},                       // Double Width Off
      { 2,  "\x1b\x45"},                   // Emphasize On
      { 2,  "\x1b\x46"},                   // Emphasize Off
      { 60, "\x1b\x5b\x67\x37\x00\x0b"
            "\xFF\xFF\xFF\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"}, // Box Left
      { 60, "\x1b\x5b\x67\x37\x00\x0b"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"}, // Box Middle
      { 60, "\x1b\x5b\x67\x37\x00\x0b"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\xff\xff\xff"}  // Box Right
 },
 {
      IBM5575,             // Control Codes for 5575
      { 12, "\x1b\x7e\x02\x00\x01\x32"
            "\x1b\x7e\x03\x00\x01\x3c"},             // Initialization
      { 1,  "\x08"},                                 // Back Space
      { 1,  "\x0a"},                                 // Line Feed
      { 1,  "\x0d"},                                 // Carriage Return
      { 6,  "\x1b\x7e\x11\x00\x01\x01"},             // Underlining On
      { 6,  "\x1b\x7e\x11\x00\x01\x00"},             // Underlining Off
      { 8,  "\x1b\x7e\x20\x00\x03\x20\x10\x02"},     // Double Width On
      { 8,  "\x1b\x7e\x20\x00\x03\x10\x10\x02"},     // Double Width Off
      { 6,  "\x1b\x7e\x0e\x00\x01\x17"},             // Emphasize On
      { 6,  "\x1b\x7e\x0e\x00\x01\x18"},             // Emphasize Off
      { 59, "\x1b\x25\x31\x00\x12"
            "\xFF\xFF\xFF\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"}, // Box Left
      { 59, "\x1b\x25\x31\x00\x12"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"}, // Box Middle
      { 59, "\x1b\x25\x31\x00\x12"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\x80\x00\x01"
            "\x80\x00\x01\x80\x00\x01\xff\xff\xff"}  // Box Right
}};

/* Constant definitions for drawtext() */
#define RIGHT  0x0001
#define CENTER 0x0002
#define LEFT   0x0004
#define BOX    0x0010
#define DOUBLE 0x0020
#define EMPH   0x0040
#define UNDER  0x0080
#define CR     0x0100
#define CRLF   0x0200

/* Prototype declaration for high level functions */
BOOL   check_date(CHAR *, CHAR *, CHAR *);
VOID   print_report(VOID);
VOID   draw_text(UCHAR *, USHORT, USHORT);

/* Prototype declaration for low level printing functions */
USHORT init_prn(VOID);   // Initialize printer
VOID   end_prn(VOID);    // Close printer
VOID   bs(VOID);         // Send "bs" cotrol code sequence to printer
VOID   lf(VOID);         // Send "lf" cotrol code sequence to printer
VOID   cr(VOID);         // Send "cr" cotrol code sequence to printer
VOID   ulon(VOID);       // Send "ulon" cotrol code sequence to printer
VOID   uloff(VOID);      // Send "uloff" cotrol code sequence to printer
VOID   dwon(VOID);       // Send "dwon" cotrol code sequence to printer
VOID   dwoff(VOID);      // Send "dwoff" cotrol code sequence to printer
VOID   emon(VOID);       // Send "emon" cotrol code sequence to printer
VOID   emoff(VOID);      // Send "emoff" cotrol code sequence to printer
VOID   boxl(VOID);       // Send "boxl" cotrol code sequence to printer
VOID   boxm(VOID);       // Send "boxm" cotrol code sequence to printer
VOID   boxr(VOID);       // Send "boxr" cotrol code sequence to printer
