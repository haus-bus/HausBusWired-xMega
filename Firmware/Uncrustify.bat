@echo off

set searchdir=%CD%

:runC
FOR /F "tokens=*" %%G IN ('DIR /B /S %searchdir%\*.cpp') DO (
IF %%~xG == .cpp (
echo Indenting file "%%G"
..\Tools\uncrustify.exe -f "%%G" -c uncrustify.cfg -o indentoutput.tmp
move /Y indentoutput.tmp "%%G"
)
)

:runH
FOR /F "tokens=*" %%G IN ('DIR /B /S %searchdir%\*.h') DO (
IF %%~xG == .h (
echo Indenting file "%%G"
..\Tools\uncrustify.exe -f "%%G" -c uncrustify.cfg -o indentoutput.tmp
move /Y indentoutput.tmp "%%G"
)
)
