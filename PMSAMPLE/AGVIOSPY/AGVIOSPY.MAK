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

agviospy.exe: agviospy.l agviospy.obj agviospy.def agviospy.res gviospy.lib
        link @agviospy.l
        rc agviospy.res

agviospy.obj: agviospy.c agviospy.h
        cl /c /Alfu /W3 /Gs agviospy.c

agviospy.res: agviospy.rc agviospy.h
        rc -r agviospy.rc

