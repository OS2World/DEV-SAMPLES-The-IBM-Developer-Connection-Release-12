//
// myioproc.cpp
//

//
// Constructor for my ioproc object....
//
MYIOProc myIOProc(MY_CHARSET_INFO, MY_IOPROC_NAME_TABLE, FOURCC_MY,
                  MMIO_IOPROC_FILEFORMAT, MMIO_MEDIATYPE_AUDIO,
                  MMIO_CANREADTRANSLATED | MMIO_CANSEEKTRANSLATED,
                  "EXT");

extern "C"
   {
   // _CRT_init is the C run-time environment initialization function.
   // It will return 0 to indicate success and -1 to indicate failure
   int _CRT_init(void);

   // __ctordtorInit constructs all the global C++ objects
   void __ctordtorInit(void);

   // __ctordtorTerm destroys all the global C++ objects
   void __ctordtorTerm(void);


   #ifndef STATIC_LINK
   // _CRT_term is the C run-time environment termination function.
   // It only needs to be called when the C run-time functions are statically
   // linked.
   //
   // If runtime calls are required for cleanup and the runtime is dynamically
   // linked, a cleanup routine should be placed on an exit list.
   void _CRT_term(void);
   #endif


   #define INITTERM_FAILURE   ((unsigned long)0)
   #define INITTERM_SUCCESS   ((unsigned long)1)

   unsigned long _System _DLL_InitTerm(unsigned long hModule, unsigned long flag)
      {
      unsigned long rc = INITTERM_SUCCESS;

      if (0 == flag)   // init
         {
         if (-1 != _CRT_init())  // initialize C runtime library
            {
            __ctordtorInit();    // make sure our global C++ objects are constructed

            myIOProc.setModuleHandle(hModule); // Pass dll handle to base class

            #ifndef STATIC_LINK
            // This is where you would register your cleanup routine as
            // an exit list funtion if dynamically linked runtime
            #endif
            }
         else
            rc = INITTERM_FAILURE;
         }
      else
         {
         __ctordtorTerm();       // destruct global C++ objects

         #ifdef STATIC_LINK
         _CRT_term();
         #endif
         }

      return rc;
      }

   LONG APIENTRY IOProc_Entry(PVOID pmmioinfo, USHORT msg, LONG lParam1, LONG lParam2)
      {
       return myIOProc.handleMMIOMessage(pmmioinfo, msg, lParam1, lParam2);
      }

   }  // end of extern "C"

//
// Constructor & any overriden methods go here.
//
