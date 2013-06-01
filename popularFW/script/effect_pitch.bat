@ECHO OFF
setlocal ENABLEDELAYEDEXPANSION

REM Riccardo Vincenzo Vincelli

REM This batch adds echo to the input mp3 files through the external program SoX. 
REM Gain-in and gain-out are fixed to 1. See "man sox" for more.

REM %1 where the mp3's to be filtered for input are found, cygwin style ("\\")
REM %2 output directory for the corrupted files
REM %3 cents 

REM Dependencies:
REM sox - sox installation directory has to be on path

cd %1
FOR %%i IN (*) DO (
	echo Shifting pitch on %%i ^.^.^.
	sox "%%i" "%%i_%3.mp3" pitch %3
	move /Y "%%i_%3.mp3" %2 1>nul 2>nul
)
echo Done^.