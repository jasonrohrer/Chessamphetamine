

if [ pieceSpecs.h -ot pieceSpecsDef.txt ];
then
	echo "pieceSpecs.h is behind pieceSpecsDef.txt"
	./genPieceSpecsCodeFromInput.sh > pieceSpecs.h

	touch pieceSpecsDef.txt
	touch -r pieceSpecsDef.txt pieceSpecs.h pieceSpecsInput.txt
fi


if [ pieceSpecs.h -ot pieceSpecsInput.txt ];
then
	echo "pieceSpecs.h is behind pieceSpecsInput.txt"
    ./genPieceSpecsCodeFromInput.sh > pieceSpecs.h

	touch pieceSpecsDef.txt
	touch -r pieceSpecsDef.txt pieceSpecs.h pieceSpecsInput.txt
fi


if [ pieceSpecsInput.txt -ot pieceSpecs.h ];
then
	echo "pieceSpecsInput.txt is behind pieceSpecs.h"
    ./genPieceSpecsInputFromCode.sh > pieceSpecsInput.h

	touch pieceSpecsDef.txt
	touch -r pieceSpecsDef.txt pieceSpecs.h pieceSpecsInput.txt
fi
