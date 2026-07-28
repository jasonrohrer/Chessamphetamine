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


/* enum defining all possible rarities
   must be in order from least rare to most rare
   (contraband never occurs)
*/
enum{
    contraband = 0,
    common,
    uncommon,
    rare,
    legendary,
    impossible,
    NUM_RARITIES };

#define  FIRST_RARITY           contraband
#define  FIRST_ROLLABLE_RARITY  common
#define  MOST_RARE_RARITY       impossible


void rarityInit( void );


ChessPiece rarityRollPiece( void );


int rarityGet( ChessPiece  inPiece );


void raritySetDrawColor( ChessPiece  inPiece );


int rarityGetLangKey( ChessPiece  inPiece );


/* gets how often a given rarity should occur
   once in X rolls

   -1 means never
*/
int rarityGetOneInCount( int  inRarity );




#ifdef RARITY_IMPLEMENTATION


#include "memoryRegister.h"
#include "roll.h"


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


static  int       rarityLangKeys  [ NUM_RARITIES ];

static  int       rarityOneInCount[ NUM_RARITIES ];

static  RollInfo  rarityRolls     [ NUM_RARITIES ];

static  int       rarityPools     [ NUM_RARITIES ];


void rarityInit( void ) {

    int  i;

    int  tempPoolItems[ NUM_CHESS_PIECES ];

    rarityColorMapSprite = maxigin_initSprite( "rarityColorMap.tga" );

    rarityLangKeys[ contraband ]  =  maxigin_initTranslationKey( "contraband" );
    rarityLangKeys[ common     ]  =  maxigin_initTranslationKey( "common"     );
    rarityLangKeys[ uncommon   ]  =  maxigin_initTranslationKey( "uncommon"   );
    rarityLangKeys[ rare       ]  =  maxigin_initTranslationKey( "rare"       );
    rarityLangKeys[ legendary  ]  =  maxigin_initTranslationKey( "legendary"  );
    rarityLangKeys[ impossible ]  =  maxigin_initTranslationKey( "impossible" );

    rarityOneInCount[ contraband ] =   -1;
    rarityOneInCount[ common     ] =    1;
    rarityOneInCount[ uncommon   ] =    4;
    rarityOneInCount[ rare       ] =   20;
    rarityOneInCount[ legendary  ] =  100;
    rarityOneInCount[ impossible ] = 1000;

    for( i = FIRST_ROLLABLE_RARITY;
         i < NUM_RARITIES;
         i ++ ) {

        rollSetup( &( rarityRolls[ i ] ),
                   rarityOneInCount[ i ],
                   1,
                   1 );

        /* special name for these, so saved game breaks if the weights change
           we roll the weight right into the discription, which is checked
           to match at load time */
        maxigin_initRegisterStaticMemory(
            &( rarityRolls[ i ] ),
            sizeof( RollInfo ),
            maxigin_stringConcat(
                "rarityRolls=",
                maxigin_intToString( rarityOneInCount[ i ] ) ) );
        }

    for( i = FIRST_ROLLABLE_RARITY;
         i < NUM_RARITIES;
         i ++ ) {

        int  numItems = 0;
        int  p;

        for( p = FIRST_CHESS_PIECE;
             p < NUM_CHESS_PIECES;
             p ++ ) {

            if( pieceRarity[ p ] == i ) {

                tempPoolItems[ numItems ] = p;
                numItems ++;
                }
            }
        
        rarityPools[ i ] = rollPoolSetup( numItems,
                                          tempPoolItems,
                                          1,
                                          1 );
        }
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


int rarityGetOneInCount( int  inRarity ) {
    return rarityOneInCount[ inRarity ];
    }


ChessPiece rarityRollPiece( void ) {

    int  i;

    int  pickedRarity = FIRST_ROLLABLE_RARITY;

    /* start with most rare, see if that hits
       if not, fall through to next, less-rare tier
       
       if we hit nothing, we leave pickedRarity set at FIRST_ROLLABLE_RARITY
       which is the default
    */
    for( i = MOST_RARE_RARITY;
         i > FIRST_ROLLABLE_RARITY;
         i -- ) {

        if( roll( &( rarityRolls[i] ) ) ) {
            pickedRarity = i;
            break;
            }
        }

    /* now that we have a rarity, roll a piece from that pool */

    return (ChessPiece)( rollItem( rarityPools[ pickedRarity ] ) );
    }
        



#endif

#endif
