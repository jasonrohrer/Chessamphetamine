/*
  Include in your C code wherever like so:

      #include "speed.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define SPEED_IMPLEMENTATION
      #include "speed.h"

*/

#ifndef SPEED_H_INCLUDED
#define SPEED_H_INCLUDED

void speedInit( void );


int speedGetStepsPerSecond( void );


/* 1 means no slow down, 2 means twice as slow, 3 means 3x slower, etc. */
void speedSetSlowFactor( int  inFactor );



#endif



#ifdef  SPEED_IMPLEMENTATION

#ifndef SPEED_IMPLEMENTATION_INCLUDED
#define SPEED_IMPLEMENTATION_INCLUDED


#include "memoryRegister.h"


static  int  speedSlowDownFactor  =  1;


void speedInit( void ) {
    REGISTER_VAL_MEM( speedSlowDownFactor );
    }



int speedGetStepsPerSecond( void ) {
    return mingin_getStepsPerSecond() * speedSlowDownFactor;
    }



void speedSetSlowFactor( int  inFactor ) {
    speedSlowDownFactor = inFactor;
    }



#endif



#endif

