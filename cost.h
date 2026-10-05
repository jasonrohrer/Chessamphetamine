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


/* A plateau cost is one that slows down as it grows, stopping
   for longer and longer on various plateaus on the way up.

   The above costInit can make very slowly-rising costs, but
   they are always linear or exponential.

   A plateau cost can be more in the shape of a square root function

   costPlateauInit( 1, 1, 1, 0 ) produces the sequence 1,2,2,3,3,3,4,4,4,4,...
*/
int  costPlateauInit( int  inStartingValue,
                      int  inStartingPlateauLength,
                      int  inPlateauLengthIncrement,
                      int  inPlateauLegthIncrementIncrement );


int costGet( int  inCostHandle );


/* peeks at future cost after one more increment.
   Does not modify state of cost */
int costIncrementPeek( int  inCostHandle );


int costIncrement( int  inCostHandle );

int costResetIncrement( int  inCostHandle );

int costLevelIncrement( int  inCostHandle );

/* resets all counters back to 0, and cost back to base cost */
int costFullReset( int  inCostHandle );



/* tests a cost, of 20 rolls of the cost,, up to level 100, and prints results
   does a costFullReset both before and after the test */
void costTest( int  inCostHandle );




#endif



#ifdef COST_IMPLEMENTATION

#ifndef COST_IMPLEMENTATION_INCLUDED
#define COST_IMPLEMENTATION_INCLUDED


#include "memoryRegister.h"


#define  MAX_NUM_COSTS               4

/* add this to the handle returned for plateau costs to differentiate them */
#define  PLATEAU_COST_HANDLE_OFFSET  MAX_NUM_COSTS



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



typedef struct CostPlateau{
        
        int  startingValue;
        int  startingPlateauLength;
        int  startingPlateauLengthIncrement;
        int  plateauLegthIncrementIncrement;

        int  currentValue;
        int  currentPlateauLength;
        int  currentPlateauLengthIncrement;
        int  currentPlateauProgress;
        
    } CostPlateau;



static  Cost  costList              [ MAX_NUM_COSTS ];

static  CostPlateau  costPlateauList[ MAX_NUM_COSTS ];

static  int   numCosts                    =  0;
static  int   numPlateauCosts             =  0;




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


    if( inIncrementCurrentDivisor == 0 ) {
        mingin_log(
            "Warning, inIncrementCurrentDivisor can't be 0 in costInit\n" );
        inIncrementCurrentDivisor = -1;
        }
    if( inIncrementCountDivisor == 0 ) {
        mingin_log(
            "Warning, inIncrementCountDivisor can't be 0 in costInit\n" );
        inIncrementCountDivisor = -1;
        }

    if( inLevelCurrentDivisor == 0 ) {
        mingin_log(
            "Warning, inLevelCurrentDivisor can't be 0 in costInit\n" );
        inLevelCurrentDivisor = -1;
        }
    if( inLevelCountDivisor == 0 ) {
        mingin_log(
            "Warning, inLevelCountDivisor can't be 0 in costInit\n" );
        inLevelCountDivisor = -1;
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
    costList[ handle ].startingLevelIncrement  = inLevelFixedIncrement;
    
    costFullReset( handle );

    REGISTER_VAL_MEM( costList[ handle ] );

    return handle;
    }



int  costPlateauInit( int  inStartingValue,
                      int  inStartingPlateauLength,
                      int  inStartingPlateauLengthIncrement,
                      int  inPlateauLegthIncrementIncrement ) {
    int  handle;

    if( numPlateauCosts >= MAX_NUM_COSTS ) {

        mingin_log( "Too many plateau costs initialized with "
                    "costPlateauInit in cost.h\n" );
        
        return -1;
        }

    handle = numPlateauCosts;
    numPlateauCosts ++;

    costPlateauList[ handle ].startingValue                  =
                                  inStartingValue;
    
    costPlateauList[ handle ].startingPlateauLength          =
                                  inStartingPlateauLength;
    
    costPlateauList[ handle ].startingPlateauLengthIncrement  =
                                  inStartingPlateauLengthIncrement;
    
    costPlateauList[ handle ].plateauLegthIncrementIncrement =
                                  inPlateauLegthIncrementIncrement;

    costPlateauList[ handle ].currentValue           =  inStartingValue;
    costPlateauList[ handle ].currentPlateauLength   =  inStartingPlateauLength;
    costPlateauList[ handle ].currentPlateauLengthIncrement  =
                                  inStartingPlateauLengthIncrement;
    
    costPlateauList[ handle ].currentPlateauProgress = 0;
    

    REGISTER_VAL_MEM( costPlateauList[ handle ] );

    return handle + PLATEAU_COST_HANDLE_OFFSET;
    }



int costGet( int  inCostHandle ) {

    int  val;
    
    if( inCostHandle == -1 ) {
        return 0;
        }

    if( inCostHandle >= PLATEAU_COST_HANDLE_OFFSET ) {

        inCostHandle -= PLATEAU_COST_HANDLE_OFFSET;

        return costPlateauList[ inCostHandle ].currentValue;
        }
    
    val = costList[ inCostHandle ].currentIncrementedVal;

    if( costList[ inCostHandle ].incrementCountDivisor != -1 ) {
        val +=
            costList  [ inCostHandle ].incrementCount
            / costList[ inCostHandle ].incrementCountDivisor;
        }

    return val;
    }


int costIncrementPeek( int  inCostHandle ) {

    int  peekedVal;

    if( inCostHandle == -1 ) {
        return 0;
        }

    if( inCostHandle >= PLATEAU_COST_HANDLE_OFFSET ) {

        static  CostPlateau  temp;

        inCostHandle -= PLATEAU_COST_HANDLE_OFFSET;

        temp = costPlateauList[ inCostHandle ];

        peekedVal = costIncrement( inCostHandle + PLATEAU_COST_HANDLE_OFFSET );

        /* restore */
        costPlateauList[ inCostHandle ] = temp;
        }
    else {
        static  Cost  temp;

        temp = costList[ inCostHandle ];

        peekedVal = costIncrement( inCostHandle );

        /* restore */
        costList[ inCostHandle ] = temp;
        }
    

    return peekedVal;  
    }


    

int costIncrement( int  inCostHandle ) {

    int  curIncrVal;
    int  newIncrVal;
    
    if( inCostHandle == -1 ) {
        return 0;
        }

    if( inCostHandle >= PLATEAU_COST_HANDLE_OFFSET ) {

        int  h  =  inCostHandle - PLATEAU_COST_HANDLE_OFFSET;

        costPlateauList[ h ].currentPlateauProgress ++;

        if( costPlateauList[ h ].currentPlateauProgress >=
            costPlateauList[ h ].currentPlateauLength ) {

            /* done with this step */
            costPlateauList[ h ].currentValue ++;

            costPlateauList[ h ].currentPlateauProgress = 0;
            
            costPlateauList[ h ].currentPlateauLength +=
                costPlateauList[ h ].currentPlateauLengthIncrement;
            
            costPlateauList[ h ].currentPlateauLengthIncrement +=
                costPlateauList[ h ].plateauLegthIncrementIncrement;
            
            }
        
        return costGet( inCostHandle );
        }

    curIncrVal = costList[ inCostHandle ].currentIncrementedVal;

    newIncrVal = curIncrVal;

    newIncrVal += costList[ inCostHandle ].fixedIncrement;

    if( costList[ inCostHandle ].incrementCurrentDivisor != -1 ) {
        newIncrVal +=
            curIncrVal
            / costList[ inCostHandle ].incrementCurrentDivisor;
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

    if( inCostHandle >= PLATEAU_COST_HANDLE_OFFSET ) {

        int  handle  =  inCostHandle - PLATEAU_COST_HANDLE_OFFSET;

        costPlateauList[ handle ].currentValue                =
            costPlateauList[ handle ].startingValue;
        
        costPlateauList[ handle ].currentPlateauLength        =
            costPlateauList[ handle ].startingPlateauLength;
        
        costPlateauList[ handle ].currentPlateauProgress      =  0;
        
        costPlateauList[ handle ].currentPlateauLengthIncrement =
            costPlateauList[ handle ].startingPlateauLengthIncrement;

        return costGet( inCostHandle );
        }

    costList[ inCostHandle ].currentIncrementedVal =
        costList[ inCostHandle ].currentVal;
    
    costList[ inCostHandle ].incrementCount        = 0;
    
    costList[ inCostHandle ].fixedIncrement        =
        costList[ inCostHandle ].startingIncrement;

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

    if( inCostHandle >= PLATEAU_COST_HANDLE_OFFSET ) {
        /* plateaus have no level increment */
        return costGet( inCostHandle );
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


    costList[ inCostHandle ].levelRiseCount ++;

    if( costList[ inCostHandle ].levelCountDivisor != -1 ) {
        newIncrVal +=
            costList  [ inCostHandle ].levelRiseCount
            / costList[ inCostHandle ].levelCountDivisor;
        }

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

    if( inCostHandle >= PLATEAU_COST_HANDLE_OFFSET ) {

        costResetIncrement( inCostHandle );
        
        inCostHandle -= PLATEAU_COST_HANDLE_OFFSET;

        /* fixme */
        return costGet( inCostHandle );
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



void costTest( int  inCostHandle ) {

    int  level;
    int  i;

    costFullReset( inCostHandle );

    costResetIncrement( inCostHandle );


    for( level = 0;
         level < 100;
         level   ++ ) {

        maxigin_logInt( "Level: ",
                        level );

        if( level == 15 ) {
            mingin_log( "hey\n" );
            }

        for( i = 0;
             i < 20;
             i   ++ ) {

            int  c  =  costGet( inCostHandle );
            
            maxigin_logInt2( "",
                             i,
                             ": ",
                             c,
                             "" );
    
            costIncrement( inCostHandle );
            }

        if( level == 14 ) {
            mingin_log( "hey\n" );
            }
        costResetIncrement( inCostHandle );
        
        costLevelIncrement( inCostHandle );
        }
    

    costFullReset( inCostHandle );
    
    }

    


#endif

#endif
