/*
  Include in your C code wherever like so:

      #include "runStats.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define RUN_STATS_IMPLEMENTATION
      #include "runStats.h"

*/

#ifndef RUN_STATS_H_INCLUDED
#define RUN_STATS_H_INCLUDED


void runStatsInit( void );


void runStatsLogLevel( int  inLevel );


void runStatsLogDeploy( ChessPiece  inPiece );


void runStatsLogPurchase( ChessPiece  inPiece );


void runStatsNewRun( void );


void runStatsQuit( void );


#endif



#ifdef  RUN_STATS_IMPLEMENTATION

#ifndef RUN_STATS_IMPLEMENTATION_INCLUDED
#define RUN_STATS_IMPLEMENTATION_INCLUDED


static  int  runStatsDataHandle  =  -1;



void runStatsInit( void ) {

    runStatsDataHandle  =  mingin_startAppendPersistData( "runStats.txt" );

    }


static char  runStatsWriteString( const char  *inString ) {

    char  success  =  0;
    
    if( runStatsDataHandle != -1 ) {
    
        success  =  mingin_writePersistData( runStatsDataHandle,
                                             maxigin_stringLength( inString ),
                                             (unsigned char *)inString );
        }

    return success;
    }



static  const char  *runStatsSpaces  =  "  ";



void runStatsLogLevel( int  inLevel ) {

    const char *s  =  maxigin_intToString( inLevel );
    const char *c  =  ":";
    
    runStatsWriteString( s );

    runStatsWriteString( c );

    runStatsWriteString( runStatsSpaces );
    }



void runStatsLogDeploy( ChessPiece  inPiece ) {
    
    const char   *tag   =  "p:";
    const char   *name  =  getPieceName( inPiece );

    runStatsWriteString( tag  );
    runStatsWriteString( name );
    runStatsWriteString( runStatsSpaces );
    }



void runStatsLogPurchase( ChessPiece  inPiece ) {
    
    const char   *tag   =  "b:";
    const char   *name  =  getPieceName( inPiece );

    runStatsWriteString( tag  );
    runStatsWriteString( name );
    runStatsWriteString( runStatsSpaces );
    }



void runStatsNewRun( void ) {

    const char  *newLines  =  "\n\n";

    runStatsWriteString( newLines );

    }



void runStatsQuit( void ) {

    if( runStatsDataHandle != -1 ) {
        mingin_endWritePersistData( runStatsDataHandle );
        }
    
    runStatsDataHandle = -1;
    }



#endif

#endif
