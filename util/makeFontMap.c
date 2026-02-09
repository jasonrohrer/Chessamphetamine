
#include <stdlib.h>
#include <stdio.h>


static void usage( const char  *inEXEName ) {

    printf( "Usage:\n\n" );

    printf( "  %s   languageFile.txt  fontMappingOut.txt\n\n",
            inEXEName );
    
    exit( 1 );
    }


static void skipPastNextQuote( FILE *inFile ) {

    int  c  =  'a';

    while( c != '"'
           &&
           c != EOF ) {

        c = fgetc( inFile );
        }
    }


/* gets string up to but not including trailing quote, but also reads/skip
   trailing quote

   result is a static buffer or NULL if scan fails */
static char *getStringToQuote( FILE *inFile ) {

    int           i            =  0;
    int           c;
    enum{         BUFFER_SIZE  =  4096 };

    static  char  buffer[ BUFFER_SIZE + 1 ];

    c = fgetc( inFile );

    while( c != '"'
           &&
           c != EOF
           &&
           i < BUFFER_SIZE ) {

        buffer[i] = (char)c;

        c = fgetc( inFile );

        i ++;
        }
    

    /* buffer has BUFFER_SIZE + 1, room for termination always */
    
    buffer[i] = '\0';

    if( i > 0 ) {
        return buffer;
        }
    else {
        return NULL;
        }
    }




/* outPoint set to the scanned code point, or -1 on failure

   outSubBytes are filled with raw bytes of code point from inBuffer
   unused bytes are set to 0 ('\0')

   returns pointer to next byte after bytes used up by scan */
static char *scanNextCodePoint( char           *inBuffer,
                                long           *outPoint,
                                unsigned char   outSubBytes[4] ) {

    unsigned char  c0;
    unsigned char  c1;
    unsigned char  c2;
    unsigned char  c3;

    outSubBytes[0] = 0;
    outSubBytes[1] = 0;
    outSubBytes[2] = 0;
    outSubBytes[3] = 0;
    
    c0 = (unsigned char)( inBuffer[ 0 ] );

    if( c0 == '\0' ) {
        *outPoint = -1;
        return inBuffer;
        }
    

    if( c0 < 128 ) {
        /* single byte case */
        *outPoint = (long)( c0 );
        
        outSubBytes[0] = c0;
        
        return &( inBuffer[ 1 ] );
        }
    else if( ( c0 & 0xE0 ) == 0xC0 ) {
        /* first byte is 110xxxxx
           2-byte case */

        c1 = (unsigned char)( inBuffer[ 1 ] );

        if( c1 == '\0' ) {
            *outPoint = -1;
            return inBuffer;
            }

        if( ( c1 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            *outPoint = -1;
            return inBuffer;
            }
        
        /* take 5 bits from first byte, and 6 bits from continuation */
        *outPoint = (long)( ( c0 & 0x1F ) << 6
                            |
                            ( c1 & 0x3F ) );

        outSubBytes[0] = c0;
        outSubBytes[1] = c1;
        
        return &( inBuffer[ 2 ] );
        }
    else if( ( c0 & 0xF0 ) == 0xE0 ) {
        /* first byte is 1110xxxx
           3-byte case */

        c1 = (unsigned char)( inBuffer[ 1 ] );

        if( c1 == '\0' ) {
            *outPoint = -1;
            return inBuffer;
            }

        if( ( c1 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            *outPoint = -1;
            return inBuffer;
            }

        c2 = (unsigned char)( inBuffer[ 2 ] );

        if( c2 == '\0' ) {
            *outPoint = -1;
            return inBuffer;
            }

        if( ( c2 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            *outPoint = -1;
            return inBuffer;
            }

        /* take 4 bits from first byte, and 6 bits from each continuation */
        *outPoint =  (long)( ( c0 & 0x0F ) << 12
                             |
                             ( c1 & 0x3F ) << 6
                             |
                             ( c2 & 0x3F ) );
        
        outSubBytes[0] = c0;
        outSubBytes[1] = c1;
        outSubBytes[2] = c2;
        
        return &( inBuffer[ 3 ] );
        }
    else if( ( c0 & 0xF8 ) == 0xF0 ) {
        /* first byte is 11110xxx
           4-byte case */
        
        c1 = (unsigned char)( inBuffer[ 1 ] );

        if( c1 == '\0' ) {
            *outPoint = -1;
            return inBuffer;
            }

        if( ( c1 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            *outPoint = -1;
            return inBuffer;
            }

        c2 = (unsigned char)( inBuffer[ 2 ] );

        if( c2 == '\0' ) {
            *outPoint = -1;
            return inBuffer;
            }

        if( ( c2 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            *outPoint = -1;
            return inBuffer;
            }
        
        c3 = (unsigned char)( inBuffer[ 3 ] );

        if( c3 == '\0' ) {
            *outPoint = -1;
            return inBuffer;
            }

        if( ( c3 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            *outPoint = -1;
            return inBuffer;
            }
        
        /* take 3 bits from first byte, and 6 bits from each continuation */
        *outPoint = (long)( ( c0 & 0x07 ) << 18
                            |
                            ( c1 & 0x3F ) << 12
                            |
                            ( c2 & 0x3F ) << 6
                            |
                            ( c3 & 0x3F ) );

        outSubBytes[0] = c0;
        outSubBytes[1] = c1;
        outSubBytes[2] = c2;
        outSubBytes[3] = c3;
        
        return &( inBuffer[ 4 ] );
        }
    else {
        /* we should never get here */
        *outPoint = -1;
        return inBuffer;
        }
    }




#define       CODE_POINT_LIST_MAX_SIZE     26
static  int   numCodePoints             =      0;


typedef struct CodePoint {
        
        long           point;
        unsigned char  bytes[4];
        
    } CodePoint;


static  CodePoint  codePointList[ CODE_POINT_LIST_MAX_SIZE ];


static void copyPointBytes( int  inListIndex,
                            unsigned char  inBytes[4] ) {

    int  b;

    for( b = 0;
         b < 4;
         b ++ ) {
        codePointList[ inListIndex ].bytes[b] = inBytes[b];
        }
    }


/* inserts into list, preserving sorted order, and ignoring duplicate points */
static void insertCodePoint( long           inCodePoint,
                             unsigned char  inBytes[4] ) {

    int  i  =  0;
    int  j  =  0;
    
    if( numCodePoints >= CODE_POINT_LIST_MAX_SIZE ) {
        printf( "Error:  trying to insert extra code point "
                "beyond the %d limit\n",
                CODE_POINT_LIST_MAX_SIZE );
        return;
        }
    

    while( i < numCodePoints
           &&
           codePointList[ i ].point < inCodePoint ) {
        i ++;
        }

    if( i == numCodePoints ) {
        /* fell off end */

        codePointList[ i ].point = inCodePoint;
        
        copyPointBytes( i,
                        inBytes );

        numCodePoints ++;

        return;
        }

    if( codePointList[i].point == inCodePoint ) {
        /* ignore dupe */
        return;
        }


    /* new item to insert mid-list */

    /* move existing to make room */

    for( j = numCodePoints;
         j > i;
         j -- ) {
        
        codePointList[j] = codePointList[j-1];
        }
    
    numCodePoints ++;

    /* now insert in the now-empty spot at i */
    codePointList[i].point = inCodePoint;

    copyPointBytes( i,
                    inBytes );
    }






static  char  testMode = 1;






int main( int          inNumArgs,
          const char  **inArgs ) {

    const char  *langFileName;
    const char  *outMapFileName;

    FILE        *langFile;
    FILE        *outMapFile;
    int          i;
    
    if( inNumArgs != 3 ) {

        if( testMode ) {
            /* supply default args for testing in debugger */

            langFileName   = "../data/english.txt";
            outMapFileName = "out.txt";
            }
        else {
            usage( inArgs[0] );
            }
        }
    else {
        langFileName   = inArgs[1];
        outMapFileName = inArgs[2];
        }
    

    langFile = fopen( langFileName,
                      "r" );

    if( langFile == NULL ) {
        printf( "\nFailed to open lang file \"%s\"\n\n",
                langFileName );
        
        usage( inArgs[0] );
        }

    outMapFile = fopen( outMapFileName,
                        "wb" );

    if( outMapFile == NULL ) {
        printf( "\nFailed to open/create output map file \"%s\"\n\n",
                outMapFileName );

        fclose( langFile );
        
        usage( inArgs[0] );
        }


    while( 1 ) {

        char          *s;
        long           point;
        unsigned char  subBytes[4];
        
        skipPastNextQuote( langFile );

        s = getStringToQuote( langFile );

        if( s == NULL ) {
            break;
            }
        
        s = scanNextCodePoint( s,
                               &point,
                               subBytes );

        while( point != -1 ) {

            if( point != ' '
                &&
                point != '\t'
                &&
                point != '\r'
                &&
                point != '\n' ) {
                
                insertCodePoint( point,
                                 subBytes );
                }
            
            s = scanNextCodePoint( s,
                                   &point,
                                   subBytes );
            }    
        }
    
    /* done scanning language file */

    fclose( langFile );
    

    /* output ordered UTF-8 char map with \n in between */

    for( i = 0;
         i < numCodePoints;
         i ++ ) {

        if( i > 0 ) {
            fprintf( outMapFile, "\n" );
            }
        
        fprintf( outMapFile, "%s", codePointList[ i ].bytes );
        }

    fclose( outMapFile );
    
    
    return 1;
    }

    
