file="/www/config/devdescr/DEVDB.tcl"
if grep -q HBW-SD6-Multikey "$file"; then
   echo "$file ist schon fuer Haus-Bus.de Taster (HBW-SD6-Multikey) konfiguriert"
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
 sed -i -e "s/DEV_HIGHLIGHT   {/DEV_HIGHLIGHT   {HBW-SD6-Multikey {{\"1\" 0.045 0.138 0.025} {\"2\" 1 0.90 0.138 0.025} {\"3\" 0.045 0.474 0.025} {\"4\" 1 0.90 0.474 0.025} {\"5\" 0.045 0.809 0.025} {\"6\" 1 0.90 0.809 0.025} {\"7\" 3 0.46 0.46 'S1' 0.3 'verdana' Font.BOLD} {\"8\" 3 0.46 0.46 'S2' 0.3 'verdana' Font.BOLD} {\"9\" 3 0.46 0.46 'S3' 0.3 'verdana' Font.BOLD} {\"10\" 3 0.46 0.46 'S4' 0.3 'verdana' Font.BOLD} {\"11\" 3 0.46 0.46 'S5' 0.3 'verdana' Font.BOLD} {\"12\" 3 0.46 0.46 'S6' 0.3 'verdana' Font.BOLD} {\"13\" 0.35 0.138 0.025} {\"14\" 1 0.6 0.138 0.025} {\"15\" 0.35 0.474 0.025} {\"16\" 1 0.6 0.474 0.025} {\"17\" 0.35 0.809 0.025} {\"18\" 1 0.6 0.809 0.025} {\"19\" 3 0.2 0.125 'LED1' 0.3 'verdana' Font.BOLD} {\"20\" 3 0.2 0.125 'LED2' 0.3 'verdana' Font.BOLD} {\"21\" 3 0.2 0.125 'LED3' 0.3 'verdana' Font.BOLD} {\"22\" 3 0.2 0.125 'LED4' 0.3 'verdana' Font.BOLD} {\"23\" 3 0.2 0.125 'LED5' 0.3 'verdana' Font.BOLD} {\"24\" 3 0.2 0.125 'LED6' 0.3 'verdana' Font.BOLD} {\"25\" 0.82 0.138 0.02} {\"26\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"27\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"28\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"29\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"30\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"31\" 0.6 0.40 0.02}  {\"32\" 3 0.46 0.4 'BR' 0.3 'verdana' Font.BOLD}} /" $file
 echo "$file erfolgreich fuer Haus-Bus.de Taster (HBW-SD6-Multikey) angepasst";
fi

if grep -q HBW-SD6-Multikey_v1 "$file"; then
   echo "$file ist schon fuer Haus-Bus.de Taster (HBW-SD6-Multikey_v1) konfiguriert"
else
 if [ ! -f "$file.backup" ];
 then
  echo "Erstelle Backup: $file.backup";
  cp $file $file.backup
 else
   echo "Backup existiert bereits: $file.backup";
 fi    
 sed -i -e 's/DEV_LIST {/DEV_LIST {HBW-SD6-Multikey_v1 /' $file
 sed -i -e 's/DEV_DESCRIPTION {/DEV_DESCRIPTION {HBW-SD6-Multikey_v1 HBW-SD6-Multikey_v1 /' $file
 sed -i -e 's/DEV_PATHS       {/DEV_PATHS       {HBW-SD6-Multikey_v1 {{50 \/config\/img\/devices\/50\/177_hbw-sd6-multikey_v1_thumb.png} {250 \/config\/img\/devices\/250\/177_hbw-sd6-multikey_v1.png}} /' $file
 sed -i -e "s/DEV_HIGHLIGHT   {/DEV_HIGHLIGHT   {HBW-SD6-Multikey_v1 {{\"1\" 0.045 0.138 0.025} {\"2\" 1 0.90 0.138 0.025} {\"3\" 0.045 0.474 0.025} {\"4\" 1 0.90 0.474 0.025} {\"5\" 0.045 0.809 0.025} {\"6\" 1 0.90 0.809 0.025} {\"7\" 3 0.46 0.46 'S1' 0.3 'verdana' Font.BOLD} {\"8\" 3 0.46 0.46 'S2' 0.3 'verdana' Font.BOLD} {\"9\" 3 0.46 0.46 'S3' 0.3 'verdana' Font.BOLD} {\"10\" 3 0.46 0.46 'S4' 0.3 'verdana' Font.BOLD} {\"11\" 3 0.46 0.46 'S5' 0.3 'verdana' Font.BOLD} {\"12\" 3 0.46 0.46 'S6' 0.3 'verdana' Font.BOLD} {\"13\" 0.35 0.138 0.025} {\"14\" 1 0.6 0.138 0.025} {\"15\" 0.35 0.474 0.025} {\"16\" 1 0.6 0.474 0.025} {\"17\" 0.35 0.809 0.025} {\"18\" 1 0.6 0.809 0.025} {\"19\" 3 0.2 0.125 'LED1' 0.3 'verdana' Font.BOLD} {\"20\" 3 0.2 0.125 'LED2' 0.3 'verdana' Font.BOLD} {\"21\" 3 0.2 0.125 'LED3' 0.3 'verdana' Font.BOLD} {\"22\" 3 0.2 0.125 'LED4' 0.3 'verdana' Font.BOLD} {\"23\" 3 0.2 0.125 'LED5' 0.3 'verdana' Font.BOLD} {\"24\" 3 0.2 0.125 'LED6' 0.3 'verdana' Font.BOLD} {\"25\" 0.82 0.138 0.02} {\"26\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"27\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"28\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"29\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"30\" 3 0.1 0.3 'DS1820' 0.3 'verdana' Font.BOLD} {\"31\" 0.6 0.40 0.02}  {\"32\" 3 0.46 0.4 'BR' 0.3 'verdana' Font.BOLD}} /" $file
 echo "$file erfolgreich fuer Haus-Bus.de Taster (HBW-SD6-Multikey_v1) angepasst";
fi

file="/www/webui/webui.js"
if grep -q HBW-SD6-Multikey "$file"; then
   echo "$file ist schon fuer Haus-Bus.de Taster (HBW-SD6-Multikey) konfiguriert"
else
 if [ ! -f "$file.backup" ];
 then
  echo "Erstelle Backup: $file.backup";
  cp $file $file.backup
 else
   echo "Backup existiert bereits: $file.backup";
 fi    
 sed -i -e "s/DEV_LIST.push('HmIP-WRC6');/DEV_LIST.push('HBW-SD6-Multikey');\nDEV_DESCRIPTION[\"HBW-SD6-Multikey\"] = \"HBW-SD6-Multikey\";\nDEV_PATHS[\"HBW-SD6-Multikey\"] = new Object();\nDEV_PATHS[\"HBW-SD6-Multikey\"][\"50\"] = \"\/config\/img\/devices\/50\/177_hbw-sd6-multikey_thumb.png\";\nDEV_PATHS[\"HBW-SD6-Multikey\"][\"250\"] = \"\/config\/img\/devices\/250\/177_hbw-sd6-multikey.png\";\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"] = new Object();\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"1\"] = [1, 0.045, 0.138, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"2\"] = [1, 0.90, 0.138, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"3\"] = [1, 0.045, 0.474, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"4\"] = [1, 0.90, 0.474, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"5\"] = [1, 0.045, 0.809, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"6\"] = [1, 0.90, 0.809, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"7\"] = [3, 0.46, 0.46, 'S1', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"8\"] = [3, 0.46, 0.46, 'S2', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"9\"] = [3, 0.46, 0.46, 'S3', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"10\"] = [3, 0.46, 0.46, 'S4', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"11\"] = [3, 0.46, 0.46, 'S5', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"12\"] = [3, 0.46, 0.46, 'S6', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"13\"] = [1, 0.35, 0.138, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"14\"] = [1, 0.6, 0.138, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"15\"] = [1, 0.35, 0.474, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"16\"] = [1, 0.6, 0.474, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"17\"] = [1, 0.35, 0.809, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"18\"] = [1, 0.6, 0.809, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"19\"] = [3, 0.20, 0.125, 'LED1', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"20\"] = [3, 0.20, 0.125, 'LED2', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"21\"] = [3, 0.20, 0.125, 'LED3', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"22\"] = [3, 0.20, 0.125, 'LED4', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"23\"] = [3, 0.20, 0.125, 'LED5', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"24\"] = [3, 0.20, 0.125, 'LED6', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"25\"] = [1, 0.82, 0.138, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"26\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"27\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"28\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"29\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"30\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"31\"] = [1, 0.60, 0.40, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey\"][\"32\"] = [3, 0.46, 0.40, 'BR', 0.3, 'verdana', Font.BOLD];\nDEV_LIST.push('HmIP-WRC6');/" $file
 echo "$file erfolgreich fuer Haus-Bus.de Taster angepasst";
fi

if grep -q HBW-SD6-Multikey_v1 "$file"; then
   echo "$file ist schon fuer Haus-Bus.de Taster (HBW-SD6-Multikey_v1) konfiguriert"
else
 if [ ! -f "$file.backup" ];
 then
  echo "Erstelle Backup: $file.backup";
  cp $file $file.backup
 else
   echo "Backup existiert bereits: $file.backup";
 fi    
 sed -i -e "s/DEV_LIST.push('HmIP-WRC6');/DEV_LIST.push('HBW-SD6-Multikey_v1');\nDEV_DESCRIPTION[\"HBW-SD6-Multikey_v1\"] = \"HBW-SD6-Multikey_v1\";\nDEV_PATHS[\"HBW-SD6-Multikey_v1\"] = new Object();\nDEV_PATHS[\"HBW-SD6-Multikey_v1\"][\"50\"] = \"\/config\/img\/devices\/50\/177_hbw-sd6-multikey_v1_thumb.png\";\nDEV_PATHS[\"HBW-SD6-Multikey_v1\"][\"250\"] = \"\/config\/img\/devices\/250\/177_hbw-sd6-multikey_v1.png\";\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"] = new Object();\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"1\"] = [1, 0.045, 0.138, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"2\"] = [1, 0.90, 0.138, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"3\"] = [1, 0.045, 0.474, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"4\"] = [1, 0.90, 0.474, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"5\"] = [1, 0.045, 0.809, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"6\"] = [1, 0.90, 0.809, 0.025];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"7\"] = [3, 0.46, 0.46, 'S1', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"8\"] = [3, 0.46, 0.46, 'S2', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"9\"] = [3, 0.46, 0.46, 'S3', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"10\"] = [3, 0.46, 0.46, 'S4', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"11\"] = [3, 0.46, 0.46, 'S5', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"12\"] = [3, 0.46, 0.46, 'S6', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"13\"] = [1, 0.35, 0.138, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"14\"] = [1, 0.6, 0.138, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"15\"] = [1, 0.35, 0.474, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"16\"] = [1, 0.6, 0.474, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"17\"] = [1, 0.35, 0.809, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"18\"] = [1, 0.6, 0.809, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"19\"] = [3, 0.20, 0.125, 'LED1', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"20\"] = [3, 0.20, 0.125, 'LED2', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"21\"] = [3, 0.20, 0.125, 'LED3', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"22\"] = [3, 0.20, 0.125, 'LED4', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"23\"] = [3, 0.20, 0.125, 'LED5', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"24\"] = [3, 0.20, 0.125, 'LED6', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"25\"] = [1, 0.82, 0.138, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"26\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"27\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"28\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"29\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"30\"] = [3, 0.10, 0.30, 'DS1820', 0.3, 'verdana', Font.BOLD];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"31\"] = [1, 0.60, 0.40, 0.02];\nDEV_HIGHLIGHT[\"HBW-SD6-Multikey_v1\"][\"32\"] = [3, 0.46, 0.40, 'BR', 0.3, 'verdana', Font.BOLD];\nDEV_LIST.push('HmIP-WRC6');/" $file
 echo "$file erfolgreich fuer Haus-Bus.de Taster (HBW-SD6-Multikey_v1) angepasst";
fi


