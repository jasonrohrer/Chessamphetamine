
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
        int j;
        unsigned char n;
        unsigned char *hashBuffer;
        int hashLength;
        unsigned char lastInputByte;
    } FlexHashState;


#include <stdio.h>

static void maxigin_flexHashInit( FlexHashState *inState,
                                  unsigned char *inHashBuffer,
                                  int inHashLength ) {

    /*
      The following code inits the hash buffer with the following properties:

      1. Each freshly inited has buffer of a different length contains
         different values.

      2. Every freshly inited has buffer starts with the byte 0x77

      3. For very long hash buffers, the repeat cycle length of data in the
         fresly inited buffer is very very long.  This has been tested up
         to buffers with 100,000,000 bytes with no cycling.
    */
    
    unsigned int j;
    unsigned char i, n, k, m, index;

    unsigned int run;
    
    int hashLength = inHashLength;
    unsigned int jBits;
    unsigned char jBitsChar;
    
    i = 0;
    k = 199;
    n = 17;
    m = 107;
    
    /* zero our buffer to start */
    for( j=0; j < hashLength; j++ ) {
        inHashBuffer[j] = 0;
        }

    /* run twice, and xor second run into bytes from first
       we xor into our all-0 buffer in the first run */
    for( run=0; run<2; run++ ) {
        
        /* offset each run by 1, in case anything about the cycling
           of our variables lines up perfectly with hashLength.
           This also means that we only do one run if hashLength = 1 */
        
        for( j=run; j < hashLength; j++ ) {
            /* walk through table values in order (by incrementing i)
               and use those values to mix with our running n, plus our
               current buffer value (0 at first, or what we computed last run
               later to get our next buffer value */

            /* also add in mix of bits from j, which makes our basic
               repeat cycle much longer.
               The pattern of jBits is more complex than a counter from 0..255
               that simply wraps around.  */
        
            jBits = j;
            while( jBits > 255 ) {
                jBits = ( jBits >> 8 ) ^ ( jBits & 0xFF );
                }
            jBitsChar = (unsigned char)jBits;
            
            /* we increment i by 1 below, which walks through every value in the
               table in order.  But we also phase shift this walk according to k
               and m, and whenever k or m increments, this phase shift changes
               dramatically, due to the flexHashTable lookups.
           
               The result is that we periodically jump to a different spot in
               the table and start incrementally walking from there,
               which makes our cycle period extremely long. */
        

            /* don't assume char is not larger than 8 bits
               so we can't count on wrap-around behavior above 255 */

            /* fixme:  can save jumps in a variable to avoid lookups...
               speedup? */
            index = (unsigned char)( ( i +
                                       flexHashTable[ k ] +
                                       flexHashTable[m] )
                                     & 0xFF );

        
            n = n ^ flexHashTable[ index ] ^ jBitsChar ^ inHashBuffer[j];
        
            inHashBuffer[j] = n;
        
        
            i = ( i + 1 ) & 0xFF;

        
            /* k increments much more slowly, roughly 1/64 as often as i
               but because we use n to decide when k increments, this happens
               on a very chaotic schedule that doesn't seem to have a pattern */
            if( n == 13 || n == 101 || n == 173  || n == 207 ) {
                //printf( "inc k when j jumped by %d\n", j - lastIncKJ );
                //lastIncKJ = j;
            
                k = ( k + 1 ) & 0xFF;
                }

            /* m increments roughly 1/256 as often as i */
            if( n == 67 ) {
                m = ( m + 1 ) & 0xFF;
                }
            }
        }
    

    /* push n forward one more time, so n is not equal to the first
       byte in our buffer in the inHashLength=1 case */
    n = n ^ flexHashTable[i];

    inState->j = 0;
    inState->n = n;
    inState->hashBuffer = inHashBuffer;
    inState->hashLength = hashLength;
    inState->lastInputByte = 0;
    }

#include <stdio.h>

static void maxigin_flexHashAdd( FlexHashState *inState,
                                 const unsigned char *inBytes,
                                 int inNumBytes ) {
    
    int j;
    unsigned char n;
    int b;
    int numBytes = inNumBytes;
    int hashLength = inState->hashLength;
    unsigned char *hashBuffer = inState->hashBuffer;
    
    n = inState->n;
    j = inState->j;
    
    /* mix in each byte of our hash buffer */


    /*
      This mixing opration is a multi-byte extension of Pearson Hashing,
      with the added twist that n holds the last hash buffer value
      that we touched.  This means that even with input that is all
      zero bytes, we still get complex mixing, as each byte in our buffer
      mixes with the previous byte in the buffer

      The basic form of the mixing operation for a sequence of bytes looks
      like this:

           while( b < numBytes ) {
               n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
               hashBuffer[j] = n;
        
               j++;
        
               if( j >= hashLength ) {
                   j = 0;
                   }
               b++;
               }

       We unroll this below to improve performance, but the actual algorithm
       is the above loop.
    */

    /*
      These limits tell us we're in the middle of our input buffer and
      hash buffer, which allows us to use the unrolled version.
      Note that if our hashLength is < 4, or our input data buffer is shorter
      than 4, we don't use these unrolled versions at all.
    */
    int bLimit = numBytes - 4;
    int jLimit = hashLength - 4;

    b = 0;

    while( b < bLimit ) {
        
        if( j < jLimit ) {
            /* safe to unroll 4x */
            
            n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
            hashBuffer[j] = n;
            j++;
            b++;
            
            n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
            hashBuffer[j] = n;
            j++;
            b++;
            
            n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
            hashBuffer[j] = n;
            j++;
            b++;
            
            n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
            hashBuffer[j] = n;
            j++;
            b++;
            } 
        

        /* back to regular loop for final j values */
        
        n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
        hashBuffer[j] = n;
        
        j++;
        
        if( j >= hashLength ) {
            j = 0;
            }
        b++;
        }


    /* and our most basic loop for final b values, toward end of buffer */
    while( b < numBytes ) {
        n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
        hashBuffer[j] = n;
        
        j++;
        
        if( j >= hashLength ) {
            j = 0;
            }
        b++;
        }
    
        

    inState->j = j;
    inState->n = n;

    if( inNumBytes > 0 ) {
        inState->lastInputByte = inBytes[ inNumBytes - 1 ];
        }
    }


static void maxigin_flexHashFinish( FlexHashState *inState ) {
    unsigned int j, run;
    unsigned char n;
    int hashLength = inState->hashLength;
    unsigned char *hashBuffer = inState->hashBuffer;
    unsigned char lastByte = inState->lastInputByte;
    
    n = inState->n;
    
    /*
      mix last input byte in 4 more times

      We do this with lastByte to improve behavior dramatically in
      few-byte-input cases (if we only have 1 input byte, for example,
      we want it to touch every byte in our hash directly)
     */
    
    for( run=0; run<4; run++ ) {
        for( j=0; j<hashLength; j++ ) {
            n = flexHashTable[ hashBuffer[j] ^ lastByte ^ n ];
            hashBuffer[j] = n;
            }
        }
    }



static void maxigin_flexHash( const unsigned char *inBytes, int inNumBytes,
                              unsigned char *inHashBuffer, int inHashLength ) {
    FlexHashState s;
    maxigin_flexHashInit( &s, inHashBuffer, inHashLength );

    maxigin_flexHashAdd( &s, inBytes, inNumBytes );

    maxigin_flexHashFinish( &s );
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




/*
 * sha1.c
 *
 * Originally witten by Steve Reid <steve@edmweb.com>
 * 
 * Modified by Aaron D. Gifford <agifford@infowest.com>
 *
 * NO COPYRIGHT - THIS IS 100% IN THE PUBLIC DOMAIN
 *
 * The original unmodified version is available at:
 *    ftp://ftp.funet.fi/pub/crypt/hash/sha/sha1.c
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR(S) OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <string.h>

/* Make sure you define these types for your architecture: */
typedef unsigned int sha1_quadbyte;	/* 4 byte type */
typedef unsigned char sha1_byte;	/* single byte type */

/*
 * Be sure to get the above definitions right.  For instance, on my
 * x86 based FreeBSD box, I define LITTLE_ENDIAN and use the type
 * "unsigned long" for the quadbyte.  On FreeBSD on the Alpha, however,
 * while I still use LITTLE_ENDIAN, I must define the quadbyte type
 * as "unsigned int" instead.
 */

#define SHA1_BLOCK_LENGTH	64
#define SHA1_DIGEST_LENGTH	20

/* The SHA1 structure: */
typedef struct _SHA_CTX {
	sha1_quadbyte	state[5];
	sha1_quadbyte	count[2];
	sha1_byte	buffer[SHA1_BLOCK_LENGTH];
} SHA_CTX;

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */

#if __BYTE_ORDER ==  __LITTLE_ENDIAN
#define blk0(i) (block->l[i] = (rol(block->l[i],24)&(sha1_quadbyte)0xFF00FF00) \
	|(rol(block->l[i],8)&(sha1_quadbyte)0x00FF00FF))
#else
#define blk0(i) block->l[i]
#endif

#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
	^block->l[(i+2)&15]^block->l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

typedef union _BYTE64QUAD16 {
	sha1_byte c[64];
	sha1_quadbyte l[16];
} BYTE64QUAD16;

/* Hash a single 512-bit block. This is the core of the algorithm. */
void SHA1_Transform(sha1_quadbyte state[5], sha1_byte buffer[64]) {
	sha1_quadbyte	a, b, c, d, e;
	BYTE64QUAD16	*block;

	block = (BYTE64QUAD16*)buffer;
	/* Copy context->state[] to working vars */
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];
	/* 4 rounds of 20 operations each. Loop unrolled. */
	R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
	R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
	R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
	R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
	R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
	R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
	R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
	R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
	R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
	R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
	R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
	R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
	R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
	R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
	R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
	R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
	R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
	R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
	R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
	R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
	/* Add the working vars back into context.state[] */
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	/* Wipe variables */
	a = b = c = d = e = 0;
}


/* SHA1_Init - Initialize new context */
void SHA1_Init(SHA_CTX* context) {
	/* SHA1 initialization constants */
	context->state[0] = 0x67452301;
	context->state[1] = 0xEFCDAB89;
	context->state[2] = 0x98BADCFE;
	context->state[3] = 0x10325476;
	context->state[4] = 0xC3D2E1F0;
	context->count[0] = context->count[1] = 0;
}

/* Run your data through this. */
void SHA1_Update(SHA_CTX *context, sha1_byte *data, unsigned int len) {
	unsigned int	i, j;

	j = (context->count[0] >> 3) & 63;
	if ((context->count[0] += len << 3) < (len << 3)) context->count[1]++;
	context->count[1] += (len >> 29);
	if ((j + len) > 63) {
	    memcpy(&context->buffer[j], data, (i = 64-j));
	    SHA1_Transform(context->state, context->buffer);
	    for ( ; i + 63 < len; i += 64) {
	        SHA1_Transform(context->state, &data[i]);
	    }
	    j = 0;
	}
	else i = 0;
	memcpy(&context->buffer[j], &data[i], len - i);
}


/* Add padding and return the message digest. */
void SHA1_Final(sha1_byte digest[SHA1_DIGEST_LENGTH], SHA_CTX *context) {
	sha1_quadbyte	i, j;
	sha1_byte	finalcount[8];

	for (i = 0; i < 8; i++) {
	    finalcount[i] = (sha1_byte)((context->count[(i >= 4 ? 0 : 1)]
	     >> ((3-(i & 3)) * 8) ) & 255);  /* Endian independent */
	}
	SHA1_Update(context, (sha1_byte *)"\200", 1);
	while ((context->count[0] & 504) != 448) {
	    SHA1_Update(context, (sha1_byte *)"\0", 1);
	}
	/* Should cause a SHA1_Transform() */
	SHA1_Update(context, finalcount, 8);
	for (i = 0; i < SHA1_DIGEST_LENGTH; i++) {
	    digest[i] = (sha1_byte)
	     ((context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
	}
	/* Wipe variables */
	i = j = 0;
	memset(context->buffer, 0, SHA1_BLOCK_LENGTH);
	memset(context->state, 0, SHA1_DIGEST_LENGTH);
	memset(context->count, 0, 8);
	memset(&finalcount, 0, 8);
}







#include <stdlib.h>
    
#define hashTestSize  107300000
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
            
            printf( "Found longest repeat string: %d,  ", longestFound );
            printf(  "%s\n", &( hexBuffer[foundA] ) );
            hexBuffer[foundA + longestFound] = old;
            }

        FILE *f = fopen( "testResults.txt", "w" );
        fprintf( f, "%s", hexBuffer );
        fclose( f );

        }


    
    if( 0 ) {
        
        int hexLen = sizeof( hexBuffer ) - 1;
        int a, o;
        
        maxigin_flexHash( 0, 0, hashTestBuffer, hashTestSize );
        
        maxigin_hexEncode( hashTestBuffer, hashTestSize, hexBuffer );

        #define patSize  8
        
        char pattern[ patSize + 1 ];

        /* jump into middle and grab a pattern */
        int pos = 1 * ( hexLen / 4 );

        for( int i=0; i<40; i++ ) {
            
            pos +=  (int)( ((double)rand() * 2000)  / (double)RAND_MAX );
        
            for( a=pos; a<pos + patSize; a++ ) {
                pattern[a-pos] = hexBuffer[a];
                }
            pattern[patSize] = '\0';

            int startPos = pos + patSize + 1;
        
            for( a=startPos; a<hexLen - patSize; a++ ) {

                if( hexBuffer[a] == pattern[0] ) {
                    o = 1;
                    while( o < patSize && hexBuffer[a+o] == pattern[o] ) {
                        o++;
                        }
                    if( o == patSize ) {
                        printf( "Found repeat of %s at pos %d\n", pattern, a );
                        break;
                        }
                    }
                }
            }
        FILE *f = fopen( "testResults.txt", "w" );
        fprintf( f, "%s", hexBuffer );
        fclose( f );

        }

    
    if( 0 ) {
        int i;

        for( i=1; i < 80; i++ ) {
            
            maxigin_flexHash( 0, 0, hashTestBuffer, i );
            maxigin_hexEncode( hashTestBuffer, i, hexBuffer );
            printf( "%s\n", hexBuffer );
            }
        }

    
    if( 0 ) {
        maxigin_flexHash( 0, 0, hashTestBuffer, hashTestSize );
        fwrite( hashTestBuffer, 1, hashTestSize, stdout );
        }


    if( 0 ) {
        FlexHashState s;

        #define hashSize 20

        unsigned char hashBuffer[ hashSize ];

        for( int i=0; i<10000000; i++ ) {
            
    
            maxigin_flexHashInit( &s, hashBuffer, hashSize );
            }
        

        }
    


    
    if( 0 ) {

        /* write hash of 0 out to stdout repeatedly
           for input to dieharder */
        
        FlexHashState s;

        #define randHashSize 10

        unsigned char hashBuffer[ randHashSize ];
    
        maxigin_flexHashInit( &s, hashBuffer, randHashSize );

        #define blockSize 1
        unsigned char block[ blockSize];
        int i;
        for( i=0; i<blockSize; i++ ) {
            block[i] = 0;
            }
    
        while( 1 ) {

            maxigin_flexHashAdd( &s, block, blockSize );
            maxigin_flexHashFinish( &s );

            fwrite( hashBuffer, 1, randHashSize, stdout );  
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
                
        for( h=2; h<= HASH_LEN; h++ ) {
            
            int hashBytes = h;
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


    int i, j, k, b;

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
        /* test on file input */

        
        FILE *f;
        unsigned long numRead;
        int i;
        FlexHashState s;
        unsigned long numBytes = 0;
        
    
        if( inNumArgs != 2 ) {
            return 1;
            }
        f = fopen( inArgs[1], "rb" );

        if( f == NULL ) {
            return 1;
            }

        maxigin_flexHashInit( &s, hashBuffer, HASH_LEN );

    
        numRead = fread( buffer, 1, BUFF_LEN, f );

        while( numRead > 0 ) {
            numBytes += numRead;
            
            maxigin_flexHashAdd( &s, buffer, (int)numRead );
            numRead = fread( buffer, 1, BUFF_LEN, f );
            }

        printf( "Hash of %lu bytes: ", numBytes );
    
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
        unsigned int numRounds = 1000000;
        
        maxigin_flexHashInit( &s, hashBuffer, HASH_LEN );

        
        for( i=0; i<numRounds; i++ ) {
            maxigin_flexHashAdd( &s, data, sizeof( data ) );
            }

        printf( "Hashed %lu bytes\n", sizeof( data ) * numRounds );
        }


    if( 0 ) {
        /* avalanche test */
        int i,b;
        #define testSize 1

        int p;
        
        #define hashBuffLen 2000
        #define numBits ( testSize * 8 )
        
        unsigned char hashBuffA[ hashBuffLen ];
        unsigned char hashBuffB[ hashBuffLen ];

        char hexBuff[ hashBuffLen * 2 + 1 ];
        
        int numTrials = 10000;

        int hotCountTotals[ numBits ];

        for( p=0; p< numBits; p++ ) {
            hotCountTotals[p] = 0;
            }
        
        for( i=0; i<numTrials; i++ ) {

            for( b=0; b<testSize; b++ ) {
                hashTestBuffer[b] =
                    (unsigned char)( (double)( rand() * 255.0 ) /
                                     (double)RAND_MAX );
                }
            
            maxigin_flexHash( hashTestBuffer, testSize, hashBuffA, hashBuffLen );

            maxigin_hexEncode( hashBuffA, hashBuffLen, hexBuff );
            //printf( "Before flip hash %s\n", hexBuff );
            
            for( p=0; p< numBits; p++ ) {
                /* change one bit */
                flipBit( hashTestBuffer, testSize, p, hashTestBuffer );

                maxigin_flexHash( hashTestBuffer, testSize,
                                  hashBuffB, hashBuffLen );
                maxigin_hexEncode( hashBuffB, hashBuffLen, hexBuff );
                //printf( "  after flip hash %s\n", hexBuff );
            
                /* xor hashes */
                for( b=0; b< hashBuffLen; b++ ) {
                    hashBuffB[b] = hashBuffA[b] ^ hashBuffB[b];
                    }

                maxigin_hexEncode( hashBuffB, hashBuffLen, hexBuff );
                //printf( "   after xor hash %s\n", hexBuff );

                int count = hotBitCount( hashBuffB, hashBuffLen );

                //printf( "   flip count = %d\n", count );
                
                hotCountTotals[p] += count;

                float percentFlipped =
                    (float)( 100 * count ) /
                    (float)( hashBuffLen * 8 );

                if( percentFlipped > 70 ||
                    percentFlipped < 30 ) {
                    printf( "Percent flipped = %f\n", percentFlipped );
                    }

                
                /* flip back */
                flipBit( hashTestBuffer, testSize, p, hashTestBuffer );
                }
            //printf( "\n\n" );
            
            }

        printf( "For %d trials of %d-length random input strings hashed "
                "into %d bytes, with one bit flipped, average bits "
                "changed for each bit:\n\n", numTrials, testSize, hashBuffLen );

        for( p=0; p< numBits; p++ ) {
            float percentFlipped =
                (float)( 100 * ( hotCountTotals[p] / numTrials ) ) /
                (float)( hashBuffLen * 8 );

            if( percentFlipped > 56 ||
                percentFlipped < 44 ) {
                printf( "%d: %d (%0.1f%%)\n",
                        p, hotCountTotals[p] / numTrials,
                        percentFlipped );
                }
            }
            
        }


    if( 1 ) {
        /* raw speed test, with no waiting for file data */

#define speedBuffSize 5000000

        unsigned char speedBuffer[ speedBuffSize ];

        int numRounds = 100;
        
        int b;
        int i;

        #define speedHashLen 157
        
        unsigned char hashBuff[ speedHashLen ];

        char hexBuff[ speedHashLen * 2 + 1 ];
        
        FlexHashState s;
        SHA_CTX shaState;
        
        for( b=0; b<speedBuffSize; b++ ) {
            speedBuffer[b] =
                (unsigned char)( (double)( rand() * 255.0 ) /
                                 (double)RAND_MAX );
            }

        if( 1 ) {
            /* test flexHash */
            
            maxigin_flexHashInit( &s, hashBuff, speedHashLen );
        
            for( i=0; i<numRounds; i++ ) {
                maxigin_flexHashAdd( &s, speedBuffer, speedBuffSize );
                }

            maxigin_flexHashFinish( &s );
            
            maxigin_hexEncode( hashBuff, speedHashLen, hexBuff );

            printf( "flexHash of %d bytes = %s\n",
                    speedBuffSize * numRounds, hexBuff );
            }
        else {
            /* test sha1 */
            unsigned char digest[ SHA1_DIGEST_LENGTH ];
            char hexDigest[ SHA1_DIGEST_LENGTH * 2 + 1 ];
            
            SHA1_Init( &shaState );

            for( i=0; i<numRounds; i++ ) {
                SHA1_Update( &shaState, speedBuffer, speedBuffSize );
                }

            SHA1_Final( digest, &shaState );

            maxigin_hexEncode( digest, SHA1_DIGEST_LENGTH, hexDigest );

            printf( "SHA1 of %d bytes = %s\n",
                    speedBuffSize * numRounds, hexDigest );
            }
        
        
        }
    
        
    
    
    return 0;
    }
