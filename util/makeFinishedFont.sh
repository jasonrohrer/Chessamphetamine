#!/usr/bin/bash



fontSize=12



if [ $# -ne 3 ]
then
   echo ""
   echo "Usage"

   echo "bash makeFinishedFont.sh  fontSrc.pcf font.txt  outFont.tga"
   echo ""

   exit 1
fi

fontFile=$1

i=0


while IFS=$' \t\r\n' read -r line
do
	# Process the line here
	printf -v paddedNum "%05d" "$i"
	echo "Processing line $paddedNum: $line"

	codepoint=$(printf '0x%04X' "'$line")

	echo "Processing codepoint $codepoint with FontForge"
	
	# call FontForge with the script and pass the codepoint.

	outFileName="tile_${paddedNum}.png"
	
	fontforge -lang=ff -c "Open(\"$fontFile\"); Select($codepoint); Export(\"$outFileName\", $fontSize );"

	mogrify -background white -gravity center -extent 12x16 $outFileName
	

	((i++))
done < "$2"

montage tile_*.png \
  -tile 1x \
  -geometry +0+0 \
  fontTemp.png


rm tile_*.png


# invert result, white font on black background
mogrify -negate fontTemp.png

mogrify -transparent black fontTemp.png

convert fontTemp.png -auto-orient -type truecolormatte $3

rm fontTemp.png
