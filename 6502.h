#ifndef H_6502
#define H_6502

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;

#define C 0
#define Z 1
#define I 2
#define D 3
#define O 6
#define N 7

#define CARRY C
#define ZERO Z
#define INTERRUPT I
#define DECIMAL D
#define OVERFLOW O
#define NEGATIVE N


#endif