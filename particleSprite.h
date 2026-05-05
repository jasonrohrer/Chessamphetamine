/*
  Include in your C code wherever like so:

      #include "particleSprite.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define PARTICLE_SPRITES_IMPLEMENTATION
      #include "particleSprite.h"

*/

#ifndef PARTICLE_SPRITE_H_INCLUDED
#define PARTICLE_SPRITE_H_NCLUDED


void particleSpriteInit( void );

int getParticleSprite( void );

int getNoGlowParticleSprite( void );

int getStarParticleSprite( void );



#ifdef PIECE_SPRITES_IMPLEMENTATION

static  int          particleSpriteHandle        =  -1;
static  int          noGlowParticleSpriteHandle  =  -1;
static  const char  *particleDataName            =  "explosionParticle.tga";

static  int          starSpriteHandle            =  -1;
static  const char  *starDataName                =  "starParticle.tga";


void particleSpriteInit( void ) {

    particleSpriteHandle = maxigin_initSprite( particleDataName );

    if( particleSpriteHandle != -1 ) {
        maxigin_initMakeGlowSprite( particleSpriteHandle,
                                    4,
                                    2 );
        }

    noGlowParticleSpriteHandle = maxigin_initSprite( particleDataName );

    starSpriteHandle = maxigin_initSprite( starDataName );

    if( starSpriteHandle != -1 ) {
        maxigin_initMakeGlowSprite( starSpriteHandle,
                                    4,
                                    2 );
        }
    }



int getParticleSprite( void ) {
    return particleSpriteHandle;
    }


int getNoGlowParticleSprite( void ) {
    return noGlowParticleSpriteHandle;
    }

int getStarParticleSprite( void ) {
    return starSpriteHandle;
    }


#endif



#endif

