@ECHO OFF
SET IP=192.168.50.43
SET OID_BASE=.1.3.6.1.4.1.47408.1


:START
cls
ECHO.
ECHO  IO128-BAT
ECHO.
ECHO (1)   Relais  1
ECHO (2)   Relais  2
ECHO (3)   Relais  3
ECHO (4)   Relais  4
ECHO (5)   Relais  5
ECHO (6)   Relais  6
ECHO (7)   Relais  7
ECHO (8)   Relais  8
ECHO (9)   Relais  9
ECHO (10)  Relais 10
ECHO (11)  Led rot
ECHO (12)  Led gruen
ECHO (13)  Beeper
ECHO.
set /p choiceInstance=Auswahl:

cls
ECHO.
ECHO  IO128-BAT
ECHO.
ECHO (0)   ausschalten
ECHO (1)   anschalten
ECHO (2)   abfragen
ECHO.
set /p choiceFunction=Auswahl:

IF "%choiceInstance%"=="1" set OID_INSTANCE=20
IF "%choiceInstance%"=="2" set OID_INSTANCE=19
IF "%choiceInstance%"=="3" set OID_INSTANCE=18
IF "%choiceInstance%"=="4" set OID_INSTANCE=17
IF "%choiceInstance%"=="5" set OID_INSTANCE=36
IF "%choiceInstance%"=="6" set OID_INSTANCE=35
IF "%choiceInstance%"=="7" set OID_INSTANCE=34
IF "%choiceInstance%"=="8" set OID_INSTANCE=33
IF "%choiceInstance%"=="9" set OID_INSTANCE=49
IF "%choiceInstance%"=="10" set OID_INSTANCE=50
IF "%choiceInstance%"=="11" set OID_INSTANCE=210
IF "%choiceInstance%"=="12" set OID_INSTANCE=209
IF "%choiceInstance%"=="13" set OID_INSTANCE=51

IF "%choiceFunction%"=="0" (
	set VALUE=0
	goto OUTPUT_CMD
	)
IF "%choiceFunction%"=="1" (
	set VALUE=1
	goto OUTPUT_CMD
	)
IF "%choiceFunction%"=="2" goto OUTPUT_REQ
	)

goto EXIT

:OUTPUT_CMD
@ECHO ON
snmpset -r:%IP% -c:public -o:%OID_BASE%.4.2314.19.%OID_INSTANCE%.1 -val:%VALUE% -tp:int
@ECHO OFF
goto START

:OUTPUT_REQ
@ECHO ON
snmpget -r:%IP% -c:public -o:%OID_BASE%.4.2314.19.%OID_INSTANCE%.1 
@ECHO OFF
goto START

:EXIT