file="/www/config/devdescr/DEVDB.tcl"
if grep -q HBW-SD6-Multikey "$file"; then
   echo "$file ist schon fuer Haus-Bus.de Taster (SD6-MultiKey-v0) konfiguriert"
else
 if [ ! -f "$file.backup" ];
 then
  echo "Erstelle Backup: $file.backup";
  cp $file $file.backup
 else
   echo "Backup existiert bereits: $file.backup";
 fi    
 sed -i -e 's/DEV_LIST {/DEV_LIST {HBW-SD6-Multikey /' $file
 sed -i -e 's/DEV_DESCRIPTION {/DEV_DESCRIPTION {HBW-SD6-Multikey HBW-SD6-Multikey /' $file
 sed -i -e 's/DEV_PATHS       {/DEV_PATHS       {HBW-SD6-Multikey {{50 \/config\/img\/devices\/50\/177_hbw-sd6-multikey_thumb.png} {250 \/config\/img\/devices\/250\/177_hbw-sd6-multikey.png}} /' $file
 sed -i -e "s/DEV_HIGHLIGHT   {/DEV_HIGHLIGHT   {HBW-SD6-Multikey {{\"1\" 1 0.045 0.138 0.025} {\"2\" 1 0.90 0.138 0.025} {\"3\" 1 0.045 0.474 0.025} {\"4\" 1 0.90 0.474 0.025} {\"5\" 1 0.045 0.809 0.025} {\"6\" 1 0.90 0.809 0.025} {\"7\" 3 0.46 0.46 'S1' 0.3 'verdana' Font.BOLD} {\"8\" 3 0.46 0.46 'S2' 0.3 'verdana' Font.BOLD} {\"9\" 3 0.46 0.46 'S3' 0.3 'verdana' Font.BOLD} {\"10\" 3 0.46 0.46 'S4' 0.3 'verdana' Font.BOLD} {\"11\" 3 0.46 0.46 'S5' 0.3 'verdana' Font.BOLD} {\"12\" 3 0.46 0.46 'S6' 0.3 'verdana' Font.BOLD} {\"13\" 1 0.35 0.138 0.025} {\"14\" 1 0.6 0.138 0.025} {\"15\" 1 0.35 0.474 0.025} {\"16\" 1 0.6 0.474 0.025} {\"17\" 1 0.35 0.809 0.025} {\"18\" 1 0.6 0.809 0.025} {\"19\" 3 0.2 0.125 'LED1' 0.3 'verdana' Font.BOLD} {\"20\" 3 0.2 0.125 'LED2' 0.3 'verdana' Font.BOLD} {\"21\" 3 0.2 0.125 'LED3' 0.3 'verdana' Font.BOLD} {\"22\" 3 0.2 0.125 'LED4' 0.3 'verdana' Font.BOLD} {\"23\" 3 0.2 0.125 'LED5' 0.3 'verdana' Font.BOLD} {\"24\" 3 0.2 0.125 'LED6' 0.3 'verdana' Font.BOLD} {\"25\" 1 0.82 0.138 0.02} {\"26\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"27\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"28\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"29\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"30\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"31\" 1 0.6 0.40 0.02}  {\"32\" 3 0.46 0.4 'BR' 0.3 'verdana' Font.BOLD}} /" $file
 echo "$file erfolgreich fuer Haus-Bus.de Taster (SD6-MultiKey-v0) angepasst";
fi
if grep -q HBW-SD6-Multikey-v1 "$file"; then
   echo "$file ist schon fuer Haus-Bus.de Taster (SD6-MultiKey-v1) konfiguriert"
else
 if [ ! -f "$file.backup" ];
 then
  echo "Erstelle Backup: $file.backup";
  cp $file $file.backup
 else
   echo "Backup existiert bereits: $file.backup";
 fi    
 sed -i -e 's/DEV_LIST {/DEV_LIST {HBW-SD6-Multikey-v1 /' $file
 sed -i -e 's/DEV_DESCRIPTION {/DEV_DESCRIPTION {HBW-SD6-Multikey-v1 HBW-SD6-Multikey-v1 /' $file
 sed -i -e 's/DEV_PATHS       {/DEV_PATHS       {HBW-SD6-Multikey-v1 {{50 \/config\/img\/devices\/50\/177_hbw-sd6-multikey_thumb.png} {250 \/config\/img\/devices\/250\/177_hbw-sd6-multikey.png}} /' $file
 sed -i -e "s/DEV_HIGHLIGHT   {/DEV_HIGHLIGHT   {HBW-SD6-Multikey-v1 {{\"1\" 1 0.045 0.138 0.025} {\"2\" 1 0.90 0.138 0.025} {\"3\" 1 0.045 0.474 0.025} {\"4\" 1 0.90 0.474 0.025} {\"5\" 1 0.045 0.809 0.025} {\"6\" 1 0.90 0.809 0.025} {\"7\" 3 0.46 0.46 'S1' 0.3 'verdana' Font.BOLD} {\"8\" 3 0.46 0.46 'S2' 0.3 'verdana' Font.BOLD} {\"9\" 3 0.46 0.46 'S3' 0.3 'verdana' Font.BOLD} {\"10\" 3 0.46 0.46 'S4' 0.3 'verdana' Font.BOLD} {\"11\" 3 0.46 0.46 'S5' 0.3 'verdana' Font.BOLD} {\"12\" 3 0.46 0.46 'S6' 0.3 'verdana' Font.BOLD} {\"13\" 1 0.35 0.138 0.025} {\"14\" 1 0.6 0.138 0.025} {\"15\" 1 0.35 0.474 0.025} {\"16\" 1 0.6 0.474 0.025} {\"17\" 1 0.35 0.809 0.025} {\"18\" 1 0.6 0.809 0.025} {\"19\" 3 0.2 0.125 'LED1' 0.3 'verdana' Font.BOLD} {\"20\" 3 0.2 0.125 'LED2' 0.3 'verdana' Font.BOLD} {\"21\" 3 0.2 0.125 'LED3' 0.3 'verdana' Font.BOLD} {\"22\" 3 0.2 0.125 'LED4' 0.3 'verdana' Font.BOLD} {\"23\" 3 0.2 0.125 'LED5' 0.3 'verdana' Font.BOLD} {\"24\" 3 0.2 0.125 'LED6' 0.3 'verdana' Font.BOLD} {\"25\" 1 0.82 0.138 0.02} {\"26\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"27\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"28\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"29\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"30\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"31\" 1 0.6 0.40 0.02}  {\"32\" 3 0.46 0.4 'BR' 0.3 'verdana' Font.BOLD}} /" $file
 echo "$file erfolgreich fuer Haus-Bus.de Taster (SD6-MultiKey-v1) angepasst";
fi
if grep -q HBW-IO-12-1W-UP "$file"; then
   echo "$file ist schon fuer Haus-Bus.de HBW-IO-12-1W-UP konfiguriert"
else
 if [ ! -f "$file.backup" ];
 then
  echo "Erstelle Backup: $file.backup";
  cp $file $file.backup
 else
   echo "Backup existiert bereits: $file.backup";
 fi    
 sed -i -e 's/DEV_LIST {/DEV_LIST {HBW-IO-12-1W-UP /' $file
 sed -i -e 's/DEV_DESCRIPTION {/DEV_DESCRIPTION {HBW-IO-12-1W-UP HBW-IO-12-1W-UP /' $file
 sed -i -e 's/DEV_PATHS       {/DEV_PATHS       {HBW-IO-12-1W-UP {{50 \/config\/img\/devices\/50\/178_hbw-io12-1w-up_thumb.png} {250 \/config\/img\/devices\/250\/178_hbw-io12-1w-up.png}} /' $file
 sed -i -e "s/DEV_HIGHLIGHT   {/DEV_HIGHLIGHT   {HBW-IO-12-1W-UP {{\"1\" 1 0.79 0.301 0.025} {\"2\" 1 0.79 0.352 0.025} {\"3\" 1 0.79 0.403 0.025} {\"4\" 1 0.79 0.454 0.025} {\"5\" 1 0.79 0.505 0.025} {\"6\" 1 0.79 0.556 0.025} {\"7\" 1 0.79 0.607 0.025} {\"8\" 1 0.79 0.658 0.025} {\"9\" 1 0.638 0.78 0.025} {\"10\" 1 0.587 0.78 0.025} {\"11\" 1 0.536 0.78 0.025} {\"12\" 1 0.485 0.78 0.025} {\"13\" 1 0.434 0.78 0.025} {\"14\" 1 0.383 0.78 0.025} {\"15\" 1 0.332 0.78 0.025} {\"16\" 1 0.281 0.78 0.025} {\"17\" 1 0.15 0.628 0.025} {\"18\" 1 0.15 0.577 0.025} {\"19\" 1 0.15 0.527 0.025} {\"20\" 1 0.15 0.476 0.025} {\"21\" 1 0.15 0.425 0.025} {\"22\" 1 0.15 0.374 0.025} {\"23\" 1 0.15 0.323 0.025} {\"24\" 1 0.15 0.272 0.025} {\"25\" 1 0.662 0.15 0.025} {\"26\" 1 0.662 0.15 0.025} {\"27\" 1 0.662 0.15 0.025} {\"28\" 1 0.662 0.15 0.025} {\"29\" 1 0.662 0.15 0.025} {\"30\" 1 0.662 0.15 0.025} {\"31\" 3 0.40 0.45 'NA' 0.3 'verdana' Font.BOLD}  {\"32\" 3 0.40 0.45 'NA' 0.3 'verdana' Font.BOLD}} /" $file
 echo "$file erfolgreich fuer Haus-Bus.de HBW-IO-12-1W-UP angepasst";
fi
if grep -q HBW-LC4-IN4-DR "$file"; then
   echo "$file ist schon fuer Haus-Bus.de HBW-LC4-IN4-DR konfiguriert"
else
 if [ ! -f "$file.backup" ];
 then
  echo "Erstelle Backup: $file.backup";
  cp $file $file.backup
 else
   echo "Backup existiert bereits: $file.backup";
 fi    
 sed -i -e 's/DEV_LIST {/DEV_LIST {HBW-LC4-IN4-DR /' $file
 sed -i -e 's/DEV_DESCRIPTION {/DEV_DESCRIPTION {HBW-LC4-IN4-DR HBW-LC4-IN4-DR /' $file
 sed -i -e 's/DEV_PATHS       {/DEV_PATHS       {HBW-LC4-IN4-DR {{50 \/config\/img\/devices\/50\/179_hbw-lc4-in4-dr_thumb.png} {250 \/config\/img\/devices\/250\/179_hbw-lc4-in4-dr.png}} /' $file
 sed -i -e 's/DEV_HIGHLIGHT   {/DEV_HIGHLIGHT   {HBW-LC4-IN4-DR {} /' $file
 echo "$file erfolgreich fuer Haus-Bus.de HBW-LC4-IN4-DR angepasst";
fi

file="/www/webui/webui.js"
if grep -q HBW-SD6-Multikey "$file"; then
   echo "$file ist schon fuer Haus-Bus.de Taster (SD6-MultiKey-v0) konfiguriert"
else
 if [ ! -f "$file.backup" ];
 then
  echo "Erstelle Backup: $file.backup";
  cp $file $file.backup
 else
   echo "Backup existiert bereits: $file.backup";
 fi    
 sed -i -e "s/DEV_LIST.push('HmIP-WRC6');/DEV_LIST.push('HBW-SD6-Multikey');\nDEV_DESCRIPTION[\"HBW-SD6-Multikey\"] = \"HBW-SD6-Multikey\";\nDEV_PATHS[\"HBW-SD6-Multikey\"] = new Object();\nDEV_PATHS[\"HBW-SD6-Multikey\"][\"50\"] = \"\/config\/img\/devices\/50\/177_hbw-sd6-multikey_thumb.png\";\nDEV_PATHS[\"HBW-SD6-Multikey\"][\"250\"] = \"\/config\/img\/devices\/250\/177_hbw-sd6-multikey.png\";\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"] = new Object();\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"1\"] = [1, 0.045, 0.138, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"2\"] = [1, 0.90, 0.138, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"3\"] = [1, 0.045, 0.474, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"4\"] = [1, 0.90, 0.474, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"5\"] = [1, 0.045, 0.809, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"6\"] = [1, 0.90, 0.809, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"7\"] = [3, 0.46, 0.46, 'S1', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"8\"] = [3, 0.46, 0.46, 'S2', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"9\"] = [3, 0.46, 0.46, 'S3', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"10\"] = [3, 0.46, 0.46, 'S4', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"11\"] = [3, 0.46, 0.46, 'S5', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"12\"] = [3, 0.46, 0.46, 'S6', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"13\"] = [1, 0.35, 0.138, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"14\"] = [1, 0.6, 0.138, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"15\"] = [1, 0.35, 0.474, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"16\"] = [1, 0.6, 0.474, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"17\"] = [1, 0.35, 0.809, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"18\"] = [1, 0.6, 0.809, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"19\"] = [3, 0.20, 0.125, 'LED1', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"20\"] = [3, 0.20, 0.125, 'LED2', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"21\"] = [3, 0.20, 0.125, 'LED3', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"22\"] = [3, 0.20, 0.125, 'LED4', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"23\"] = [3, 0.20, 0.125, 'LED5', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"24\"] = [3, 0.20, 0.125, 'LED6', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"25\"] = [1, 0.82, 0.138, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"26\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"27\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"28\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"29\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"30\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"31\"] = [1, 0.60, 0.40, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"32\"] = [3, 0.46, 0.40, 'BR', 0.3, 'verdana', Font.BOLD];\nDEV_LIST.push('HmIP-WRC6');/" $file
 echo "$file erfolgreich fuer Haus-Bus.de Taster (SD6-MultiKey-v0) angepasst";
fi
if grep -q HBW-SD6-Multikey-v1 "$file"; then
   echo "$file ist schon fuer Haus-Bus.de Taster (SD6-MultiKey-v1) konfiguriert"
else
 if [ ! -f "$file.backup" ];
 then
  echo "Erstelle Backup: $file.backup";
  cp $file $file.backup
 else
   echo "Backup existiert bereits: $file.backup";
 fi    
 sed -i -e "s/DEV_LIST.push('HmIP-WRC6');/DEV_LIST.push('HBW-SD6-Multikey-v1');\nDEV_DESCRIPTION[\"HBW-SD6-Multikey-v1\"] = \"HBW-SD6-Multikey-v1\";\nDEV_PATHS[\"HBW-SD6-Multikey-v1\"] = new Object();\nDEV_PATHS[\"HBW-SD6-Multikey-v1\"][\"50\"] = \"\/config\/img\/devices\/50\/177_hbw-sd6-multikey_thumb.png\";\nDEV_PATHS[\"HBW-SD6-Multikey-v1\"][\"250\"] = \"\/config\/img\/devices\/250\/177_hbw-sd6-multikey.png\";\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"] = new Object();\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"1\"] = [1, 0.045, 0.138, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"2\"] = [1, 0.90, 0.138, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"3\"] = [1, 0.045, 0.474, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"4\"] = [1, 0.90, 0.474, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"5\"] = [1, 0.045, 0.809, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"6\"] = [1, 0.90, 0.809, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"7\"] = [3, 0.46, 0.46, 'S1', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"8\"] = [3, 0.46, 0.46, 'S2', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"9\"] = [3, 0.46, 0.46, 'S3', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"10\"] = [3, 0.46, 0.46, 'S4', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"11\"] = [3, 0.46, 0.46, 'S5', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"12\"] = [3, 0.46, 0.46, 'S6', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"13\"] = [1, 0.35, 0.138, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"14\"] = [1, 0.6, 0.138, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"15\"] = [1, 0.35, 0.474, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"16\"] = [1, 0.6, 0.474, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"17\"] = [1, 0.35, 0.809, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"18\"] = [1, 0.6, 0.809, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"19\"] = [3, 0.20, 0.125, 'LED1', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"20\"] = [3, 0.20, 0.125, 'LED2', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"21\"] = [3, 0.20, 0.125, 'LED3', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"22\"] = [3, 0.20, 0.125, 'LED4', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"23\"] = [3, 0.20, 0.125, 'LED5', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"24\"] = [3, 0.20, 0.125, 'LED6', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"25\"] = [1, 0.82, 0.138, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"26\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"27\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"28\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"29\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"30\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"31\"] = [1, 0.60, 0.40, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey-v1\"][\"32\"] = [3, 0.46, 0.40, 'BR', 0.3, 'verdana', Font.BOLD];\nDEV_LIST.push('HmIP-WRC6');/" $file
 echo "$file erfolgreich fuer Haus-Bus.de Taster (SD6-MultiKey-v1) angepasst";
fi
if grep -q HBW-IO-12-1W-UP "$file"; then
   echo "$file ist schon fuer Haus-Bus.de HBW-IO-12-1W-UP konfiguriert"
else
 if [ ! -f "$file.backup" ];
 then
  echo "Erstelle Backup: $file.backup";
  cp $file $file.backup
 else
   echo "Backup existiert bereits: $file.backup";
 fi    
 sed -i -e "s/DEV_LIST.push('HmIP-WRC6');/DEV_LIST.push('HBW-IO-12-1W-UP');\nDEV_DESCRIPTION[\"HBW-IO-12-1W-UP\"] = \"HBW-IO-12-1W-UP\";\nDEV_PATHS[\"HBW-IO-12-1W-UP\"] = new Object();\nDEV_PATHS[\"HBW-IO-12-1W-UP\"][\"50\"] = \"\/config\/img\/devices\/50\/178_hbw-io12-1w-up_thumb.png\";\nDEV_PATHS[\"HBW-IO-12-1W-UP\"][\"250\"] = \"\/config\/img\/devices\/250\/178_hbw-io12-1w-up.png\";\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"] = new Object();\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"1\"] = [1, 0.79, 0.301, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"2\"] = [1, 0.79, 0.352, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"3\"] = [1, 0.79, 0.403, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"4\"] = [1, 0.79, 0.454, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"5\"] = [1, 0.79, 0.505, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"6\"] = [1, 0.79, 0.556, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"7\"] = [1, 0.79, 0.607, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"8\"] = [1, 0.79, 0.658, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"9\"] = [1, 0.638, 0.78, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"10\"] = [1, 0.587, 0.78, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"11\"] = [1, 0.536, 0.78, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"12\"] = [1, 0.485, 0.78, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"13\"] = [1, 0.434, 0.78, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"14\"] = [1, 0.383, 0.78, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"15\"] = [1, 0.332, 0.78, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"16\"] = [1, 0.281, 0.78, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"17\"] = [1, 0.15, 0.628, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"18\"] = [1, 0.15, 0.577, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"19\"] = [1, 0.15, 0.527, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"20\"] = [1, 0.15, 0.476, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"21\"] = [1, 0.15, 0.425, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"22\"] = [1, 0.15, 0.374, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"23\"] = [1, 0.15, 0.323, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"24\"] = [1, 0.15, 0.272, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"25\"] = [1, 0.662, 0.15, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"26\"] = [1, 0.662, 0.15, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"27\"] = [1, 0.662, 0.15, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"28\"] = [1, 0.662, 0.15, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"29\"] = [1, 0.662, 0.15, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"30\"] = [1, 0.662, 0.15, 0.025];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"31\"] = [3, 0.40, 0.45, 'NA', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-IO-12-1W-UP\"][\"32\"] = [3, 0.40, 0.45, 'NA', 0.3, 'verdana', Font.BOLD];\nDEV_LIST.push('HmIP-WRC6');/" $file
 echo "$file erfolgreich fuer Haus-Bus.de HBW-IO-12-1W-UP angepasst";
fi
if grep -q HBW-LC4-IN4-DR "$file"; then
   echo "$file ist schon fuer Haus-Bus.de HBW-LC4-IN4-DR konfiguriert"
else
 if [ ! -f "$file.backup" ];
 then
  echo "Erstelle Backup: $file.backup";
  cp $file $file.backup
 else
   echo "Backup existiert bereits: $file.backup";
 fi    
 sed -i -e "s/DEV_LIST.push('HmIP-WRC6');/DEV_LIST.push('HBW-LC4-IN4-DR');\nDEV_DESCRIPTION[\"HBW-LC4-IN4-DR\"] = \"HBW-LC4-IN4-DR\";\nDEV_PATHS[\"HBW-LC4-IN4-DR\"] = new Object();\nDEV_PATHS[\"HBW-LC4-IN4-DR\"][\"50\"] = \"\/config\/img\/devices\/50\/179_hbw-lc4-in4-dr_thumb.png\";\nDEV_PATHS[\"HBW-LC4-IN4-DR\"][\"250\"] = \"\/config\/img\/devices\/250\/179_hbw-lc4-in4-dr.png\";\nDEV_HIGHLIGHT[\"HBW-LC4-IN4-DR\"] = new Object();\nDEV_LIST.push('HmIP-WRC6');/" $file
 echo "$file erfolgreich fuer Haus-Bus.de HBW-LC4-IN4-DR angepasst";
fi

