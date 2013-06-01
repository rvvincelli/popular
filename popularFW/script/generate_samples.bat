@ECHO OFF
setlocal ENABLEDELAYEDEXPANSION

REM Riccardo Vincenzo Vincelli

REM Dependencies:
REM mp3splt - http://mp3splt.sourceforge.net/
REM mp3info - http://ibiblio.org/mp3info/
REM mp3splt is to be called in its own directory (it is not a parameter here, 
REM see below) since it expects to find its plugins in the same dir; mp3info 
REM just needs to be on path.

REM %1 where the mp3's to be sampled are found .mp3 extension assumed
REM %2 text file containing selected mp3s
REM %3 how many seconds to extract
REM %4 dest dir for the samples

REM see generate_bitstreams.bat for additional REMs

cd %1
FOR /F "tokens=*" %%a IN (%2) DO (
	echo Generating sample for %%a ^.^.^.
	mp3info -p %%S "%%a"> tmp
	set /P secs=<tmp
	del tmp
	set /A score=!secs!-%3 + 1
	set /A startsec=!random!%%score%
	set /A endsec=!startsec!+%3
	set /A fromsec=!startsec!%%60
	set /A tosec=!endsec!%%60 		
	set /A score = !startsec!-!fromsec!
	set /A frommin=!score!/60	
	set /A score=!endsec!-!tosec!
	set /A tomin=!score!/60
	"C:\Program Files\mp3splt_2.4.3_i386\mp3splt" -Q -d %4 -o @f^.mp3_%3s "%%a" ^
	 !frommin!^.!fromsec! !tomin!^.!tosec!
)
echo Done^.