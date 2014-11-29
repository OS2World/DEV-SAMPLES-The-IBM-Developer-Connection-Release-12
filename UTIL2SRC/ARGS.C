/*===========================================================================*
 * == main program ==
 * args.c - Print all command line arguments.
 *
 * (C)Copyright IBM Corporation, 1989, 1991.               Brian E. Yoder
 *
 * 11/08/89 - Created.
 * 11/08/89 - Initial version completed, for AIX PS/2.
 * 04/03/91 - Updated for C/2 and DOS.
 * 04/29/91 - Include <os2.h> to be sure we can get to it.
 *===========================================================================*/

char ver[] = "args (C)IBM Corp. 1989";
char author[] = "Brian E. Yoder";

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys\types.h>
#include <os2.h>

/*===========================================================================*
 * Main program entry point
 *===========================================================================*/

main(argc, argv, envp)

int    argc;
char **argv;
char **envp;

{
  int rc;                      /* Return code storage */

  /* Display first argument: our program name */

  printf("Invoked as: %s\n\n", *argv);

  argc--;                      /* Ignore 1st argument (program name) */
  argv++;

  /* Display each successive argument */

  while (argc != 0)            /* For each successive argument: */
  {
     printf("%s\n", *argv);          /* Display argument */
     argc--;                         /* Decrement arg count */
     argv++;                         /* Point to next argument */
  }

  return(0);                   /* Return */
}
