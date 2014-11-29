echo off
cls
echo ************* Hello2 C Sample Program Command File (.CMD) *****************
echo ÿ
echo  This is the .CMD file containing the commands to compile, link, and run
echo  the Hello2 Sample Program.  ** Note:  A call is made to the command
echo  file PMPATH.CMD which sets the environment for this process.
echo ÿ
echo ***************************************************************************
setlocal
echo on
call %1\toolkt12\c\samples\pm\pmpath.cmd
cd hello2
echo off
rem ***************************************************************************
rem * the following call is for the IBM MASM/2 1.0 make utility               *
rem ***************************************************************************
rem make hello2.mak
rem ***************************************************************************
rem * the following call is for the IBM C/2 1.10 make utility                 *
rem ***************************************************************************
make /f hello2.mak hello2.exe
echo You are now ready to run the HELLO2 Sample you have just built:
pause
if exist hello2.exe goto okay
echo Error generating HELLO2 sample.
echo Please check that your Toolkit and C/2 Compiler are installed correctly.
pause
goto done
:okay
echo on
hello2
pause
:done
endlocal
