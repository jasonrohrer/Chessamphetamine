/*
  Include in your C code wherever like so:

      #include "moveAnim.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define MOVE_ANIM_IMPLEMENTATION
      #include "moveAnim.h"

*/

#ifndef MOVE_ANIM_H_INCLUDED
#define MOVE_ANIM_H_INCLUDED


#include "board.h"
#include "pieceSprites.h"


void moveAnimInit( void );



typedef unsigned char AnimPhase;

enum{
    move = 0,
    laser,
    explode,
    multiplier,
    addition
    };

#define  MAX_ANIM_PHASES  BN

#define  NUM_ANIM_PARAMS  5

typedef struct AnimProgress {

        int        numPhases;
        int        phaseNumber;
        int        phaseProgress;

        /* type of phase from our enum above */
        AnimPhase  phases[ MAX_ANIM_PHASES ];

        /* each phase may have multiple parameters that the phase
           interprets in unique ways */
        short      params[ MAX_ANIM_PHASES ][ NUM_ANIM_PARAMS ];

        /* for accumulating multiplier factors that build up
           as modifiers propagate */
        char           anyMultFactors;
        long           multFactors      [ BH ][ BW ];
        unsigned char  multFactorFades  [ BH ][ BW ];
        char           multFactorFadeDir[ BH ][ BW ];

        /* used for shimmering in multFactors */
        MaxiginRand    multRandA;
        MaxiginRand    multRandB;

        int            sparkleProgress;
        
    } AnimProgress;
        


/*
  Initialized outMoveProgress for animating the move.
*/
void initMoveAnimation( BoardState    *inState,
                        Move          *inMove,
                        Captured      *inCaptured,
                        BoardState    *inNewState,
                        AnimProgress  *outMoveProgress );

/* 
   returns  0 when not done yet
            1 when done
*/
char stepMoveAnimation( BoardState    *inState,
                        Move          *inMove,
                        Captured      *inCaptured,
                        BoardState    *inNewState,
                        AnimProgress  *inMoveProgress );



/* draws entire board with move animation according to inMoveProgress */
void drawMoveAnimation( int            inBoardCenterX,
                        int            inBoardCenterY,
                        BoardState    *inState,
                        Move          *inMove,
                        Captured      *inCaptured,
                        BoardState    *inNewState,
                        AnimProgress  *inMoveProgress );



#ifdef MOVE_ANIM_IMPLEMENTATION


static  int  beepUp       =  -1;
static  int  beepDown     =  -1;
static  int  shooshGood   =  -1;
static  int  splatterBad  =  -1;
static  int  laserSound   =  -1;
static  int  addSound     =  -1;
static  int  multSound    =  -1;

static  const char  *laserShortSpriteNames[4] = { "laserShortLeft.tga",
                                                  "laserShortRight.tga",
                                                  "laserShortUp.tga",
                                                  "laserShortDown.tga" };
static  const char  *laserShortGlowNames  [4] = { "laserShortLeftGlow.tga",
                                                  "laserShortRightGlow.tga",
                                                  "laserShortUpGlow.tga",
                                                  "laserShortDownGlow.tga" };
static  int          laserShortSprites    [4];
static  int          laserShortGlowSprites[4];


static  const char  *laserMidSpriteNames  [2] = { "laserHorMid.tga",
                                                  "laserVertMid.tga" };
static  const char  *laserMidGlowNames    [2] = { "laserHorMidGlow.tga",
                                                  "laserVertMidGlow.tga" };
static  int          laserMidSprites      [2];
static  int          laserMidGlowSprites  [2];



static  const char  *laserStartSpriteNames[4] = { "laserLeftStart.tga",
                                                  "laserRightStart.tga",
                                                  "laserUpStart.tga",
                                                  "laserDownStart.tga" };
static  const char  *laserStartGlowNames  [4] = { "laserLeftStartGlow.tga",
                                                  "laserRightStartGlow.tga",
                                                  "laserUpStartGlow.tga",
                                                  "laserDownStartGlow.tga" };

static  const char  *laserEndSpriteNames  [4] = { "laserLeftEnd.tga",
                                                  "laserRightEnd.tga",
                                                  "laserUpEnd.tga",
                                                  "laserDownEnd.tga" };
static  const char  *laserEndGlowNames    [4] = { "laserLeftEndGlow.tga",
                                                  "laserRightEndGlow.tga",
                                                  "laserUpEndGlow.tga",
                                                  "laserDownEndGlow.tga" };
static  int          laserStartSprites    [4];
static  int          laserStartGlowSprites[4];
static  int          laserEndSprites      [4];
static  int          laserEndGlowSprites  [4];


static  int          laserBackGlintSprite;
static  int          laserBackGlintGlow;

static  int          laserPawnTopGlintSprite;
static  int          laserPawnTopGlintGlow;


static  int          modifierFont;


void moveAnimInit( void ) {

    int  i;
    int  fontStrip;
    

    beepUp = maxigin_initSoundEffect( "beepUp.wav" );
    beepDown = maxigin_initSoundEffect( "beepDown.wav" );
    shooshGood = maxigin_initSoundEffect( "shooshGood.wav" );
    splatterBad = maxigin_initSoundEffect( "splatterBad.wav" );
    laserSound = maxigin_initSoundEffect( "laser_sd_16.wav" );
    addSound = maxigin_initSoundEffect( "add_sd_5.wav" );
    multSound = maxigin_initSoundEffect( "mult_sd_7.wav" );

    for( i = 0;
         i < 4;
         i ++ ) {

        laserShortSprites[i] = maxigin_initSprite( laserShortSpriteNames[i] );
        laserShortGlowSprites[i] = maxigin_initSprite( laserShortGlowNames[i] );

        laserStartSprites[i] = maxigin_initSprite( laserStartSpriteNames[i] );
        laserStartGlowSprites[i] = maxigin_initSprite( laserStartGlowNames[i] );

        laserEndSprites[i] = maxigin_initSprite( laserEndSpriteNames[i] );
        laserEndGlowSprites[i] = maxigin_initSprite( laserEndGlowNames[i] );
        }

    for( i = 0;
         i < 2;
         i ++ ) {

        laserMidSprites[i] = maxigin_initSprite( laserMidSpriteNames[i] );
        laserMidGlowSprites[i] = maxigin_initSprite( laserMidGlowNames[i] );

        }

    laserBackGlintSprite = maxigin_initSprite( "laserBackSideGlint.tga"     );
    laserBackGlintGlow   = maxigin_initSprite( "laserBackSideGlintGlow.tga" );
    
    laserPawnTopGlintSprite = maxigin_initSprite( "laserPawnTopGlint.tga"     );
    laserPawnTopGlintGlow   = maxigin_initSprite( "laserPawnTopGlintGlow.tga" );

    fontStrip = maxigin_initSpriteStrip( "modifierFont.tga",
                                         8 );

    if( fontStrip != -1 ) {

        maxigin_initMakeGlowSpriteStrip( fontStrip,
                                         2,
                                         2 );

        /* hazy, faded black shadow  top-to-bottom */
        if(0)maxigin_initMakeDropShadowSpriteStrip(
            fontStrip,
            4,
            2,
            192,
            192,
            60,
            30,
            50,
            0 );

        modifierFont = maxigin_initFont( fontStrip,
                                         "modifierFont.txt",
                                         0,
                                         4,
                                         0 );
        }
    }



/* The signature for a move animation init function.
   We define one of these for each piece type in the enum in chess.h.

   Initialized outMoveProgress.
*/
typedef void (*MoveAnimInitFunction)( BoardState    *inState,
                                      Move          *inMove,
                                      Captured      *inCaptured,
                                      BoardState    *inNewState,
                                      AnimProgress  *outMoveProgress );



/* The signature for a move animation step function.
   We define one of these for each piece type in the enum in chess.h.

   Updates inMoveProgress and returns 1 when animation is done.
*/
typedef char (*MoveAnimStepFunction)( BoardState    *inState,
                                      Move          *inMove,
                                      Captured      *inCaptured,
                                      BoardState    *inNewState,
                                      AnimProgress  *inMoveProgress );



/* The signature for a move animation draw function.
   We define one of these for each piece type in the enum in chess.h.
*/
typedef void (*MoveAnimDrawFunction)( int            inBoardCenterX,
                                      int            inBoardCenterY,
                                      BoardState    *inState,
                                      Move          *inMove,
                                      Captured      *inCaptured,
                                      BoardState    *inNewState,
                                      AnimProgress  *inMoveProgress );


static void noPieceInit( BoardState    *inState,
                         Move          *inMove,
                         Captured      *inCaptured,
                         BoardState    *inNewState,
                         AnimProgress  *inMoveProgress ) {
    (void)inState;
    (void)inMove;
    (void)inCaptured;
    (void)inNewState;
    (void)inMoveProgress;
    }


static char noPieceStep( BoardState    *inState,
                         Move          *inMove,
                         Captured      *inCaptured,
                         BoardState    *inNewState,
                         AnimProgress  *inMoveProgress ) {
    (void)inState;
    (void)inMove;
    (void)inCaptured;
    (void)inNewState;
    (void)inMoveProgress;
    return 1;
    }



/* draws entire board with move animation according to inMoveProgress */
static void noPieceDraw( int            inBoardCenterX,
                         int            inBoardCenterY,
                         BoardState    *inState,
                         Move          *inMove,
                         Captured      *inCaptured,
                         BoardState    *inNewState,
                         AnimProgress  *inMoveProgress ) {
    
    (void)inBoardCenterX;
    (void)inBoardCenterY;
    (void)inState;
    (void)inMove;
    (void)inCaptured;
    (void)inNewState;
    (void)inMoveProgress;
    }



static void initMultFactors( AnimProgress  *outMoveProgress ) {
    int y;
    int x;

    outMoveProgress->anyMultFactors = 0;
    
    for( y = 0;
         y < BH;
         y ++ ) {
        for( x = 0;
             x < BW;
             x ++ ) {

            outMoveProgress->multFactors      [ y ][ x ] = 1;
            outMoveProgress->multFactorFades  [ y ][ x ] = 0;
            outMoveProgress->multFactorFadeDir[ y ][ x ] = 0;
            }
        }

    /* use same seed every time */
    maxigin_randSeed( &( outMoveProgress->multRandA ),
                      0xDEADBEEF );
    outMoveProgress->multRandB = outMoveProgress->multRandA;

    outMoveProgress->sparkleProgress = 0;
    }



static void defaultPieceInit( BoardState    *inState,
                              Move          *inMove,
                              Captured      *inCaptured,
                              BoardState    *inNewState,
                              AnimProgress  *outMoveProgress ) {

    (void)inState;
    (void)inMove;
    (void)inCaptured;
    (void)inNewState;
    
    outMoveProgress->numPhases     = 1;
    outMoveProgress->phaseNumber   = 0;
    outMoveProgress->phaseProgress = 0;

    outMoveProgress->phases[0] = move;

    initMultFactors( outMoveProgress );
    }




static char defaultPieceStep( BoardState    *inState,
                              Move          *inMove,
                              Captured      *inCaptured,
                              BoardState    *inNewState,
                              AnimProgress  *inMoveProgress ) {
    
    int  pixDist  =  boardGetPixelDistance( inMove->startPos[0],
                                            inMove->startPos[1],
                                            inMove->endPos[0],
                                            inMove->endPos[1] );
    int  r        =  mingin_getStepsPerSecond();

    
    (void)inState;
    (void)inNewState;
    
    
    if( inMoveProgress->phaseProgress < pixDist ) {

        /* still on piece movement portion of animtion */
        
        inMoveProgress->phaseProgress += ( 4 * 60 ) / r;

        if( inMoveProgress->phaseProgress > pixDist ) {
            inMoveProgress->phaseProgress = pixDist;
            
            if( inCaptured->num == 0 ) {
                /* nothing to explode */

                /* play sound at end of piece move */

                if( inState->nextToMove == CHESS_WHITE ) {
                    maxigin_playSoundEffect( beepUp,
                                             256 );
                    }
                else {
                    maxigin_playSoundEffect( beepDown,
                                             256 );
                    }

                /* animation done */
                return 1;
                }
            else {
                
                /* play sound effect at start of capture explosions */
                
                int   oldScore  =  getScore( inState );
                int   newScore  =  getScore( inNewState );

                if( oldScore <= newScore ) {
                    maxigin_playSoundEffect( shooshGood,
                                             512 );
                    }
                else {
                    maxigin_playSoundEffect( splatterBad,
                                             512 );
                    }
                }
            }
        }
    else {
        /* onto explosion of captured pieces */

        int  explodeProgress  =  inMoveProgress->phaseProgress - pixDist;
        
        explodeProgress = stepExplodingPiece( explodeProgress );

        if( explodeProgress == -1 ) {
            /* done exploding */
            return 1;
            }

        inMoveProgress->phaseProgress = pixDist + explodeProgress;
        }

    return 0;
    }



static void defaultPieceDraw( int            inBoardCenterX,
                              int            inBoardCenterY,
                              BoardState    *inState,
                              Move          *inMove,
                              Captured      *inCaptured,
                              BoardState    *inNewState,
                              AnimProgress  *inMoveProgress ) {

    int  pixDist  =  boardGetPixelDistance( inMove->startPos[0],
                                            inMove->startPos[1],
                                            inMove->endPos[0],
                                            inMove->endPos[1] );
    boardDraw( inBoardCenterX,
               inBoardCenterY );

    if( inMoveProgress->phaseProgress < pixDist ) {
        /* draw move in progress */

        drawBoardState( inState,
                        0,
                        0,
                        0,
                        0,
                        inMove,
                        inMoveProgress->phaseProgress,
                        pixDist,
                        inBoardCenterX,
                        inBoardCenterY,
                        0 );
        }
    else {
        /* move done, draw end state */
        drawBoardState( inNewState,
                        0,
                        0,
                        0,
                        0,
                        inMove,
                        0,
                        0,
                        inBoardCenterX,
                        inBoardCenterY,
                        0 );

        if( inCaptured->num > 0 ) {

            /* there are captured pieces, draw them */

            int  pn;
            int  explodingProgress  =  inMoveProgress->phaseProgress - pixDist;
            
            for( pn = 0;
                 pn < inCaptured->num;
                 pn ++ ) {

                BoardPiece  *bp  =  &( inCaptured->pieces[ pn ] );
            
                drawExplodingPiece( bp->p,
                                    inBoardCenterX,
                                    inBoardCenterY,
                                    bp->row,
                                    bp->col,
                                    explodingProgress );
                }
            }
        }
    }



static int  laserPhaseLen     =  512;
static int  modifierPhaseLen  =  512;

typedef struct HitBatches {
        /* at most BMAX batches */
        /* indices into captured list */
        int  startC[ BMAX ];
        int  endC  [ BMAX ];
    } HitBatches;
        

static int getLaserHitDepth( int          inPieceRow,
                             int          inPieceCol,
                             Captured    *inCaptured,
                             HitBatches  *inHit ) {
    
    /* n s e w */
    int  dirCounts[4]  =  {  0,  0,  0,  0 };
    int  i;
    int  max;

    for( i = 0;
         i < BMAX;
         i ++ ) {
        
        inHit->startC[i] = -1;
        inHit->endC  [i] = -1;
        }
        
    for( i = 0;
         i < inCaptured->num;
         i ++ ) {

        int  r            =  inCaptured->pieces[ i ].row;
        int  c            =  inCaptured->pieces[ i ].col;
        int  oldDirCount  =  0;
        
        if( r == inPieceRow
            &&
            c == inPieceCol ) {
            /* direct non-laser capture */
            continue;
            }
        
        if( r < inPieceRow ) {
            oldDirCount = dirCounts[0];
            dirCounts[0] ++;
            }
        else if( r > inPieceRow ) {
            oldDirCount = dirCounts[1];
            dirCounts[1] ++;
            }
        else if( c < inPieceCol ) {
            oldDirCount = dirCounts[2];
            dirCounts[3] ++;
            }
        else if( c > inPieceRow ) {
            oldDirCount = dirCounts[2];
            dirCounts[2] ++;
            }

        /* update our batches
           This is a tricky way to do this, where oldDirCount
           tells us which batch to update... we can have up to four
           pieces in each batch, one from each NSEW direction */
        if( inHit->startC[ oldDirCount ] == -1 ) {
            inHit->startC[ oldDirCount ] = i;
            }
        
        inHit->endC[ oldDirCount ] = i;
        }

    max = 0;

    for( i = 0;
         i < 4;
         i ++ ) {
        if( dirCounts[i] > max ) {
            max = dirCounts[i];
            }
        }
    
    return max;
    }



/* updates outMoveProgress, which must be alread inited
   with any phases that happen before space effects */
static void spaceEffectsInit( BoardState    *inState,
                              Move          *inMove,
                              AnimProgress  *outMoveProgress ) {

    static  BoardState             postMoveState;
    static  FullBoardSpaceEffects  effects;


    /* gather these in order walking backwards from our target space */
    static  SpaceEffect            types     [ BN ];
    static  int                    values    [ BN ];
    static  int                    sourceRows[ BN ];
    static  int                    sourceCols[ BN ];
    static  int                    targetRows[ BN ];
    static  int                    targetCols[ BN ];

    static  int                    frontierR [ BN ];
    static  int                    frontierC [ BN ];
    static  char                   visitMap  [ BH ][ BW ];
    
    int  numEffects   =  0;
    int  numFrontier  =  0;
    int  r            =  inMove->endPos[0];
    int  c            =  inMove->endPos[1];
    int  i;
    int  y;
    int  x;


    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {
            visitMap[ y ][ x ] = 0;
            }
        }
        
    
    postMoveState = *inState;

    postMoveState.grid[ r ][ c ] = 
        postMoveState.grid[ inMove->startPos[0] ][ inMove->startPos[1] ];
    
    postMoveState.grid[ inMove->startPos[0] ][ inMove->startPos[1] ] = noPiece;
    
    getSpaceEffects( &postMoveState,
                     inState->nextToMove,
                     &effects );

    compoundSpaceEffects( r,
                          c,
                          &effects );

    frontierR[0] = r;
    frontierC[0] = c;
    numFrontier  = 1;

    visitMap[ r ][ c ] = 1;

    while( numFrontier > 0 ) {

        int                  fR  = frontierR[ numFrontier - 1 ];
        int                  fC  = frontierC[ numFrontier - 1 ];
        ActiveSpaceEffects  *a;
        
        numFrontier --;

        a = &( effects.grid[ fR ][ fC ] );

        for( i = 0;
             i < a->num;
             i ++ ) {

            types     [ numEffects ] = a->effectType [ i ];
            values    [ numEffects ] = a->effectValue[ i ];
            sourceRows[ numEffects ] = a->sourceRow  [ i ];
            sourceCols[ numEffects ] = a->sourceCol  [ i ];
            targetRows[ numEffects ] = fR;
            targetCols[ numEffects ] = fC;
            
            numEffects ++;

            if( ! visitMap[ a->sourceRow[ i ] ][ a->sourceCol[ i ] ] ) {
                frontierR[ numFrontier ] = a->sourceRow[ i ];
                frontierC[ numFrontier ] = a->sourceCol[ i ];

                numFrontier++;
                visitMap[ a->sourceRow[ i ] ][ a->sourceCol[ i ] ] =  1;
                }
            }
        }

    
    /* stick them in as animation phases in reverse order */

    for( i =  numEffects - 1;
         i >= 0;
         i -- ) {

        int  p  =  outMoveProgress->numPhases;
        
        if( types[i] == add ) {
            outMoveProgress->phases[ p ]      = addition;
            outMoveProgress->params[ p ][ 0 ] = (short)( sourceRows[i] );
            outMoveProgress->params[ p ][ 1 ] = (short)( sourceCols[i] );
            outMoveProgress->params[ p ][ 2 ] = (short)( targetRows[i] );
            outMoveProgress->params[ p ][ 3 ] = (short)( targetCols[i] );
            outMoveProgress->params[ p ][ 4 ] = (short)( values[i] );
            outMoveProgress->numPhases ++;
            }
        else if( types[i] == multiply ) {
            outMoveProgress->phases[ p ]      = multiplier;
            outMoveProgress->params[ p ][ 0 ] = (short)( sourceRows[i] );
            outMoveProgress->params[ p ][ 1 ] = (short)( sourceCols[i] );
            outMoveProgress->params[ p ][ 2 ] = (short)( targetRows[i] );
            outMoveProgress->params[ p ][ 3 ] = (short)( targetCols[i] );
            outMoveProgress->params[ p ][ 4 ] = (short)( values[i] );
            outMoveProgress->numPhases ++;
            } 
        }
    }




static void laserPieceInit( BoardState    *inState,
                            Move          *inMove,
                            Captured      *inCaptured,
                            BoardState    *inNewState,
                            AnimProgress  *outMoveProgress ) {

    static  HitBatches  hit;
    
    int  numLasers  =  0;
    int  i;
    int  p;

    (void)inState;
    (void)inNewState;

    initMultFactors( outMoveProgress );
    
    if( inCaptured->num > 0 ) {

        numLasers = getLaserHitDepth( inMove->endPos[0],
                                      inMove->endPos[1],
                                      inCaptured,
                                      &hit );
        }
    
    outMoveProgress->numPhases     = 1;
    outMoveProgress->phaseNumber   = 0;
    outMoveProgress->phaseProgress = 0;

    outMoveProgress->phases[0] = move;

    spaceEffectsInit( inState,
                      inMove,
                      outMoveProgress );

    p = outMoveProgress->numPhases;
    
    outMoveProgress->numPhases += numLasers * 2;
    
    for( i = 0;
         i < numLasers;
         i ++ ) {

        short  startC  =  (short)( hit.startC[i] );
        short  endC    =  (short)( hit.endC  [i] );
        
        outMoveProgress->phases[p]    = laser;
        outMoveProgress->params[p][0] = startC;
        outMoveProgress->params[p][1] = endC;
        p++;

        outMoveProgress->phases[p]    = explode;
        outMoveProgress->params[p][0] = startC;
        outMoveProgress->params[p][1] = endC;
        p++;
        }   
    }



/* gets state and captured list for special piece after it moves but
   before it fires */
static void getCaptureMidState( BoardState  *inState,
                                Move        *inMove,
                                Captured    *inCaptured,
                                BoardState  *outMidState,
                                Captured    *outMidCaptured ) {

    *outMidState    = *inState;
    *outMidCaptured = *inCaptured;

    if( outMidCaptured->num > 0 ) {

        /* cut off all but the first one,
           extras are always laser hits */

        outMidCaptured->num = 1;

        if( outMidCaptured->pieces[ 0 ].row != inMove->endPos[ 0 ]
            ||
            outMidCaptured->pieces[ 0 ].col != inMove->endPos[ 1 ] ) {

            /* even the single capture is hit at a distance */
            outMidCaptured->num = 0;
            }
        }

    /* put piece where it lands in outMidState */
    outMidState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ] = noPiece;
    outMidState->grid[ inMove->endPos  [0] ][ inMove->endPos  [1] ] =
        inState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ];
    }


/* mid state has pieces removed up to and included inCutoff in capture list */
static void getCaptureCutoffMidState( BoardState  *inState,
                                      Move        *inMove,
                                      Captured    *inCaptured,
                                      BoardState  *outMidState,
                                      Captured    *outMidCaptured,
                                      int          inCutoff ) {
    int  i;
    
    *outMidState    = *inState;
    *outMidCaptured = *inCaptured;

    for( i = 0;
         i <= inCutoff;
         i++ ) {
        
        int  r  =  inCaptured->pieces[ i ].row;
        int  c  =  inCaptured->pieces[ i ].col;
        
        outMidState->grid[ r ][ c ] = noPiece;
        }

    /* put piece where it lands in outMidState */
    outMidState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ] = noPiece;
    outMidState->grid[ inMove->endPos  [0] ][ inMove->endPos  [1] ] =
        inState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ];
    }



static char multiPhaseStep( BoardState    *inState,
                            Move          *inMove,
                            Captured      *inCaptured,
                            BoardState    *inNewState,
                            AnimProgress  *inMoveProgress ) {

    static  BoardState  midState;
    static  Captured    midCaptured;
    
    int        pn  =  inMoveProgress->phaseNumber;
    AnimPhase  p   =  inMoveProgress->phases[ pn ];
    int        r   =  mingin_getStepsPerSecond();
    
    (void)inNewState;
    

    if( p == move ) {
        char  baseMoveDone;

        getCaptureMidState( inState,
                            inMove,
                            inCaptured,
                            &midState,
                            &midCaptured );

        /* use base move for main piece move and for any direct-piece capture
           explosion */
        
        baseMoveDone = defaultPieceStep( inState,
                                         inMove,
                                         &midCaptured,
                                         &midState,
                                         inMoveProgress );
        if( baseMoveDone ) {
            inMoveProgress->phaseNumber ++;
            inMoveProgress->phaseProgress = 0;
            }
        }
    else if( p == laser ) {
        if( inMoveProgress->phaseProgress == 0 ) {
            maxigin_playSoundEffect( laserSound,
                                     512 );
            }
        inMoveProgress->phaseProgress += ( 30 * 60 ) / r;

        if( inMoveProgress->phaseProgress >= laserPhaseLen ) {
            inMoveProgress->phaseNumber ++;
            inMoveProgress->phaseProgress = 0;
            }
        }
    else if( p == explode ) {
        int  explodeProgress  =  inMoveProgress->phaseProgress;

        if( explodeProgress == 0 ) {
            int  startC     =  inMoveProgress->params[ pn ][ 0 ];
            int  endC       =  inMoveProgress->params[ pn ][ 1 ];
            int  oldScore;
            int  newScore;

            getCaptureCutoffMidState( inState,
                                      inMove,
                                      inCaptured,
                                      &midState,
                                      &midCaptured,
                                      startC -  1 );

            oldScore = getScore( &midState );

            getCaptureCutoffMidState( inState,
                                      inMove,
                                      inCaptured,
                                      &midState,
                                      &midCaptured,
                                      endC );
            
            newScore = getScore( &midState );

            if( oldScore <= newScore ) {
                maxigin_playSoundEffect( shooshGood,
                                         512 );
                }
            else {
                maxigin_playSoundEffect( splatterBad,
                                         512 );
                }
            }
        

        explodeProgress = stepExplodingPiece( explodeProgress );

        if( explodeProgress == -1 ) {
            /* done exploding */
            inMoveProgress->phaseNumber ++;
            inMoveProgress->phaseProgress = 0;
            }
        else {
            inMoveProgress->phaseProgress = explodeProgress;
            }
        }
    else if( p == multiplier
             ||
             p == addition ) {

        if( inMoveProgress->phaseProgress == 0 ) {
            int  sourceR  =  inMoveProgress->params[ pn ][ 0 ];
            int  sourceC  =  inMoveProgress->params[ pn ][ 1 ];
            int  targetR  =  inMoveProgress->params[ pn ][ 2 ];
            int  targetC  =  inMoveProgress->params[ pn ][ 3 ];
            int  value    =  inMoveProgress->params[ pn ][ 4 ];
            
            if( p == multiplier ) {
                maxigin_playSoundEffect( multSound,
                                         512 );
                }
            else {
                maxigin_playSoundEffect( addSound,
                                         512 );
                }

            /* start (or continue) fade-in of mult-factor display on target */

            if( p == multiplier ) {
                inMoveProgress->multFactors[ targetR ][ targetC ]
                    *= value;
                }
            else {
                inMoveProgress->multFactors[ targetR ][ targetC ]
                    += value;
                }
            inMoveProgress->multFactorFadeDir[ targetR ][ targetC ] = 1;

            /* fade out source */
            inMoveProgress->multFactorFadeDir[ sourceR ][ sourceC ] = 0;
            inMoveProgress->anyMultFactors = 1;
            }

        inMoveProgress->phaseProgress += ( 15 * 60 ) / r;

        if( inMoveProgress->phaseProgress >= modifierPhaseLen ) {
            inMoveProgress->phaseNumber ++;
            inMoveProgress->phaseProgress = 0;
            }
        }


    /* deal with mult factor fading progress */
    if( inMoveProgress->anyMultFactors ) {

        char  forceFadeOut = 0;
        char  allDone      = 1;
        
        int   y;
        int   x;

        
        inMoveProgress->sparkleProgress += ( 20 * 60 ) / r;

        if( inMoveProgress->sparkleProgress >= 80 ) {
            inMoveProgress->sparkleProgress = 0;
            
            /* advance rand to change sparkle */
            inMoveProgress->multRandA = inMoveProgress->multRandB;
            }
        
        
        if( p != multiplier
            &&
            p != addition ) {

            /* done with addition/multiplier phases, and there are some
               lingering mult factors, fade them out */
            forceFadeOut = 1;
            }

        for( y = 0;
             y < BH;
             y ++ ) {
            for( x = 0;
                 x < BW;
                 x ++ ) {

                if( forceFadeOut ) {
                    inMoveProgress->multFactorFadeDir[ y ][ x ] = 0;
                    }

                if( inMoveProgress->multFactorFadeDir[ y ][ x ] == 1 ) {
                    allDone = 0;
                    }

                if( inMoveProgress->multFactorFadeDir[ y ][ x ] == 1
                    &&
                    inMoveProgress->multFactorFades  [ y ][ x ] <  255 ) {

                    int  newFade =
                        inMoveProgress->multFactorFades[ y ][ x ]
                        + ( 10 * 60 ) / r;

                    if( newFade >= 255 ) {
                        inMoveProgress->multFactorFades[ y ][ x ] = 255;
                        }
                    else {
                        inMoveProgress->multFactorFades[ y ][ x ] =
                            (unsigned char)newFade;
                        }

                    }
                else if( inMoveProgress->multFactorFadeDir[ y ][ x ] == 0
                         &&
                         inMoveProgress->multFactorFades  [ y ][ x ] >  0 ) {

                    int  newFade =
                        inMoveProgress->multFactorFades[ y ][ x ]
                        - ( 10 * 60 ) / r;

                    if( newFade <= 0 ) {
                        inMoveProgress->multFactorFades[ y ][ x ] = 0;
                        }
                    else {
                        inMoveProgress->multFactorFades[ y ][ x ] =
                            (unsigned char)newFade;
                        allDone = 0;
                        }
                    }
                }
            }

        if( allDone ) {
            inMoveProgress->anyMultFactors = 0;
            }
        }
    
        
    
    
    if( inMoveProgress->phaseNumber >= inMoveProgress->numPhases ) {
        /* done with all phases */
        return 1;
        }
    else {
        return 0;
        }
    
    }


static unsigned char getLaserGlowFade( int  inProgress ) {
    long  glowFade;
    long  progressRange = laserPhaseLen;

    glowFade = ( inProgress * 255 ) / progressRange;

    return (unsigned char)glowFade;
    }



static void drawLaser( int  inBoardCenterX,
                       int  inBoardCenterY,
                       int  inFromRow,
                       int  inFromCol,
                       int  inToRow,
                       int  inToCol,
                       int  inProgress ) {

    int   sI;
    int   midI;

    int   r;
    int   c;
    int   midRowStart;
    int   midRowEnd;
    
    int   midColStart;
    int   midColEnd;
    
    int   xOffStart;
    int   yOffStart;
    int   xOffEnd;
    int   yOffEnd; 
    int   numLaserSteps;
    
    int   sourcePosX;
    int   sourcePosY;

    int   targetPosX;
    int   targetPosY;

    unsigned char  glowFade  = getLaserGlowFade( inProgress );

    
    boardGetSquareCenter( inBoardCenterX,
                          inBoardCenterY,
                          inFromRow,
                          inFromCol,
                          &sourcePosX,
                          &sourcePosY );

    boardGetSquareCenter( inBoardCenterX,
                          inBoardCenterY,
                          inToRow,
                          inToCol,
                          &targetPosX,
                          &targetPosY );
    
    
    if( inFromRow == inToRow ) {
        /* horizontal */

        midI = 0;

        if( inFromCol > inToCol ) {
            /* left */

            sI = 0;

            numLaserSteps = inFromCol - inToCol;

            if( numLaserSteps == 1 ) {
                xOffStart = -16;
                yOffStart = -16;
                }
            else {
                xOffStart = -16;
                yOffStart = -16;
                xOffEnd   =   0;
                yOffEnd   = -16;

                midColStart = inToCol   + 1;
                midColEnd   = inFromCol - 2;
                }
            }
        else {
            /* right */

            sI = 1;
            numLaserSteps = inToCol - inFromCol;

            if( numLaserSteps == 1 ) {
                xOffStart = +16;
                yOffStart = -16;
                }
            else {
                xOffStart = +16;
                yOffStart = -16;
                xOffEnd   =   0;
                yOffEnd   = -16;

                midColStart = inFromCol + 2;
                midColEnd   = inToCol   - 1;
                }
            }
        }
    else {
        /* vertical */

        midI = 1;
        
        if( inFromRow > inToRow ) {
            /* up */

            numLaserSteps = inFromRow - inToRow;

            sI = 2;

            if( numLaserSteps == 1 ) {
                xOffStart = 0;
                yOffStart = -30;
                }
            else {
                xOffStart =   0;
                yOffStart = -23;
                xOffEnd   =   0;
                yOffEnd   =   -7;

                midRowStart = inToRow   + 1;
                midRowEnd   = inFromRow - 2;
                }
            }
        else {
            /* down */

            numLaserSteps = inToRow - inFromRow;

            sI = 3;

            if( numLaserSteps == 1 ) {
                xOffStart = 0;
                yOffStart = -5;
                }
            else {
                xOffStart =  0;
                yOffStart = -7;
                xOffEnd   =  0;
                yOffEnd   =  -19;

                midRowStart = inFromRow + 1;
                midRowEnd   = inToRow   - 2;
                }
            }
        }
    
    

    if( numLaserSteps == 1 ) {
        
        maxigin_drawResetColor();
            
        maxigin_drawSprite( laserShortSprites[sI],
                            sourcePosX + xOffStart,
                            sourcePosY + yOffStart );


        maxigin_drawToggleAdditive( 1 );

        maxigin_drawSetAlpha( glowFade );
            
        maxigin_drawSprite( laserShortGlowSprites[sI],
                            sourcePosX + xOffStart,
                            sourcePosY + yOffStart );
            
        maxigin_drawToggleAdditive( 0 );

        maxigin_drawResetColor();
        return;
        }

    /* else draw tiled laser from start to end with at least
       one mid-section in between */

    /* fixme:  draw mid sections */

    maxigin_drawResetColor();
            
    maxigin_drawSprite( laserStartSprites[sI],
                        sourcePosX + xOffStart,
                        sourcePosY + yOffStart );

        
    maxigin_drawSprite( laserEndSprites[sI],
                        targetPosX + xOffEnd,
                        targetPosY + yOffEnd  );

    if( midI == 0 ) {
        for( c =  midColStart;
             c <= midColEnd;
             c ++ ) {

            int  midPosX;
            int  midPosY;

            boardGetSquareCenter( inBoardCenterX,
                                  inBoardCenterY,
                                  0,
                                  c,
                                  &midPosX,
                                  &midPosY );
            
            midPosY = sourcePosY + yOffStart;

            maxigin_drawSprite( laserMidSprites[ midI ],
                                midPosX,
                                midPosY);
            }
        }
    else {
        for( r =  midRowStart;
             r <= midRowEnd;
             r ++ ) {

            int  midPosX;
            int  midPosY;

            boardGetSquareCenter( inBoardCenterX,
                                  inBoardCenterY,
                                  r,
                                  0,
                                  &midPosX,
                                  &midPosY );
            
            midPosX = sourcePosX + xOffStart;

            maxigin_drawSprite( laserMidSprites[ midI ],
                                midPosX,
                                midPosY);
            }
        }
    


    maxigin_drawToggleAdditive( 1 );

    maxigin_drawSetAlpha( glowFade );
            
    maxigin_drawSprite( laserStartGlowSprites[sI],
                        sourcePosX + xOffStart,
                        sourcePosY + yOffStart );

    maxigin_drawSprite( laserEndGlowSprites[sI],
                        targetPosX + xOffEnd,
                        targetPosY + yOffEnd );


    if( midI == 0 ) {
        for( c =  midColStart;
             c <= midColEnd;
             c ++ ) {

            int  midPosX;
            int  midPosY;

            boardGetSquareCenter( inBoardCenterX,
                                  inBoardCenterY,
                                  0,
                                  c,
                                  &midPosX,
                                  &midPosY );
            
            midPosY = sourcePosY + yOffStart;

            maxigin_drawSprite( laserMidGlowSprites[ midI ],
                                midPosX,
                                midPosY);
            }
        }
    else {
        for( r =  midRowStart;
             r <= midRowEnd;
             r ++ ) {

            int  midPosX;
            int  midPosY;

            boardGetSquareCenter( inBoardCenterX,
                                  inBoardCenterY,
                                  r,
                                  0,
                                  &midPosX,
                                  &midPosY );
            
            midPosX = sourcePosX + xOffStart;

            maxigin_drawSprite( laserMidGlowSprites[ midI ],
                                midPosX,
                                midPosY);
            }
        }

    maxigin_drawToggleAdditive( 0 );
    
    maxigin_drawResetColor();
    }



static void multiPhaseDraw( int            inBoardCenterX,
                            int            inBoardCenterY,
                            BoardState    *inState,
                            Move          *inMove,
                            Captured      *inCaptured,
                            BoardState    *inNewState,
                            AnimProgress  *inMoveProgress ) {

    static  BoardState  midState;
    static  Captured    midCaptured;
    
    int        pn  =  inMoveProgress->phaseNumber;
    AnimPhase  p   =  inMoveProgress->phases[ inMoveProgress->phaseNumber ];
    
    (void)inNewState;
    

    if( p == move ) {
        getCaptureMidState( inState,
                            inMove,
                            inCaptured,
                            &midState,
                            &midCaptured );

        defaultPieceDraw( inBoardCenterX,
                          inBoardCenterY,
                          inState,
                          inMove,
                          &midCaptured,
                          &midState,
                          inMoveProgress );
        }
    else if( p == laser ) {

        /* n, s, e, w order, with noPiece marking empty spots */
        static  BoardPiece  laserCaptured[4];

        int            laserProgress  =  inMoveProgress->phaseProgress;
        DrawBoardMask  mask;
        int            i;
        int            destR          =  inMove->endPos[0];
        int            destC          =  inMove->endPos[1];
        int            destX;
        int            destY;
        ChessPiece     mainP;
        int            southCapR      =  0;
        int            southCapX      =  0;
        int            southCapY      =  0;
        int            startC;
        int            endC;
            
            
        mainP = inNewState->grid[ destR ][ destC ];
            
        for( i = 0;
             i < 4;
             i ++ ) {
                
            laserCaptured[i].p = noPiece;
            }

        startC = inMoveProgress->params[ pn ][ 0 ];
        endC   = inMoveProgress->params[ pn ][ 1 ];

        getCaptureCutoffMidState( inState,
                                  inMove,
                                  inCaptured,
                                  &midState,
                                  &midCaptured,
                                  startC - 1 );

        for( i = startC;
             i <= endC;
             i ++ ) {

            int  r  =  inCaptured->pieces[i].row;
            int  c  =  inCaptured->pieces[i].col;

            if( r < destR ) {
                /* north */
                laserCaptured[0] = inCaptured->pieces[i];
                continue;
                }
            if( r > destR ) {
                /* south */
                laserCaptured[1] = inCaptured->pieces[i];
                southCapR        = laserCaptured[1].row;
                    
                continue;
                }
            if( c > destC ) {
                /* east */
                laserCaptured[2] = inCaptured->pieces[i];
                continue;
                }
            if( c < destC ) {
                /* west */
                laserCaptured[3] = inCaptured->pieces[i];
                continue;
                }
            }

        boardDraw( inBoardCenterX,
                   inBoardCenterY );

        if( inMove->endPos[0] > 0 ) {

            /* draw pieces north of main piece row */
            getRowsAboveMask( &mask,
                              inMove->endPos[0] - 1 );
            
            drawBoardState( &midState,
                            0,
                            0,
                            0,
                            0,
                            inMove,
                            0,
                            0,
                            inBoardCenterX,
                            inBoardCenterY,
                            &mask );
            }


        /* draw main piece's shadow behind any up laser */

        boardGetSquareCenter( inBoardCenterX,
                              inBoardCenterY,
                              destR,
                              destC,
                              &destX,
                              &destY );

        drawPieceShadowOnly( mainP,
                             destX,
                             destY );


        if( laserCaptured[0].p != noPiece ) {

            /* draw up laser behind main piece */

            drawLaser( inBoardCenterX,
                       inBoardCenterY,
                       inMove->endPos[0],
                       inMove->endPos[1],
                       laserCaptured[0].row,
                       laserCaptured[0].col,
                       laserProgress );
            }
            

        /* draw main piece's row without main piece */

        getRowMask( &mask,
                    destR );
            
        clearMaskSpot( &mask,
                       destR,
                       destC );

        drawBoardState( &midState,
                        0,
                        0,
                        0,
                        0,
                        inMove,
                        0,
                        0,
                        inBoardCenterX,
                        inBoardCenterY,
                        &mask );

        /* now draw main piece in row on top of shadow drawn before */

        drawPieceBaseAndGlowOnly( mainP,
                                  destX,
                                  destY );

        if( ( mainP & CHESS_TYPE_MASK  ) == laserPawn
            &&
            ( mainP & CHESS_COLOR_MASK ) == CHESS_WHITE ) {
            /* special case of white (back-facing) laser pawn
               draw an extra glint on top */

            int  glintOffsetY  =  -19;
                

            maxigin_drawResetColor();

            maxigin_drawSprite( laserPawnTopGlintSprite,
                                destX,
                                destY + glintOffsetY );
            maxigin_drawToggleAdditive( 1 );

            maxigin_drawSetAlpha( getLaserGlowFade( laserProgress ) );

            maxigin_drawSprite( laserPawnTopGlintGlow,
                                destX,
                                destY + glintOffsetY );
            maxigin_drawResetColor();
            maxigin_drawToggleAdditive( 0 );
            }
                


        /* draw everything to south,
           not including row of south captured piece */

        if( destR < BH - 1 ) {
                

            if( laserCaptured[1].p != noPiece ) {
                
                getRowRangeMask( &mask,
                                 destR + 1,
                                 southCapR - 1 );
                }
            else {
                getRowsBelowMask( &mask,
                                  destR + 1 );
                }
            
            
            drawBoardState( &midState,
                            0,
                            0,
                            0,
                            0,
                            inMove,
                            0,
                            0,
                            inBoardCenterX,
                            inBoardCenterY,
                            &mask );
            }
            

        if( laserCaptured[1].p != noPiece ) {

            boardGetSquareCenter( inBoardCenterX,
                                  inBoardCenterY,
                                  laserCaptured[1].row,
                                  laserCaptured[1].col,
                                  &southCapX,
                                  &southCapY );

            drawPieceShadowOnly( laserCaptured[1].p,
                                 southCapX,
                                 southCapY );
            }
            

        /* draw remaining 4 lasers on top of those pieces */

        for( i = 1;
             i < 4;
             i ++ ) {
                
            if( laserCaptured[i].p != noPiece ) {
                drawLaser( inBoardCenterX,
                           inBoardCenterY,
                           inMove->endPos[0],
                           inMove->endPos[1],
                           laserCaptured[i].row,
                           laserCaptured[i].col,
                           laserProgress );
                }
            }
            

        /* now draw bottom section on top */
        if( destR < BH - 1 ) {
                

            if( laserCaptured[1].p != noPiece ) {

                /* row with south captured piece */

                int  glintOffsetY  =  -14;

                getRowMask( &mask,
                            southCapR );

                /* don't draw piece itself, since it's shadow already drawn
                   above */
                clearMaskSpot( &mask,
                               southCapR,
                               destC );

                drawBoardState( &midState,
                                0,
                                0,
                                0,
                                0,
                                inMove,
                                0,
                                0,
                                inBoardCenterX,
                                inBoardCenterY,
                                &mask );

                /* now draw rest of piece, above shadow drawn behind
                   laser before */
                drawPieceBaseAndGlowOnly( laserCaptured[1].p,
                                          southCapX,
                                          southCapY );

                /* now draw back-side glint */

                maxigin_drawResetColor();

                maxigin_drawSprite( laserBackGlintSprite,
                                    southCapX,
                                    southCapY + glintOffsetY );
                maxigin_drawToggleAdditive( 1 );

                maxigin_drawSetAlpha( getLaserGlowFade( laserProgress ) );

                maxigin_drawSprite( laserBackGlintGlow,
                                    southCapX,
                                    southCapY + glintOffsetY );
                maxigin_drawResetColor();
                maxigin_drawToggleAdditive( 0 );

                if( southCapR < BH - 1  ) {

                    getRowsBelowMask( &mask,
                                      southCapR + 1 );
                    
                    drawBoardState( &midState,
                                    0,
                                    0,
                                    0,
                                    0,
                                    inMove,
                                    0,
                                    0,
                                    inBoardCenterX,
                                    inBoardCenterY,
                                    &mask );
                    } 
                }
            
            }
        /* end of p == laser case */
        }
    else if( p == explode ) {

        int  explodingProgress  =  inMoveProgress->phaseProgress;
        int  startC;
        int  endC;
        int  c;

        startC = inMoveProgress->params[ pn ][ 0 ];
        endC   = inMoveProgress->params[ pn ][ 1 ];

        getCaptureCutoffMidState( inState,
                                  inMove,
                                  inCaptured,
                                  &midState,
                                  &midCaptured,
                                  endC );

        boardDraw( inBoardCenterX,
                   inBoardCenterY );
    
        /* draw mid state with captures up to this point removed */
        drawBoardState( &midState,
                        0,
                        0,
                        0,
                        0,
                        inMove,
                        0,
                        0,
                        inBoardCenterX,
                        inBoardCenterY,
                        0 );
            
        for( c = startC;
             c <= endC;
             c ++ ) {

            BoardPiece  *bp  =  &( inCaptured->pieces[ c ] );

            drawExplodingPiece( bp->p,
                                inBoardCenterX,
                                inBoardCenterY,
                                bp->row,
                                bp->col,
                                explodingProgress );
            }
        }
    else if( p == multiplier
             ||
             p == addition ) {

        int            sourceR        =  inMoveProgress->params[ pn ][ 0 ];
        int            sourceC        =  inMoveProgress->params[ pn ][ 1 ];
        int            targetR        =  inMoveProgress->params[ pn ][ 2 ];
        int            targetC        =  inMoveProgress->params[ pn ][ 3 ];
        int            sourceX;
        int            sourceY;
        int            targetX;
        int            targetY;
        int            deltaX;
        int            deltaY;
        int            drawX;
        int            drawY;
        int            glintOffsetY  =  -11;
        unsigned char  glintFade     =  255;
        const char    *displayText;
        const char    *symbol        =  "x";
        long           parH          =  10;
        long           parY;
        long           halfPhaseLen  =  modifierPhaseLen / 2;
        long           endPhase      =  modifierPhaseLen / 8;
        
        if( p == addition ) {
            symbol = "+";
            }


        if( inMoveProgress->phaseProgress < halfPhaseLen ) {
            
            glintFade =
                (unsigned char)(
                    ( (long)( inMoveProgress->phaseProgress ) * 255 )
                    / halfPhaseLen );
            }
        else if( inMoveProgress->phaseProgress > modifierPhaseLen - endPhase ) {

            glintFade =
                (unsigned char)(
                    ( ( modifierPhaseLen -
                        (long)( inMoveProgress->phaseProgress ) )
                      * 255 ) / endPhase );  
            }
        
        
        boardGetSquareCenter( inBoardCenterX,
                              inBoardCenterY,
                              targetR,
                              targetC,
                              &targetX,
                              &targetY );

        boardGetSquareCenter( inBoardCenterX,
                              inBoardCenterY,
                              sourceR,
                              sourceC,
                              &sourceX,
                              &sourceY );

        deltaX = targetX - sourceX;
        deltaY = targetY - sourceY;

        drawX = (int)( sourceX +
                       ( (long)inMoveProgress->phaseProgress * deltaX ) /
                       (long)modifierPhaseLen );
        drawY = (int)( sourceY +
                       ( (long)inMoveProgress->phaseProgress * deltaY ) /
                       (long)modifierPhaseLen );

        /* parabolic */
        /*
          example equation for height of 6 and phase length of 512
          (6.0 * 512/( 256 * 256 ) ) * x - (6.0/( 256 * 256 ) ) * x * x
        */

        parY = ( parH * modifierPhaseLen * inMoveProgress->phaseProgress
            - parH * inMoveProgress->phaseProgress
                 * inMoveProgress->phaseProgress )
            /
            ( halfPhaseLen * halfPhaseLen );

        /* do this for both horizontal and vertical moves,
           since it makes a visual bouncing effect in both cases */
        drawY -= (int)parY;
        
        
        boardDraw( inBoardCenterX,
                   inBoardCenterY );

        getCaptureMidState( inState,
                            inMove,
                            inCaptured,
                            &midState,
                            &midCaptured );
    
        drawBoardState( &midState,
                        0,
                        0,
                        0,
                        0,
                        inMove,
                        0,
                        0,
                        inBoardCenterX,
                        inBoardCenterY,
                        0 );

        maxigin_drawResetColor();

        maxigin_drawSetAlpha( glintFade );
        
        /*
        maxigin_drawSprite( laserBackGlintSprite,
                            targetX,
                            targetY+ glintOffsetY );
        */

        displayText =
            maxigin_stringConcat(
                symbol,
                maxigin_intToString( inMoveProgress->params[ pn ][ 4 ] ) );
        
        maxigin_drawText( modifierFont,
                          displayText,
                          drawX,
                          drawY + glintOffsetY,
                          MAXIGIN_CENTER );
        }


    
    /* draw mult-factor glow over pieces
     this may linger and fade out after multiplier animation phases are done */
    
    if( inMoveProgress->anyMultFactors ) {
        
        int          mY;
        int          mX;
        MaxiginRand  oldRand = inMoveProgress->multRandA;
        
        getCaptureMidState( inState,
                            inMove,
                            inCaptured,
                            &midState,
                            &midCaptured );
        
        for( mY = 0;
             mY < BH;
             mY ++ ) {
            
            for( mX = 0;
                 mX < BW;
                 mX ++ ) {
                
                int            bX;
                int            bY;
                unsigned char  f          =
                    inMoveProgress->multFactorFades[ mY ][ mX ];
                long           v          =
                    inMoveProgress->multFactors    [ mY ][ mX ];
                long           glowFade;
                long           glowMax    =  f / 2;
                long           glowMin    =  f / 8;
                long           glowRange  =  glowMax - glowMin;
                
                if( f == 0 ) {
                    continue;
                    }
                    
                boardGetSquareCenter( inBoardCenterX,
                                      inBoardCenterY,
                                      mY,
                                      mX,
                                      &bX,
                                      &bY );
                /* repeat glow to make it stronger as mult factor
                   rises,
                   but skip first one, since there's no glow at mult-factor
                   1 (it starts at 2) */

                if( 1 ) {
                    
                    
                    drawPieceSparkles( midState.grid[ mY ][ mX ],
                                       bX,
                                       bY,
                                       &( inMoveProgress->multRandA ),
                                       (int)( v * 10 ),
                                       f );
                    
                    }

                /* reaches glowMax when v hits 10 */
                glowFade = ( glowRange * v ) / 10 + glowMin;

                if( glowFade > glowMax ) {
                    glowFade = glowMax;
                    }
                
                drawPieceGlowOnly( midState.grid[ mY ][ mX ],
                                   bX,
                                   bY,
                                   (unsigned char)glowFade );
                }
            }

        /* restore rand state after drawing, so rand state doesn't
           keep advancing with each new frame drawn
           let anim step advance it */
        
        /* save post-drawing state, so we can use it in step to advance
           rand by more than one step.
           If we just advance rand by calling it once in step,
           we simply inch one frame forward in these sparkle animations,
           which causes them to repeat */
        inMoveProgress->multRandB = inMoveProgress->multRandA;
        
        inMoveProgress->multRandA = oldRand;
        }
    }

                       



static MoveAnimInitFunction initFunctions[] =
                                { noPieceInit,
                                  defaultPieceInit,
                                  defaultPieceInit,
                                  defaultPieceInit,
                                  defaultPieceInit,
                                  defaultPieceInit,
                                  defaultPieceInit,
                                  /* laser rook and pawn
                                     use same */
                                  laserPieceInit,
                                  laserPieceInit,
                                  defaultPieceInit,
                                  defaultPieceInit };

CHECK_ARRAY_LENGTH( initFunctions,
                    NUM_CHESS_PIECES );



static MoveAnimStepFunction stepFunctions[] =
                                { noPieceStep,
                                  defaultPieceStep,
                                  defaultPieceStep,
                                  defaultPieceStep,
                                  defaultPieceStep,
                                  defaultPieceStep,
                                  defaultPieceStep,
                                  /* laser rook and pawn
                                     use same */
                                  multiPhaseStep,
                                  multiPhaseStep,
                                  defaultPieceStep,
                                  defaultPieceStep };

CHECK_ARRAY_LENGTH( stepFunctions,
                    NUM_CHESS_PIECES );



static MoveAnimDrawFunction drawFunctions[] =
                                { noPieceDraw,
                                  defaultPieceDraw,
                                  defaultPieceDraw,
                                  defaultPieceDraw,
                                  defaultPieceDraw,
                                  defaultPieceDraw,
                                  defaultPieceDraw,
                                  /* laser rook and pawn
                                     use same */
                                  multiPhaseDraw,
                                  multiPhaseDraw,
                                  defaultPieceDraw,
                                  defaultPieceDraw };

CHECK_ARRAY_LENGTH( drawFunctions,
                    NUM_CHESS_PIECES );


void initMoveAnimation( BoardState    *inState,
                        Move          *inMove,
                        Captured      *inCaptured,
                        BoardState    *inNewState,
                        AnimProgress  *outMoveProgress ) {

    ChessPiece  p  =  inState->grid[ inMove->startPos[0] ]
                                   [ inMove->startPos[1] ];
    ChessPiece  t  =  p & CHESS_TYPE_MASK;
    
    initFunctions[ t ]( inState,
                        inMove,
                        inCaptured,
                        inNewState,
                        outMoveProgress );
    }



char stepMoveAnimation( BoardState    *inState,
                        Move          *inMove,
                        Captured      *inCaptured,
                        BoardState    *inNewState,
                        AnimProgress  *inMoveProgress ) {

    ChessPiece  p  =  inState->grid[ inMove->startPos[0] ]
                                   [ inMove->startPos[1] ];
    ChessPiece  t  =  p & CHESS_TYPE_MASK;
    
    return stepFunctions[ t ]( inState,
                               inMove,
                               inCaptured,
                               inNewState,
                               inMoveProgress );
    }




/* draws entire board with move animation according to inMoveProgress */
void drawMoveAnimation( int            inBoardCenterX,
                        int            inBoardCenterY,
                        BoardState    *inState,
                        Move          *inMove,
                        Captured      *inCaptured,
                        BoardState    *inNewState,
                        AnimProgress  *inMoveProgress ) {

    ChessPiece  p  =  inState->grid[ inMove->startPos[0] ]
                                   [ inMove->startPos[1] ];
    ChessPiece  t  =  p & CHESS_TYPE_MASK;
    
    drawFunctions[ t ]( inBoardCenterX,
                        inBoardCenterY,
                        inState,
                        inMove,
                        inCaptured,
                        inNewState,
                        inMoveProgress );
    }



#endif


#endif
