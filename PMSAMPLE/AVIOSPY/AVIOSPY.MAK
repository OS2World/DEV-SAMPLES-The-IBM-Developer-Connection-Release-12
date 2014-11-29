# Compiler Switches:
#
# /c    - Supresses Linking
# /Alfu - Customised large memory model
#         (l) far pointers for code
#         (f) far pointers for data
#         (u) different segments for stack and data
# /W3   - Sets warning message level
# /Gs   - Removes stack probes
# /Gc   - Controls function calling sequences

aviospy.exe: aviospy.l aviospy.obj aviospy.def aviospy.res viospy.lib
        link @aviospy.l
        rc aviospy.res

aviospy.obj: aviospy.c aviospy.h
        cl /c /Alfu /W3 /Gs aviospy.c

aviospy.res: aviospy.rc aviospy.h
        rc -r aviospy.rc

