/*
  Include in your C code wherever like so:

      #include "unlocks.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define UNLOCKS_IMPLEMENTATION
      #include "unlocks.h"

*/

#ifndef UNLOCKS_H_INCLUDED
#define UNLOCKS_H_INCLUDED


/* handle for action controller to toggle through unlocks
   for info */
void unlocksInit( int  inViewActionHandle,
                  int  inViewSound );


/* register that player checkmated a level.
   Checkmating certain levels may unlock things */
void unlocksBeatLevel( int  inLevel );



void unlocksDraw( int  inPosX,
                  int  inPosY );


void unlocksStep( int  inPosX,
                  int  inPosY  );


int unlocksGetExtraSideboardSlots( void );

int unlocksGetExtraShopSlots( void );

int unlocksGetMinNumSaleSlots( void );


void unlocksCancelViewer( void );

char unlocksIsViewerActive( void );


#endif



#ifdef  UNLOCKS_IMPLEMENTATION

#ifndef UNLOCKS_IMPLEMENTATION_INCLUDED
#define UNLOCKS_IMPLEMENTATION_INCLUDED


#define  NUM_UNLOCKS  3

static  char           unlockEnabled      [ NUM_UNLOCKS ];

static  int            unlockPosX         [ NUM_UNLOCKS ];
static  int            unlockPosY         [ NUM_UNLOCKS ];

static  int            unlockSprites      [ NUM_UNLOCKS ];

static  unsigned char  unlockExtraGlowFade[ NUM_UNLOCKS ];

static  int            unlockAtLevel      [ NUM_UNLOCKS ];

static  int            unlockFrameSprite;
static  int            unlockGlintSprite;
static  int            unlockViewerSprite;

static  int            unlockSound                           =  -1;
static  int            unlockLastOver                        =  -1;
static  int            unlockViewActionHandle;
static  int            unlockViewSound;

static  char           unlockViewDown                        =   0;
static  char           unlockViewerCanceled                  =   0;

static  int            lang_unlockDes;
static  int            lang_unlocks       [ NUM_UNLOCKS ];
static  int            lang_lockedA;
static  int            lang_lockedB;


static  const char     hashMessage[]   =  "you are only cheating yourself";


/* gets the correct hex hash for an unlock string ending with a space */
static const char *getUnlockStringHash(  const char  *inUnlockString ) {

    const char  *stringToHash = maxigin_stringConcat( inUnlockString,
                                                      hashMessage );
    
    return maxigin_stringHash8Byte( stringToHash );
    }



static void unlocksWrite( void ) {

    static  char  s[ NUM_UNLOCKS + 2 ];

    int          i;
    const char  *hexHash;
    int          file;
    
    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {

        if( unlockEnabled[ i ] ) {
            s[ i ] = '1';
            }
        else {
            s[ i ] = '0';
            }
        }
    s[ NUM_UNLOCKS ] = ' ';
    s[ NUM_UNLOCKS  + 1 ] = '\0';

    hexHash = getUnlockStringHash( s );

    file = mingin_startWritePersistData( "unlocks.ini" );

    if( file != -1 ) {

        char  success  =  mingin_writePersistData( file,
                                                   /* include space */
                                                   NUM_UNLOCKS + 1,
                                                   (unsigned char *)s );
        if( success ) {
            int  hexLen  =  maxigin_stringLength( hexHash );

            success = mingin_writePersistData( file,
                                               hexLen,
                                               (unsigned char *)hexHash );
            }
        mingin_endWritePersistData( file );
        }
    }


static void unlocksRead( void ) {

    static  char  s       [ NUM_UNLOCKS + 2 ];
    static  char  readHash[ 16 + 1 ];
    
    int          i;
    const char  *correctHexHash;
    int          file;
    int          numBytes;
    int          numRead;
    
    file = mingin_startReadPersistData( "unlocks.ini",
                                        &numBytes );

    if( file == -1 ) {
        return;
        }
    
    if( numBytes != NUM_UNLOCKS + 1 + 16 ) {
        mingin_endReadPersistData( file );
        return;
        }
    
    numRead = mingin_readPersistData( file,
                                      NUM_UNLOCKS + 1,
                                      (unsigned char*)s );

    if( numRead != NUM_UNLOCKS + 1 ) {
        mingin_endReadPersistData( file );
        return;
        }

    if( s[ NUM_UNLOCKS ] != ' ' ) {
        mingin_endReadPersistData( file );
        return;
        }
    s[ NUM_UNLOCKS + 1 ] = '\0';

    correctHexHash = getUnlockStringHash( s );

    numRead = mingin_readPersistData( file,
                                      16,
                                      (unsigned char*)readHash );

    mingin_endReadPersistData( file );

    if( numRead != 16 ) {
        return;
        }
    readHash[ 16 ] = '\0';

    if( ! maxigin_stringsEqual( correctHexHash,
                                readHash ) ) {
        return;
        }

    /* hash matched!  Load unlocks */
    
    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {

        if( s[ i ] == '1' ) {
            unlockEnabled[ i ] = 1;
            }
        else {
            unlockEnabled[ i ] = 0;
            }
        }
    }



void unlocksInit( int  inViewActionHandle,
                  int  inViewSound ) {

    int  i;

    int  offsetX  =  15;
    int  curX     =  - ( ( NUM_UNLOCKS - 1 ) * offsetX ) / 2;

    unlockViewActionHandle = inViewActionHandle;
    unlockViewSound        = inViewSound;

    unlockFrameSprite = maxigin_initSprite( "unlockFrame.tga" );
    unlockGlintSprite = maxigin_initSprite( "unlockGlint.tga" );

    unlockSprites[0] = maxigin_initSprite( "unlockSideboard.tga" );
    unlockSprites[1] = maxigin_initSprite( "unlockShopSlot.tga"  );
    unlockSprites[2] = maxigin_initSprite( "unlockSale.tga"      );

    unlockViewerSprite = maxigin_initSprite( "unlockViewer.tga" );
    

    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {
        maxigin_initMakeGlowSprite( unlockSprites[ i ],
                                    4,
                                    2 );
        unlockPosX[ i ] = curX;
        unlockPosY[ i ] = 0;

        curX += offsetX;

        unlockEnabled[ i ] = 0;

        unlockExtraGlowFade[ i ] = 0;
        }

    unlockAtLevel[ 0 ] = 8;
    unlockAtLevel[ 1 ] = 16;
    unlockAtLevel[ 2 ] = 32;

    unlockSound = maxigin_initSoundEffect( "unlock_sd_24.wav" );


    lang_unlockDes    = maxigin_initTranslationKey( "unlockDes" );
    lang_unlocks[ 0 ] = maxigin_initTranslationKey( "unlock0"   );
    lang_unlocks[ 1 ] = maxigin_initTranslationKey( "unlock1"   );
    lang_unlocks[ 2 ] = maxigin_initTranslationKey( "unlock2"   );

    lang_lockedA      = maxigin_initTranslationKey( "unlockLockedA" );
    lang_lockedB      = maxigin_initTranslationKey( "unlockLockedB" );
                
    unlocksRead();

    REGISTER_ARRAY_MEM( unlockEnabled       );
    REGISTER_ARRAY_MEM( unlockExtraGlowFade );
    REGISTER_VAL_MEM( unlockLastOver );

    REGISTER_VAL_MEM( unlockViewDown );

    REGISTER_VAL_MEM( unlockViewerCanceled );
    }


void unlocksDraw( int  inPosX,
                  int  inPosY ) {

    int  i;
    int  j;
    int  glowRep  =  3;
    

    maxigin_drawResetColor();

    maxigin_drawButtonHintSprite( unlockViewActionHandle,
                                  inPosX + unlockPosX[ 0 ] - 13,
                                  inPosY + unlockPosY[ 0 ] );
    
    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {
        int  x  =  unlockPosX[ i ] + inPosX;
        int  y  =  unlockPosY[ i ] + inPosY;

        if( i == unlockLastOver
            &&
            unlockExtraGlowFade[ i ] == 255 ) {
            maxigin_drawSprite( unlockViewerSprite,
                                x,
                                y - 7 );
            }
        
        maxigin_drawSprite( unlockFrameSprite,
                            x,
                            y );
        
        if( ! unlockEnabled[ i ] ) {

            maxigin_drawSprite( unlockGlintSprite,
                                x,
                                y );
            }
        else {
            maxigin_drawSprite( unlockSprites[ i ],
                                x,
                                y );

            
            
            for( j = 0;
                 j < glowRep - 1;
                 j ++ ) {
                maxigin_drawSpriteGlowOnly( unlockSprites[ i ],
                                            x,
                                            y );
                }

            if( unlockExtraGlowFade[ i ] > 0 ) {
                maxigin_drawSetAlpha( unlockExtraGlowFade[ i ] );
                
                for( j = 0;
                     j < glowRep - 1;
                     j ++ ) {
                    maxigin_drawSpriteGlowOnly( unlockSprites[ i ],
                                                x,
                                                y );
                    }
                
                maxigin_drawResetColor();
                }
            }
        }

    if( unlockLastOver != -1
        &&
        unlockExtraGlowFade[ unlockLastOver ] > 0 ) {

        int  centX  =  MAXIGIN_GAME_NATIVE_W - 42;
        int  centY  =  MAXIGIN_GAME_NATIVE_H / 2;

        
        if( unlockEnabled[ unlockLastOver ] ) {

            drawDescriptionText( lang_unlockDes,
                                 lang_unlocks[ unlockLastOver ],
                                 centX,
                                 centY,
                                 unlockExtraGlowFade[ unlockLastOver ] );

            maxigin_drawSetColor( 70,
                                  198,
                                  87,
                                  unlockExtraGlowFade[ unlockLastOver ] );
            
            drawDescriptionFrame( centX,
                                  centY );
            }
        else {

            maxigin_drawResetColor();

            maxigin_setLanguageFontIndex( 1 );

            maxigin_drawSetAlpha( unlockExtraGlowFade[ unlockLastOver ] );

            maxigin_drawLangText( lang_lockedA,
                                  centX,
                                  centY - 15,
                                  MAXIGIN_CENTER );

            numberDrawText(
                maxigin_stringConcat(
                    maxigin_intToString( unlockAtLevel[ unlockLastOver ] ),
                    "+" ),
                centX,
                centY,
                0,
                MAXIGIN_CENTER );

            maxigin_setLanguageFontIndex( 1 );

            maxigin_drawLangText( lang_lockedB,
                                  centX,
                                  centY + 15,
                                  MAXIGIN_CENTER );
            
            maxigin_setLanguageFontIndex( 0 );
            }
        
        }
    }



void unlocksStep( int  inPosX,
                  int  inPosY  ) {

    int  i;

    int  pointerX;
    int  pointerY;
    int  w;
    int  h;

    int  overI      =  -1;
    int  r          =  mingin_getStepsPerSecond();
    int  deltaFade  =  ( 20 * 60 ) / r;

    maxigin_getSpriteDimensions( unlockFrameSprite,
                                 &w,
                                 &h );

    w /= 2;
    h /= 2;

    if( maxigin_getPointerLocation( &pointerX,
                                    &pointerY ) ) {
        
        for( i = 0;
             i < NUM_UNLOCKS;
             i ++ ) {
            
            int  x   =  unlockPosX[ i ] + inPosX;
            int  y   =  unlockPosY[ i ] + inPosY;

            int  dx  =  pointerX - x;
            int  dy  =  pointerY - y;

            if( dx >= -w
                &&
                dx <=  w
                &&
                dy >= -h
                &&
                dy <=  h ) {

                unlockExtraGlowFade[ i ] = 255;

                overI = i;
                unlockLastOver = i;
                break;
                }
            }
        }
    else {

        if( ! unlockViewDown
            &&
            maxigin_isButtonDown( unlockViewActionHandle ) ) {
            unlockViewDown = 1;
            unlockViewerCanceled = 0;

            if( unlockLastOver == -1
                ||
                unlockLastOver == NUM_UNLOCKS - 1 ) {
                unlockLastOver = 0;
                }
            else {
                unlockLastOver ++;
                }
            maxigin_playSoundEffect( unlockViewSound,
                                     256 );
            }   

        if( unlockLastOver != -1
            &&
            ! unlockViewerCanceled ) {
                
            unlockExtraGlowFade[ unlockLastOver ] = 255;
                
            overI = unlockLastOver;
            }
        }
    
    if( ! maxigin_isButtonDown( unlockViewActionHandle ) ) {
        unlockViewDown = 0;
        }

    
    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {

        if( i != overI
            &&
            unlockExtraGlowFade[ i ] > 0 ) {

            int  newHighlight = unlockExtraGlowFade[i] - deltaFade;

            if( newHighlight > 0 ) {
                unlockExtraGlowFade[i] = (unsigned char)newHighlight;
                }
            else {
                unlockExtraGlowFade[i] = 0;
                if( i == unlockLastOver ) {
                    unlockLastOver = -1;
                    }
                }
            }
        }
    }



void unlocksBeatLevel( int  inLevel ) {
    int  i;
    
    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {

        if( ! unlockEnabled[ i ]
            &&
            unlockAtLevel[ i ] <= inLevel ) {

            maxigin_playSoundEffect( unlockSound,
                                     512 );

            unlockEnabled[ i ] = 1;

            unlocksWrite();

            /* at most one unlock per level beaten */
            
            return;
            }
        }
    
    }



int unlocksGetExtraSideboardSlots( void ) {

    if( unlockEnabled[ 0 ] ) {
        return 1;
        }
    else {
        return 0;
        }
    }



int unlocksGetExtraShopSlots( void ) {

    if( unlockEnabled[ 1 ] ) {
        return 1;
        }
    else {
        return 0;
        }
    }



int unlocksGetMinNumSaleSlots( void ) {

    if( unlockEnabled[ 2 ] ) {
        return 1;
        }
    else {
        return 0;
        }
    }
    


void unlocksCancelViewer( void ) {
    unlockViewerCanceled = 1;
    }



char unlocksIsViewerActive( void ) {
    if( ! unlockViewerCanceled
        &&
        unlockLastOver != -1 ) {
        return 1;
        }
    return 0;
    }

    

    

#endif



#endif
