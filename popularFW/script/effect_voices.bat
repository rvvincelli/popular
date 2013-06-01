@ECHO OFF
setlocal ENABLEDELAYEDEXPANSION

REM Riccardo Vincenzo Vincelli

REM This batch mixes the input mp3 files with typical OTAATP (over-the-air at-
REM the-pub) noise.

REM %1 where the mp3's to be filtered for input are found, cygwin style ("\\")
REM %2 output directory for the corrupted files
REM %3 voices file

REM Dependencies:
REM sox - sox installation directory has to be on path

cd %1
FOR %%i IN (*) DO (
	echo Resampling OTAATP for %%i ^.^.^.
	mp3info -p %%Q "%%i" > tmp
	set /P rate=<tmp 
	del tmp
	sox %3 -r !rate! "%3_!rate!.mp3"	
	echo Adding it ^.^.^.
	sox -m "%%i" "%3_!rate!.mp3" "%%i_voices.mp3"
	move /Y "%%i_voices.mp3" %2 1>nul 2>nul
)	
echo Done^.