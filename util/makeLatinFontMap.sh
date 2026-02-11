
gcc -g -o makeFontMap makeFontMap.c


cat ../data/english.txt ../data/french.txt > latinTemp.txt


./makeFontMap ./latinTemp.txt ../data/latinFont.txt

rm latinTemp.txt


./makeFontTemplate.sh ../data/latinFont.txt latinTemplate.png latinEmptyGrid.png
