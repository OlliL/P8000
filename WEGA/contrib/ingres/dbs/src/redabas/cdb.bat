@echo %ECHO%
if %1.==. goto usage
if %2.==. goto usage
if %3.==. set $DBP=0
goto ok
:usage
echo usage: cdb db dbi pathno
goto ende
:ok
copy %2 .\%1$.bat > nul
ed - .\%1$.bat < %INDP%\files\cdb.ed > nul
call .\%1$.bat %1 %$DBP%
rm .\%1$.bat
:ende
set $DBP=
