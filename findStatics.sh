if [ $# -ne 1 ]
then
	echo "Need name of source file."
	exit
fi

sourceFile=$1


grep "^static " $1 | sed "s/(.*//" | sed "s/\[.*//" | sed "s/;//" | sed "s/ =.*//" | sed "s/.* //" | sed "s/\*//" | sort | uniq