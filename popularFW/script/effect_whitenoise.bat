@ECHO OFF
setlocal ENABLEDELAYEDEXPANSION

REM Riccardo Vincenzo Vincelli

REM This batch adds whitenoise to the input mp3 files, all assumed to be stereo,
REM all lasting 5 seconds, through the external program SoX. Whitenoise 
REM disturbance creation cannot be done once and for all (ie we do not require
REM a precise samplerate for the input files; the -m option accepts only 
REM arguments sharing the same samplerate). 

REM %1 where the mp3's to be filtered for input are found, cygwin style ("\\")
REM %2 output directory for the corrupted files
REM %3 volume parameter

REM Dependencies:
REM mp3info (see generate_bitstreams.bat)
REM sox - sox installation directory has to be on path

cd %1
FOR %%i IN (*) DO (
	mp3info -p %%Q "%%i" > tmp
	set /P rate=<tmp 
	del tmp
	echo Generating the whitenoise for %%i ^.^.^.
	sox -r !rate! -n output^.mp3 synth 5 whitenoise vol %3 channels 2 	
	echo Adding noise to the file ^.^.^.
	sox -m "%%i" output^.mp3 "%%i_%4.mp3"
	move /Y "%%i_%4.mp3" %2 1>nul 2>nul
)	
del output^.mp3
echo Done^.