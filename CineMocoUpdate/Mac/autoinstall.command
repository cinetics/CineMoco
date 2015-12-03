#!/bin/bash
version=25
hexfile="v$version"".hex"

baseDir="`dirname "$BASH_SOURCE"`/.."

# Step 3:  Place "hardware" in ~/Documents/Arduino
printf "\nCopying 'hardware' to ~/Documents/Arduino ...\n"
cp -rp "$baseDir/hardware ~/Documents/Arduino"
printf "Done.\n"

# Step 4:  Place "glcd" and "MoCoM" in ~/Documents/Arduino/libraries
printf "\nCopying 'glcd' & 'MoCoM' to ~/Documents/Arduino/libraries ...\n"
cp -rp "$baseDir/glcd $baseDir/MoCoM ~/Documents/Arduino/libraries"
printf "Done.\n\n"

# Step <<all the rest>>: Upload *.hex file to chip.
jaja="$baseDir/Mac"

printf "\nBegin uploading cineMoco binary!!\n\n"

# Because some of the controllers will enumerate as *usbserial-xxxxxxxx
"$jaja/avrdude" -C "$jaja/avrdude.conf" -v -p m2560 -c stk500v1 -P /dev/cu.usbserial* -b 57600 -D -U flash:w:"$jaja/$hexfile"
#===========================


#printf "Getting Arduino ..."
#wget http://arduinodownload

#open $baseDir/CineMoco-v2.5/miniE_cineMoco/miniE_cineMoco.ino
