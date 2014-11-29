

extern "C" {
   //--------------------------------------------------------
   void EXPENTRY fappend(char* FileName, char* Data);
   //--------------------------------------------------------
   void EXPENTRY vfappendf(char* FileName, char* Format, va_list args);
   //--------------------------------------------------------
   void fprintf2(char* FileName, char* Format, ...);
   //--------------------------------------------------------
   void EXPENTRY fappenddate( char* FileName);
   //--------------------------------------------------------
   void rrprintf(char* Format, ...);
   //--------------------------------------------------------
   void beepprintf(char* Format, ...);
   //--------------------------------------------------------
   void EXPENTRY WinMBox( char* msg );
};

