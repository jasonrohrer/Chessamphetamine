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
    noParticles = 0,
    laserHeatParticles,
    NUM_PARTICLE_TYPES };



typedef struct ParticleState {
        
        int  type;
        int  progress;
        
    } ParticleState;



void initParticleSystem( void );



void drawParticles( ParticleState  *inState,
                    int             inSpriteCenterX,
                    int             inSpriteCenterY );



#ifdef PARTICLE_SYSTEM_IMPLEMENTATION



void initParticleSystem( void ) {
    }



void drawParticles( ParticleState  *inState,
                    int             inSpriteCenterX,
                    int             inSpriteCenterY ) {
    (void)inState;
    (void)inSpriteCenterX;
    (void)inSpriteCenterY;
    }



#endif

#endif
