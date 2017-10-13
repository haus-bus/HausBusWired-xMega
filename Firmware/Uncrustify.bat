@echo off

set searchdir=%CD%

ren *.cppproj *.uncrustify

:runC
FOR /F "tokens=*" %%G IN ('DIR /B /S %searchdir%\*.cpp') DO (
echo Indenting file "%%G"
..\Tools\uncrustify.exe -f "%%G" -c uncrustify.cfg -o indentoutput.tmp
move /Y indentoutput.tmp "%%G"
)

:runH
FOR /F "tokens=*" %%G IN ('DIR /B /S %searchdir%\*.h') DO (
echo Indenting file "%%G"
..\Tools\uncrustify.exe -f "%%G" -c uncrustify.cfg -o indentoutput.tmp
move /Y indentoutput.tmp "%%G"
)

ren *.uncrustify *.cppproj 