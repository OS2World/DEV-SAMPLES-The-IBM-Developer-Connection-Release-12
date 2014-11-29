#include "SDTSInst.hh"
#include <odtypes.h>
#include <rmanager.hh>
#include <odprtreg.hh>
#include <iostream.h>

void SimpleDTSPartInstall::Register()
{
    RegistryManager * registrationMgr;
    registrationMgr = new RegistryManager;

    ODPartHandlerRegistry * partRegistry;
    partRegistry = registrationMgr->GetODPartHandlerRegistry();

    string className = "SimpleDTSPart";
    string dllName    = "SimplDTS";

    long rc = partRegistry->RegisterPartHandlerClass( className,
                                                      dllName,
                                                      TRUE,
                                                      0);

    // returns 0 if successfully added, 4 if successfully replaced

    if( (rc == 0) || (rc == 4))
       cout << "Registration completed successfully \n";
    else
       cout << " Registration Failed .... rc = " << rc << "\n";

    // delete registrationMgr; Note: Can't delete SOMMSingleInstance metaclasses
    return;
}

main(int argc, char *argv[], char *envp[])
{
   SimpleDTSPartInstall * s = new SimpleDTSPartInstall;;
   s->Register();
}
