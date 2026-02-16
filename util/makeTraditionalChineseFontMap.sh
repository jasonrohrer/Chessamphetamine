
fontFile="sourceFonts/fusionPixel/fusion-pixel-12px-proportional-zh_hant.pcf"


gcc -g -o makeFontMap makeFontMap.c


cat ../data/traditionalChinese.txt > tcTemp.txt


./makeFontMap ./tcTemp.txt ../data/traditionalChineseFont.txt

rm tcTemp.txt




./makeFinishedFont.sh $fontFile ../data/traditionalChineseFont.txt ../data/traditionalChineseFont.tga
