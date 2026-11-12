# make seems to be slightly faster than my parallel compilation bash script
# it's also simpler and cleaner

# 4 jobs in parallel, and don't print each executed command (since that
# clutters the emacs display)
MAKEFLAGS += -j4 -s

COMPILE_FLAGS = -c -g -std=c89 -fno-builtin -pedantic -Wall -Wextra -Werror -Wconversion -Wshadow -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement


Chessamphetamine: game.o maxigin_imp.o mingin_imp.o header_impA.o header_impB.o
	gcc -o Chessamphetamine mingin_imp.o maxigin_imp.o game.o header_impA.o header_impB.o -lX11 -lXrandr -lGLX -lGL -lasound -lpthread

.c.o:
	gcc ${COMPILE_FLAGS} -o $@ $<


# a special dependency file that changes when any of the .h
# files in allHeaders change in their non-implementation section
# i.e., whenever the public call signatures change

interfaces.txt: $(wildcard *.h)
	gcc -E -P -dD -x c allGameHeaders.h > interfaces.temp
	cmp -s interfaces.temp interfaces.txt || mv interfaces.temp interfaces.txt
	rm -f interfaces.temp


maxigin_imp.o: maxigin.h mingin.h maxigin_imp.c gameSize.h

mingin_imp.o: mingin.h mingin_imp.c

header_impA.o: interfaces.txt header_impA.c allGameHeaders.h pieceSprites.h chess.h moveAnim.h shop.h maxigin.h mingin.h

header_impB.o: interfaces.txt header_impB.c allGameHeaders.h board.h pieceSpecs.h memoryRegister.h particleSprite.h gameSize.h simTest.h money.h numbers.h checkDisplay.h util.h colors.h pinch.h fixedMath.h particleSystem.h chessArrayCheck.h pieceDescriptions.h nav.h levels.h deck.h button.h hearts.h sideBoard.h deckView.h cost.h rarity.h roll.h slotLift.h formation.h unlocks.h playerDeck.h runStats.h postMoveEffects.h screen.h rarityWindow.h moveDiagramWindow.h vfdDigit.h tipDisplay.h maxigin.h mingin.h

game.o: interfaces.txt allGameHeaders.h game.c maxigin.h mingin.h

