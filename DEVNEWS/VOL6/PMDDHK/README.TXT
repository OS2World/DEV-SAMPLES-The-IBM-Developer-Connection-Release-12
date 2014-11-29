Readme File for PMDDHK.DLL

Written by: Michael R. Cooper 
            IBM Boca Raton 
            PM Advanced Graphics Design & Development

VNET ID: COOP@VNET.IBM.COM
COMUSERV ID: Mike Cooper [IBM] 73204,471

OS2.INI file setup
------------------

Upon bootup, the PM Graphics engine will check the OS2.INI file for any modules that want to hook
or monitor the calls made to the current display driver.  The check will be to see if the
PM_ED_HOOKS entry exists.  If the entry does exist then it will load the module associated with
the entry.  The entry should be set up as follows:

The Application Name looked for is -------- PM_ED_HOOKS
The Key_Name is  -------------------------- [MODULENAME]
The Data Field is a Fully Qualified Path -- [DRIVE][PATH][MODULENAME]

example:
    APPLICATION    PM_ED_HOOKS
    KEY_NAME       PMDDHK
    DATA           C:\OS2\DLL\PMDDHK.DLL

PMDDHK.DLL information
----------------------
This module will only run in conjunction with 32Bit display drivers.

Compiler used
-------------
IBM CSET++ 2.0

Other Reference Material
------------------------
IBM OS/2 2.X DDK
Presentation Driver Reference - OS/2 Technical Library


