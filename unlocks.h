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
                  int  inPointerActionHandle,
                  int  inDifficultyActionHandle,
                  int  inViewSound,
                  int  inPickDifficultySound );


/* register that player checkmated a level.
   Checkmating certain levels may unlock things */
void unlocksBeatLevel( int  inLevel );


char unlocksWouldBeNewWin( int  inLevel );


void unlocksDraw( int  inPosX,
                  int  inPosY );


void unlocksDrawParticles( int  inPosX,
                           int  inPosY );


void unlocksStep( int  inPosX,
                  int  inPosY  );


int unlocksGetExtraSideboardSlots( void );

int unlocksGetExtraShopSlots( void );

int unlocksGetMinNumSaleSlots( void );

int unlocksGetExtraNewRecruitsBaskets( void );

int unlocksGetExtraDraws( void );


void unlocksCancelViewer( void );

char unlocksIsViewerActive( void );


/* clears all permanent unlocks */
void unlocksHardReset( void );


char unlocksAreAnyUnlocked( void );


void unlocksToggleDifficultyPicking( char  inCanPick );




#endif


#include "tipDisplay.h"


#ifdef  UNLOCKS_IMPLEMENTATION

#ifndef UNLOCKS_IMPLEMENTATION_INCLUDED
#define UNLOCKS_IMPLEMENTATION_INCLUDED


#define  NUM_UNLOCKS           6
#define  NUM_NEXT_QUEST_LEVELS 6

static  char           unlockEnabled      [ NUM_UNLOCKS ];

static  int            unlockPosX         [ NUM_UNLOCKS ];
static  int            unlockPosY         [ NUM_UNLOCKS ];

static  int            unlockSprites      [ NUM_UNLOCKS ];

static  unsigned char  unlockExtraGlowFade[ NUM_UNLOCKS ];

static  int            unlockAtLevel      [ NUM_UNLOCKS ];

static  int            unlockFrameSprite;
static  int            unlockShadowSprite;
static  int            unlockShadowDownSprite;
static  int            unlockGlintSprite;

static  int            winFrameSprite;
static  int            winShadowSprite;
static  int            winShadowDownSprite;
static  int            winGlintSprite;
static  int            winShineSprite;
static  char           winOver;
static  unsigned char  winExtraGlowFade;

static  int            winPosX;
static  int            winPosY;
static  int            winAtLevel                       =   64;

static  char           winHasHappened                   =    0;
static  int            winSound                         =   -1;

static  int            nextQuestFrameSprite;
static  int            nextQuestFrameDownSprite;
static  int            nextQuestGlintSprite;
static  int            nextQuestShineSmallSpriteStrip;
static  int            nextQuestShineBigSpriteStrip;

static  int            nextQuestPosX;
static  int            nextQuestPosY;

static  int            nextQuestUnlockLevel             =    0;
static  int            nextQuestPickedLevel             =    0;
static  char           nextQuestOver                    =    0;

static  int            unlockSound                      =   -1;
static  int            unlockLastOver                   =   -1;
static  int            unlockViewActionHandle;
static  int            unlockPointerActionHandle;
static  int            unlockDifficultyActionHandle;

static  char           unlockDifficultyDown             =    0;
static  char           unlockPointerDown                =    0;

static  int            unlockViewSound;
static  int            unlockPickDiffSound;

static  char           unlockViewDown                   =    0;
static  char           unlockViewerCanceled             =    0;

static  int            unlockExploding                  =   -1;
static  int            unlockExplodingProgress          =    0;
static  int            unlockExplodingMax               =  512;

static  char           winExploding                     =    0;
static  int            winExplodingProgress             =    0;

static  int            nextQuestExploding               =   -1;
static  int            nextQuestExplodingProgress       =    0;

static  char           difficultyPickingOn              =    0;

static  int            lang_unlockDes;
static  int            lang_unlocks[ NUM_UNLOCKS ];
static  int            lang_lockedA;
static  int            lang_lockedB;

static  int            lang_winA;
static  int            lang_winB;
static  int            lang_win;

static  int            lang_questPick;
static  int            lang_nextQuestUnlocks[ NUM_NEXT_QUEST_LEVELS ];

static  int            lang_everythingUnlocked;



static  const char     hashMessage[]  =  "you are only cheating yourself";


/* gets the correct hex hash for an unlock string ending with a space */
static const char *getUnlockStringHash(  const char  *inUnlockString ) {

    const char  *stringToHash = maxigin_stringConcat( inUnlockString,
                                                      hashMessage );
    
    return maxigin_stringHash8Byte( stringToHash );
    }



static void unlocksWrite( void ) {

    static  char  s[ NUM_UNLOCKS + 3 ];

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

    /* one digit for up to 6 unlocks */
    s[ NUM_UNLOCKS     ] = (char)( '0' + nextQuestUnlockLevel );
    s[ NUM_UNLOCKS + 1 ] = ' ';
    s[ NUM_UNLOCKS + 2 ] = '\0';

    hexHash = getUnlockStringHash( s );

    file = mingin_startWritePersistData( "unlocks.ini" );

    if( file != -1 ) {

        char  success  =  mingin_writePersistData( file,
                                                   /* include space */
                                                   NUM_UNLOCKS + 2,
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

    static  char  s       [ NUM_UNLOCKS + 3 ];
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
    
    if( numBytes != NUM_UNLOCKS + 2 + 16 ) {
        mingin_endReadPersistData( file );
        return;
        }
    
    numRead = mingin_readPersistData( file,
                                      NUM_UNLOCKS + 2,
                                      (unsigned char*)s );

    if( numRead != NUM_UNLOCKS + 2 ) {
        mingin_endReadPersistData( file );
        return;
        }

    if( s[ NUM_UNLOCKS + 1 ] != ' ' ) {
        mingin_endReadPersistData( file );
        return;
        }
    s[ NUM_UNLOCKS + 2 ] = '\0';

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

    nextQuestUnlockLevel = s[ NUM_UNLOCKS ] - '0';
    }



void unlocksInit( int  inViewActionHandle,
                  int  inPointerActionHandle,
                  int  inDifficultyActionHandle,
                  int  inViewSound,
                  int  inPickDifficultySound ) {

    int  i;

    int  offsetY  =  11;
    int  curY     =  - ( ( NUM_UNLOCKS - 1 ) * offsetY ) / 2;

    unlockViewActionHandle = inViewActionHandle;
    unlockPointerActionHandle = inPointerActionHandle;
    unlockDifficultyActionHandle = inDifficultyActionHandle;
    
    unlockViewSound        = inViewSound;
    unlockPickDiffSound = inPickDifficultySound;

    unlockFrameSprite = maxigin_initSprite( "unlockFrame.tga" );
    unlockShadowSprite = maxigin_initSprite( "unlockFrameShadow.tga" );
    unlockShadowDownSprite = maxigin_initSprite( "unlockFrameShadowDown.tga" );
    unlockGlintSprite = maxigin_initSprite( "unlockGlint.tga" );

    unlockSprites[ 0 ] = maxigin_initSprite( "unlockSideboard.tga" );
    unlockSprites[ 1 ] = maxigin_initSprite( "unlockShopSlot.tga"  );
    unlockSprites[ 2 ] = maxigin_initSprite( "unlockSale.tga"      );
    unlockSprites[ 3 ] = maxigin_initSprite( "unlockSale.tga"      );
    unlockSprites[ 4 ] = maxigin_initSprite( "unlockSale.tga"      );
    unlockSprites[ 5 ] = maxigin_initSprite( "unlockSale.tga"      );
    
    

    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {
        maxigin_initMakeGlowSprite( unlockSprites[ i ],
                                    4,
                                    2 );
        unlockPosX[ i ] = 0;
        if( i % 2 == 0 ) {
            unlockPosX[ i ] = 3;
            }
        else {
            unlockPosX[ i ] = -3;
            }
        
        unlockPosY[ i ] = curY;

        curY += offsetY;

        unlockEnabled[ i ] = 0;

        unlockExtraGlowFade[ i ] = 0;
        }

    unlockAtLevel[ 0 ] = 16;
    unlockAtLevel[ 1 ] = 24;
    unlockAtLevel[ 2 ] = 32;
    unlockAtLevel[ 3 ] = 40;
    unlockAtLevel[ 4 ] = 48;
    unlockAtLevel[ 5 ] = 56;

    winFrameSprite      = maxigin_initSprite( "winFrame.tga" );
    winShadowSprite     = maxigin_initSprite( "winFrameShadow.tga" );
    winShadowDownSprite = maxigin_initSprite( "winFrameShadowDown.tga" );
    winGlintSprite      = maxigin_initSprite( "winGlint.tga" );
    winShineSprite      = maxigin_initSprite( "winShine.tga" );

    maxigin_initMakeGlowSprite( winShineSprite,
                                4,
                                2 );
    winPosX = -2;
    winPosY = unlockPosY[ NUM_UNLOCKS - 1 ] + 15;


    nextQuestFrameSprite = maxigin_initSprite( "nextQuestFrame.tga" );
    nextQuestFrameDownSprite = maxigin_initSprite( "nextQuestFrameDown.tga" );
    
    nextQuestGlintSprite = maxigin_initSprite( "nextQuestGlint.tga" );
    nextQuestShineSmallSpriteStrip =
        maxigin_initSpriteStrip( "nextQuestShineSmall.tga",
                                 1 );
    nextQuestShineBigSpriteStrip =
        maxigin_initSpriteStrip( "nextQuestShineBig.tga",
                                 3 );

    maxigin_initMakeGlowSpriteStrip( nextQuestShineSmallSpriteStrip,
                                     1,
                                     2 );
    maxigin_initMakeGlowSpriteStrip( nextQuestShineBigSpriteStrip,
                                     2,
                                     2 );

    nextQuestPosX = -2;
    nextQuestPosY = unlockPosY[ NUM_UNLOCKS - 1 ] + 32;


    unlockSound = maxigin_initSoundEffect( "unlock_sd_24.wav" );
    winSound = maxigin_initSoundEffect( "win_sd_9.wav" );


    lang_unlockDes    = maxigin_initTranslationKey( "unlockDes" );
    lang_unlocks[ 0 ] = maxigin_initTranslationKey( "unlock0"   );
    lang_unlocks[ 1 ] = maxigin_initTranslationKey( "unlock1"   );
    lang_unlocks[ 2 ] = maxigin_initTranslationKey( "unlock2"   );

    lang_lockedA      = maxigin_initTranslationKey( "unlockLockedA" );
    lang_lockedB      = maxigin_initTranslationKey( "unlockLockedB" );

    lang_winA = maxigin_initTranslationKey( "winReqA" );
    lang_winB = maxigin_initTranslationKey( "winReqB" );
    lang_win  = maxigin_initTranslationKey( "win" );

    lang_questPick = maxigin_initTranslationKey( "unlockMouseOver" );


    lang_nextQuestUnlocks[ 0 ] = maxigin_initTranslationKey( "unlockQuest1" );
    lang_nextQuestUnlocks[ 1 ] = maxigin_initTranslationKey( "unlockQuest2" );
    lang_nextQuestUnlocks[ 2 ] = maxigin_initTranslationKey( "unlockQuest3" );
    lang_nextQuestUnlocks[ 3 ] = maxigin_initTranslationKey( "unlockQuest4" );
    lang_nextQuestUnlocks[ 4 ] = maxigin_initTranslationKey( "unlockQuest5" );
    lang_nextQuestUnlocks[ 5 ] = maxigin_initTranslationKey( "unlockQuest6" );

    lang_everythingUnlocked =
        maxigin_initTranslationKey( "everythingUnlocked" );
    
                
    unlocksRead();

    REGISTER_ARRAY_MEM( unlockEnabled       );
    REGISTER_ARRAY_MEM( unlockExtraGlowFade );
    REGISTER_VAL_MEM( unlockLastOver );

    REGISTER_VAL_MEM( unlockViewDown );

    REGISTER_VAL_MEM( unlockViewerCanceled );

    REGISTER_VAL_MEM( unlockExploding );
    REGISTER_VAL_MEM( unlockExplodingProgress );

    REGISTER_VAL_MEM( winOver );
    REGISTER_VAL_MEM( winExtraGlowFade );

    REGISTER_VAL_MEM( winHasHappened );

    REGISTER_VAL_MEM( winExploding );
    REGISTER_VAL_MEM( winExplodingProgress );

    REGISTER_VAL_MEM( nextQuestUnlockLevel );
    REGISTER_VAL_MEM( nextQuestPickedLevel );

    REGISTER_VAL_MEM( nextQuestOver );

    REGISTER_VAL_MEM( nextQuestExploding );
    REGISTER_VAL_MEM( nextQuestExplodingProgress );

    REGISTER_VAL_MEM( difficultyPickingOn );

    REGISTER_VAL_MEM( unlockDifficultyDown );
    REGISTER_VAL_MEM( unlockPointerDown );
    }



void unlocksDraw( int  inPosX,
                  int  inPosY ) {

    int  i;
    int  j;
    int  glowRep      =  3;
    int  x;
    int  y;
    int  numSprites;
    int  xStart;
    int  nqTempX;
    int  nqTempY;

    maxigin_drawResetColor();

    maxigin_drawAddButtonHint( unlockViewActionHandle,
                               inPosX + unlockPosX[ 0 ] - 10,
                               inPosY + unlockPosY[ 0 ] );
    
    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {
        
        x = unlockPosX[ i ] + inPosX;
        y = unlockPosY[ i ] + inPosY;

        if( i == unlockLastOver
            &&
            unlockExtraGlowFade[ i ] == 255 ) {

            maxigin_drawSprite( unlockShadowDownSprite,
                                x,
                                y );
            y += 1;
            }
        else {
            maxigin_drawSprite( unlockShadowSprite,
                                x,
                                y );
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

        int  centX  =  MAXIGIN_GAME_NATIVE_W - 38;
        int  centY  =  MAXIGIN_GAME_NATIVE_H / 2 - 10;

        
        if( unlockEnabled[ unlockLastOver ] ) {

            drawDescriptionText( lang_unlockDes,
                                 lang_unlocks[ unlockLastOver ],
                                 centX,
                                 centY,
                                 unlockExtraGlowFade[ unlockLastOver ] );

            }
        
        }


    x = winPosX + inPosX;
    y = winPosY + inPosY;
    
    maxigin_drawResetColor();

    if( winOver ) {

        maxigin_drawSprite( winShadowDownSprite,
                            x,
                            y );
        y += 1;
        }
    else {
        maxigin_drawSprite( winShadowSprite,
                            x,
                            y );
        }
    
    maxigin_drawSprite( winFrameSprite,
                        x,
                        y );

    if( winHasHappened ) {

        maxigin_drawSprite( winShineSprite,
                            x,
                            y );
        for( j = 0;
             j < glowRep - 1;
             j ++ ) {
            maxigin_drawSpriteGlowOnly( winShineSprite,
                                        x,
                                        y );
            }

        if( winExtraGlowFade > 0 ) {
            maxigin_drawSetAlpha( winExtraGlowFade );

            for( j = 0;
                 j < glowRep - 1;
                 j ++ ) {
                maxigin_drawSpriteGlowOnly( winShineSprite,
                                            x,
                                            y );
                }
            }
        }
    else {
        maxigin_drawSprite( winGlintSprite,
                            x,
                            y );
        }


    x = nextQuestPosX + inPosX;
    y = nextQuestPosY + inPosY;

    nqTempX = x;
    nqTempY = y;

    maxigin_drawResetColor();

    if( nextQuestOver ) {

        maxigin_drawSprite( nextQuestFrameDownSprite,
                            x,
                            y );
        nqTempY ++;
        }
    else {
        maxigin_drawSprite( nextQuestFrameSprite,
                            x,
                            y );
        }



    /* now draw next quest lights */
    
    numSprites = maxigin_getNumSpritesInStrip(
        nextQuestShineSmallSpriteStrip );
    xStart = nqTempX - 3;

    x = xStart;

    y = nqTempY - 7;

    if( numSprites > NUM_NEXT_QUEST_LEVELS ) {
        numSprites = NUM_NEXT_QUEST_LEVELS;
        }

    for( i = 0;
         i < numSprites
         &&
         i < nextQuestUnlockLevel;
         i   ++ ) {

        int  s  =  maxigin_getSpriteFromStrip(
                       nextQuestShineSmallSpriteStrip,
                       i );        

        if( nextQuestExploding == i + 1
            &&
            winExploding ) {

            /* we're going to explode this light to turn it on,
               but it hasn't started yet */
            
            continue;
            }

        
        maxigin_drawSprite( s,
                            x,
                            y );
        for( j = 0;
             j < glowRep - 1 + 1;
             j ++ ) {
            maxigin_drawSpriteGlowOnly( s,
                                        x,
                                        y );
            }

        x += 4;

        if( i == 2 ) {
            x = xStart;
            y += 7;
            }
        }

    numSprites = maxigin_getNumSpritesInStrip(
        nextQuestShineBigSpriteStrip );

    xStart = nqTempX - 3;

    x = xStart;

    y = nqTempY - 4;

    if( numSprites > NUM_NEXT_QUEST_LEVELS ) {
        numSprites = NUM_NEXT_QUEST_LEVELS;
        }

    for( i = 0;
         i < numSprites
         &&
         i < nextQuestPickedLevel;
         i   ++ ) {

        int  s  =  maxigin_getSpriteFromStrip(
                       nextQuestShineBigSpriteStrip,
                       i );


        maxigin_drawSprite( s,
                            x,
                            y );
        for( j = 0;
             j < glowRep - 1 + 1;
             j ++ ) {
            maxigin_drawSpriteGlowOnly( s,
                                        x,
                                        y );
            }

        x += 4;

        if( i == 2 ) {
            x = xStart;
            y += 7;
            }
        }

    
    if( nextQuestUnlockLevel < 3 ) {
        x = nqTempX + 7;
        y = nqTempY - 7;
        
        maxigin_drawSprite( nextQuestGlintSprite,
                            x,
                            y );
        }

    if( difficultyPickingOn ) {
        maxigin_drawAddButtonHint(
            unlockDifficultyActionHandle,
            nextQuestPosX + inPosX - 11,
            nextQuestPosY + inPosY );
        }
    
    }



void unlocksDrawParticles( int  inPosX,
                           int  inPosY ) {

    maxigin_drawResetColor();

    if( unlockExploding != -1 ) {
        
        unsigned char  a;
        int            i   =  unlockExploding;
        int            x   =  unlockPosX[ i ] + inPosX;
        int            y   =  unlockPosY[ i ] + inPosY;

        a = (unsigned char)(
            ( (long)( unlockExplodingMax - unlockExplodingProgress ) * 255 )
            / unlockExplodingMax );

        maxigin_drawToggleAdditive( 1 );
        maxigin_drawExplodingSprite( unlockSprites[ i ],
                                  getParticleSprite(),
                                  x + 2,
                                  y,
                                  x + 2,
                                  y,
                                  30,
                                  unlockExplodingProgress,
                                  unlockExplodingMax,
                                  a );
        maxigin_drawToggleAdditive( 0 );
        }

    
    if( winExploding ) {

        unsigned char  a;
        int            x   =  winPosX + inPosX;
        int            y   =  winPosY + inPosY;

        a = (unsigned char)(
            ( (long)( unlockExplodingMax - winExplodingProgress ) * 255 )
            / unlockExplodingMax );

        maxigin_drawToggleAdditive( 1 );
        maxigin_drawExplodingSprite( winShineSprite,
                                  getParticleSprite(),
                                  x - 1,
                                  y,
                                  x - 1,
                                  y - 1,
                                  30,
                                  winExplodingProgress,
                                  unlockExplodingMax,
                                  a );
        maxigin_drawToggleAdditive( 0 );
        }
    else if( nextQuestExploding != -1 ) {

        /* wait until win explosion done before drawing next quest
           unlock light explosion */
        
        unsigned char  a;
        int            x   =  nextQuestPosX + inPosX - 3;
        int            y   =  nextQuestPosY + inPosY - 7;
        int            s   =  maxigin_getSpriteFromStrip(
                                  nextQuestShineSmallSpriteStrip,
                                  nextQuestExploding - 1 );

        x += 4 * ( ( nextQuestExploding - 1 ) % 3 );
        if( nextQuestExploding > 3 ) {
            y += 7;
            }
        
        a = (unsigned char)(
            ( (long)( unlockExplodingMax - nextQuestExplodingProgress ) * 255 )
            / unlockExplodingMax );

        maxigin_drawToggleAdditive( 1 );
        maxigin_drawResetColor();
        maxigin_drawExplodingSprite( s,
                                     getParticleSprite(),
                                     x,
                                     y,
                                     x,
                                     y,
                                     30,
                                     nextQuestExplodingProgress,
                                     unlockExplodingMax,
                                     a );
        maxigin_drawResetColor();
        maxigin_drawExplodingSprite( s,
                                     getParticleSprite(),
                                     x,
                                     y,
                                     x - 3,
                                     y,
                                     30,
                                     nextQuestExplodingProgress,
                                     unlockExplodingMax,
                                     a );
        maxigin_drawResetColor();
        maxigin_drawExplodingSprite( s,
                                     getParticleSprite(),
                                     x,
                                     y,
                                     x + 3,
                                     y,
                                     30,
                                     nextQuestExplodingProgress,
                                     unlockExplodingMax,
                                     a );
        maxigin_drawResetColor();
        maxigin_drawExplodingSprite( s,
                                     getParticleSprite(),
                                     x,
                                     y,
                                     x,
                                     y - 1,
                                     30,
                                     nextQuestExplodingProgress,
                                     unlockExplodingMax,
                                     a );
        maxigin_drawResetColor();
        maxigin_drawExplodingSprite( s,
                                     getParticleSprite(),
                                     x,
                                     y,
                                     x,
                                     y + 1,
                                     30,
                                     nextQuestExplodingProgress,
                                     unlockExplodingMax,
                                     a );
        maxigin_drawToggleAdditive( 0 );
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

    if( maxigin_getActivePointerLocation( &pointerX,
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

                if( unlockExtraGlowFade[ i ] < 255 ) {
                    maxigin_playSoundEffect( unlockViewSound,
                                             256 );
                    }
                
                unlockExtraGlowFade[ i ] = 255;

                overI = i;
                unlockLastOver = i;
                winOver = 0;
                nextQuestOver = 0;
                
                break;
                }
            }

        if( overI == -1 ) {
            /* check if over win light */
            int  x   =  winPosX + inPosX;
            int  y   =  winPosY + inPosY;

            int  dx  =  pointerX - x;
            int  dy  =  pointerY - y;
            
            maxigin_getSpriteDimensions( winFrameSprite,
                                         &w,
                                         &h );
            w /= 2;
            h /= 2;

            if( dx >= -w
                &&
                dx <=  w
                &&
                dy >= -h
                &&
                dy <=  h ) {

                if( winExtraGlowFade < 255 ) {
                    maxigin_playSoundEffect( unlockViewSound,
                                             256 );
                    }
                
                winExtraGlowFade = 255;

                winOver = 1;
                nextQuestOver = 0;
                }
            else {
                winOver = 0;
                }

            if( ! winOver ) {
                /* check for next quest mouse-over */
                
                x = nextQuestPosX + inPosX;
                y = nextQuestPosY + inPosY;

                dx = pointerX - x;
                dy = pointerY - y;
                
                maxigin_getSpriteDimensions( nextQuestFrameSprite,
                                             &w,
                                             &h );
                w /= 2;
                h /= 2;
                
                if( dx >= -w
                    &&
                    dx <=  w
                    &&
                    dy >= -h
                    &&
                    dy <=  h ) {

                    if( ! nextQuestOver ) {
                        maxigin_playSoundEffect( unlockViewSound,
                                                 256 );
                        }

                    nextQuestOver = 1;
                    }
                else {
                    nextQuestOver = 0;
                    }


                if( nextQuestOver
                    &&
                    maxigin_isButtonDown( unlockPointerActionHandle ) ) {

                    if( ! unlockPointerDown
                        &&
                        difficultyPickingOn
                        &&
                        nextQuestUnlockLevel > 0 ) {

                        nextQuestPickedLevel ++;

                        maxigin_playSoundEffect( unlockPickDiffSound,
                                                 512 );

                        if( nextQuestPickedLevel > nextQuestUnlockLevel ) {
                            nextQuestPickedLevel = 0;
                            }
                        }
                    unlockPointerDown = 1;
                    }
                else {
                    unlockPointerDown = 0;
                    }
                }
            }
        }
    else {
        unlockPointerDown = 0;

        if( ! unlockViewDown
            &&
            maxigin_isButtonDown( unlockViewActionHandle ) ) {
            unlockViewDown = 1;
            unlockViewerCanceled = 0;

            if( unlockLastOver == -1
                &&
                ! winOver ) {
                unlockLastOver = 0;
                winOver = 0;
                nextQuestOver = 0;
                }
            else if( unlockLastOver == NUM_UNLOCKS - 1 ) {
                unlockLastOver = -1;
                winOver = 1;
                }
            else if( winOver == 1 ) {
                winOver = 0;
                nextQuestOver = 1;
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
        if( winOver
            &&
            ! unlockViewerCanceled ) {
            winExtraGlowFade = 255;
            }
        }
    
    if( ! maxigin_isButtonDown( unlockViewActionHandle ) ) {
        unlockViewDown = 0;
        }

    if( maxigin_isButtonDown( unlockDifficultyActionHandle ) ) {
        
        if( ! unlockDifficultyDown
            &&
            difficultyPickingOn
            &&
            nextQuestUnlockLevel > 0 ) {

            nextQuestPickedLevel ++;

            maxigin_playSoundEffect( unlockPickDiffSound,
                                     512 );

            if( nextQuestPickedLevel > nextQuestUnlockLevel ) {
                nextQuestPickedLevel = 0;
                }
            }
        unlockDifficultyDown = 1;
        }
    else {
        unlockDifficultyDown = 0;
        }


    if( nextQuestOver ) {

        if( difficultyPickingOn ) {

            tipDisplaySet( lang_questPick,
                           -1,
                           -1,
                           1,
                           0 );
            }
        else if( nextQuestUnlockLevel < 6 ) {

            tipDisplaySet(
                lang_nextQuestUnlocks[ nextQuestUnlockLevel ],
                -1,
                -1,
                1,
                0 );
            }
        else {
            tipDisplaySet( lang_everythingUnlocked,
                           -1,
                           -1,
                           1,
                           0 );
            }

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

    if( ! winOver
        &&
        winExtraGlowFade > 0 ) {

        int  newHighlight = winExtraGlowFade - deltaFade;

        if( newHighlight > 0 ) {
            winExtraGlowFade = (unsigned char)newHighlight;
            }
        else {
            winExtraGlowFade = 0;
            }
        }


    
    if( overI != -1
        &&
        ! unlockEnabled[ overI ] ) {

        int  level  =  unlockAtLevel[ overI ];

        if( overI > 0
            &&
            ! unlockEnabled[ overI - 1 ] ) {
            /* we're mousing over beyond the next-to-unlock thing.
               show ? */
            level = -2;
            }


        tipDisplaySet( lang_lockedA,
                       level,
                       lang_lockedB,
                       1,
                       0 );
        }
    else if( winOver ) {

        if( winHasHappened ) {

            tipDisplaySet( lang_win,
                           -1,
                           -1,
                           1,
                           0 );

            }
        else {

            int  level  =  winAtLevel;

            if( ! unlockEnabled[ NUM_UNLOCKS - 1 ] ) {
                /* show ? */
                level = -2;
                }

            tipDisplaySet( lang_winA,
                           level,
                           lang_winB,
                           1,
                           0 );
            }
        }



    if( unlockExploding != -1 ) {
        unlockExplodingProgress += ( 10 * 60 ) / r;

        if( unlockExplodingProgress > unlockExplodingMax ) {
            unlockExplodingProgress = 0;
            unlockExploding = -1;
            }
        }

    
    if( winExploding ) {
        winExplodingProgress += ( 10 * 60 ) / r;

        if( winExplodingProgress > unlockExplodingMax ) {
            winExplodingProgress = 0;
            winExploding = 0;

            if( nextQuestExploding != -1 ) {
                /* done with win exploding, and there's a nextQuest
                   light to explode too... play unlock sound */
                
                maxigin_playSoundEffect( unlockSound,
                                         512 );
                }
            }
        }
    else if( nextQuestExploding != -1 ) {
        nextQuestExplodingProgress += ( 10 * 60 ) / r;
        
        if( nextQuestExplodingProgress > unlockExplodingMax ) {
            nextQuestExplodingProgress = 0;
            nextQuestExploding = -1;
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

            unlockExploding = i;
            unlockExplodingProgress = 0;

            unlocksWrite();

            /* at most one unlock per level beaten */
            
            return;
            }
        }

    if( unlocksWouldBeNewWin( inLevel ) ) {

        maxigin_playSoundEffect( winSound,
                                 512 );

        winHasHappened = 1;
        winExploding = 1;
        winExplodingProgress = 0;

        /* unlock next quest, if needed */

        if( nextQuestUnlockLevel < NUM_NEXT_QUEST_LEVELS ) {
            if( nextQuestPickedLevel == nextQuestUnlockLevel ) {
                nextQuestUnlockLevel ++;
                
                nextQuestExploding = nextQuestUnlockLevel;
                nextQuestExplodingProgress = 0;
                
                unlocksWrite();
                }
            }
        }
    
    }



char unlocksWouldBeNewWin( int  inLevel ) {

    if( ! winHasHappened
        &&
        inLevel >= winAtLevel ) {
        return 1;
        }
    return 0;
    }
    


void unlocksHardReset( void ) {
    
    int  i;

    if( unlocksAreAnyUnlocked() ) {

        const char  *backupFileName;
        long         sec;
        long         msec;
        
        mingin_getRunningTime( &sec,
                               &msec );

        backupFileName = maxigin_stringConcat5( "unlocks_backup_",
                                                maxigin_intToString( (int)sec ),
                                                "_",
                                                maxigin_intToString( (int)msec ),
                                                ".ini" );
            
        mingin_renamePersistData( "unlocks.ini",
                                  backupFileName );

        maxigin_playSoundEffect( unlockSound,
                                 512 );
        }
    
    
    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {

        unlockEnabled[ i ] = 0;

        }
    
    nextQuestUnlockLevel = 0;
    
    unlocksWrite();
    }


char unlocksAreAnyUnlocked( void ) {
    int  i;
    
    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {

        if( unlockEnabled[ i ] ) {
            return 1;
            }
        }
    if( nextQuestUnlockLevel > 0 ) {
        return 1;
        }
    
    return 0;
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



int unlocksGetExtraNewRecruitsBaskets( void ) {
    /* not implemented yet */
    return 0;
    }



int unlocksGetExtraDraws( void ) {
    /* not implemented yet */
    return 0;
    }



void unlocksCancelViewer( void ) {
    if( winOver != 0
        ||
        unlockLastOver != -1 ) {

        tipDisplayClear();
        }
    
    unlockViewerCanceled = 1;
    winOver = 0;
    unlockLastOver = -1;
    }



char unlocksIsViewerActive( void ) {
    if( ! unlockViewerCanceled
        &&
        unlockLastOver != -1
        &&
        unlockEnabled[ unlockLastOver ] ) {

        /* only count as enabled if we're over an unlocked item
           mousing over locked items or win light uses tip display */
        return 1;
        }
    return 0;
    }



void unlocksToggleDifficultyPicking( char  inCanPick ) {
    difficultyPickingOn = inCanPick;
    }


    

#endif



#endif
