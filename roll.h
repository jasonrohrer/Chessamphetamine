/*
  Include in your C code wherever like so:

      #include "roll.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define ROLL_IMPLEMENTATION
      #include "roll.h"

*/

#ifndef ROLL_H_INCLUDED
#define ROLL_H_INCLUDED


typedef struct RollInfo {
        
        int  baseOneInCount;
        
        int  currentOneInCount;

        int  missCount;
        
    } RollInfo;


/* initializes a roll that will happen, on average, once in inOneInCount rolls */
void rollInit( RollInfo  *inRoll,
              int        inOneInCount );


/* returns 1 on hit, 0 on miss */
char roll( RollInfo  *inRoll );

void rollReset( RollInfo  *inRoll );


void rollTest( void );



/* initialize a roll pool of items that are all equally likely.

   returns handle to pool
 */
int  rollPoolInit( int  inNumItems,
                   int  inItems[] );

int  rollItem( int  inRollPoolHandle );



#ifdef ROLL_IMPLEMENTATION


static  MaxiginRand  rollRand;
static  char         rollRandReady  =  0;




void rollInit( RollInfo  *inRoll,
              int        inOneInCount ) {

    if( ! rollRandReady ) {

        maxigin_randSeed( &rollRand,
                          mingin_getEntropySeed() );
        REGISTER_VAL_MEM( rollRand );

        rollRandReady = 1;
        }
    
    inRoll->baseOneInCount = inOneInCount;

    rollReset( inRoll );
    }



/* returns 1 on hit, 0 on miss */
char roll( RollInfo  *inRoll ) {

    char hit  =  0;
    
    int  val  =  maxigin_randRange( &rollRand,
                                    1,
                                    inRoll->currentOneInCount );
    if( val == 1 ) {
        hit = 1;
        }
    else {
        inRoll->missCount ++;

        if( inRoll->missCount >= inRoll->currentOneInCount ) {
            /* reached our expected wait time
               if we kept going with the same hit chance, we'd expect to
               go on for currentOneInCount rolls again from here before hitting.
               
               Perform bad-luck mitigation by doubling the chance of a hit
               from here on out, cutting our expected wait time in half
               
               I.e., if we expect to hit 1 in 100 rolls, and we go 100 rolls
               without a hit, we normally expect to go 100 more rolls without
               a hit.  I.e., once we hit 100, we expect to go 200 total.

               By doubling the hit rate after we hit 100, we expect to go 50
               more before we hit, so our expected total wait time is 150.

               We keep doing this, whenever we reach our current expected wait
               time, cutting it in half again, approaching 2x our base expected
               wait time, Zeno-style.
            */
            if( inRoll->currentOneInCount >= 3 ) {
                /* stop dividing once we hit 1 in 2
                   so we never reach the point of a sure thing */

                if( inRoll->currentOneInCount == 3 ) {
                    /* special case:
                       we hit 3 misses when rolling 1 in 3
                       go to our final state of 1 in 2 */
                    inRoll->currentOneInCount = 2;
                    }
                else {
                    inRoll->currentOneInCount /= 2;
                    }

                /* reset miss count every time we do this */
                inRoll->missCount = 0;
                }
            }      
        }
    
    if( hit ){
        rollReset( inRoll );
        }
    return hit;
    }
    
                       
    

void rollReset( RollInfo  *inRoll ) {
    inRoll->currentOneInCount = inRoll->baseOneInCount;
    inRoll->missCount         = 0;
    }



void rollTest( void ) {

    RollInfo  r;

    int       hitInCounts[ 1000 ];
    int       i;
    int       numTrials             =  1000000;
    int       maxHitCount           =  1000;
    
    rollInit( &r,
              100 );

    for( i = 0;
         i < maxHitCount;
         i ++ ) {

        hitInCounts[ i ] = 0;
        }

    
    for( i = 0;
         i < numTrials;
         i ++ ) {

        int  c  = 1;

        rollReset( &r );

        while( ! roll( &r ) ) {
            c ++;
            }

        if( c < maxHitCount ) {
            hitInCounts[c] ++;
            }
        }

    
    for( i = 0;
         i < maxHitCount;
         i ++ ) {

        if( hitInCounts[ i ] > 0 ) {

            maxigin_logInt2( "",
                             i,
                             " ",
                             hitInCounts[ i ],
                             "" );
            }
        
        }


    }






#endif

#endif
