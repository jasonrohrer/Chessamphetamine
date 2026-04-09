#ifndef MEMORY_REGISTER_INCLUDED
#define MEMORY_REGISTER_INCLUDED

#define REGISTER_VAL_MEM( x )  \
    maxigin_initRegisterStaticMemory( &x, sizeof(x), #x )

#define REGISTER_ARRAY_MEM( a ) \
    maxigin_initRegisterStaticMemory( a, sizeof(a), #a )

#endif
