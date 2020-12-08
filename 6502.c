#include "vcs.h"
#include "6502.h"

#ifndef TARGET_N64
#include <stdio.h>
#else
#define printf(...)
#endif

// the 3 registers
u16 pc = 0;
u8 sp = 0;
// 7  bit  0
// ---- ----
// NVss DIZC
// |||| ||||
// |||| |||+- Carry
// |||| ||+-- Zero
// |||| |+--- Interrupt Disable
// |||| +---- Decimal
// ||++------ No CPU effect, see: the B flag
// |+-------- Overflow
// +--------- Negative
u8 status = 0;
u8 x = 0;
u8 y = 0;
u8 a = 0;

u32 cycle_timer = 0;



#define UPDATE_STATUS(cond, flag) {if(cond) set_status(flag); else clear_status(flag);}

#define CYC(x) (x - 1)
#define ZPG(x) ((x) & 0xFF)
#include "6502.h"
#include "vcs_core.inc.c"


// status helpers
void set_status(u32 flag) {
    status |= (1 << flag);
}
u8 get_status(u32 flag) {
    return (status >> flag) & 1;
}
void clear_status(u32 flag) {
    status &= ~(1 << flag);
}

// TODO: figure out if we even want this function!
// void upd_status(status_t flag) {
//     status_t tmp;
//     u32 shift = 0;
//     while (tmp != NO_STATUS) {
//         if (shift != CARRY) { // TODO: figure out how to handle carries here
//             if (tmp & STATUS_MASK) {
//                 if ((tmp & STATUS_MASK) == SET) {
//                     set_status(shift);
//                 }
//                 else if ((tmp & STATUS_MASK) == SET) {
//                     clear_status(shift);
//                 }
//             }
//         }
//         tmp >>= STAT_FLAG_LENGTH;
//         shift++;
//     }
// }

// stack helpers
void s_push(u8 x) {
    mem_write_u8(sp--, x);
}
u8 s_pop(void) {
    return mem_read_u8(++sp);
}

void ins_brk(VOID) { // 0x00
    set_status(INTERRUPT);
}

void ins_php(VOID) { // 0x08
    s_push(status);
}

void ins_ora(u8 opcode, u8 hi, u8 lo, status_t status) {
    u8 mem;

    switch (opcode) {
        case 0x01:
            mem = mem_read_u8(mem_read_u16((x + hi) & 0xFF));
            break;
        case 0x05:
            mem = mem_read_u8((x + hi) & 0xFF);
            break;
        case 0x09:
            mem = hi;
            break;
        case 0x0D:
            mem = mem_read_u8(TO_U16(hi, lo));
            break;
        case 0x11:
            mem = mem_read_u8(y + mem_read_u8(TO_U16(hi, lo)));
            break;
        case 0x15:
            mem = mem_read_u8(x + hi);
            break;
        case 0x19:
            mem = mem_read_u8(y + TO_U16(hi, lo));
            break;
        case 0x1D:
            mem = mem_read_u8(x + TO_U16(hi, lo));
            break;
    }

    a |= mem;
    UPDATE_STATUS(a >> 7, NEGATIVE);
    UPDATE_STATUS(!a, ZERO);
}

void ins_asl(u8 opcode, u8 hi, u8 lo, status_t status) {
    u8 temp;

    switch (opcode) {
        case 0x06:
            temp = mem_read_u8(hi);
            break;
        case 0x0A:
            temp = a;
            break;
        case 0x0E:
            temp = mem_read_u8(TO_U16(hi, lo));
            break;
        case 0x16:
            temp = mem_read_u8(x + hi);
            break;
        case 0x1E:
            temp = mem_read_u8(x + TO_U16(hi, lo));
            break;
    }
    UPDATE_STATUS(temp >> 7, CARRY);
    temp <<= 1;
    switch (opcode) {
        case 0x06:
            mem_write_u8(hi, temp);
            break;
        case 0x0A:
            a = temp;
            break;
        case 0x0E:
            mem_write_u8(TO_U16(hi, lo), temp);
            break;
        case 0x16:
            mem_write_u8(x + hi, temp);
            break;
        case 0x1E:
            mem_write_u8(x + TO_U16(hi, lo), temp);
            break;
    }
    UPDATE_STATUS(temp >> 7, NEGATIVE);
    UPDATE_STATUS(!temp, ZERO);
}

void ins_bpl(UNUSED u8 opcode, u8 offset, UNUSED u8 byte_2, status_t status) {
    if (get_status(NEGATIVE) == 0) {
        if ((offset >> 7) == 1)
            pc -= ((~(offset) - 1) & 0xFF);
        else 
            pc += offset;
        // pc = 0;
        set_status(IS_BRANCH);
        clear_status(NEGATIVE);
    }
}

void ins_clc(VOID) {
    clear_status(CARRY);
}

void ins_jsr(UNUSED u8 opcode, u8 hi, u8 lo, status_t status) {
    // s_push(status);
    s_push((pc + 2) >> 16);
    s_push((pc + 2) & 0xFF);
    pc = TO_U16(hi, lo);
}

void ins_and(u8 opcode, u8 hi, u8 lo, status_t status) {
    u8 mem;

    switch (opcode) {
        case 0x21:
            mem = mem_read_u8(mem_read_u16((x + hi) & 0xFF));
            break;
    }

    a &= mem;
    UPDATE_STATUS(a >> 7, NEGATIVE);
    UPDATE_STATUS(!a, ZERO);
}

void ins_bit(u8 opcode, u8 hi, u8 lo, status_t status) {
    u8 mem;
    u8 acc = a;
    switch (opcode) {
        case 0x24:
            mem = mem_read_u8(hi);
            break;
    }
    UPDATE_STATUS((mem >> 6) & 1, OVERFLOW);
    UPDATE_STATUS((mem >> 7) & 1, NEGATIVE);
    UPDATE_STATUS(mem & acc, ZERO);
}

Insn_6502 insn_array[] = {
    /* 0x00 */ {0x00, ins_brk, "BRK", 1, SET_STATUS(I), 7},
    /* 0x01 */ {0x01, ins_ora, "ORA", 2, SET_STATUS(N) | SET_STATUS(Z), 6},
    INSN_NULL,
    INSN_NULL,
    INSN_NULL,
    /* 0x05 */ {0x05, ins_ora, "ORA", 2, SET_STATUS(N) | SET_STATUS(Z), 3},
    /* 0x06 */ {0x06, ins_asl, "ASL", 2, SET_STATUS(N) | SET_STATUS(Z) | SET_STATUS(C), 5},
    INSN_NULL,
    /* 0x08 */ {0x08, ins_php, "PHP", 1, NO_STATUS, 3},
    /* 0x09 */ {0x09, ins_ora, "ORA", 2, SET_STATUS(N) | SET_STATUS(Z), 2},
    /* 0x0A */ {0x0A, ins_asl, "ASL", 1, SET_STATUS(N) | SET_STATUS(Z) | SET_STATUS(C), 2},
    INSN_NULL,
    INSN_NULL,
    /* 0x0D */ {0x0D, ins_ora, "ORA", 3, SET_STATUS(N) | SET_STATUS(Z), 4},
    /* 0x0E */ {0x0E, ins_asl, "ASL", 3, SET_STATUS(N) | SET_STATUS(Z) | SET_STATUS(C), 6},
    INSN_NULL,
    /* 0x10 */ {0x10, ins_bpl, "BPL", 2, NO_STATUS, 2},
    /* 0x11 */ {0x11, ins_ora, "ORA", 2, SET_STATUS(N) | SET_STATUS(Z), 5},
    INSN_NULL,
    INSN_NULL,
    INSN_NULL,
    /* 0x15 */ {0x15, ins_ora, "ORA", 2, SET_STATUS(N) | SET_STATUS(Z), 4},
    /* 0x16 */ {0x16, ins_asl, "ASL", 2, SET_STATUS(N) | SET_STATUS(Z) | SET_STATUS(C), 6},
    INSN_NULL,
    /* 0x18 */ {0x18, ins_clc, "CLC", 1, CLEAR_STATUS(C), 2},
    /* 0x19 */ {0x19, ins_ora, "ORA", 3, SET_STATUS(N) | SET_STATUS(Z), 4},
    INSN_NULL,
    INSN_NULL,
    INSN_NULL,
    /* 0x1D */ {0x1D, ins_ora, "ORA", 3, SET_STATUS(N) | SET_STATUS(Z), 4},
    /* 0x1E */ {0x1E, ins_asl, "ASL", 3, SET_STATUS(N) | SET_STATUS(Z) | SET_STATUS(C), 7},
    INSN_NULL,
    /* 0x20 */ {0x20, ins_jsr, "JSR", 3, NO_STATUS, 6},
    /* 0x21 */ {0x21, ins_and, "AND", 2, SET_STATUS(N) | SET_STATUS(Z), 6},
    INSN_NULL,
    INSN_NULL,
    /* 0x24 */ {0x24, ins_bit, "BIT", 2, SET_STATUS(Z), 3},


};

int run_one_cycle(u8 *game_rom) {
    if (cycle_timer > 0) cycle_timer--;
    else {
        printf("%04X: %02X %s %02X\n", pc, game_rom[pc], insn_array[game_rom[pc]].nm, game_rom[pc + 1]);
        if (game_rom[pc] == 0x02) return -1;
        if (insn_array[game_rom[pc]].func != NULL) {
            insn_array[game_rom[pc]].func(
                game_rom[pc],
                game_rom[pc + 1],
                game_rom[pc + 2],
                insn_array[game_rom[pc]].status
                );
            cycle_timer += CYC(insn_array[game_rom[pc]].cycles);
        } else {
            printf("Invalid Instruction\n");
        }
        printf("vars: a:%02X x:%02X y:%02X sp:%02X status: %02X\n", a, x, y, sp, status);
        if (get_status(IS_BRANCH)) {
            clear_status(IS_BRANCH);
            // printf("%04X\n", pc);
        } else {
            pc += insn_array[game_rom[pc]].len;
        }
    }
    return 0;
}

int main(void) {
    vcs_boot(vcs_rom);
    while (1) {
        if (run_one_cycle(vcs_rom) != 0) break;
    }
}




