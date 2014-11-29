// int3286.h
// define NDEBUG to the compiler for retail builds: /DNDEBUG
// see assert.h in your compiler for NDEBUG example

// function prototype for int3 in INT3286.LIB
// use a calling convention where caller cleans the stack
void far cdecl int3(int);

#ifndef NDEBUG
  // debug build; parameter is line number of macro
  #define INT3  int3(__LINE__)
#else
  // retail build; define the macro to be nothing
  #define INT3
#endif

