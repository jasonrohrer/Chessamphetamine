/*
  Include in your C code wherever like so:

      #include "cost.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define COST_IMPLEMENTATION
      #include "cost.h"

*/

#ifndef COST_H_INCLUDED
#define COST_H_INCLUDED


/*
  Examples:

  Balatro reroll cost:

  costInit( 5, 1,
            -1, -1, 0,
            0, -1, -1 0 );

*/



/*
  inBaseCost                 the starting cost value, eg.  5

  inFixedIncrement           Increase the cost by this value every time we
                             increment.  If this value was 3, our costs would be
                             5, 8, 11, 14, etc.
                             Set to 0 to not include this factor.

  inIncrementCurrentDivisor  when we increment the cost, we divide the current
                             value by this amount and add it to the current
                             value
                             If this is set to 2, for example, our cost
                             would increment by 5/2 (rounded down), so our costs
                             would be 5, 7, 10, 15, 22, etc.
                             Set to -1 to not included this factor.

  inIncrementCountDivisor    when we increment the cost, we divide the total
                             count of increments so far by this amount
                             and add it to the current value.
                             This can be used to make our cost rise one every
                             5 increments.  For example, if this is set to 5,
                             then our costs would be:
                             5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, etc.
                             Set to -1 to not include this factor.

  inIncrementIncrement       How much our fixed increment increases with
                             each increment.  For example, if inFixedIncrement
                             was 1, and this value was 1, our costs would be
                             5, 6, 8, 11, 15, 20, etc.
                             Set to 0 to not include this factor.

  inLevelFixedIncrement      fixed increment when we go up in levels
                             Set to 0 to not use.

  inLevelCurrentDivisor      divisor for the current value, added in,
                             when we go up in levels.
                             Set to -1 to not use.

  inLevelCountDivisor        divisor for the current level count
                             (we can have the cost rise every X levels by
                             setting this to X)
                             Set to -1 to not use.

  inLevelIncrementIncrement  How much level increment goes up with each level.
                             Set to 0 to not use.

  Returns  count handle
*/                       
int  costInit( int  inBaseCost,
               int  inFixedIncrement,
               int  inIncrementCurrentDivisor,
               int  inIncrementCountDivisor,
               int  inIncrementIncrement,
               int  inLevelFixedIncrement,
               int  inLevelCurrentDivisor,
               int  inLevelCountDivisor,
               int  inLevelIncrementIncrement );

int costGet( int  inCostHandle );

int costIncrement( int  inCostHandle );

int costResetIncrement( int  inCostHandle );

int costLevelIncrement( int  inCostHandle );

/* resets all counters back to 0, and cost back to base cost */
int costFullReset( int  inCostHandle );



#ifdef COST_IMPLEMENTATION


#include "memoryRegister.h"


#define  MAX_NUM_COSTS   3

typedef struct Cost{

        int  baseCost;
        int  fixedIncrement;
        int  incrementCurrentDivisor;
        int  incrementCountDivisor;
        int  incrementIncrement;
        int  levelFixedIncrement;
        int  levelCurrentDivisor;
        int  levelCountDivisor;
        int  levelIncrementIncrement;

        /* base value at current level */
        int  currentVal;
        /* value that may be temporarily incremented above base */
        int  currentIncrementedVal;

        int  incrementCount;
        int  levelRiseCount;

        int  startingIncrement;
        int  startingLevelIncrement;

    } Cost;


static  Cost  costList[ MAX_NUM_COSTS ];

static  int   numCosts                    =  0;


int  costInit( int  inBaseCost,
               int  inFixedIncrement,
               int  inIncrementCurrentDivisor,
               int  inIncrementCountDivisor,
               int  inIncrementIncrement,
               int  inLevelFixedIncrement,
               int  inLevelCurrentDivisor,
               int  inLevelCountDivisor,
               int  inLevelIncrementIncrement ) {

    int  handle;

    if( numCosts >= MAX_NUM_COSTS ) {

        mingin_log( "Too many costs initialized with costInit in cost.h\n" );
        
        return -1;
        }

    handle = numCosts;
    numCosts ++;


    costList[ handle ].baseCost                = inBaseCost;
    costList[ handle ].fixedIncrement          = inFixedIncrement;
    costList[ handle ].incrementCurrentDivisor = inIncrementCurrentDivisor;
    costList[ handle ].incrementCountDivisor   = inIncrementCountDivisor;
    costList[ handle ].incrementIncrement      = inIncrementIncrement;
    costList[ handle ].levelFixedIncrement     = inLevelFixedIncrement;
    costList[ handle ].levelCurrentDivisor     = inLevelCurrentDivisor;
    costList[ handle ].levelCountDivisor       = inLevelCountDivisor;
    costList[ handle ].levelIncrementIncrement = inLevelIncrementIncrement;

    costList[ handle ].startingIncrement       = inFixedIncrement;
    costList[ handle ].startingLevelIncrement  = inLevelIncrementIncrement;
    
    costFullReset( handle );

    REGISTER_VAL_MEM( costList[ handle ] );

    return handle;
    }



int costGet( int  inCostHandle ) {
    if( inCostHandle == -1 ) {
        return 0;
        }
    return costList[ inCostHandle ].currentIncrementedVal;
    }

    

int costIncrement( int  inCostHandle ) {

    int  curIncrVal;
    int  newIncrVal;
    
    if( inCostHandle == -1 ) {
        return 0;
        }

    curIncrVal = costList[ inCostHandle ].currentIncrementedVal;

    newIncrVal = curIncrVal;

    newIncrVal += costList[ inCostHandle ].fixedIncrement;

    if( costList[ inCostHandle ].incrementCurrentDivisor != -1 ) {
        newIncrVal +=
            curIncrVal
            / costList[ inCostHandle ].incrementCurrentDivisor;
        }

    if( costList[ inCostHandle ].incrementCountDivisor != -1 ) {
        newIncrVal +=
            costList  [ inCostHandle ].incrementCount
            / costList[ inCostHandle ].incrementCountDivisor;
        }

    costList[ inCostHandle ].incrementCount ++;
        
    costList[ inCostHandle ].fixedIncrement +=
        costList[ inCostHandle ].incrementIncrement;

    costList[ inCostHandle ].currentIncrementedVal = newIncrVal;

    return costGet( inCostHandle );
    }



int costResetIncrement( int  inCostHandle ) {
    if( inCostHandle == -1 ) {
        return 0;
        }

    costList[ inCostHandle ].currentIncrementedVal =
        costList[ inCostHandle ].currentVal;
    
    costList[ inCostHandle ].incrementCount          = 0;

    return costGet( inCostHandle );
    }



int costLevelIncrement( int  inCostHandle ) {
    
    int  curVal;
    int  curIncrVal;
    int  newVal;
    int  newIncrVal;
    
    if( inCostHandle == -1 ) {
        return 0;
        }

    curVal = costList[ inCostHandle ].currentVal;
    curIncrVal = costList[ inCostHandle ].currentIncrementedVal;

    newVal = curVal;
    newIncrVal = curIncrVal;

    newVal     += costList[ inCostHandle ].levelFixedIncrement;
    newIncrVal += costList[ inCostHandle ].levelFixedIncrement;

    if( costList[ inCostHandle ].levelCurrentDivisor != -1 ) {
        newVal     += curVal     / costList[ inCostHandle ].levelCurrentDivisor;
        newIncrVal += curIncrVal / costList[ inCostHandle ].levelCurrentDivisor;
        }

    if( costList[ inCostHandle ].levelCountDivisor != -1 ) {
        newVal     +=
            costList  [ inCostHandle ].levelRiseCount
            / costList[ inCostHandle ].levelCountDivisor;
        
        newIncrVal +=
            costList  [ inCostHandle ].levelRiseCount
            / costList[ inCostHandle ].incrementCountDivisor;
        }

    costList[ inCostHandle ].levelRiseCount ++;
    

    costList[ inCostHandle ].levelFixedIncrement +=
        costList[ inCostHandle ].levelIncrementIncrement;

    costList[ inCostHandle ].currentVal            = newVal;
    costList[ inCostHandle ].currentIncrementedVal = newIncrVal;

    return costGet( inCostHandle );
    }



/* resets all counters back to 0, and cost back to base cost */
int costFullReset( int  inCostHandle ) {
    if( inCostHandle == -1 ) {
        return 0;
        }
    
    costList[ inCostHandle ].currentVal              =
        costList[ inCostHandle ].baseCost;
    
    costList[ inCostHandle ].currentIncrementedVal =
        costList[ inCostHandle ].baseCost;
    
    costList[ inCostHandle ].incrementCount          = 0;
    
    costList[ inCostHandle ].levelRiseCount          = 0;

    costList[ inCostHandle ].fixedIncrement          =
        costList[ inCostHandle ].startingIncrement;
    
    costList[ inCostHandle ].levelFixedIncrement     =
        costList[ inCostHandle ].startingLevelIncrement;

    return costGet( inCostHandle );
    }
    


#endif
#endif
