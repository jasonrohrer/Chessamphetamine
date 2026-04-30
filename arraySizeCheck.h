#ifndef ARRAY_SIZE_CHECK_H_INCLUDED
#define ARRAY_SIZE_CHECK_H_INCLUDED

/*
  These helper macros allow compile-time verification that a static array
  is exactly the size we expect it to be.

  There are a bunch of a arrays in different parts of the code
  that must all have exactly NUM_CHESS_PIECES elements (one per piece).

  Doing this only catches too many elements at compile time, not too
  few elements (the compiler inits any missing elements with zeros):

   static int pieceValue[ NUM_CHESS_PIECES ] = { 0,
                                                 1,
                                                 3,
                                                 3,
                                                 5,
                                                 9,
                                                 999,
                                                 7,
                                                 2,
                                                 6 };

  So, the best way is to define the array like this, with no explicit
  size:

   static int pieceValue[] = { 0,
                               1,
                               3,
                               3,
                               5,
                               9,
                               999,
                               7,
                               2,
                               6 };

  And then after that, call the provided macro to check the length:


   CHECK_ARRAY_LENGTH( pieceValue,
                      NUM_CHESS_PIECES );

  This will throw compile-time errors if the array is too long or
  too short
*/


/* this gets the number of elements in a static array */
#define ARRAY_LENGTH( array ) \
    ( sizeof( array ) / sizeof( array[0] ) )


/* this throws an error if the array is longer than the desired length */
#define CHECK_ARRAY_TOO_LONG( array, desiredLen ) \
    typedef char ERROR__ ## array ## __contains_more_than__ \
                 ## desiredLen ## __elements[ \
                     ARRAY_LENGTH( array ) > desiredLen ? -1 : 1 ]

#define CHECK_ARRAY_TOO_SHORT( array, desiredLen ) \
    typedef char ERROR__ ## array ## __contains_less_than__ \
                 ## desiredLen ## __elements[ \
                     ARRAY_LENGTH( array ) < desiredLen ? -1 : 1 ]


#define CHECK_ARRAY_LENGTH( array, desiredLen ) \
    CHECK_ARRAY_TOO_LONG ( array, desiredLen );  \
    CHECK_ARRAY_TOO_SHORT( array, desiredLen )
   

#endif
