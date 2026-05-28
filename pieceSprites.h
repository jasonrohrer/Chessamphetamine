/*
  Include in your C code wherever like so:

      #include "pieceSprites.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define PIECE_SPRITES_IMPLEMENTATION
      #include "pieceSprites.h"

*/

#ifndef PIECE_SPRITES_H_INCLUDED
#define PIECE_SPRITES_H_INCLUDED



#include "chess.h"
#include "particleSprite.h"



void pieceSpritesInit( void );


void drawPiece( ChessPiece  inPiece,
                int         inBaseCenterX,
                int         inBaseCenterY );

/* draws the shadow component of a piece */
void drawPieceShadowOnly( ChessPiece  inPiece,
                          int         inBaseCenterX,
                          int         inBaseCenterY );

void drawPieceGlowOnly( ChessPiece     inPiece,
                        int            inBaseCenterX,
                        int            inBaseCenterY,
                        unsigned char  inAlpha );


/* draws the non-shadow component of a piece (base sprite and glow) */
/* sets color automatically based on which side piece is from */
void drawPieceBaseAndGlowOnly( ChessPiece  inPiece,
                               int         inBaseCenterX,
                               int         inBaseCenterY );


void drawPieceBaseAndGlowOnlyNoColor( ChessPiece  inPiece,
                                      int         inBaseCenterX,
                                      int         inBaseCenterY );


void drawPieceSparkles( ChessPiece     inPiece,
                        int            inBaseCenterX,
                        int            inBaseCenterY,
                        MaxiginRand   *inRand,
                        int            inNumSparkles,
                        unsigned char  inAlpha );


/* start with inProgress = 0
   returns new progress after one explosion step
   returns -1 when explosion done
*/
int stepExplodingPiece( int  inProgress );

int getExplodingPieceMaxProgress( void );


void drawExplodingPiece( ChessPiece  inPiece,
                         int         inBoardCenterX,
                         int         inBoardCenterY,
                         int         inRow,
                         int         inCol,
                         int         inProgress );



/* used to mask out some board squares when drawing board state */
typedef struct DrawBoardMask{

        /* 1 to draw, 0 to skip */
        char  grid[BH][BW];

    } DrawBoardMask;



void getRowRangeMask( DrawBoardMask  *inMask,
                      int             inStartRow,
                      int             inEndRow );

void getRowMask( DrawBoardMask  *inMask,
                 int             inRow );

void getRowsAboveMask( DrawBoardMask  *inMask,
                       int             inRow );

void getRowsBelowMask( DrawBoardMask  *inMask,
                       int             inRow );

void clearMaskSpot( DrawBoardMask  *inMask,
                    int             inRow,
                    int             inCol );

void clearMaskRow( DrawBoardMask  *inMask,
                   int             inRow );



/* inMoveProgress goes from 0 to inMoveProgressMax
   if inMoveProgressMax is 0, inMove is ignored

   if inMask is 0 (null), it is ignored
*/
void drawBoardState( BoardState     *inState,
                     char            inCheckmate,
                     char            inStalemate,
                     char            inDrawGame,
                     int             inLoserColor,
                     Move           *inMove,
                     int             inMoveProgress,
                     int             inMoveProgressMax,
                     int             inBoardCenterX,
                     int             inBoardCenterY,
                     DrawBoardMask  *inMask );



/* sets maxigin draw color to piece color for future draw calls
   inPieceColor  is CHESS_WHITE or CHESS_BLACK */
void drawSetPieceColor( int  inPieceColor );



ChessPiece getPointerOverPiece( BoardState  *inState,
                                int          inBoardCenterX,
                                int          inBoardCenterY,
                                int         *outRow,
                                int         *outCol );



void drawPieceHighlight( BoardState    *inState,
                         int            inBoardCenterX,
                         int            inBoardCenterY,
                         int            inPieceRow,
                         int            inPieceCol,
                         unsigned char  inFade );




#ifdef PIECE_SPRITES_IMPLEMENTATION


static  int  pieceBottomHeight  =  6;

static  int          pieceSpriteHandles[ NUM_CHESS_PIECES ];
static  int          pieceOffsetY      [ NUM_CHESS_PIECES ];


#define PIECE_SPRITE_FILE_LIST( C, V )            \
    V( C, 0,   noPiece,      ""                 ) \
    V( C, 1,   pawn,         "pawn.tga"         ) \
    V( C, 2,   bishop,       "bishop.tga"       ) \
    V( C, 3,   knight,       "knight.tga"       ) \
    V( C, 4,   rook,         "rook.tga"         ) \
    V( C, 5,   queen,        "queen.tga"        ) \
    V( C, 6,   king,         "king.tga"         ) \
    V( C, 7,   laserRook,    "rook.tga"         ) \
    V( C, 8,   laserPawn,    "pawn.tga"         ) \
    V( C, 9,   doublingPawn, "doublingPawn.tga" ) \
    V( C, 10,  addingRook,   "addingRook.tga"   ) \
    V( C, 11,  rocket,       "rocket.tga"       )

static const char *pieceSpriteFiles[] = {
    MAKE_CHESS_ARRAY( PIECE_SPRITE_FILE_LIST )
    };

CHECK_CHESS_ARRAY( pieceSpriteFiles,
                   PIECE_SPRITE_FILE_LIST );



/* extra sprite to be drawn on top of sprite with
   no piece-color modification,
   second index is 0 => white, 1 => black */

static  int          pieceSpriteExtraHandles[ NUM_CHESS_PIECES ][2];



#define PIECE_EXTRA_OFFSET_Y_LIST( C, V )            \
    V( C, 0,   noPiece,      ARRAY_PAIR(  0,  0 )  ) \
    V( C, 1,   pawn,         ARRAY_PAIR(  0,  0 )  ) \
    V( C, 2,   bishop,       ARRAY_PAIR(  0,  0 )  ) \
    V( C, 3,   knight,       ARRAY_PAIR(  0,  0 )  ) \
    V( C, 4,   rook,         ARRAY_PAIR(  0,  0 )  ) \
    V( C, 5,   queen,        ARRAY_PAIR(  0,  0 )  ) \
    V( C, 6,   king,         ARRAY_PAIR(  0,  0 )  ) \
    V( C, 7,   laserRook,    ARRAY_PAIR(  2,  2 )  ) \
    V( C, 8,   laserPawn,    ARRAY_PAIR( -1,  0 )  ) \
    V( C, 9,   doublingPawn, ARRAY_PAIR(  0,  0 )  ) \
    V( C, 10,  addingRook,   ARRAY_PAIR(  0,  0 )  ) \
    V( C, 11,  rocket,       ARRAY_PAIR(  0,  0 )  )

static int pieceExtraOffsetY[][2] = {
    MAKE_CHESS_ARRAY( PIECE_EXTRA_OFFSET_Y_LIST )
    };

CHECK_CHESS_ARRAY( pieceExtraOffsetY,
                   PIECE_EXTRA_OFFSET_Y_LIST );



#define PIECE_EXTRA_SPRITE_LIST( C, V )                             \
    V( C, 0,   noPiece,      ARRAY_PAIR( "", "" )  )                \
    V( C, 1,   pawn,         ARRAY_PAIR( "", "" )  )                \
    V( C, 2,   bishop,       ARRAY_PAIR( "", "" )  )                \
    V( C, 3,   knight,       ARRAY_PAIR( "", "" )  )                \
    V( C, 4,   rook,         ARRAY_PAIR( "", "" )  )                \
    V( C, 5,   queen,        ARRAY_PAIR( "", "" )  )                \
    V( C, 6,   king,         ARRAY_PAIR( "", "" )  )                \
    V( C, 7,   laserRook,    ARRAY_PAIR( "rookLasers.tga",          \
                                         "rookLasers.tga"      )  ) \
    V( C, 8,   laserPawn,    ARRAY_PAIR( "pawnLasersWhite.tga",     \
                                         "pawnLasersBlack.tga" )  ) \
    V( C, 9,   doublingPawn, ARRAY_PAIR( "", "" )  )                \
    V( C, 10,  addingRook,   ARRAY_PAIR( "", "" )  )                \
    V( C, 11,  rocket,       ARRAY_PAIR( "", "" )  )

static  const char  *pieceSpriteExtraFiles[][2] = {
    MAKE_CHESS_ARRAY( PIECE_EXTRA_SPRITE_LIST )
    };

CHECK_CHESS_ARRAY( pieceSpriteExtraFiles,
                   PIECE_EXTRA_SPRITE_LIST );




static  int          slashSpriteHandle      =  -1;
static  int          whiteFlagSpriteHandle  =  -1;
static  int          noMoveSpriteHandle     =  -1;


void pieceSpritesInit( void ) {

    int  i;


    maxigin_logInt( "Size of enum: ",
                    sizeof( ChessPiece ) );
    
    for( i = FIRST_CHESS_PIECE;
         i < NUM_CHESS_PIECES;
         i ++ ) {

        int   j;
        char  foundMatch  =  0;
        
        pieceSpriteHandles[ i ] = -1;

        if( maxigin_stringsEqual( pieceSpriteFiles[i],
                                  "" ) ) {
            /* skip blank file names */
            continue;
            }
        
        for( j = FIRST_CHESS_PIECE;
             j < i;
             j ++ ) {

            if( maxigin_stringsEqual( pieceSpriteFiles[i],
                                      pieceSpriteFiles[j] ) ) {

                /* we already loaded this file for a previous piece,
                   re-use it */
                
                foundMatch = 1;

                pieceSpriteHandles[ i ] = pieceSpriteHandles[ j ];
                pieceOffsetY      [ i ] = pieceOffsetY      [ j ];

                break;
                }
            }

        if( foundMatch ) {
            continue;
            }
        
        /* else a different sprite data file from what we've already processed */
        
        pieceSpriteHandles[ i ] = maxigin_initSprite( pieceSpriteFiles[i] );

        pieceOffsetY[ i ] = 0;
        
        if( pieceSpriteHandles[i] != -1 ) {

            int  w;
            int  h;
                
            /* hazy black border around top only */
            maxigin_initMakeDropShadowSprite( pieceSpriteHandles[i],
                                              5,
                                              2,
                                              0,
                                              255,
                                              60,
                                              30,
                                              100,
                                              0 );

            
            maxigin_initMakeGlowSprite( pieceSpriteHandles[i],
                                        4,
                                        2 );

            maxigin_getSpriteDimensions( pieceSpriteHandles[i],
                                         &w,
                                         &h );

            pieceOffsetY[ i ] = -( h / 2 - pieceBottomHeight );
            }
        }


    
    for( i = FIRST_CHESS_PIECE;
         i < NUM_CHESS_PIECES;
         i ++ ) {

        int  ci;

        for( ci = 0;
             ci < 2;
             ci ++ ) {
            
            int   j;
            char  foundMatch  =  0;

            pieceSpriteExtraHandles[ i ][ ci ] = -1;
            
            if( maxigin_stringsEqual( pieceSpriteExtraFiles[ i ][ ci ],
                                      "" ) ) {
                /* skip blank file names */
                continue;
                }
            
            for( j = FIRST_CHESS_PIECE;
                 j <= i;
                 j ++ ) {

                int  cj;

                for( cj = 0;
                     cj < ci;
                     cj ++ ) {
                
                    if( maxigin_stringsEqual(
                            pieceSpriteExtraFiles[ i ][ ci ],
                            pieceSpriteExtraFiles[ j ][ cj ] ) ) {

                        /* we already loaded this file for a previous piece,
                           re-use it */
                
                        foundMatch = 1;

                        pieceSpriteExtraHandles[ i ][ ci ] =
                            pieceSpriteExtraHandles[ j ][ cj ];

                        break;
                        }
                    }
                if( foundMatch ) {
                    break;
                    }
                }
            
            if( foundMatch ) {
                continue;
                }
        
            /* else a different sprite data file from what we've
               already processed */
        
            pieceSpriteExtraHandles[ i ][ ci ] = maxigin_initSprite(
                pieceSpriteExtraFiles[ i ][ ci ] );

            if( pieceSpriteExtraHandles[ i ][ ci ] != -1 ) {

                /* hazy, faded black shadow  top-to-bottom */
                maxigin_initMakeDropShadowSprite(
                    pieceSpriteExtraHandles[ i ][ ci ],
                    4,
                    2,
                    192,
                    192,
                    60,
                    30,
                    50,
                    0 );
            
                maxigin_initMakeGlowSprite(
                    pieceSpriteExtraHandles[ i ][ ci ],
                    4,
                    2 );
                }
            }
        }
    


    slashSpriteHandle = maxigin_initSprite( "checkmateSlash.tga" );

    if( slashSpriteHandle != -1 ) {
        maxigin_initMakeGlowSprite( slashSpriteHandle,
                                    4,
                                    2 );

        /* hazy drop shadow top to bottom */
        maxigin_initMakeDropShadowSprite( slashSpriteHandle,
                                          5,
                                          2,
                                          255,
                                          255,
                                          100,
                                          0,
                                          100,
                                          0 );
        }

    whiteFlagSpriteHandle = maxigin_initSprite( "whiteFlag.tga" );

    if( whiteFlagSpriteHandle != -1 ) {
        maxigin_initMakeGlowSprite( whiteFlagSpriteHandle,
                                    4,
                                    2 );

        /* hazy drop shadow top to bottom */
        maxigin_initMakeDropShadowSprite( whiteFlagSpriteHandle,
                                          5,
                                          2,
                                          255,
                                          255,
                                          100,
                                          0,
                                          100,
                                          0 );
        }

    noMoveSpriteHandle = maxigin_initSprite( "noMoveSquare.tga" );

    if( noMoveSpriteHandle != -1 ) {
        maxigin_initMakeGlowSprite( noMoveSpriteHandle,
                                    4,
                                    2 );

        /* hazy drop shadow top to bottom */
        maxigin_initMakeDropShadowSprite( noMoveSpriteHandle,
                                          5,
                                          2,
                                          255,
                                          255,
                                          100,
                                          0,
                                          100,
                                          0 );
        }
    }



void drawSetPieceColor( int  inPieceColor ) {
    if( inPieceColor == CHESS_BLACK ) {
        colorsApplyBlackPieceColor();
        }
    else {
        colorsApplyWhitePieceColor();
        }
    }



static int getPieceColorIndex( ChessPiece  inPieceColor ) {
    
    if( inPieceColor == CHESS_WHITE ) {
        return 0;
        }
    else {
        return 1;
        }
    }



void drawPiece( ChessPiece  inPiece,
                int         inBaseCenterX,
                int         inBaseCenterY ) {

    ChessPiece  rawP          =  inPiece & CHESS_TYPE_MASK;
    ChessPiece  c             =  inPiece & CHESS_COLOR_MASK;
    int         cIndex        =  getPieceColorIndex( c );
    
    drawSetPieceColor( c );
    
    maxigin_drawSprite( pieceSpriteHandles[ rawP ],
                        inBaseCenterX,
                        inBaseCenterY + pieceOffsetY[ rawP ] );

    maxigin_drawResetColor();

    if( pieceSpriteExtraHandles[ rawP ][ cIndex ] != -1 ) {
        maxigin_drawSprite( pieceSpriteExtraHandles[ rawP ][ cIndex ],
                            inBaseCenterX,
                            inBaseCenterY
                            + pieceOffsetY[ rawP ]
                            + pieceExtraOffsetY[ rawP ][ cIndex ] );
        }
    }



void drawPieceShadowOnly( ChessPiece  inPiece,
                          int         inBaseCenterX,
                          int         inBaseCenterY ) {
    
    ChessPiece  rawP          =  inPiece & CHESS_TYPE_MASK;
    
    maxigin_drawSpriteShadowOnly( pieceSpriteHandles[ rawP ],
                                  inBaseCenterX,
                                  inBaseCenterY + pieceOffsetY[ rawP ] );
    }



void drawPieceGlowOnly( ChessPiece     inPiece,
                        int            inBaseCenterX,
                        int            inBaseCenterY,
                        unsigned char  inAlpha ) {

    ChessPiece  rawP          =  inPiece & CHESS_TYPE_MASK;
    ChessPiece  c             =  inPiece & CHESS_COLOR_MASK;
    int         cIndex        =  getPieceColorIndex( c );

    maxigin_drawToggleAdditive( 1 );
    
    drawSetPieceColor( c );

    maxigin_drawSetAlpha( inAlpha );
    
    maxigin_drawSpriteGlowOnly( pieceSpriteHandles[ rawP ],
                                inBaseCenterX,
                                inBaseCenterY + pieceOffsetY[ rawP ] );
    
    maxigin_drawSpriteGlowOnly( pieceSpriteHandles[ rawP ],
                                inBaseCenterX,
                                inBaseCenterY + pieceOffsetY[ rawP ] );

    maxigin_drawResetColor();

    maxigin_drawSetAlpha( inAlpha );

    if( pieceSpriteExtraHandles[ rawP ][ cIndex ] != -1 ) {
        
        maxigin_drawSpriteGlowOnly( pieceSpriteExtraHandles[ rawP ][ cIndex ],
                                    inBaseCenterX,
                                    inBaseCenterY
                                    + pieceOffsetY[ rawP ]
                            + pieceExtraOffsetY[ rawP ][ cIndex ] );
        }

    maxigin_drawToggleAdditive( 0 );
    }




static void drawPieceBaseAndGlowOnlyInternal( ChessPiece  inPiece,
                                              int         inBaseCenterX,
                                              int         inBaseCenterY,
                                              char        inUseColor ) {
    
    
    ChessPiece  rawP          =  inPiece & CHESS_TYPE_MASK;
    ChessPiece  c             =  inPiece & CHESS_COLOR_MASK;
    int         cIndex        =  getPieceColorIndex( c );

    if( inUseColor ) {
        drawSetPieceColor( c );
        }
    
    maxigin_drawBaseSprite( pieceSpriteHandles[ rawP ],
                            inBaseCenterX,
                            inBaseCenterY + pieceOffsetY[ rawP ] );
    
    maxigin_drawSpriteGlowOnly( pieceSpriteHandles[ rawP ],
                                inBaseCenterX,
                                inBaseCenterY + pieceOffsetY[ rawP ] );

    if( inUseColor ) {
        maxigin_drawResetColor();
        }

    if( pieceSpriteExtraHandles[ rawP ][ cIndex ] != -1 ) {
        /* for extra overlays, draw full sprite, including shadows, since
           those shadows are over top of base piece itself */
        
        maxigin_drawSprite( pieceSpriteExtraHandles[ rawP ][ cIndex ],
                            inBaseCenterX,
                            inBaseCenterY
                            + pieceOffsetY[ rawP ]
                            + pieceExtraOffsetY[ rawP ][ cIndex ] );
        }
    }



void drawPieceBaseAndGlowOnly( ChessPiece  inPiece,
                               int         inBaseCenterX,
                               int         inBaseCenterY ) {

    drawPieceBaseAndGlowOnlyInternal( inPiece,
                                      inBaseCenterX,
                                      inBaseCenterY,
                                      1 );
    }



void drawPieceBaseAndGlowOnlyNoColor( ChessPiece  inPiece,
                                      int         inBaseCenterX,
                                      int         inBaseCenterY ) {
    
    drawPieceBaseAndGlowOnlyInternal( inPiece,
                                      inBaseCenterX,
                                      inBaseCenterY,
                                      0 );
    }



void drawPieceSparkles( ChessPiece     inPiece,
                        int            inBaseCenterX,
                        int            inBaseCenterY,
                        MaxiginRand   *inRand,
                        int            inNumSparkles,
                        unsigned char  inAlpha ) {

    ChessPiece  rawP            =  inPiece & CHESS_TYPE_MASK;
    ChessPiece  c               =  inPiece & CHESS_COLOR_MASK;
    int         partSprite      =  getStarParticleSprite();

    maxigin_drawToggleAdditive( 1 );
    
    drawSetPieceColor( c );

    if( c == CHESS_WHITE ) {

        /* desaturate gold color to make it clip white */
        maxigin_drawSetColorSaturation( 8500 );
        }

    maxigin_drawSpriteSparkles( pieceSpriteHandles[ rawP ],
                                partSprite,
                                inBaseCenterX,
                                inBaseCenterY + pieceOffsetY[ rawP ],
                                inRand,
                                inNumSparkles,
                                inAlpha,
                                /* draw sparkles over full sprite extent */
                                -1,
                                -1,
                                -1,
                                -1 );

    maxigin_drawToggleAdditive( 0 );
    }
        


static  int  explodeMax  =  512;


int stepExplodingPiece( int  inProgress ) {

    if( inProgress == -1 ) {
        return -1;
        }

    if( getParticleSprite() == -1 ) {
        return -1;
        }
    
    inProgress += ( 30 * 60 ) / mingin_getStepsPerSecond();
    

    if( inProgress >= explodeMax ) {
        inProgress = -1;
        }

    return inProgress;
    }



int getExplodingPieceMaxProgress( void ) {
    return explodeMax;
    }



void drawExplodingPiece( ChessPiece  inPiece,
                         int         inBoardCenterX,
                         int         inBoardCenterY,
                         int         inRow,
                         int         inCol,
                         int         inProgress ) {

    ChessPiece     rawP              =  inPiece & CHESS_TYPE_MASK;
    ChessPiece     c                 =  inPiece & CHESS_COLOR_MASK;
    int            cIndex            =  getPieceColorIndex( c );
    
    unsigned char  a;
    int            x;
    int            y;
    int            glowPartSprite    =  getParticleSprite();
    int            noGlowPartSprite  =  getNoGlowParticleSprite();
    int            partSpriteToUse; 
    int            baseCenterY;
    
    if( glowPartSprite == -1
        ||
        noGlowPartSprite == -1 ) {
        return;
        }

    boardGetSquareCenter( inBoardCenterX,
                          inBoardCenterY,
                          inRow,
                          inCol,
                          &x,
                          &y );

    baseCenterY =  y + pieceOffsetY[ rawP ];


    if( rawP == king ) {

        if( c == CHESS_WHITE ) {

            /* red, player lost */
            maxigin_drawSetColor( 255, 0, 0, 255 );
            }
        else {
            /* green, player won */
            maxigin_drawSetColor( 0, 255, 0, 255 );
            }

        maxigin_drawToggleAdditive( 0 );

        partSpriteToUse = noGlowPartSprite;
        }
    else {

        /* explosion of non-king, colored with piece color */
        
        drawSetPieceColor( c );

    
        if( c == CHESS_WHITE ) {

            /* desaturate gold color to make it clip white */
            maxigin_drawSetColorSaturation( 8500 );
            }

        maxigin_drawToggleAdditive( 1 );

        partSpriteToUse = glowPartSprite;
        }
    


    a = (unsigned char)( ( (long)( explodeMax - inProgress ) * 255 )
                         / explodeMax );

   

    maxigin_drawExplodingSprite( pieceSpriteHandles[ rawP ],
                                 partSpriteToUse,
                                 x,
                                 baseCenterY,
                                 x,
                                 baseCenterY,
                                 BOARD_SQUARE_SIZE / 4,
                                 inProgress,
                                 explodeMax,
                                 a );

    maxigin_drawResetColor();

    if( pieceSpriteExtraHandles[ rawP ][ cIndex ] != -1 ) {
        maxigin_drawExplodingSprite( pieceSpriteExtraHandles[ rawP ][ cIndex ],
                                     partSpriteToUse,
                                     x,
                                     baseCenterY
                                     + pieceExtraOffsetY[ rawP ][ cIndex ],
                                     /* explosion centered on base sprite */
                                     x,
                                     baseCenterY,
                                     BOARD_SQUARE_SIZE / 4,
                                     inProgress,
                                     explodeMax,
                                     a );
        }
    maxigin_drawToggleAdditive( 0 );
    }



void drawBoardState( BoardState     *inState,
                     char            inCheckmate,
                     char            inStalemate,
                     char            inDrawGame,
                     int             inLoserColor,
                     Move           *inMove,
                     int             inMoveProgress,
                     int             inMoveProgressMax,
                     int             inBoardCenterX,
                     int             inBoardCenterY,
                     DrawBoardMask  *inMask ) {

    int         x;
    int         y;
    char        drawMoving  =  0;
    int         mX          =  0;
    int         mY          =  0;
    ChessPiece  mP          =  noPiece;


    if( inStalemate ) {
        /* draw no move x's around loser's king, under other pieces */

        for( y = 0;
             y < BH;
             y ++ ) {
        
            for( x = 0;
                 x < BW;
                 x ++ ) {

                ChessPiece  p    =  inState->grid[y][x];
                ChessPiece  t    =  p & CHESS_TYPE_MASK;
                ChessPiece  c    =  p & CHESS_COLOR_MASK;

                if( t == king
                    &&
                    c == inLoserColor ) {

                    int dx;
                    int dy;

                    for( dy = -1;
                         dy <= 1;
                         dy ++ ) {

                        int  oy  =  y + dy;

                        if( oy >= 0
                            &&
                            oy < BH ) {
                            /* oy in bounds  */
                        
                            for( dx = -1;
                                 dx <= 1;
                                 dx ++ ) {

                                int  ox  =  x + dx;

                                if( ox >= 0
                                    &&
                                    ox < BW ) {

                                    /* ox in bounds */

                                    if( ox != x
                                        ||
                                        oy != y ) {
                                        /* no x on the king's pos itself */
                            
                                        int  pX;
                                        int  pY;

                                        boardGetSquareCenter( inBoardCenterX,
                                                              inBoardCenterY,
                                                              oy,
                                                              ox,
                                                              &pX,
                                                              &pY );
                                        maxigin_drawSprite( noMoveSpriteHandle,
                                                            pX,
                                                            pY );
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    
    

    if( inMoveProgressMax != 0 ) {
        /* a move is in progress  */
        int  sY;
        int  sX;
        
        int  dX;
        int  dY;

        long  workingX;
        long  workingY;


        boardGetSquareCenter( inBoardCenterX,
                              inBoardCenterY,
                              inMove->startPos[0],
                              inMove->startPos[1],
                              &sX,
                              &sY );
        
        boardGetSquareCenter( inBoardCenterX,
                              inBoardCenterY,
                              inMove->endPos[0],
                              inMove->endPos[1],
                              &dX,
                              &dY );

        workingX = dX - sX;
        workingY = dY - sY;

        workingX *= inMoveProgress;
        workingY *= inMoveProgress;

        mX = (int)( workingX / inMoveProgressMax ) + sX;
        mY = (int)( workingY / inMoveProgressMax ) + sY;

        mP = inState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ];
        
        drawMoving = 1;
        }
    

    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            ChessPiece  p    =  inState->grid[y][x];
            int         pX;
            int         pY;

            if( inMask != 0 ) {
                if( inMask->grid[y][x] == 0 ) {
                    /* skip this masked-out spot on board */
                    continue;
                    }
                }
            

            boardGetSquareCenter( inBoardCenterX,
                                  inBoardCenterY,
                                  y,
                                  x,
                                  &pX,
                                  &pY );

            if( drawMoving ) {
                /* see if we're at a good order spot where we should
                   finall draw this piece behind this next piece */

                if( pY >= mY ) {
                    /* next piece is first spot (which may contain a piece)
                       that should be in
                       front of our moving piece,
                       draw our moving piece now, just behind it */
                    drawPiece( mP,
                               mX,
                               mY );
                    drawMoving = 0;
                    }
                }

            if( p != noPiece ) {

                if( inMoveProgressMax != 0
                    &&
                    inMove->startPos[0] == y
                    &&
                    inMove->startPos[1] == x ) {

                    /* skip this piece, because it's moving
                       draw it separately */
                    }
                else {
                    
                    drawPiece( p,
                               pX,
                               pY );

                    if( inCheckmate
                        ||
                        inDrawGame ) {
                        
                        ChessPiece  c  =  p & CHESS_COLOR_MASK;
                        ChessPiece  t  =  p & CHESS_TYPE_MASK;

                        if( t == king
                            &&
                            c == inLoserColor ) {

                            if( inCheckmate ) {
                                /* draw checkmate slash */
                                maxigin_drawSprite( slashSpriteHandle,
                                                    pX,
                                                    pY + pieceOffsetY[ t ] );
                                }
                            else {
                                /* draw white flag  */
                                maxigin_drawSprite( whiteFlagSpriteHandle,
                                                    pX,
                                                    pY + pieceOffsetY[ t ] );
                                }
                            }
                        }  
                    }
                }
            }
        }
    }



static void clearMask( DrawBoardMask  *inMask ) {
    
    int  x;
    int  y;
    
    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            inMask->grid[y][x] = 0;
            }
        }
    }



void getRowRangeMask( DrawBoardMask  *inMask,
                      int             inStartRow,
                      int             inEndRow ) {

    int  x;
    int  y;
    
    clearMask( inMask );

    for( y = inStartRow;
         y <= inEndRow;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            inMask->grid[y][x] = 1;
            }
        }
    }



void getRowMask( DrawBoardMask  *inMask,
                 int             inRow ) {
    
    getRowRangeMask( inMask,
                     inRow,
                     inRow );
    }



void getRowsAboveMask( DrawBoardMask  *inMask,
                       int             inRow ) {

    getRowRangeMask( inMask,
                     0,
                     inRow );
    }



void getRowsBelowMask( DrawBoardMask  *inMask,
                       int             inRow ) {


    getRowRangeMask( inMask,
                     inRow,
                     BH - 1 );
    }



void clearMaskSpot( DrawBoardMask  *inMask,
                    int             inRow,
                    int             inCol ) {

    inMask->grid[ inRow ][ inCol ] = 0;
    }



void clearMaskRow( DrawBoardMask  *inMask,
                   int             inRow ) {

    int  x;

    for( x = 0;
         x < BW;
         x ++ ) {

        inMask->grid[ inRow ][ x ] = 0;
        }
    }



ChessPiece getPointerOverPiece( BoardState  *inState,
                                int          inBoardCenterX,
                                int          inBoardCenterY,
                                int         *outRow,
                                int         *outCol ) {
    int  y;
    int  x;

    int  pointerX;
    int  pointerY;

    if( ! maxigin_getPointerLocation( &pointerX,
                                      &pointerY ) ) {
        /* pointer not available */
        return noPiece;
        }

    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            ChessPiece  p  =  inState->grid[y][x];
            ChessPiece  t;
            ChessPiece  c;
            int         cIndex;
            int         pX;
            int         pY;

            int         spritePixX;
            int         spritePixY;
            
            if( p == noPiece ) {
                continue;
                }

            t      = p & CHESS_TYPE_MASK;
            c      = p & CHESS_COLOR_MASK;
            cIndex = getPieceColorIndex( c );

            boardGetSquareCenter( inBoardCenterX,
                                  inBoardCenterY,
                                  y,
                                  x,
                                  &pX,
                                  &pY );

            pY += pieceOffsetY[ t ];

            spritePixX = pointerX - pX;
            spritePixY = pointerY - pY;
            

            if( maxigin_isInSprite( pieceSpriteHandles[ t ],
                                    spritePixX,
                                    spritePixY ) ) {
                *outRow = y;
                *outCol = x;
                return p;
                }

            /* pointer not in sprite */ 

            if( pieceSpriteExtraHandles[ t ][ cIndex ] != -1 ) {
                /* check if it's in piece's extra sprite */

                pY += pieceExtraOffsetY[ t ][ cIndex ];

                spritePixY = pointerY - pY;
                
                if( maxigin_isInSprite(
                        pieceSpriteExtraHandles[ t ][ cIndex ],
                        spritePixX,
                        spritePixY ) ) {

                    *outRow = y;
                    *outCol = x;
                    return p;
                    }
                }
            }
        }
    

    return noPiece;   
    }




void drawPieceHighlight( BoardState    *inState,
                         int            inBoardCenterX,
                         int            inBoardCenterY,
                         int            inPieceRow,
                         int            inPieceCol,
                         unsigned char  inFade ) {

    int  pX;
    int  pY;
    
    boardGetSquareCenter( inBoardCenterX,
                          inBoardCenterY,
                          inPieceRow,
                          inPieceCol,
                          &pX,
                          &pY );

    drawPieceGlowOnly( inState->grid[ inPieceRow ][ inPieceCol ],
                       pX,
                       pY,
                       inFade );
    }



#endif


#endif
