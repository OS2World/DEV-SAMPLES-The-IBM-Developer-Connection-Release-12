
// divewind.hpp
//
//    This file contains two class definitions: the first, for the
//    IFrameWindow derived class which will act as a standard
//    window giving us titlebar, sizing, etc.
//
//    The second, for the actual DiveWindow itself.  The DiveWindow
//    will be the child of the IFrameWindow.


#ifndef _DIVEWIND_HPP_
#define _DIVEWIND_HPP_


class DiveWindow : public ICanvas,
                   public IDiveHandler {
   public:
      DiveWindow( unsigned long, IWindow*, IWindow* );

      void paintMe();

   // inline members
      Boolean DiveDisabled( IEvent& );
      Boolean DiveEnabled( IEvent& );


      ISize getSize() { return myDive.getImageSize(); }

   private:
      Dive myDive;
      unsigned long myImage[16];
      Boolean pleaseBlit;
      IThread paintThread;
};



class StandardWindow : public IFrameWindow {
   public:
      StandardWindow();
      ~StandardWindow();

   private:
      DiveWindow *pDiveWindow;
};



// inline member implementations

inline Boolean DiveWindow::DiveDisabled( IEvent &event )
{
   pleaseBlit = false;
   myDive.setupBlitter();

   return true;
}

inline Boolean DiveWindow::DiveEnabled( IEvent &event )
{
   if (!pleaseBlit)
   {
      myDive.setupDestination( this );
      pleaseBlit = true;
   }
   return true;
}


#endif

