@echo off

set searchdir=%CD%\..\src

:runC
FOR /F "tokens=*" %%G IN ('DIR /B /S %searchdir%\*.c*') DO (
echo Indenting file "%%G"
uncrustify.exe -f "%%G" -c uncrustify.cfg -o indentoutput.tmp
move /Y indentoutput.tmp "%%G"
)

:runH
FOR /F "tokens=*" %%G IN ('DIR /B /S %searchdir%\*.h') DO (
echo Indenting file "%%G"
uncrustify.exe -f "%%G" -c uncrustify.cfg -o indentoutput.tmp
move /Y indentoutput.tmp "%%G"
)

