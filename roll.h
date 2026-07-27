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


void rollInit( void );


typedef struct RollInfo {
        
        int  baseOneInCount;
        
        int  currentOneInCount;

        int  missCount;

        int  flatnessNumerator;
        int  flatnessDenominator;
        
    } RollInfo;


/*
  initializes a roll that will happen, on average, once in inOneInCount rolls

  Fraction defined by ( inFlatnessNumerator/inFlatnessDenominator )
  determines flattening effect, where smaller values rein in long miss
  streaks sooner, and larger values allow longer miss streaks

  Passing 1/1 for these make bad luck mitigation kick in after
  inOneInCount misses.

  Passing 2/1 doesn't mitigate until (2 * inOneInCount) misses.

  Passing 1/2 mitigates after (inOneInCount / 2)  misses
*/
void rollSetup( RollInfo  *inRoll,
                int        inOneInCount,
                int        inFlatnessNumerator,
                int        inFlatnessDenominator );


/* returns 1 on hit, 0 on miss */
char roll( RollInfo  *inRoll );

void rollReset( RollInfo  *inRoll );


void rollTest( void );


/* initialize a roll pool of items that are all equally likely.

   returns handle to pool
 */
int rollPoolSetup( int  inNumItems,
                   int  inItems[],
                   int  inFlatnessNumerator,
                   int  inFlatnessDenominator );

int rollItem( int  inRollPoolHandle );

void rollPoolReset( int  inRollPoolHandle );


void rollPoolTest( void );



#ifdef ROLL_IMPLEMENTATION


static  MaxiginRand  rollRand;


#define  ROLL_MAX_TOTAL_POOL_ITEMS  128

#define  ROLL_MAX_POOLS              16


static  int  rollPoolItems            [ ROLL_MAX_TOTAL_POOL_ITEMS ];
static  int  rollPoolMissCounts       [ ROLL_MAX_TOTAL_POOL_ITEMS ];
static  int  rollPoolMissCountTriggers[ ROLL_MAX_TOTAL_POOL_ITEMS ];
static  int  rollPoolWeights          [ ROLL_MAX_TOTAL_POOL_ITEMS ];


static  int  rollPoolNumTotalItems  =  0;


typedef struct RollPool {

        int  numItems;

        /* pointer into rollPoolItems */
        int  *items;

        /* pointer into rollPoolMissCounts */
        int  *missCounts;

        /* pointer into rollPoolMissCountTriggers */
        int  *missCountTriggers;

        int  *weights;

        int  totalWeight;

        int  flatnessNumerator;
        int  flatnessDenominator;
        
    } RollPool;



static  RollPool  rollPools[ ROLL_MAX_POOLS ];

static  int       rollNumPools  =  0;



void rollInit( void ) {

    int  i;
    
    maxigin_randSeed( &rollRand,
                      mingin_getEntropySeed() );

    for( i = 0;
         i < ROLL_MAX_TOTAL_POOL_ITEMS;
         i ++ ) {

        rollPoolItems            [ i ] = -1;
        rollPoolMissCounts       [ i ] = 0;
        rollPoolMissCountTriggers[ i ] = 1;
        rollPoolWeights          [ i ] = 1;
        }
    
    REGISTER_VAL_MEM( rollRand );

    REGISTER_ARRAY_MEM( rollPoolItems      );
    REGISTER_ARRAY_MEM( rollPoolMissCounts );
    REGISTER_ARRAY_MEM( rollPoolMissCountTriggers );
    REGISTER_ARRAY_MEM( rollPoolWeights    );
    }



void rollSetup( RollInfo  *inRoll,
                int        inOneInCount,
                int        inFlatnessNumerator,
                int        inFlatnessDenominator ) {

    
    inRoll->baseOneInCount      = inOneInCount;
    inRoll->flatnessNumerator   = inFlatnessNumerator;
    inRoll->flatnessDenominator = inFlatnessDenominator;

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

        if(1)
        if( inRoll->missCount
            >=
            ( inRoll->flatnessNumerator
              * inRoll->currentOneInCount )
            / inRoll->flatnessDenominator ) {
            
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

    int       hitInCounts[ 2000 ];
    int       i;
    int       numTrials             =  1000000;
    int       maxHitCount           =  2000;
    int       sum                   =  0;
    int       worst                 =  0;
    
    rollSetup( &r,
               100,
               1,
               1 );

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
        if( c > worst ) {
            worst = c;
            }
        }
    
    
    for( i = 0;
         i < maxHitCount;
         i ++ ) {

        sum += hitInCounts[ i ];

       
        
        if( hitInCounts[ i ] > 0 ) {

            if( 0)
            maxigin_logInt( "Percent: ",
                            ( sum * 100 ) / numTrials );
             
            maxigin_logInt2( "",
                             i,
                             " ",
                             hitInCounts[ i ],
                             "" );
            }
        
        }
    }



int  rollPoolSetup( int  inNumItems,
                    int  inItems[],
                    int  inFlatnessNumerator,
                    int  inFlatnessDenominator ) {

    int        handle  =  rollNumPools;
    RollPool  *p;
    int        i;
    
    if( rollNumPools
        >=
        ROLL_MAX_POOLS ) {
        
        mingin_log( "Already too many roll pools to add more in "
                    "rollPoolSetup in roll.h\n" );
        
        return -1;
        }

    
    if( inNumItems + rollPoolNumTotalItems
        >
        ROLL_MAX_TOTAL_POOL_ITEMS ) {

        mingin_log( "Already too many roll pool items to add more in "
                    "rollPoolSetup in roll.h\n" );

        return -1;
        }

    p = &( rollPools[ handle ] );

    rollNumPools++;

    p->numItems    = inNumItems;
    p->items       = &( rollPoolItems            [ rollPoolNumTotalItems ] );
    p->missCounts  = &( rollPoolMissCounts       [ rollPoolNumTotalItems ] );
    p->missCountTriggers
                   = &( rollPoolMissCountTriggers[ rollPoolNumTotalItems ] );
    p->weights     = &( rollPoolWeights          [ rollPoolNumTotalItems ] );

    
    
    p->totalWeight         = inNumItems;
    p->flatnessNumerator   = inFlatnessNumerator;
    p->flatnessDenominator = inFlatnessDenominator;
    
    for( i = 0;
         i < inNumItems;
         i ++ ) {

        rollPoolItems[ rollPoolNumTotalItems ] = inItems[ i ];

        rollPoolNumTotalItems ++;
        }
    
    rollPoolReset( handle );
    
    return handle;
    }


/*
static void checkWeight( RollPool *p ) {
    
    int  i;
    int  checkWeight = 0;
    
    for( i = 0;
         i < p->numItems;
         i ++ ) {
        checkWeight += p->weights[ i ];
        }

    if( p->totalWeight != checkWeight ) {
        mingin_log( "Hey\n" );
        }
    }
*/     

int  rollItem( int  inRollPoolHandle ) {
    
    RollPool  *p;
    int        weightPick;
    int        i;
    int        cumulativeWeight  =   0;
    int        pick              =  -1;
    
    
    if( inRollPoolHandle == -1 ) {
        return 0;
        }

    p = &( rollPools[ inRollPoolHandle ] );

    weightPick = maxigin_randRange( &rollRand,
                                    1,
                                    p->totalWeight );
    
    for( i = 0;
         i < p->numItems;
         i ++ ) {

        cumulativeWeight += p->weights[i];

        if( pick == -1
            &&
            cumulativeWeight >= weightPick ) {

            pick = i;
            
            }
        else {
            p->missCounts[i] ++;
            }
        }

    
    /* reset count and weight of hit item */
    p->missCounts[ pick ]         =  0;
    p->missCountTriggers[ pick ]  =  p->numItems;
    p->totalWeight               -=  p->weights[ pick ];
    p->weights   [ pick ]         =  1;
    p->totalWeight               +=  1;

    
    /* now handle any that have missed the expected number of times */
    if( 1 )
    for( i = 0;
         i < p->numItems;
         i ++ ) {

        if( p->missCounts[ i ]
            >=
            ( p->flatnessNumerator * p->missCountTriggers[ i ] )
            / p->flatnessDenominator
            &&
            p->missCountTriggers[ i ] > 2 ) {
            
            /* we've gone the expected number of trials for this item
               given its share of the total weight */

            /* want half the expected rolls before hit of what we
               had before */
            int  newTrigger      =  p->missCountTriggers[ i ] / 2;
            
            if( newTrigger < 2 ) {
                /* don't ever go below a 1/2 chance of hitting
                   never a sure thing */
                newTrigger = 2;
                }

            /* If we compute true new weight for item so that it
               has newTrigger expected rolls before hit, taking account
               of weight of all other items, the weight can blow up
               as missed items chase each other, competing for weight.

               Also, we can't double the weight of a missed item,
               because in large pools with a large group of missed items,
               it could take a long time to roll all of the missed ones,
               even if they collectively dominate the weight of the pool.

               In that case, they may double dozens of times, resulting
               in int overflow.

               Instead, just give our item an appropriate fraction
               of the base weight of the pool
            */
            
            p->totalWeight   -=  p->weights[ i ];

            p->weights[ i ]   =  p->numItems / newTrigger;
            
            p->totalWeight   +=  p->weights[ i ];

            /* reset the miss count, so we can start counting again
               until we reach the new, smaller expected miss count going forward
            */
            p->missCounts[ i ] = 0;

            /* remember the new, smaller miss count trigger */
            p->missCountTriggers[ i ] = newTrigger;
            }
        }



    return p->items[ pick ];
    }



void  rollPoolReset( int  inRollPoolHandle ) {

    RollPool  *p;
    int        i;
    
    if( inRollPoolHandle == -1 ) {
        return;
        }

    p = &( rollPools[ inRollPoolHandle ] );


    for( i = 0;
         i < p->numItems;
         i ++ ) {

        p->missCounts       [ i ] = 0;
        p->missCountTriggers[ i ] = p->numItems;
        p->weights          [ i ] = 1;
        }
    p->totalWeight = p->numItems;
    }



void rollPoolTest( void ) {

    int       items[ 100 ];
    int       hitInCounts[ 5000 ];
    int       i;
    int       j;
    int       numTrials             =  100000;
    int       numItems              =  20;
    int       maxHitCount           =  5000;
    
    int  poolHandle;

    for( i = 0;
         i < numItems;
         i ++ ) {
        items[ i ] = i;
        }

    poolHandle = rollPoolSetup( numItems,
                                items,
                                1,
                                1 );

    for( i = 0;
         i < maxHitCount;
         i ++ ) {

        hitInCounts[ i ] = 0;
        }

    for( i = 0;
         i < 40;
         i ++ ) {

        int  item  =  rollItem( poolHandle );
        
        mingin_log( maxigin_intToString( item ) );
        mingin_log( " " );

        hitInCounts[ item ] ++;
        }

    mingin_log( "\n\n" );

    
    for( i = 0;
         i < numItems;
         i ++ ) {

        maxigin_logInt2( "",
                         i,
                         " ",
                         hitInCounts[ i ],
                         "" );
        }
    mingin_log( "\n\n" );

    
    return;

    for( i = 0;
         i < maxHitCount;
         i ++ ) {

        hitInCounts[ i ] = 0;
        }

    
    for( i = 0;
         i < numTrials;
         i ++ ) {

        char  itemsHit[ 100 ];
        char  allHit            =  0;
        int   c                 =  0;

        for( j = 0;
             j < numItems;
             j ++ ) {
            itemsHit[ j ] = 0;
            }
        
        rollPoolReset( poolHandle );

        while( ! allHit ) {

            int  item  =  rollItem( poolHandle );

            if( ! itemsHit[ item ] ) {

                itemsHit[ item ] = 1;

                allHit = 1;
                for( j = 0;
                     j < numItems;
                     j ++ ) {
                    if( ! itemsHit[ j ] ) {
                        allHit = 0;
                        break;
                        }
                    }
                }
            
            c ++;
            }

        if( c < maxHitCount ) {
            hitInCounts[c] ++;
            }
        }


    mingin_log( "Roll pool test results:\n" );
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
