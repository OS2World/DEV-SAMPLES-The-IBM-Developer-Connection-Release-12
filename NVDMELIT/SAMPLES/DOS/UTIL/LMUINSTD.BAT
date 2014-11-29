@ECHO OFF
REM
REM  Licensed Materials -- Program Property of IBM
REM
REM  LMUINSTD.BAT
REM
REM  IBM LAN NetView Management Utilities
REM  5622-153 (C) Copyright IBM Corp. 1991, 1994. All rights reserved.
REM
REM  US Government Users Restricted Rights -
REM  Use, duplication or disclosure restricted by
REM  GSA ADP Schedule Contract with IBM Corp.
REM  Licensed Materials - Property of IBM
REM
REM  Program to install LMU DOS support.
REM
REM  Command syntax:
REM
REM           LMUINSTD drive source path
REM  ÿ
REM    Where:
REM  ÿ   drive    The target drive (for example C:)  The directory will
REM  ÿ            always be \LMU2
REM  ÿ
REM  ÿ   source path  (for example I:\img\lnmu\10\csdinst\lmu2)
REM
REM  The drive and directory containing LMUINSTD must be made the current
REM  drive and directory before executing LMUINSTD.
REM
REM   EXAMPLE   To install LMU DOS support type the following commands:
REM
REM             LMUINSTD D: I:\CID\IMG\LNMU\11
REM
IF .%1 == . GOTO DEFAULT
IF %1 == ? GOTO HELP
IF .%2 == . GOTO DEFALT2
IF %2 == ? GOTO HELP
SET LMU_DRIVE=%1
SET LMU_DIR=%LMU_DRIVE%\LMU2
SET SRC_Dir=%2
GOTO INSTALL
:DEFAULT
SET LMU_DRIVE=C:
SET LMU_DIR=C:\LMU2
:DEFALT2
SET SRC_DIR=I:\img\lnmu\10\csdinst\lmu2
:INSTALL
ECHO ÿ
ECHO IBM LAN NetView Management Utilities
ECHO 5622-153 (C) Copyright IBM Corp. 1991, 1994. All rights reserved.
ECHO ÿ
MD %LMU_DIR%                >NUL
ECHO LMUINSTD: Copying SYSLEVEL.LMU
COPY %SRC_DIR%\SYSLEVEL.LMU %LMU_DIR% >NUL
ATTRIB %LMU_DIR%\SYSLEVEL.LMU -R >NUL
ECHO LMUINSTD: Copying LMUCUSTW.EXE
COPY %SRC_DIR%\LMUCUSTW.EXE %LMU_DIR% >NUL
ATTRIB %LMU_DIR%\LMUCUSTW.EXE -R >NUL
ECHO LMUINSTD: Copying QDOSVPD.COM
COPY %SRC_DIR%\QDOSVPD.COM  %LMU_DIR% >NUL
ATTRIB %LMU_DIR%\QDOSVPD.COM  -R >NUL
ECHO LMUINSTD: Copying ADAPTERS.TBL
COPY %SRC_DIR%\ADAPTERS.TBL %LMU_DIR% >NUL
ATTRIB %LMU_DIR%\ADAPTERS.TBL -R >NUL
ECHO LMUINSTD: Copying ADAPTERS.SMP
COPY %SRC_DIR%\ADAPTERS.SMP %LMU_DIR% >NUL
ATTRIB %LMU_DIR%\ADAPTERS.SMP -R >NUL
ECHO LMUINSTD: Copying CRITFILE.SMP
COPY %SRC_DIR%\CRITFILE.SMP %LMU_DIR% >NUL
ATTRIB %LMU_DIR%\CRITFILE.SMP -R >NUL
ECHO LMUINSTD: Copying USERVPD.SMP
COPY %SRC_DIR%\USERVPD.SMP  %LMU_DIR% >NUL
ATTRIB %LMU_DIR%\USERVPD.SMP  -R >NUL
ECHO LMUINSTD: Copying CVT_VPD.EXE
COPY %SRC_DIR%\CVT_VPD.EXE  %LMU_DIR% >NUL
ATTRIB %LMU_DIR%\CVT_VPD.EXE  -R >NUL
ECHO LMUINSTD: Copying LMUDOSHB.COM
COPY %SRC_DIR%\LMUDOSHB.COM %LMU_DIR% >NUL
ATTRIB %LMU_DIR%\LMUDOSHB.COM -R >NUL
ECHO LMUINSTD: Copying AUEDOSAL.COM
COPY %SRC_DIR%\AUEDOSAL.COM %LMU_DIR% >NUL
ATTRIB %LMU_DIR%\AUEDOSAL.COM -R >NUL
ECHO LMUINSTD: Copying DOSVIRGA.COM
COPY %SRC_DIR%\DOSVIRGA.COM %LMU_DIR% >NUL
ATTRIB %LMU_DIR%\DOSVIRGA.COM -R >NUL
ECHO LMUINSTD: Copying DRWATCHD.COM
COPY %SRC_DIR%\DRWATCHD.COM %LMU_DIR% >NUL
ATTRIB %LMU_DIR%\DRWATCHD.COM -R >NUL
:DONE
%LMU_DRIVE%
CD \LMU2
ECHO ÿ
ECHO LMUINSTD: Installation complete
ECHO ÿ
GOTO EXIT
:HELP
ECHO ÿ
ECHO IBM LAN NetView Management Utilities
ECHO 5622-153 (C) Copyright IBM Corp. 1991, 1994. All rights reserved.
ECHO ÿ
ECHO US Government Users Restricted Rights -
ECHO Use, duplication or disclosure restricted by
ECHO GSA ADP Schedule Contract with IBM Corp.
ECHO Licensed Materials - Property of IBM
ECHO ÿ
ECHO Program to install LMU DOS support.
ECHO ÿ
ECHO Command syntax:
ECHO ÿ
ECHO          LMUINSTD drive source path
ECHO ÿ
ECHO   Where:
ECHO ÿ   drive    The target drive (for example C:)  The directory will
ECHO ÿ            always be \LMU2
ECHO ÿ
ECHO ÿ   source path  (for example I:\img\lnmu\10\csdinst\lmu2)
ECHO ÿ
:EXITÿ
