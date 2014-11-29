#ifndef sdtsinst_HH_INCLUDED
#define sdtsinst_HH_INCLUDED

#include <som.hh>

#pragma SOMAsDefault(on)

class SOMClass;
class SOMObject;
class SimpleDTSPartInstall;

#pragma SOMAsDefault(pop)
#include <somobj.hh>
#pragma SOMNonDTS(on)
#pragma SOMAsDefault(on)

class SimpleDTSPartInstall : public SOMObject {

     #pragma SOMClassName(*, "SimpleDTSPartInstall")
     #pragma SOMNoMangling(*)
     #pragma SOMNoDataDirect(*)
     #pragma SOMNonDTS(*)
     #pragma SOMCallstyle (idl)
     #pragma SOMAsDefault(off)
     #pragma SOMAsDefault(pop)

  public :
     virtual void Register();

     #pragma SOMReleaseOrder ( \
                              "Register")
};

#pragma SOMAsDefault(pop)
#pragma SOMNonDTS(pop)

#endif /* sdtsinst_HH_INCLUDED */
