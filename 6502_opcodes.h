#ifndef OPCODES_6502_H
#define OPCODES_6502_H

// i wanted this list to be an enum but doing defines makes it easier to sort

// naming convention:
// INS_(insn_name)_(addressing mode)

#define INS_BRK       0x00

// OR accumulator
#define INS_ORA_IND_X 0x01
#define INS_ORA_ZPG   0x05
#define INS_ORA_IMM   0x09
#define INS_ORA_ABS   0x0D
#define INS_ORA_IND_Y 0x11
#define INS_ORA_ZPG_X 0x15
#define INS_ORA_ABS_Y 0x19
#define INS_ORA_ABS_X 0x1D

// arithmetic shift left by 1
#define INS_ASL_ACC   0x0A
#define INS_ASL_ZPG   0x06
#define INS_ASL_ZPG_X 0x16
#define INS_ASL_ABS   0x0E
#define INS_ASL_ABS_X 0x1E

// push status onto stack
#define INS_PHP       0x08

// branch if plus
#define INS_BPL       0x10

// clear carry flag
#define INS_CLC       0x18

// jump to subroutine
#define INS_JSR       0x20

// bit test on accumulator (AND but result isnt stored)
#define INS_BIT_ZPG   0x24
#define INS_BIT_ABS   0x2C

// AND accumulator
#define INS_AND_IMM   0x29
#define INS_AND_ZPG   0x25
#define INS_AND_ZPG_X 0x35
#define INS_AND_ABS   0x2D
#define INS_AND_ABS_X 0x3D
#define INS_AND_ABS_Y 0x39
#define INS_AND_IND_X 0x21
#define INS_AND_IND_Y 0x31

// rotate left by one
#define INS_ROL_ACC   0x2A
#define INS_ROL_ZPG   0x26
#define INS_ROL_ZPG_X 0x36
#define INS_ROL_ABS   0x2E
#define INS_ROL_ABS_X 0x3E

#endif
