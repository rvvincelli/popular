@ECHO OFF
setlocal ENABLEDELAYEDEXPANSION

REM Riccardo Vincenzo Vincelli

REM This batch performs the fingerprint matching for the given samples against
REM the given db, returning the results.

REM REM Parameters - give it like c:\foo\ not c:\foo
REM %1 the directory where the backward-algorithm binary is found (ie the 
REM Eclipse build dir), win style ("\" path separator; anyway cmd reduces \\...\
REM to just \)
REM %2 the db directory, cyg style ("\\")
REM %3 the samples directory, cyg style

REM The parsing of the results is tailored on the program output. The input 
REM song name occurs once or twice, with the first occurrence always on line 3, 
REM once if the match fails, twice if not. A match can be perfect (second
REM occurrence at line no 7) or not.

REM Important: make sure you have no active command prompts in the dirs where
REM the program executes. 

cd %3
set count=0
set matches=0
set pmatches=0
FOR %%i IN (*) DO (
	echo Executing query no^. !count! ^.^.^.
	cd %1
	popularBW %3"%%i" %2 > tmp 
	type tmp >> "results%random%.txt"
	FOR /F "delims=_" %%j IN ("%%i") DO grep -n "%%j" tmp | tail -n 1 > tmp2
	FOR /F "delims=:" %%j IN (tmp2) DO (
		IF %%j EQU 6 (
			set /A matches=!matches!+1
			set /A pmatches=!pmatches!+1
		) ELSE (
			IF %%j NEQ 3 (set /A matches=!matches!+1)
	  	  )
	)
	set /A count=!count!+1
)
del tmp tmp2 1>nul 2>nul
set /A perc=100*!matches!/!count!
echo ^# of hits^: !matches!^/!count! ^(!perc!^%% approx^.^)
set /A perc=100*!pmatches!/!count!
echo ^# of perfect hits^: !pmatches!^/!count! ^(!perc!^%% approx^.^)