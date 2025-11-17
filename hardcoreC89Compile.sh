# This is the compilation command that I used during development to force
# myself to stick to pure c89.
# 

gcc -g -std=c89 -fno-builtin -pedantic -Wall -Wextra -Werror -Wconversion -Wshadow -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement -o Chessamphetamine *.c -lX11 -lXext