pieces=$(grep "enum defining all possible chess piece types" chess.h -A 15 | tail +3 | sed "s/ =.*//" | sed "s/ *//" | sed "s/,//")

specs=$(grep "@" pieceSpecsDef.txt | sed "s/ .*//" | tail +2 | sed "s/@//")

listNames=$(grep "@" pieceSpecsDef.txt | sed "s/[^ ]*  *//" | sed "s/ .*//"  | tail +2)

specsArray=( $specs )

listNamesArray=( $listNames )

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

		printf "%-24s %s\n" "$spec" "$val"
		((index++))
	done

	((pieceNum++))
done
