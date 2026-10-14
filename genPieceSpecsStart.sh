pieces=$(grep "enum defining all possible chess piece types" chess.h -A 15 | tail +3 | sed "s/ =.*//" | sed "s/ *//" | sed "s/,//")

specs=$(grep "@" pieceSpecsInput.txt | sed "s/ .*//" | tail +2 | sed "s/@//")

listNames=$(grep "@" pieceSpecsInput.txt | sed "s/[^ ]*  *//" | sed "s/ .*//"  | tail +2)

specsArray=( $specs )

listNamesArray=( $lisNames )

pieceNum=1

for piece in $pieces;
do
	echo ""
	echo ""
	
	echo "\$$piece"

	index=0

	for list in $listNames;
	do
		spec=${specsArray[$index]};


		val=$(grep "#define $list" *.h -A 14 -h | tail +2 | sed -n "${pieceNum}p" | sed "s/V( *C, *[0-9]*, *[A-Za-z]*, *//" | sed "s/ *//" | sed 's/  *) *\\$//')

		echo "$spec $val"
		((index++))
	done

	((pieceNum++))
done
