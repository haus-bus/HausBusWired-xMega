@echo off

set searchdir=%CD%

for /r Y:\ %i in ("*.cppproj") do echo ren "%i" "%~ni.uncrustify"

:runC
FOR /F "tokens=*" %%G IN ('DIR /B /S %searchdir%\*.cpp') DO (
echo Indenting file "%%G"
utils\bin\uncrustify.exe -f "%%G" -c uncrustify.cfg -o indentoutput.tmp
move /Y indentoutput.tmp "%%G"
)

:runH
FOR /F "tokens=*" %%G IN ('DIR /B /S %searchdir%\*.h') DO (
echo Indenting file "%%G"
utils\bin\uncrustify.exe -f "%%G" -c uncrustify.cfg -o indentoutput.tmp
move /Y indentoutput.tmp "%%G"
)

for /r Y:\ %i in ("*.uncrustify") do echo ren "%i" "%~ni.cppproj"