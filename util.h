/*
  Include in your C code wherever like so:

      #include "util.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define UTIL_IMPLEMENTATION
      #include "util.h"

*/

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED



/* a parabola that varies over inT in [0..inTMax] that peaks at inPeak */
int parabola( int  inT,
              int  inTMax,
              int  inPeak );


/* tweens current value to target value with step size, returns new value */
unsigned char tweenToByte( unsigned char  inCurrentVal,
                           unsigned char  inTargetVal,
                           int            inStepSize );



#ifdef UTIL_IMPLEMENTATION



int parabola( int  inT,
              int  inTMax,
              int  inPeak ) {
    /* float formula is
       y = 4 * inPeak * ( inT / inTMax ) * ( 1 - inT / inTMax )

       rewritten with division at end for fixed point as:

       y = 4 * inPeak * (inT * (inTMax - inT ) ) / ( inTMax * inTMax )

    */
    
    long  tMax2  =  (long)inTMax * (long)inTMax;
    long  t      =  (long)inT;
    long  y      =  4 * inPeak * ( t * (inTMax - t ) );

    return (int)( y / tMax2 );
    }



unsigned char tweenToByte( unsigned char  inCurrentVal,
                           unsigned char  inTargetVal,
                           int            inStepSize ) {

    int  newVal  =  inCurrentVal;
    
    if( newVal < inTargetVal ) {

        if( newVal < inTargetVal - inStepSize ) {
            newVal += inStepSize;
            }
        else {
            newVal = inTargetVal;
            }
        }
    else if( newVal > inTargetVal ) {

        if( newVal > inTargetVal + inStepSize ) {
            newVal -= inStepSize;
            }
        else {
            newVal = inTargetVal;
            }
        }

    return (unsigned char)newVal;
    }



#endif

#endif
