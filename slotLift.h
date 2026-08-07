/*
  Include in your C code wherever like so:

      #include "slotLift.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define SLOT_LIFT_IMPLEMENTATION
      #include "slotLift.h"

*/

#ifndef SLOT_LIFT_H_INCLUDED
#define SLOT_LIFT_H_INCLUDED


/* returns 1 if done with phase (either lifting or dropping, if either
   is 1), or if no phase selected (both are 0).

   returns 0 if phase still running
*/
char slotLiftStep( char  inLifting,
                   char  inDropping,
                   int   inMaxLift,
                   int   inNumSlots,
                   int   inLift[],
                   int   inSmoothLift[],
                   int   inPieceHitSound  );


#endif



#ifdef  SLOT_LIFT_IMPLEMENTATION

#ifndef SLOT_LIFT_IMPLEMENTATION_INCLUDED
#define SLOT_LIFT_IMPLEMENTATION_INCLUDED




char slotLiftStep( char  inLifting,
                   char  inDropping,
                   int   inMaxLift,
                   int   inNumSlots,
                   int   inLift[],
                   int   inSmoothLift[],
                   int   inPieceHitSound ) {

    char  phaseDone  =  1;
    

    if( inLifting
        ||
        inDropping ) {

        int  i;
        int  r                     =  mingin_getStepsPerSecond();
        
        int  scaleFactor           =  ( inMaxLift * inMaxLift )
                                      / MAXIGIN_GAME_NATIVE_H;
        
        int   minLiftForNextStart  =  30;
        int   stepSize             =  ( 4 * 60 ) / r;

        if( inLifting ) {

            for( i = inNumSlots - 1;
                 i >= 0;
                 i -- ) {
                if( inLift[i] == 0 ) {
                    /* skip sound for first one
                       since button press makes sound */
                    if( i != inNumSlots - 1 ) {
                        maxigin_playSoundEffect( inPieceHitSound,
                                                 256 );
                        }
                    }
                if( inLift[i] < inMaxLift ) {

                    phaseDone = 0;
                    
                    inLift[i] += stepSize;

                    if( inLift[i] > inMaxLift ) {
                        inLift[i] = inMaxLift;
                        }

                    inSmoothLift[i] = ( inLift[i] * inLift[i] ) / scaleFactor;

                    if( inLift[i] < minLiftForNextStart ) {
                        /* stagger lift */
                        break;
                        }
                    }
                }
            }
        else if( inDropping ) {
        
            for( i = inNumSlots - 1;
                 i >= 0;
                 i -- ) {
            
                if( inLift[i] > 0  ) {

                    phaseDone = 0;
                    
                    inLift[i] -= stepSize;

                    if( inLift[i] <= 0  ) {
                        inLift[i] = 0;
                        maxigin_playSoundEffect( inPieceHitSound,
                                                 256 );
                        }

                    inSmoothLift[i] = ( inLift[i] * inLift[i] ) / scaleFactor;

                    if( inLift[i] > inMaxLift - minLiftForNextStart ) {
                        /* stagger drop */
                        break;
                        }
                    }
                }
            }
        }
    
    return phaseDone;
    }


#endif



#endif
