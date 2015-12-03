version=23
hexfile="v$version"".hex"

baseDir="`dirname "$BASH_SOURCE"`/.."
jaja="$baseDir/Mac"

"$jaja/avrdude" -C"$jaja/avrdude.conf" -v -v -v -v -patmega2560 -cusbasp -Pusb -e -Ulock:w:0x3F:m -Uefuse:w:0xFD:m -Uhfuse:w:0xD8:m -Ulfuse:w:0xFF:m

"$jaja/avrdude" -C"$jaja/avrdude.conf" -v -v -v -v -patmega2560 -cusbasp -Pusb -Uflash:w:"$jaja/ATmegaBOOT_168_mega2560_miniE2M.hex":i -Ulock:w:0x0F:m 
