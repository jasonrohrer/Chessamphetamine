/*
  Include in your C code wherever like so:

      #include "nav.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define NAV_IMPLEMENTATION
      #include "nav.h"
*/


/* for navigation with keyboard and controller */

#ifndef NAV_H_INCLUDED
#define NAV_H_INCLUDED


void navInit( void );

void navStep( void );


/* gets the direction the user is navigating in.
   returns 0,0 for no navigation, which will also happen if user is holding
   a given navigation direction and the action repeat timer hasn't expired */
void navGetDir( int  *outDeltaX,
                int  *outDeltaY );



#ifdef NAV_IMPLEMENTATION

/* avoid game's mappings */
enum {
    NAV_UP = 100,
    NAV_DOWN,
    NAV_LEFT,
    NAV_RIGHT,
    NAV_STICK_VERT,
    NAV_STICK_HOR
    };


static  MinginButton  navUpMapping[]    = { MGN_BUTTON_DPAD_UP,
                                            MGN_KEY_UP,
                                            MGN_KEY_W,
                                            MGN_MAP_END };

static  MinginButton  navDownMapping[]  = { MGN_BUTTON_DPAD_DOWN,
                                            MGN_KEY_DOWN,
                                            MGN_KEY_S,
                                            MGN_MAP_END };

static  MinginButton  navLeftMapping[]  = { MGN_BUTTON_DPAD_LEFT,
                                            MGN_KEY_LEFT,
                                            MGN_KEY_A,
                                            MGN_MAP_END };

static  MinginButton  navRightMapping[] = { MGN_BUTTON_DPAD_RIGHT,
                                            MGN_KEY_RIGHT,
                                            MGN_KEY_A,
                                            MGN_MAP_END };


static  MinginStick  navStickVertMapping[] = { MGN_STICK_LEFT_Y,
                                               MGN_STICK_RIGHT_Y,
                                               MGN_MAP_END };

static  MinginStick  navStickHorMapping[]  = { MGN_STICK_LEFT_X,
                                               MGN_STICK_RIGHT_X,
                                               MGN_MAP_END };

void navInit( void ) {

    maxigin_registerButtonMapping( NAV_UP,
                                   navUpMapping );
    maxigin_registerButtonMapping( NAV_DOWN,
                                   navDownMapping );
    maxigin_registerButtonMapping( NAV_LEFT,
                                   navLeftMapping );
    maxigin_registerButtonMapping( NAV_RIGHT,
                                   navRightMapping );
    

    maxigin_registerStickAxis( NAV_STICK_VERT,
                               navStickVertMapping );
    maxigin_registerStickAxis( NAV_STICK_HOR,
                               navStickHorMapping );
    
    }


static  int  navLastXDir  =  0;
static  int  navLastYDir  =  0;


static void navGetLiveDir( int  *outDeltaX,
                           int  *outDeltaY ) {

    /* buttons before sticks */
    char  anyButtons  =  0;

    *outDeltaX = 0;
    *outDeltaY = 0;
    

    if( maxigin_isButtonDown( NAV_UP ) ) {
        *outDeltaY = -1;
        anyButtons =  1;
        }
    else if( maxigin_isButtonDown( NAV_DOWN ) ) {
        *outDeltaY = 1;
        anyButtons = 1;
        }

    if( maxigin_isButtonDown( NAV_LEFT ) ) {
        *outDeltaX = -1;
        anyButtons =  1;
        }
    else if( maxigin_isButtonDown( NAV_RIGHT ) ) {
        *outDeltaX = 1;
        anyButtons = 1;
        }

    if( !anyButtons ) {

        int   pos;
        int   min;
        int   max;
        char  avail  =  maxigin_getStickPosition( NAV_STICK_VERT,
                                                  &pos,
                                                  &min,
                                                  &max );
        if( avail ){
            /* up is negative on sticks */
            int   mid     =  ( max - min ) / 2 + min;
            int   thresh  =  ( min - mid ) / 3 + mid;
        
            if( pos <= thresh ) {
                *outDeltaY = -1;
                }
            else {
                mid    = ( max - min ) / 2 + min;
                thresh = ( max - mid ) / 3 + mid;
        
                if( pos >= thresh ) {
                    *outDeltaY = 1;
                    }
                }
            }
        

        avail  =  maxigin_getStickPosition( NAV_STICK_HOR,
                                            &pos,
                                            &min,
                                            &max );
        if( avail ){
            int   mid     =  ( max - min ) / 2 + min;
            int   thresh  =  ( min - mid ) / 3 + mid;
        
            if( pos <= thresh ) {
                *outDeltaX = -1;
                }
            else {
                mid    = ( max - min ) / 2 + min;
                thresh = ( max - mid ) / 3 + mid;
        
                if( pos >= thresh ) {
                    *outDeltaX = 1;
                    }
                }
            }
        }
    }



void navGetDir( int  *outDeltaX,
                int  *outDeltaY ) {

    int  liveX;
    int  liveY;

    navGetLiveDir( &liveX,
                   &liveY );

    if( liveX != navLastXDir
        ||
        liveY != navLastYDir ) {

        navLastXDir = liveX;
        navLastYDir = liveY;

        *outDeltaX = navLastXDir;
        *outDeltaY = navLastYDir;
        return;
        }

    /* else user is holding same direction */

    /* fixme:  repeat timer */

    *outDeltaX = 0;
    *outDeltaY = 0;
    
    }


#endif

#endif
