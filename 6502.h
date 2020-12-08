#ifndef H_6502
#define H_6502

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;

#define NULL (void*)0
#define TRUE 1
#define FALSE 0

#define UNUSED __attribute__((unused))

#define C 0
#define Z 1
#define I 2
#define D 3
#define IS_BRANCH 4
#define V 6
#define N 7

#define CARRY C
#define ZERO Z
#define INTERRUPT I
#define DECIMAL D
#define OVERFLOW V
#define NEGATIVE N

#define SET 1
#define CLEAR 2
#define SET_STATUS(x) (SET << x)
#define CLEAR_STATUS(x) (CLEAR << x)
#define NO_STATUS 0

typedef UNUSED u16 status_t;
#define STAT_FLAG_LENGTH 2
#define STATUS_MASK 3

typedef struct {
    u8 opcode;
    void (*func)(u8 opcode, u8 hi, u8 lo, status_t status);
    char nm[4];
    u8 len;
    status_t status;
    u8 cycles;
} Insn_6502;

#define INSN_NULL {0x00, NULL, "NIL", 0, 0, 0}

// helpers
#define TO_U16(h, l) ((h << 16) | l)
#define VOID UNUSED u8 opcode, UNUSED u8 hi, UNUSED u8 lo, UNUSED status_t status

#endif