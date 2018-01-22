@echo off
echo.
echo Mit diesem Skript wird der Haus-Bus.de Multitaster in der Homematic CCU installiert
echo Vorher muss der SSH Zugang in der Homematic Oberflaeche unter Einstellungen aktiviert werden
echo.
echo INFO: Im naechsten Schritt werden Sie nach der IP und dem SSH Passwort der CCU gefragt.
echo Falls danach eine weitere Abfrage auf englisch kommt, diese bitte bestaetigen und anschliessend das Tool auf jeden Fall nochmal aufrufen!
echo.
pause
echo.
echo.
set /p ip=Hier bitte nun die IP Adresse oder den Hostnamen der Homematic CCU eingeben: 
set /p password=Hier bitte das vergebene Passwort fuer den SSH Zugang eingeben: 
set user=root

echo. 
echo INFO: Bei der ersten Verbindung zur CCU kommt eine Meldung, dass der Server bisher unbekannt ist. Dort bitte bestaetigen mit 'y'
echo. 

echo.
echo Kopiere neue Dateien
plink %user%@%ip% -pw %password% mount -o remount,rw /
pscp  -batch -pw %password% ../CCU-files/firmware/hs485types/hbw_sd6_multikey.xml %user%@%ip%:/firmware/hs485types/
pscp  -batch -pw %password% ../CCU-files/firmware/hs485types/hbw_lc4_in4_dr.xml %user%@%ip%:/firmware/hs485types/

plink -batch %user%@%ip% -pw %password% chmod 755 /firmware/hs485types/hbw_sd6_multikey.xml
plink -batch %user%@%ip% -pw %password% chmod 755 /firmware/hs485types/hbw_lc4_in4_dr.xml

pscp  -batch -pw %password% addCcu.sh %user%@%ip%:/
plink -batch %user%@%ip% -pw %password% chmod 755 /addCcu.sh
plink -batch %user%@%ip% -pw %password% /addCcu.sh
plink -batch %user%@%ip% -pw %password% rm /addCcu.sh

pscp  -batch -pw %password% ../CCU-files/www/config/img/devices/250/177_hbw-sd6-multikey.png %user%@%ip%:/www/config/img/devices/250/
pscp  -batch -pw %password% ../CCU-files/www/config/img/devices/250/178_hbw-io12-1w-up.png %user%@%ip%:/www/config/img/devices/250/
pscp  -batch -pw %password% ../CCU-files/www/config/img/devices/250/179_hbw-lc4-in4-dr.png %user%@%ip%:/www/config/img/devices/250/

plink -batch %user%@%ip% -pw %password% chmod 755 /www/config/img/devices/250/177_hbw-sd6-multikey.png
plink -batch %user%@%ip% -pw %password% chmod 755 /www/config/img/devices/250/178_hbw-io12-1w-up.png
plink -batch %user%@%ip% -pw %password% chmod 755 /www/config/img/devices/250/179_hbw-lc4-in4-dr.png

pscp  -batch -pw %password% ../CCU-files/www/config/img/devices/50/177_hbw-sd6-multikey_thumb.png %user%@%ip%:/www/config/img/devices/50/
pscp  -batch -pw %password% ../CCU-files/www/config/img/devices/50/178_hbw-io12-1w-up_thumb.png %user%@%ip%:/www/config/img/devices/50/
pscp  -batch -pw %password% ../CCU-files/www/config/img/devices/50/179_hbw-lc4-in4-dr_thumb.png %user%@%ip%:/www/config/img/devices/50/

plink -batch %user%@%ip% -pw %password% chmod 755 /www/config/img/devices/50/177_hbw-sd6-multikey_thumb.png
plink -batch %user%@%ip% -pw %password% chmod 755 /www/config/img/devices/50/178_hbw-io12-1w-up_thumb.png
plink -batch %user%@%ip% -pw %password% chmod 755 /www/config/img/devices/50/179_hbw-lc4-in4-dr_thumb.png

pscp -pw %password% %user%@%ip%:/www/webui/webui.js webui.js
pscp -pw %password% %user%@%ip%:/www/config/devdescr/DEVDB.tcl DEVDB.tcl


plink %user%@%ip% -pw %password% mount -o remount,ro /
echo.
echo Alle klar. Fertig!
echo.
echo Nun CCU neustarten und anschliessend bei laufender CCU einmal den Taster mit Spannung versorgen. 
echo Falls der Taster schon aktiv ist, einfach Die Tasten S5 und S6 für ca. 1s gleichzeitig betätigen.
echo.
echo Anschliessend wird der Taster im Posteingang der CCU eingetragen und kann verwendet werden.
echo.
echo Bei Problemen gibts Rat per Email an info@haus-bus.de
echo.
echo Viel Spass!
echo.
echo.
pause
