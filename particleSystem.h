/*
  Include in your C code wherever like so:

      #include "particleSystem.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define PARTICLE_SYSTEM_IMPLEMENTATION
      #include "particleSystem.h"

*/

#ifndef PARTICLE_SYSTEM_H_INCLUDED
#define PARTICLE_SYSTEM_H_INCLUDED



enum{
    noParticle = 0,
    laserHeatParticle,
    NUM_PARTICLE_TYPES };



typedef struct ParticleState {
        
        int  type;
        int  sourceSprite;
        int  progress;
        int  spriteCenterX;
        int  spriteCenterY;
        
    } ParticleState;



void particleSystemInit( void );



void drawParticles( ParticleState  *inState,
                    unsigned char   inFade );



#ifdef PARTICLE_SYSTEM_IMPLEMENTATION


static  int  pixelParticleSprite  =  -1;


void particleSystemInit( void ) {
    pixelParticleSprite = maxigin_initSprite( "pixelParticle.tga" );
    }



typedef void (*ParticleDrawFunction)( ParticleState  *inState,
                                      unsigned char   inFade );



static void drawNoParticles( ParticleState  *inState,
                             unsigned char   inFade ) {
    (void)inState;
    (void)inFade;
    }


static int pickSeededVal( int  inIndex,
                          int  inPage,
                          int  inMinVal,
                          int  inMaxVal ) {
    
    MaxiginRand  rand;

    maxigin_randSeed( &rand,
                      (unsigned long)inIndex ^ ( (unsigned long)inPage << 8 ) );

    return maxigin_randRange( &rand,
                              inMinVal,
                              inMaxVal );
    }



static void drawLaserHeatParticles( ParticleState  *inState,
                                    unsigned char   inFade ) {

    int  maxNumParticles  =  50;
    int  i;
    int  t                =  inState->progress;
    int  s                =  inState->sourceSprite;
    int  p                =  8;
    int  page             =  100;
    int  sW;
    int  sH;
    int  sWHalf;
    int  sHHalf;

    char oldAdditive      =  maxigin_drawGetAdditive();

    maxigin_drawToggleAdditive( 1 );
    
    maxigin_getSpriteDimensions( s,
                                 &sW,
                                 &sH );
    sWHalf = sW / 2;
    sHHalf = sH / 2;
    
    
    for( i = 0;
         i < maxNumParticles;
         i ++ ) {

        int  jitter      =  pickSeededVal( i,
                                          page++,
                                          -10,
                                          10 );
        int  birthTime   =  i * p + jitter;
        int  age         =  t - birthTime;
        int  lifetime    =  pickSeededVal( i,
                                           page++,
                                           200,
                                           400 );
        if( age >= 0
            &&
            age < lifetime ) {
            /* particle alive */

            /* fixed point scaled by 1024 */
            long          speed     =  pickSeededVal( i,
                                                      page++,
                                                      32,
                                                      92 );
            int           sourceX;
            int           sourceY;
            int           yOffset;

            long          fade      =  (long)(lifetime - age) * 255 /
                                       (long)lifetime;
            MaxiginColor  c;
            
            fade = fade * inFade / 255;

            sourceX = pickSeededVal( i,
                                     page++,
                                     0,
                                     sW - 1 );
            
            sourceY = pickSeededVal( i,
                                     page++,
                                     0,
                                     sH - 1 );

            maxigin_getSpritePixel( s,
                                    sourceX,
                                    sourceY,
                                    &c );

            fade = fade * c.val[3] / 255;

            yOffset = (int)( - speed * age / 1024 );

            maxigin_drawSetColor( c.val[0],
                                  c.val[1],
                                  c.val[2],
                                  (unsigned char)fade );

            maxigin_drawSprite(
                pixelParticleSprite,
                inState->spriteCenterX + sourceX - sWHalf,
                inState->spriteCenterY + sourceY - sHHalf + yOffset );
            
            }
        }

    maxigin_drawToggleAdditive( oldAdditive );
    }



static ParticleDrawFunction particleDrawFunctions[] = { drawNoParticles,
                                                        drawLaserHeatParticles };

CHECK_ARRAY_LENGTH( particleDrawFunctions,
                    NUM_PARTICLE_TYPES );



void drawParticles( ParticleState  *inState,
                    unsigned char   inFade ) {

    particleDrawFunctions[ inState->type ]( inState,
                                            inFade );
    }



#endif

#endif
