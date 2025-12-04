if [ $# -ne 2 ]
then
	echo "Usage:"
	echo "./giveStaticsPrefix.sh sourceFile.h prefix_"
	exit
fi

prefix=$2

./findStatics.sh $1 > staticsPrefix.tmp



while IFS= read -r line; do

	#sed -i 's/$line/$prefix$line/g' $1

	echo "$line  ->  $prefix$line"
	
done < "staticsPrefix.tmp"


echo ""
echo "About to make these above ^^ replacements in $1"

echo ""

echo -n "Press ENTER to continue: "

read


while IFS= read -r line; do
	
	sed -i "s/$line/$prefix$line/g" $1
	
done < "staticsPrefix.tmp"


rm staticsPrefix.tmp