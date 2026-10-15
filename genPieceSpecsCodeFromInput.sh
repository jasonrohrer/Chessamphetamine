specs=$(cat pieceSpecsDef.txt | sed "/^#/d" | grep "@" | sed "s/ .*//" | sed "s/@//")

listNames=$(cat pieceSpecsDef.txt | sed "/^#/d" | grep "@" | sed "s/[^ ]*  *//" | sed "s/ .*//")

defaultVals=$(cat pieceSpecsDef.txt | sed "/^#/d" | grep "@" | sed "s/[^ ]*  *//" | sed "s/[A-Z_]* *//")

pieceTags=$( grep '\$' pieceSpecsInput.txt )
pieceNames=$( grep '\$' pieceSpecsInput.txt | sed 's/\$//' )


specsArray=( $specs )

listNamesArray=( $listNames )

defaultValsArray=( $defaultVals )

specNum=0

echo "#ifndef PIECE_SPECS_H_INCLUDED"
echo "#define PIECE_SPECS_H_INCLUDED"


for spec in $specs;
do

	list=${listNamesArray[$specNum]}

	default=${defaultValsArray[$specNum]}

	echo
	echo
	echo "#define $list( C, V ) \\"

	pieceNum=0

	for piece in $pieceNames;
	do
		

		block=$( awk -v piece="$piece" '$0 == "$" piece { found=1; next } /^\$/ && found  { exit } found { print }' pieceSpecsInput.txt)

		value=$(echo "$block" | sed -n "s/^${spec}  *//p")

		if [ -z "$value" ]; then
			value="$default"
		fi

		printf "    V( C, %3d,  %-15s %-23s ) \\" "$pieceNum" "$piece," "$value"
		echo
		
		((pieceNum++))
	done


	
	((specNum++))
done


echo
echo

echo "#endif"
