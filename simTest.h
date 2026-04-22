#ifndef SIM_TEST_H_INCLUDED
#define SIM_TEST_H_INCLUDED

#include "chess.h"

static void runChessTest( void ) {

    BoardState  s;
    int         i;

    int            stalemateCount  =  0;
    int            whiteWinCount   =  0;
    int            blackWinCount   =  0;
    int            drawCount       =  0;

    int            movesToDraw     =  100;
    int            numRuns         =  1000;
    int            noScoreMoves    =  0;
    char           gameOver        =  0;
    unsigned long  seed            =  12035793;
    unsigned long  drawSeed        =  0;
    
    for( i = 0;
         i < numRuns;
         i ++ ) {
        
        gameOver     = 0;
        noScoreMoves = 0;

        seed++;

        chessSeed( seed );
        if(0)getStartBoard( &s );
        getTestBoard( &s );

        maxigin_logInt( "\n\nSeed: ",
                        (int)seed );
        
        while( ! gameOver ) {
            
            BoardState  nextS;
            Move        m;
            Captured    c;

            if( getChessMove( &s,
                              &m,
                              &c,
                              &nextS ) ) {
                int  oldScore  =  getScore( &s );

                if( oldScore == getScore( &nextS ) ) {
                    noScoreMoves ++;

                    if( noScoreMoves > movesToDraw ) {
                        drawCount ++;
                        gameOver = 1;
                        mingin_log( "Draw\n" );
                        drawSeed = seed;
                        }
                    }
                else {
                    noScoreMoves = 0;
                    }
                    
                applyMove( &s,
                           &m,
                           &nextS );
                }
            else {
                int  losingSide;
                
                if( isCheckmate( &s,
                                 &losingSide ) ) {

                    if( losingSide == CHESS_BLACK ) {
                        whiteWinCount ++;
                        mingin_log( "White\n" );  
                        }
                    else {
                        blackWinCount ++;
                        mingin_log( "Black\n" );
                        }
                    gameOver = 1;
                    }
                else {
                    stalemateCount ++;
                    gameOver = 1;
                    mingin_log( "Stalemate\n" );
                    }
                }
            
            }

        }

    maxigin_logInt( "Out of runs: ",
                    numRuns );
    maxigin_logInt( "White Wins: ",
                    whiteWinCount );
    maxigin_logInt( "Black Wins: ",
                    blackWinCount );
    maxigin_logInt( "Stalemates: ",
                    stalemateCount );
    maxigin_logInt( "Draws: ",
                    drawCount );

    maxigin_logInt( "Most recent Draw seed: ",
                    (int)drawSeed );

    }



#endif
