/****************************************************************************/
/*    PROGRAM NAME: FLUSHALL.C                                              */
/*    Program to flush all buffers on a system as preparation for           */
/*    power off.                                                            */
/*                                                                          */
/*    FUNCTION:                                                             */
/*    This program uses the DosShutDown() function to flush all system      */
/*    buffers in preparation for a power off. This program should only be   */
/*    used if any other system shutdown options are inaccessible due to     */
/*    system malfunction. It can be executed by the NET RUN command from a  */
/*    OS/2 LAN Requester.                                                   */
/*                                                                          */
/*    SIDE EFFECT:                                                          */
/*    System must be restarted after program execution if the return code   */
/*    is 0. Other return code indicate that the FLUSHALL program failed.    */
/*                                                                          */
/*    COMPILE OPTIONS:                                                      */
/*    icc /C flushall.c                                                     */
/*    link386 flushall;                                                     */
/*                                                                          */
/*    AUTHOR:                                                               */
/*    Ingolf Lindberg, ITSC Austin, 1993                                    */
/****************************************************************************/

#define INCL_DOSFILEMGR                /* File Manager definitions required */
#include <os2.h>
#include <stdio.h>

int main ( int argc, char *argv[] )
{

 ULONG   ulReserved;   /* Reserved, must be zero */
 APIRET  rc;           /* Return code */

 ulReserved = 0;       /* Reserved, must be set to zero */

 rc = DosShutdown(ulReserved);
 if (rc != 0)
 {
  printf("FLUSHALL error: Return Code was : %ld", rc);
  return 99;
 }

 printf("FLUSHALL completed with Return Code 0\n");

}
