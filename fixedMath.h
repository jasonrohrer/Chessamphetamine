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
unsigned long longSquareRoot( unsigned long  inVal );




#ifdef FIXED_MATH_IMPLEMENTATION


unsigned long longSquareRoot( unsigned long  inVal ) {

    unsigned  long  bit     =  1;
    unsigned  long  result  =  0;

    /* find the highest power of 4 <= inVal */

    bit = 1;

    while( bit <= inVal / 4 ) {
        bit = bit << 2;
        }

    /* this is the restoring square root algorithm */
    while( bit != 0 ) {

        if( inVal >= result
            &&
            inVal - result >= bit ) {

            inVal -= result;
            inVal -= bit;

            result = ( result >> 1 ) + bit;
            }
        else {
            result = result >> 1;
            }
        bit = bit >> 2;
        }
    return result;
    }


#endif
#endif
