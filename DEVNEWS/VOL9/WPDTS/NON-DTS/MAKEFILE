################################################################################
#                                                                              #
#  Build the DLL.  Since there's only one source file, compile and link        #
#    in one step.                                                              #
#                                                                              #
################################################################################
icluixmp.dll: icluixmp.cpp   \
              icluixmp.hpp   \
              icluixmp.xih   \
              icluixmp.xh    \
              icluixmp.def   \
              icluixmp.res   \
              icluixmp.h
    icc @<<
 /N30 /Gd /Gm /Ge- /Ls /Wgen /Fm /Q /B"/map:full"
icluixmp.cpp
somtk.lib
icluixmp.def
<<
    rc icluixmp.res icluixmp.dll        # Bind the resource file

################################################################################
#                                                                              #
#  Run the SOM compiler.  If it builds one, it will build all.                 #
#                                                                              #
################################################################################
icluixmp.xih \
icluixmp.xh  \
icluixmp.def \
icluixmp.cpp:
    sc -mnoint -s"xih;xh;def;xc" icluixmp.idl

################################################################################
#                                                                              #
#  Compile the resources.                                                      #
#                                                                              #
################################################################################
icluixmp.res: icluixmp.rc icluixmp.h icluixmp.dlg
    rc -r icluixmp.rc
