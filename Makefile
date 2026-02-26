# make seems to be slightly faster than my parallel compilation bash script
# it's also simpler and cleaner

# 4 jobs in parallel, and don't print each executed command (since that
# clutters the emacs display)
MAKEFLAGS += -j4 -s

COMPILE_FLAGS = -c -g -std=c89 -fno-builtin -pedantic -Wall -Wextra -Werror -Wconversion -Wshadow -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement


Chessamphetamine: game.o maxigin_imp.o mingin_imp.o
	gcc -o Chessamphetamine mingin_imp.o maxigin_imp.o game.o -lX11 -lXrandr -lGLX -lGL -lasound -lpthread

.c.o:
	gcc ${COMPILE_FLAGS} -o $@ $<

maxigin_imp.o: maxigin.h mingin.h maxigin_imp.c

mingin_imp.o: mingin.h mingin_imp.c

game.o: maxigin.h mingin.h game.c


