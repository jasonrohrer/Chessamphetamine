# This is the compilation command that I used during development to force
# myself to stick to pure c89.
#

# This script does much faster conditional and parallel compilation.
#

compileFlags="-g -std=c89 -fno-builtin -pedantic -Wall -Wextra -Werror -Wconversion -Wshadow -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement"

pidA=0
pidB=0
pidC=0

anyRebuilt=0

if [[ mingin.h -nt mingin_imp.o || mingin_imp.c -nt mingin_imp.o ]]; then
	rm -f mingin_imp.o
	gcc $compileFlags -c mingin_imp.c -o mingin_imp.o &
	pidA=$!
	anyRebuilt=1
fi


if [[ mingin.h -nt maxigin_imp.o || maxigin.h -nt maxigin_imp.o ||  maxigin_imp.c -nt maxigin_imp.o ]]; then
	rm -f maxigin_imp.o
	gcc $compileFlags -c maxigin_imp.c -o maxigin_imp.o &
	pidB=$!
	anyRebuilt=1
fi


if [[ mingin.h -nt game.o || maxigin.h -nt game.o || game.c -nt game.o ]]; then
	rm -f game.o
	gcc $compileFlags -c game.c -o game.o &
	pidC=$!
	anyRebuilt=1
fi

if [[ "$pidA" -ne 0 ]]; then
	wait $pidA
fi

if [[ "$pidB" -ne 0 ]]; then
	wait $pidB
fi

if [[ "$pidC" -ne 0 ]]; then
	wait $pidC
fi


if [[ $anyRebuilt -ne 0 ]]; then
	if [[ -e mingin_imp.o && -e maxigin_imp.o && -e game.o ]]; then
		gcc -o Chessamphetamine mingin_imp.o maxigin_imp.o game.o -lX11 -lXrandr -lGLX -lGL -lasound -lpthread
	fi
fi

