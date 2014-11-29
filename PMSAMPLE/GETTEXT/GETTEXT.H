/*ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป*/
/*บ  Art User exits definitions Include File                        บ*/
/*บ  Author : Marc Fiammante    Date: July 91                       บ*/
/*ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ*/
extern HWND      pascal  InformWindow;/* ART Window Handle to send   */
                                      /* messages to                 */

/* ART User exit messages  definition                                */

#define   PMTST_USER_EXIT_BASE  (4*WM_USER)
#define   PMTST_ADD_TO_OUTPUT   (PMTST_USER_EXIT_BASE+1)
#define   PMTST_QUERY_SELECTED  (PMTST_USER_EXIT_BASE+2)
#define   PMTST_QUERY_HMQ       (PMTST_USER_EXIT_BASE+3)
#define   PMTST_QUERY_HAB       (PMTST_USER_EXIT_BASE+4)

