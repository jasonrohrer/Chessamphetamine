/*
  Include in your C code wherever like so:

      #include "fixedMath.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define FIXED_MATH_IMPLEMENTATION
      #include "fixedMath.h"

*/

#ifndef FIXED_MATH_H_INCLUDED
#define FIXED_MATH_H_INCLUDED


/* computes the floored square root of inVal */
long longSquareRoot( long  inVal );




#ifdef FIXED_MATH_IMPLEMENTATION


long longSquareRoot( long  inVal ) {

    long startVal = 1;

    if( inVal > 10000 ) {
        startVal = 100;
        }
    else if( inVal > 5000 ) {
        startVal = 70;
        }
    else if( inVal > 1000 ) {
        startVal = 31;
        }
    else if( inVal > 100 ) {
        startVal = 10;
        }

    while( startVal * startVal <= inVal ) {
        startVal ++;
        }

    return startVal - 1;
    }


#endif
#endif
