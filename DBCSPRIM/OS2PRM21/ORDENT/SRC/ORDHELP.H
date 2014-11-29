
/************   OS/2 Application Primer Sample   **********************/
/*                                                                    */
/* Module Name : ORDHELP.H                                            */
/* Version : 1.0 (No change for OS/2 V2.x)                            */
/*                                                                    */
/* A sample program for "OS/2 DBCS Application Primer".               */
/* Header file to use a HELP Manager function common routine.         */
/*                                                                    */
/**********************************************************************/

extern  HELPINIT hmiHelpData;
extern  HWND     hwndHelpInstance;

VOID    HelpInit(CHAR *);
VOID    HelpAssociate(HWND);
VOID    HelpDestroyAssociate(HWND);
