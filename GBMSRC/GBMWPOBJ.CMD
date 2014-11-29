/* REXX exec to make the Workplace Shell objects for GBM */

Call rxfuncadd sysloadfuncs, rexxutil, sysloadfuncs
Call sysloadfuncs

If Pos('\GBM',Translate(Value('PATH',,'OS2ENVIRONMENT'))) > 0 Then Do
  drive = Substr(Translate(Value('PATH',,'OS2ENVIRONMENT')),Pos('\GBM',Translate(Value('PATH',,'OS2ENVIRONMENT')))-2,2)
  Say "Found "drive"\GBM on the path"
  fullpath = drive||'\GBM'
End
Else Do
  Say "Please type full path to where GBM installed (eg: D:\GBM)"
  Parse Upper Pull fullpath
End

rc = SysFileTree(fullpath, 'file', 'D')
If file.0 <> 0 Then Do
  assoc = '*.BMP,*.VGA,*.BGA,*.RLE,*.DIB,*.RL4,*.RL8,*.GIF,*.PCX,*.TIF,*.TGA,*.VST,*.AFI,*.IFF,*.LBM,*.VID,*.PGM,*.PPM,*.KPS,*.IAX,*.XBM,*.SPR,*.SPRITE,*.PSE,*.PSEG,*.PSEG*,*.IMG,*.CVP'
  Call SysCreateObject 'WPProgram','GbmV2','<WP_DESKTOP>','EXENAME='||fullpath||'\GBMV2.EXE;PROGTYPE=PM;PARAMETERS=%*;STARTUPDIR='||fullpath||';ASSOCFILTER='||assoc,'R'
  Call SysCreateObject 'WPProgram','GbmV','<WP_DESKTOP>','EXENAME='||fullpath||'\GBMV.EXE;PROGTYPE=PM;PARAMETERS=-e %*;STARTUPDIR='||fullpath||';ASSOCFILTER='||assoc,'R'
  Say "Workplace Shell Objects for GbmV and GbmV2 created on the desktop."
  Say "These have associations for all GBM file formats, including *.BMP files."
  Say "This means you can just click on a bitmap file to get it displayed."
  Say "However, the Icon Editor also lays multiple claims to *.BMP files."
  Say "So you may get Icon Editor in preference to GBM if you click on a bitmap."
  Say "I would recommend removing its claims, so that GBM is the default."
  Say "If so, find the following items :-"
  Say "  \OS2\ICONEDIT.EXE file (perhaps by using Drives)."
  Say "  Icon Editor program reference object in OS/2 System -> Productivity."
  Say "  \TOOLKT21\OS2BIN\ICONEDIT.EXE file (perhaps by using Drives)."
  Say "  Icon Editor program reference object in Toolkit 2.1 -> PM Development Tools."
  Say "  any other copies of ICONEDIT.EXE and program reference objects."
  Say "and then remove the following from the Settings -> Association page :-"
  Say "  Bitmap entry from Current types listbox."
  Say "  *.BMP entry from the Current names listbox."
End
Else Do
  Say "The directory " fullpath " does not exist"
End

Return 0
