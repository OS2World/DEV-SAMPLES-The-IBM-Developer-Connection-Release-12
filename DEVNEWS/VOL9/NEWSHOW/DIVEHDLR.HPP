
// divehdlr.hpp
//
//    This is the class definition of the Dive Handler class.
//    code was originally written by Maria Ingold, and minor
//    modifications later added by Larry Kyrala.


#ifndef _DIVEHDLR_HPP_
#define _DIVEHDLR_HPP_



class IDiveHandler : public IHandler
{
   typedef IHandler Inherited;

   public:
      virtual IDiveHandler& handleEventsFor( IWindow* );
      virtual IDiveHandler& stopHandlingEventsFor( IWindow* );

      virtual Boolean dispatchHandlerEvent( IEvent& );
      virtual Boolean DiveDisabled( IEvent& );
      virtual Boolean DiveEnabled( IEvent& );
};


inline Boolean IDiveHandler::DiveDisabled( IEvent &event )
{
   return( false );
}


inline Boolean IDiveHandler::DiveEnabled( IEvent &event )
{
   return( false );
}


#endif

