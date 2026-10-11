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


void runStatsLogString( const char  *inString );


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



static  const char  *runStatsSpaces   =  "  ";

/* 0 for play mode
   1 for buy mode */
static  char         runStatsBuyMode  =  0;



void runStatsLogLevel( int  inLevel ) {

    const char *s  =  maxigin_intToString( inLevel );

    runStatsWriteString( "\n" );
    
    runStatsWriteString( s );

    runStatsWriteString( ":" );

    runStatsBuyMode = 1;

    runStatsWriteString( runStatsSpaces );
    }



void runStatsLogDeploy( ChessPiece  inPiece ) {
    
    const char   *name  =  getPieceName( inPiece );

    if( runStatsBuyMode ) {
        runStatsWriteString( "\nplayed:  "  );
        runStatsBuyMode = 0;
        }
    
    runStatsWriteString( name );
    runStatsWriteString( runStatsSpaces );
    }



void runStatsLogString( const char  *inString ) {

    runStatsWriteString( inString );
    }



void runStatsLogPurchase( ChessPiece  inPiece ) {
    
    const char   *name  =  getPieceName( inPiece );

    if( ! runStatsBuyMode ) {
        runStatsWriteString( "\nbought:  "  );
        runStatsBuyMode = 1;
        }

    runStatsWriteString( name );
    runStatsWriteString( runStatsSpaces );
    }



void runStatsNewRun( void ) {

    const char  *newLines  =  "\n\n\n";

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
