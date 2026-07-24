/*
  Include in your C code wherever like so:

      #include "rarity.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define RARITY_IMPLEMENTATION
      #include "rarity.h"

*/

#ifndef RARITY_H_INCLUDED
#define RARITY_H_INCLUDED


/* enum defining all possible rarities */
enum{
    contraband = 0,
    common,
    uncommon,
    rare,
    legendary,
    NUM_RARITIES };

#define  FIRST_RARITY  contraband


void rarityInit( void );


int rarityGet( ChessPiece  inPiece );


void raritySetDrawColor( ChessPiece  inPiece );


int rarityGetLangKey( ChessPiece  inPiece );




#ifdef RARITY_IMPLEMENTATION


static  int  rarityColorMapSprite  =  -1;



#define PIECE_RARITY( C, V )  \
    V( C, 0,   noPiece,      contraband )    \
    V( C, 1,   pawn,         contraband )    \
    V( C, 2,   bishop,       common     )    \
    V( C, 3,   knight,       common     )    \
    V( C, 4,   rook,         uncommon   )    \
    V( C, 5,   queen,        rare       )    \
    V( C, 6,   king,         contraband )    \
    V( C, 7,   laserRook,    legendary  )    \
    V( C, 8,   laserPawn,    rare       )    \
    V( C, 9,   doublingPawn, rare       )    \
    V( C, 10,  addingRook,   legendary  )    \
    V( C, 11,  rocket,       rare       )

static  int  pieceRarity[] = {
    MAKE_CHESS_ARRAY( PIECE_RARITY )
    };

CHECK_CHESS_ARRAY( pieceRarity,
                   PIECE_RARITY );


static  int  rarityLangKeys[ NUM_RARITIES ];


void rarityInit( void ) {

    rarityColorMapSprite = maxigin_initSprite( "rarityColorMap.tga" );

    rarityLangKeys[ contraband ]  =  maxigin_initTranslationKey( "contraband" );
    rarityLangKeys[ common     ]  =  maxigin_initTranslationKey( "common"     );
    rarityLangKeys[ uncommon   ]  =  maxigin_initTranslationKey( "uncommon"   );
    rarityLangKeys[ rare       ]  =  maxigin_initTranslationKey( "rare"       );
    rarityLangKeys[ legendary  ]  =  maxigin_initTranslationKey( "legendary"  );
    }


int rarityGet( ChessPiece  inPiece ) {

    return pieceRarity[ inPiece & CHESS_TYPE_MASK ];
    }

    


void raritySetDrawColor( ChessPiece  inPiece ) {

    MaxiginColor  c;

    maxigin_getSpritePixel( rarityColorMapSprite,
                            rarityGet( inPiece ),
                            0,
                            &c );

    maxigin_drawSetColor( c.comp.red,
                          c.comp.green,
                          c.comp.blue,
                          c.comp.alpha );
    }



int rarityGetLangKey( ChessPiece  inPiece ) {

    int  r  =  rarityGet( inPiece );

    return rarityLangKeys[ r ];
    }





#endif

#endif
