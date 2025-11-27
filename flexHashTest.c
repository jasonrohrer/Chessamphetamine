
static const unsigned char flexHashTable[256] = {
    108,   35,   77,  207,    9,  111,  203,  175,
     70,  142,  194,  252,  115,  141,   32,  182,
    174,   15,  129,   33,   16,   43,  160,  144,
    149,   30,  197,  185,   54,  246,   75,  169,
    103,   66,   57,  240,   23,   68,  244,  232,
     81,   91,  147,  150,  223,   85,  124,  187,
    167,  110,  222,   80,   20,   95,  131,   74,
    242,   65,   49,   11,  139,   64,  181,    2,
    138,  176,   73,  239,  208,   39,  155,  163,
    168,  233,   50,  107,  173,  134,  180,  196,
      0,   69,  121,   82,  132,   17,   42,  212,
    143,   51,  192,  119,   21,   13,  137,  172,
    186,   60,  211,   47,  237,   46,   25,  254,
     53,  195,  198,    3,  250,   71,  227,  213,
    102,   78,  220,  146,  243,   37,  166,    4,
    190,   97,  178,  251,   45,  100,  159,  165,
    219,   34,  116,   22,  202,  179,  157,   38,
    230,  214,  118,  171,  151,  101,  199,   14,
    158,   98,  156,  117,   96,  231,  148,   92,
     12,  130,  161,  206,  218,  113,  193,  245,
     59,   24,    6,  112,  205,   55,  153,  247,
     88,  128,   36,  229,  170,  120,  210,  145,
    209,   26,    8,  200,  221,  177,   67,   89,
    215,  188,  235,  152,  133,  154,  136,  104,
     31,  204,   99,  241,   63,  164,   62,  109,
      1,  248,  191,  106,  140,   84,  226,  189,
    225,   40,  184,  114,   61,  122,  126,  217,
    183,  224,   93,  162,   87,   58,   83,  255,
     10,  105,   76,   28,  201,    7,   56,   52,
    123,  236,   72,  249,  216,  253,   19,   41,
     44,   48,  135,   27,   79,   29,   94,  238,
      5,   18,  228,  127,  125,   86,  234,   90
    };





typedef struct FlexHashState {
        unsigned char i;
        unsigned char n;
        unsigned char *hashBuffer;
        unsigned int hashLength;
    } FlexHashState;


static void maxigin_flexHashInit( FlexHashState *inState,
                                  unsigned char *inHashBuffer,
                                  int inHashLength ) {
    unsigned int j;
    unsigned char i, n;
    unsigned int hashLength = (unsigned)inHashLength;
    unsigned int jBits;
    
    /* initialize output with the hash of a 0-byte string */
    i = 0;
    n = 0;
    
    for( j=0; j < hashLength; j++ ) {
        /* walk through table values in order (by incrementing i)
           and use those values to mix with our running n to get
           our next buffer value */

        /* also add in mix of bits from j,
           so that as hash buffer gets longer and
           longer, the init pattern has a much longer repeat cycle length
           (the current tested repeat cycle length is 65536) */
        
        jBits = j;
        while( jBits > 255 ) {
            jBits = ( jBits >> 8 ) ^ ( jBits & 0xFF );
            }

        /*
        n = ( n + flexHashTable[i] + jBits ) & 0xFF;
        
        inHashBuffer[j] = flexHashTable[ n ];
        
        i = ( i + 1 ) & 0xFF;
        */
        
        n = n ^ flexHashTable[i] ^ jBits;
        
        inHashBuffer[j] = n;

        /* don't assume char is not larger than 8 bits
           so we can't count on wrap-around behavior above 255 */
        i = ( i + 1 ) & 0xFF;
        }

    /* push n forward one more time, so n is not equal to the first
       byte in our buffer in the inHashLength=1 case */
    n = n ^ flexHashTable[i];
        
    /* fixme:  is there some kind of rotation thing we can do here
       to extend the length of the period */
    
    inState->i = 0;
    inState->n = n;
    inState->hashBuffer = inHashBuffer;
    inState->hashLength = hashLength;
    }

#include <stdio.h>

static void maxigin_flexHashAdd( FlexHashState *inState,
                                 const unsigned char *inBytes,
                                 int inNumBytes ) {
    
    unsigned int j;
    unsigned char i, n;
    unsigned int b;
    unsigned int numBytes = (unsigned)inNumBytes;
    unsigned int hashLength = inState->hashLength;
    unsigned char *hashBuffer = inState->hashBuffer;

    
    i = inState->i;
    n = inState->n;

    
    /* mix in each byte of our input */
    for( b=0; b< numBytes; b++ ) {
        /* mix byte and byte count into our n state */
        n = n ^ inBytes[b] ^ i;
        
        for( j=0; j < hashLength; j++ ) {

            n = flexHashTable[ hashBuffer[j] ^ n ];
                
            hashBuffer[j] = n;   
            }

        /* i keeps track of how many bytes we've processed, mod 256 */
        i = ( i + 1 )  & 0xFF;
            
        /* push n forward one more time, so it's not equal to the last
           byte in our hash buffer (which is also the first/only byte, in the
           case of 1-byte hashes) */
        n = flexHashTable[ n ];
        }
    
    inState->i = i;
    inState->n = n;
    }



static void maxigin_flexHash( const unsigned char *inBytes, int inNumBytes,
                              unsigned char *inHashBuffer, int inHashLength ) {
    FlexHashState s;
    maxigin_flexHashInit( &s, inHashBuffer, inHashLength );

    maxigin_flexHashAdd( &s, inBytes, inNumBytes );
    }



#include <stdio.h>

#define BUFF_LEN 1024
unsigned char buffer[ BUFF_LEN ];

#define HASH_LEN 20
unsigned char hashBuffer[ HASH_LEN ];
unsigned char hashBufferB[ HASH_LEN ];
int hitCounts[65536];


static int hotBitCount( unsigned char *inBuffer, int inNumBytes ) {
    int count = 0;
    int i, k;
    for( i=0; i<inNumBytes; i++ ) {
        unsigned char b = inBuffer[i];

        for( k=0; k<8; k++ ) {
            if( b & ( 1 << k ) ) {
                count++;
                }
            }
        }
    /*
    printf( "Byte string " );
    for( i =0; i<inNumBytes; i++ ) {
        printf( "%08b", inBuffer[i] );
        }
    printf( " has %d hot bits\n", count );
    */
    return count;
    }


/* flip kth bit of inBuffer and store result in inResultBuffer */
static void flipBit( unsigned char *inBuffer, int inNumBytes,
                     int inK, unsigned char *inResultBuffer ) {
    int byte = inK / 8;
    int bit = inK % 8;
    int i;
    for( i =0; i<inNumBytes; i++ ) {
        inResultBuffer[i] = inBuffer[i];
        }
    inResultBuffer[byte] = inResultBuffer[byte] ^ ( 1 << bit );

    /*
    printf( "Flipping %dth bit\n", inK );
    for( i =0; i<inNumBytes; i++ ) {
        printf( "%08b", inBuffer[i] );
        }
    printf( "\n" );
    for( i =0; i<inNumBytes; i++ ) {
        printf( "%08b", inResultBuffer[i] );
        }
    printf( "\n" );
    */
    }


/* for 4-bit nibbles */
static char nibbleToHex( unsigned char inNibble ) {
    if( inNibble < 10 ) {
        return (char)( '0' + inNibble );
        }
    else {
        return (char)( 'A' + ( inNibble - 10 ) );
        }
    }



void maxigin_hexEncode( const unsigned char *inBytes, int inNumBytes,
                        char *inHexBuffer ) {
    int stringPos = 0;
    int i;
    
    for( i=0; i < inNumBytes; i++ ) {
        unsigned char b = inBytes[i];
        unsigned char upper = ( b >> 4 );
        unsigned char lower = b & 0x0F;

        inHexBuffer[ stringPos ] = nibbleToHex( upper );

        stringPos++;
        
        inHexBuffer[ stringPos ] = nibbleToHex( lower );
        
        stringPos++;
        }
    
    inHexBuffer[stringPos] = '\0';
    }



    
#define hashTestSize  33000
unsigned char hashInputBuffer[ hashTestSize ];
unsigned char hashTestBuffer[hashTestSize];
char hexBuffer[ hashTestSize * 2 + 1 ];

int main( int inNumArgs, const char **inArgs ) {

    if( 0 ) {
        int longestFound = 0;
        int foundA;
        int hexLen = sizeof( hexBuffer ) - 1;
        int a, b, o;
        
        maxigin_flexHash( 0, 0, hashTestBuffer, hashTestSize );
        
        maxigin_hexEncode( hashTestBuffer, hashTestSize, hexBuffer );

        for( a=0; a<hexLen; a++ ) {
            for( b=a+1; b<hexLen; b++ ) {
                if( hexBuffer[a] == hexBuffer[b] ) {
                    o = 1;
                    while( hexBuffer[a+o] == hexBuffer[b+o] ) {
                        o++;

                        if( hexBuffer[b+o] == '\0' ) {
                            break;
                            }  
                        }
                    if( o > longestFound ) {
                        longestFound = o;
                        foundA = a;
                        }
                    }
                }
            }
        
        if( longestFound > 0 ) {
            char old = hexBuffer[foundA + longestFound];
            hexBuffer[foundA + longestFound] = '\0';
            
            printf( "Found longest repeat string: %d", longestFound );
            printf(  "%s\n", &( hexBuffer[foundA] ) );
            hexBuffer[foundA + longestFound] = old;
            }


        }

    
    if( 1 ) {
        
    FlexHashState s;

    #define hashSize 12

    unsigned char hashBuffer[ hashSize ];
    
    maxigin_flexHashInit( &s, hashBuffer, hashSize );

    #define blockSize 1
    unsigned char block[ blockSize];
    int i;
    for( i=0; i<blockSize; i++ ) {
        block[i] = 0;
        }
    
    while( 1 ) {

        maxigin_flexHashAdd( &s, block, blockSize );

        fwrite( hashBuffer, 1, hashSize, stdout );  
        }
        }


    if( 0 ) {
        int i, j, b, h;
        

        for( h=1; h< HASH_LEN; h++ ) {
            
            int hashBytes = h;
            int hotCount;
            int colCount = 0;
        
            for( i=0; i<256; i++ ) {
                unsigned char c1[1];
                c1[0] = (unsigned char)i;
        
                maxigin_flexHash( c1, 1,
                                  hashBuffer, hashBytes );

                /*
                printf( "Hash of %d = ", i );
                for( b=0; b< hashBytes; b++ ) {
                    printf( "%02X", hashBuffer[ b ] );
                    }
                printf( "\n" );
                */
            
                for( j=0; j<256; j++ ) {
                    if( i == j ) {
                        continue;
                        }
                    unsigned char c2[1];
                
                    c2[0] = (unsigned char)j;
                
                    maxigin_flexHash( c2, 1,
                                      hashBufferB, hashBytes );
                    /* xor hashes */
                    for( b=0; b< hashBytes; b++ ) {
                        hashBufferB[b] = hashBuffer[b] ^ hashBufferB[b];
                        }
                
                    hotCount = hotBitCount( hashBufferB, hashBytes );
                    if( hotCount == 0 ) {
                        colCount++;
                        }
                    }
                }

            printf( "1 into %d hash bytes:  %d collisions\n",
                    hashBytes, colCount );

            }
        }


    
    if( 0 ) {
        int i, j, b, h;

        unsigned char hashResults[65536][ HASH_LEN ];
        unsigned char xorResult[ HASH_LEN ];
        
        for( h=2; h<= HASH_LEN; h++ ) {
            
            int hashBytes = h;
            int hotCount;
            int colCount = 0;
        
            for( i=0; i<65536; i++ ) {
                unsigned char c1[2];
                c1[0] = (unsigned char)( ( i >> 8 ) & 0xFF );
                c1[1] = (unsigned char)( i & 0xFF );
        
                maxigin_flexHash( c1, 2,
                                  hashResults[i], hashBytes );
                }
            
            for( i=0; i<65536; i++ ) {
                for( j=0; j<65536; j++ ) {
                    char anyDiffer = 0;
                    
                    if( i == j ) {
                        continue;
                        }
                    
                    if( hashResults[i][0] != hashResults[j][0] ) {
                        /* first byte differs */
                        continue;
                        }
                    
                    for( b=1; b< hashBytes; b++ ) {
                        if( hashResults[i][b] != hashResults[j][b] ) {
                            anyDiffer = 1;
                            break;
                            }
                        }
                    if( ! anyDiffer ) {
                        colCount++;
                        }
                    }
                }

            printf( "2 into %d hash bytes:  %d collisions\n",
                    hashBytes, colCount );

            }
        }

    
    
    if( 0 ) {
        
    int numRead;
    FILE *f;
    FlexHashState s;
    int i, j, k, b;
    int colCount = 0;

    for( i=0; i<65536; i++ ) {
        hitCounts[i] = 0;
        }
    #define maxNumHot 65
    int hotCountHits[maxNumHot];
    int hotCountTrials = 0;

    for( i=0; i<maxNumHot; i++ ) {
        hotCountHits[i] = 0;
        }
    

    for( i=0; i<256; i++ ) {
        for( j=0; j<256; j++ ) {
            unsigned char c1[2];
            unsigned char c2[2];
            int hashBytes = 8;
            int hotCount;
        
            c1[0] = (unsigned char)i;
            c1[1] = (unsigned char)j;
        
            maxigin_flexHash( c1, 2,
                              hashBuffer, hashBytes );
            for( k=0; k<8; k++ ) {
            
                flipBit( c1, 2, k, c2 );

                /* rehash */
                maxigin_flexHash( c2, 2,
                                  hashBufferB, hashBytes );

                /* xor hashes */
                for( b=0; b< hashBytes; b++ ) {
                    hashBufferB[b] = hashBuffer[b] ^ hashBufferB[b];
                    }
                hotCount = hotBitCount( hashBufferB, hashBytes );
                hotCountHits[ hotCount ] ++;
                hotCountTrials++;

                if( hotCount == 0 ) {
                    printf( "Zero bits changed for %08b%08b %d-bit flipped to "
                            "%08b%08b\n", c1[0], c1[1], k, c2[0], c2[1] );
                    }
                }
            }
        }
    


    for( i=0; i<maxNumHot; i++ ) {
        printf( "%d: %d  (%f)\n", i, hotCountHits[i],
                (float)( hotCountHits[i] ) / (float)hotCountTrials );
        }

        }
    
    
            
    /*
    for( i=0; i<256; i++ ) {
        for( j=0; j<256; j++ ) {
            for( k=0; k<256; k++ ) {
                unsigned char c[3];
                c[0] = (unsigned char)i;
                
                c[1] = (unsigned char)j;
                c[2] = (unsigned char)k;
                
                maxigin_flexHash( c, 3,
                                  hashBuffer, 2 );
                
                hitCounts[ hashBuffer[0] << 8 | hashBuffer[1] ] ++;
                
                }
        
            }
                
        }
    int biggest = 0;

    #define histBins 324
    
    int histogram[histBins];

    for( i=0; i<histBins; i++ ) {
        histogram[i] = 0;
        }
    
    for( i=0; i<65536; i++ ) {
        if( hitCounts[i] > 1 ) {
            colCount += hitCounts[i] - 1;
            if( hitCounts[i] > biggest ) {
                biggest = hitCounts[i];
                }
            histogram[ hitCounts[i] ] ++;
            }
        }
    printf( "%d total collisions, most in one bin = %d\n", colCount, biggest );

    printf( "Histogram:\n" );
    for( i=2; i<=biggest; i++ ) {
        int num = histogram[i];
        printf( "%d:  %d", i, histogram[i] );
        printf( "\n" );
        }
    */
    
    
    /*
            printf( "Hash of %d = %02X\n", i, hashBuffer[i][0] );
            }
    
    
    
    for( i=0; i<256; i++ ) {
        for( j=0; j<256; j++ ) {
            if( i != j ) {
                if( hashBuffer[i][0] == hashBuffer[j][0] ) {
                    colCount++;
                    }
                }
            }
        }
    printf( "%d collisions\n", colCount );
    
    */
        

    if( 0 ) {
        FILE *f;
        int numRead;
        int i;
        FlexHashState s;
        
    
    if( inNumArgs != 2 ) {
        return 1;
        }
    f = fopen( inArgs[1], "rb" );

    if( f == NULL ) {
        return 1;
        }

    maxigin_flexHashInit( &s, hashBuffer, HASH_LEN );

    
    numRead = fread( buffer, BUFF_LEN, 1, f );

    while( numRead > 0 ) {
        maxigin_flexHashAdd( &s, buffer, numRead );
        numRead = fread( buffer, BUFF_LEN, 1, f );
        }

    printf( "Hash: " );
    
    for( i=0; i<HASH_LEN; i++ ) {
        
        printf( "%02X", hashBuffer[i] );
        }
    printf( "\n" );
    
    
    fclose( f );
        }


    if( 0 ) {
        /* test raw speed */
        int i;
        FlexHashState s;
        unsigned char data[20];
        int numRounds = 1000000;
        
        maxigin_flexHashInit( &s, hashBuffer, HASH_LEN );

        
        for( i=0; i<numRounds; i++ ) {
            maxigin_flexHashAdd( &s, data, sizeof( data ) );
            }

        printf( "Hashed %d bytes\n", sizeof( data ) * numRounds );
        }
    
    return 0;
    }
