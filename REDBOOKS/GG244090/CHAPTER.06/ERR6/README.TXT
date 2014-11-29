To run this math sample you have to the following:

1. Create a "Servers" directory on the namespace

   cdscp create dir /.:/Servers

2. Set the RPC_DEFAULT_ENTRY environment variable

On OS/2
   set RPC_DEFAULT_ENTRY=/.:/Servers/MyError

On AIX
   export RPC_DEFAULT_ENTRY=/.:/Servers/MyError

On MS Windows
   specify RPC_DEFAULT_ENTRY=/.:/Servers/MyError in DCE.INI file

