#!/usr/bin/bash



fontFile="sourceFonts/fusionPixel/fusion-pixel-12px-proportional-zh_hant.pcf"


fontSize=12



if [ $# -ne 3 ]
then
   echo ""
   echo "Usage"

   echo "bash makeFontTemplate.sh  font.txt  fontTemplate.png  emptyGrid.png"
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
	outGridFileName="grid_${paddedNum}.png"
	
	fontforge -lang=ff -c "Open(\"$fontFile\"); Select($codepoint); Export(\"$outFileName\", $fontSize );"

	mogrify -background white -gravity center -extent 12x16 $outFileName

	mogrify -fill "$bgA" -opaque white $outFileName

	# place next to an empty square of opposite grid color
	convert $outFileName -size 12x16 xc:"$bgB" +append temp.png

	mv temp.png $outFileName

	convert -size 12x16 xc:"$bgB" $outGridFileName
	
	# pink descender zone
	convert $outFileName -fill "rgba(255,0,255,0.15)" -draw "rectangle 0,13 23,15" temp.png

	mv temp.png $outFileName

	convert $outGridFileName -fill "rgba(255,0,255,0.15)" -draw "rectangle 0,13 23,15" temp.png

	mv temp.png $outGridFileName

	
	# blue accent zone
	convert $outFileName -fill "rgba(0,0,255,0.15)" -draw "rectangle 0,0 23,3" temp.png

	mv temp.png $outFileName

	convert $outGridFileName -fill "rgba(0,0,255,0.15)" -draw "rectangle 0,0 23,3" temp.png

	mv temp.png $outGridFileName

	
	# yellow lower-case ascender zone
	convert $outFileName -fill "rgba(255,255,0,0.15)" -draw "rectangle 0,4 23,6" temp.png

	mv temp.png $outFileName


	convert $outGridFileName -fill "rgba(255,255,0,0.15)" -draw "rectangle 0,4 23,6" temp.png

	mv temp.png $outGridFileName
	
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
   

montage grid_*.png \
  -tile 1x \
  -geometry +0+0 \
  $3

rm grid_*.png


# one black pixel in corner of empty grid so that Aesprite palette has black
convert $3 -fill "rgba(0,0,0,1.0)" -draw "rectangle 0,0 0,0" temp.png

mv temp.png $3


# actually, invert the grid so we can draw with white instead, since
# that's the eventual target for pixel fonts

mogrify -negate $3
