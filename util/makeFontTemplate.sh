#!/usr/bin/bash



fontFile="/home/jasonrohrer/Downloads/fusionPixelFont/fusion-pixel-12px-proportional-zh_hant.pcf"

#fontFile="/home/jasonrohrer/Downloads/fusionPixelFont/fusion-pixel-10px-proportional-latin.pcf"

fontSize=12

#fontFile="/usr/share/fonts/X11/misc/unifont.pcf.gz"


if [ $# -ne 2 ]
then
   echo ""
   echo "Usage"

   echo "bash makeFontTemplate.sh  englishFont.txt   englishFontTemplate.png"
   echo ""

   exit 1
fi

i=0

bgA="#eeeeee"
bgB="#ffffff"

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

	mogrify -background white -gravity center -extent 12x12 $outFileName

	mogrify -fill "$bgA" -opaque white $outFileName

	# place next to an empty square of opposite grid color
	convert $outFileName -size 12x12 xc:"$bgB" +append temp.png

	mv temp.png $outFileName
	
	# swap bg colors to make checkerboard
	bgTmp=$bgA
	bgA=$bgB
	bgB=$bgTmp
	
	((i++))
done < "$1"

montage tile_*.png \
  -tile 1x \
  -geometry +0+0 \
  $2


rm tile_*.png
   
