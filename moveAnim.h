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


/* inMoveProgress should start at 0

   inMoveProgress is updated with values that have meaning internally
   (range may vary depending on piece type that is moving)

   returns  0 when not done yet
            1 when done
*/
char stepMoveAnimation( BoardState  *inState,
                        Move        *inMove,
                        Captured    *inCaptured,
                        BoardState  *inNewState,
                        int         *inMoveProgress );



/* draws entire board with move animation according to inMoveProgress */
void drawMoveAnimation( int          inBoardCenterX,
                        int          inBoardCenterY,
                        BoardState  *inState,
                        Move        *inMove,
                        Captured    *inCaptured,
                        BoardState  *inNewState,
                        int          inMoveProgress );



#ifdef MOVE_ANIM_IMPLEMENTATION


static  int  beepUp       =  -1;
static  int  beepDown     =  -1;
static  int  shooshGood   =  -1;
static  int  splatterBad  =  -1;
static  int  laserSound   =  -1;

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



void moveAnimInit( void ) {

    int  i;
    

    beepUp = maxigin_initSoundEffect( "beepUp.wav" );
    beepDown = maxigin_initSoundEffect( "beepDown.wav" );
    shooshGood = maxigin_initSoundEffect( "shooshGood.wav" );
    splatterBad = maxigin_initSoundEffect( "splatterBad.wav" );
    laserSound = maxigin_initSoundEffect( "laser_sd_16.wav" );

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

    laserBackGlintSprite = maxigin_initSprite( "laserBackSideGlint.tga" );
    laserBackGlintGlow = maxigin_initSprite( "laserBackSideGlintGlow.tga" );
    }




/* The signature for a move animation step function.
   We define one of these for each piece type in the enum in chess.h.

   Updates inMoveProgress and returns 1 when animation is done.
*/
typedef char (*MoveAnimStepFunction)( BoardState  *inState,
                                      Move        *inMove,
                                      Captured    *inCaptured,
                                      BoardState  *inNewState,
                                      int         *inMoveProgress );

/* The signature for a move animation draw function.
   We define one of these for each piece type in the enum in chess.h.
*/
typedef void (*MoveAnimDrawFunction)( int          inBoardCenterX,
                                      int          inBoardCenterY,
                                      BoardState  *inState,
                                      Move        *inMove,
                                      Captured    *inCaptured,
                                      BoardState  *inNewState,
                                      int          inMoveProgress );



static char noPieceStep( BoardState  *inState,
                         Move        *inMove,
                         Captured    *inCaptured,
                         BoardState  *inNewState,
                         int         *inMoveProgress ) {
    (void)inState;
    (void)inMove;
    (void)inCaptured;
    (void)inNewState;
    (void)inMoveProgress;
    return 1;
    }



/* draws entire board with move animation according to inMoveProgress */
static void noPieceDraw( int          inBoardCenterX,
                         int          inBoardCenterY,
                         BoardState  *inState,
                         Move        *inMove,
                         Captured    *inCaptured,
                         BoardState  *inNewState,
                         int          inMoveProgress ) {
    
    (void)inBoardCenterX;
    (void)inBoardCenterY;
    (void)inState;
    (void)inMove;
    (void)inCaptured;
    (void)inNewState;
    (void)inMoveProgress;
    }




static char defaultPieceStep( BoardState  *inState,
                              Move        *inMove,
                              Captured    *inCaptured,
                              BoardState  *inNewState,
                              int         *inMoveProgress ) {
    
    int  pixDist  =  boardGetPixelDistance( inMove->startPos[0],
                                            inMove->startPos[1],
                                            inMove->endPos[0],
                                            inMove->endPos[1] );
    int  r        =  mingin_getStepsPerSecond();

    
    (void)inState;
    (void)inNewState;
    
    
    if( *inMoveProgress < pixDist ) {

        /* still on piece movement portion of animtion */
        
        *inMoveProgress += ( 4 * 60 ) / r;

        if( *inMoveProgress > pixDist ) {
            *inMoveProgress = pixDist;
            
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

        int  explodeProgress  =  *inMoveProgress - pixDist;
        
        explodeProgress = stepExplodingPiece( explodeProgress );

        if( explodeProgress == -1 ) {
            /* done exploding */
            return 1;
            }

        *inMoveProgress = pixDist + explodeProgress;
        }

    return 0;
    }



static void defaultPieceDraw( int          inBoardCenterX,
                              int          inBoardCenterY,
                              BoardState  *inState,
                              Move        *inMove,
                              Captured    *inCaptured,
                              BoardState  *inNewState,
                              int          inMoveProgress ) {

    int  pixDist  =  boardGetPixelDistance( inMove->startPos[0],
                                            inMove->startPos[1],
                                            inMove->endPos[0],
                                            inMove->endPos[1] );
    boardDraw( inBoardCenterX,
               inBoardCenterY );

    if( inMoveProgress < pixDist ) {
        /* draw move in progress */

        drawBoardState( inState,
                        0,
                        0,
                        0,
                        0,
                        inMove,
                        inMoveProgress,
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
            int  explodingProgress  =  inMoveProgress - pixDist;
            
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



/* push this way above progress range used by a default move,
   which would be diagonal pixels across board plus 512 for explosion */
static int  laserStart  =  10000;
static int  laserMax    =  10000 + 512;



/* gets state and captured list for laser rook after it moves but
   before it fires */
static void getLaserRookMidState( BoardState  *inState,
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

    /* put rook where it lands in outMidState */
    outMidState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ] = noPiece;
    outMidState->grid[ inMove->endPos  [0] ][ inMove->endPos  [1] ] =
        inState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ];
    }


    

static char laserRookStep( BoardState  *inState,
                           Move        *inMove,
                           Captured    *inCaptured,
                           BoardState  *inNewState,
                           int         *inMoveProgress ) {

    static  BoardState  midState;
    static  Captured    midCaptured;
    
    char  takeDefault       =  0;
    int   r;
    int   explodeProgress;

    (void)inNewState;
    
    
    if( *inMoveProgress < laserStart ) {
        
        char  baseMoveDone;

        getLaserRookMidState( inState,
                              inMove,
                              inCaptured,
                              &midState,
                              &midCaptured );

        /* use base move for rook move and for any direct-piece capture
           explosion.
           Fire lasers after that's all done */
        
        baseMoveDone = defaultPieceStep( inState,
                                         inMove,
                                         &midCaptured,
                                         &midState,
                                         inMoveProgress );
        if( !baseMoveDone ) {
            return 0;
            }

        
        if( inCaptured->num == 0 ) {
            takeDefault = 1;
            }
        else if( inCaptured->num == 1 ) {

            BoardPiece  bp  =  inCaptured->pieces[0];

            if( bp.row == inMove->endPos[0]
                &&
                bp.col == inMove->endPos[1] ) {

                /* single piece captured directly by rook, not by laser */

                takeDefault = 1;
                }
            }

        if( takeDefault ) {
            /* done after default is done, no lasers fired */
            return 1;
            }
        
        /* start lasers on next step */
        *inMoveProgress = laserStart;

        maxigin_playSoundEffect( laserSound,
                                 512 );
        
        return 0;
        }
    


    r = mingin_getStepsPerSecond();

    if( *inMoveProgress < laserMax ) {
        /* step laser animation itself */
        
        *inMoveProgress += ( 30 * 60 ) / r;

        if( *inMoveProgress >= laserMax ) {
            /* start post-laser explosions on next step */
            *inMoveProgress = laserMax;

            if( inState->nextToMove == CHESS_WHITE ) {
                
                maxigin_playSoundEffect( shooshGood,
                                         512 );
                }
            else {
                maxigin_playSoundEffect( splatterBad,
                                         512 );
                }
            }

        return 0;
        }

    /* now explode pieces hit by laser */

    explodeProgress = *inMoveProgress - laserMax;

    explodeProgress = stepExplodingPiece( explodeProgress );

    if( explodeProgress == -1 ) {
        /* done exploding */
        return 1;
        }

    *inMoveProgress = laserMax + explodeProgress;

    return 0;
    }



static unsigned char getLaserGlowFade( int  inProgress ) {
    long  glowFade;
    long  progressRange = laserMax - laserStart;

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

                       


static void laserRookDraw( int          inBoardCenterX,
                           int          inBoardCenterY,
                           BoardState  *inState,
                           Move        *inMove,
                           Captured    *inCaptured,
                           BoardState  *inNewState,
                           int          inMoveProgress ) {

    static  BoardState  midState;
    static  Captured    midCaptured;

    if( inMoveProgress < laserMax ) {

        getLaserRookMidState( inState,
                              inMove,
                              inCaptured,
                              &midState,
                              &midCaptured );
        

        if( inMoveProgress < laserStart ) {
            
            defaultPieceDraw( inBoardCenterX,
                              inBoardCenterY,
                              inState,
                              inMove,
                              &midCaptured,
                              &midState,
                              inMoveProgress );
            }
        else {

            /* hold board still from now during laser fire */

            /* n, s, e, w order, with noPiece marking empty spots */
            static  BoardPiece  laserCaptured[4];

            int            laserProgress  =  inMoveProgress - laserStart;
            DrawBoardMask  mask;
            int            i;
            int            destR          =  inMove->endPos[0];
            int            destC          =  inMove->endPos[1];
            int            southCapR;
            int            southCapX;
            int            southCapY;
            
            for( i = 0;
                 i < 4;
                 i ++ ) {
                
                laserCaptured[i].p = noPiece;
                }

            for( i = 0;
                 i < inCaptured->num;
                 i ++ ) {

                int  r  =  inCaptured->pieces[i].row;
                int  c  =  inCaptured->pieces[i].col;

                if( r == destR
                    &&
                    c == destC ) {
                    /* our direct, non-laser capture */
                    continue;
                    }

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

            /* test drawing up lasers behind rook */

            /* fixme:  rook shadow overlaps, and it doesn't
               look good, and anyway, we need to draw laser over
               pieces BEHIND rook too.

               So we need a version of drawBoardState that draws
               rows, and that can skip pieces.

               So we can draw parts of rook (like shadow) under
               an up laser */

            if( inMove->endPos[0] > 0 ) {

                /* draw pieces north of rook row */
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



            if( laserCaptured[0].p != noPiece ) {

                /* draw up laser behind rook*/

                drawLaser( inBoardCenterX,
                           inBoardCenterY,
                           inMove->endPos[0],
                           inMove->endPos[1],
                           laserCaptured[0].row,
                           laserCaptured[0].col,
                           laserProgress );
                }
            

            /* draw rook's row and everything to south,
               not including row of south captured piece */

            if( laserCaptured[1].p != noPiece ) {
                
                getRowRangeMask( &mask,
                                 inMove->endPos[0],
                                 southCapR - 1 );
                }
            else {
                getRowsBelowMask( &mask,
                                  inMove->endPos[0] );
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
        

        return;
        }

    /* got here, we're done with laser fire, explode those pieces */

    boardDraw( inBoardCenterX,
               inBoardCenterY );
    
    /* draw end state */
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
        int  explodingProgress  =  inMoveProgress - laserMax;
            
        for( pn = 0;
             pn < inCaptured->num;
             pn ++ ) {

            BoardPiece  *bp  =  &( inCaptured->pieces[ pn ] );

            /* don't explot any captured pieces that our rook landed
               on directly, since those were already exploded by default move */
            if( bp->row != inMove->endPos[0]
                ||
                bp->col != inMove->endPos[1] ) {
             
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





static MoveAnimStepFunction stepFunctions[ NUM_CHESS_PIECES ] =
                                              { noPieceStep,
                                                defaultPieceStep,
                                                defaultPieceStep,
                                                defaultPieceStep,
                                                defaultPieceStep,
                                                defaultPieceStep,
                                                defaultPieceStep,
                                                laserRookStep,
                                                defaultPieceStep };


static MoveAnimDrawFunction drawFunctions[ NUM_CHESS_PIECES ] =
                                              { noPieceDraw,
                                                defaultPieceDraw,
                                                defaultPieceDraw,
                                                defaultPieceDraw,
                                                defaultPieceDraw,
                                                defaultPieceDraw,
                                                defaultPieceDraw,
                                                laserRookDraw,
                                                defaultPieceDraw };



char stepMoveAnimation( BoardState  *inState,
                        Move        *inMove,
                        Captured    *inCaptured,
                        BoardState  *inNewState,
                        int         *inMoveProgress ) {

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
void drawMoveAnimation( int          inBoardCenterX,
                        int          inBoardCenterY,
                        BoardState  *inState,
                        Move        *inMove,
                        Captured    *inCaptured,
                        BoardState  *inNewState,
                        int          inMoveProgress ) {

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
