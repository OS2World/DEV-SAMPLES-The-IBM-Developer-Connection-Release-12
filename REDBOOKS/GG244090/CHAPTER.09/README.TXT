This is an example for a client/server application using the CDS namespace
and the security service of DCE. 

It implements a simple distributed message queuing system. 


FILES:
        README.TXT              this file.
        Makefile.aix            makefile for AIX
        Makefile.os2            makefile for OS/2
        Makefile.os2            makefile for Windows
        mbox.idl                IDL interface description
        mbox.acf                attribute configuration file
        client.c                client part
        server.c                server main module
        manager.c               server manager module
        security.c              server security checking module
        common.h                common header file
        os2.def                 linker definition file for OS/2
        win.def                 linker definition file for Windows



SETUP SERVER:
        1) Login as root and authenticate as cell_admin.
        2) Create a principal called MessageBox with rgy_edit.
        3) Create an account for MessageBox valid for login and
           group subsys/dce/cds-server. This group implies the
           right to write to the CDS namespace.
        4) Create a directory in the CDS name space with the
           cdscp command:
           cdscp add directory /.:/Servers
        5) On the machine wich runs the server application 'mbox'
           export the password for the account MessageBox to the
           local default keytab file. Be sure the the password is 
           the same as specified for the account.
           # rgy_edit
           Current site is: registry server at /.:/subsys/dce/sec/master
           rgy_edit==> ktadd -p MessageBox
           Enter password:xxxxxxx
           Re-enter password to verify:xxxxxxx
           rgy_edit==> exit
           bye.
        6) Authenticate as MessageBox and become root.
        7) Run the server application:
           # mbox
           Wait for the message:
           .......
           Server /.:/Servers/MessageBox listening.

If you rerun the server you only have to perform step 6) and 7). If you
choose to run the server on a different machine additinal you have to
export the password to the local keytab file (step 5).


CLIENT USAGE:

           AIX and Windows: message [-c] [principal] [Message ...]
           OS/2:            message [/c] [principal] [Message ...]

The client application 'message' provides the user interface for the simple
message queuing system. Without any arguments it look for the oldest message
for your principal. If there is a message waiting for you it prints the
message and discards it.
If you want to send a message just type principal name and the message in the
commandline of the client.

Only a principal with control rights on the CDS name space entry
                /.:/Servers/MessageBox
can use the option 'c' to create a new message box for the principal given
on the commandline. If you are authenticated as the principal MessageBox,
you will have the control right on the entry /.:/Servers/MessageBox because
MessageBox is the owner of this entry. But you can change the ACLs on that
entry to provide access to the create option for other principals.
