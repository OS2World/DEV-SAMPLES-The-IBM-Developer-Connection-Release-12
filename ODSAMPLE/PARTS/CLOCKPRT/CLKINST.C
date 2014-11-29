/* File:  Clkinst.c */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ODREGAPI.H>

#define NO_ERROR 0
#define PART_INFO_REPLACED 4

void handleError( int rc);

void main()
{

        /* register the simplepart class */
        int rc = 0;
        unsigned long size;
        char * list;

        rc = ODRegisterPartHandlerClass((PSZ)"ClockPart", (PSZ)"ClockPrt",TRUE, 0);

        if( rc != NO_ERROR && rc != PART_INFO_REPLACED )
            handleError(rc);


        /* query all part handlers to verify part registered okay */
        rc = ODQueryPartHandlerList(0, 0, &list, &size);

        if(rc == 0 )
        {
          printf("The parthandler list is:  %s\n", list);
        }

        return;

}

void handleError(int rc)
{

        printf("Error registering or querying the part handler, rc = %d\n", rc);
        exit(EXIT_FAILURE);

}

