@ECHO OFF
setlocal ENABLEDELAYEDEXPANSION

REM Riccardo Vincenzo Vincelli

REM This batch generates the database/reference bitstreams for a given list of 
REM mp3s, starting at arbitrary position. The destination directories is 
REM assumed to be empty, or at least containing files that may be overwritten.

REM Note: The script is conceived for a Windows Cygwin environment (eg pathnames 
REM require proper formatting). And yes I'm choosing batch rather than shell
REM scripting. stdout and stderr are null redirected for speed reasons, for 
REM debug make sure to remove 1>nul and 2>nul. The script does not delete any 
REM file, input and outputs are copied or moved.

REM Parameters - give it like c:\\foo\\bar\ not c:\foo\bar
REM %1 the directory where the forward-algorithm binary and the needed files are
REM found (ie the Eclipse build dir)
REM %2 where the mp3s to be filtered for input are found
REM %3 dest dir for the bitstreams

REM Important: make sure you have no active command prompts in the dirs where
REM popularFW executes. 

REM As an aside: batch syntax, caveats and general approach to scripting is a 
REM real pain in the ass imho.

REM Take a look at generate_db_samples_bitstream_pattern for code description. 
REM The only parameter that differs is the %3, the number of mp3's to be 
REM randomly picked.

cd %2
FOR %%i IN (*) DO (
	echo Generating bitstream for %%i ^.^.^.
	cd %1
	popularFW %2"%%i"
	cd %2
	move /Y "%%i.dat" %3 1>nul 2>nul
)
echo Done^.