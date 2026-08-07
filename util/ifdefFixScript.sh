

for file in *.h; do
	
	dotHFile=$file



	cat $dotHFile | sed "s/#ifdef \([A-Z_]*\)_IMPLEMENTATION/#endif\n\n\n\n#ifdef  \1_IMPLEMENTATION\n\n#ifndef \1_IMPLEMENTATION_INCLUDED\n#define \1_IMPLEMENTATION_INCLUDED\n\n/" > tempOut.h

	mv tempOut.h $dotHFile

done
