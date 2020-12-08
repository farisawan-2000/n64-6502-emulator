#include "vcs.h"
#include "6502.h"

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

#define VOID UNUSED u8 m, UNUSED u8 reg

#define UPDATE_STATUS(cond, flag) {if(cond) set_status(flag); else clear_status(flag);}

#define CYC(x) (x - 1)
#define ZPG(x) ((x) & 0xFF)
#define UPDATE_CYCLE_TIME() cycle_timer = CYC(insn_array[vcs_rom[pc]].cycles);
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

void upd_status(status_t flag) {
    status_t tmp;
    u32 shift = 0;
    while (tmp != NO_STATUS) {
        if (shift != CARRY) { // TODO: figure out how to handle carries here
            if (tmp & STATUS_MASK) {
                if ((tmp & STATUS_MASK) == SET) {
                    set_status(shift);
                }
                else if ((tmp & STATUS_MASK) == SET) {
                    clear_status(shift);
                }
            }
        }
        tmp >>= STAT_FLAG_LENGTH;
        shift++;
    }
}

// stack helpers
void s_push(u8 x) {
    mem_write_u8(sp--, x);
}

void ins_brk(VOID) { // 0x00
    set_status(INTERRUPT);
}

// void ins_ora_ind(u8 m, u8 reg) { // 0x01
//  u16 addr = mem_read_u16((x + m) & 0xFF);
//  a |= mem_read_u8(addr);
//  UPDATE_STATUS(a >> 7, NEGATIVE);
//  UPDATE_STATUS(!a, ZERO);
// }

// void ins_ora_zpg(u8 m, u8 reg) { // 0x05
//  a |= mem_read_u8((x + m) & 0xFF);
//  UPDATE_STATUS(a >> 7, NEGATIVE);
//  UPDATE_STATUS(!a, ZERO);
// }

// void ins_asl_zpg(u8 m, UNUSED u8 reg) { // 0x06
//  u32 s = mem_read_u8(m);
//  UPDATE_STATUS(a >> 7, CARRY);
//  a <<= 1;
//  mem_write_u8(m, a);
//  UPDATE_STATUS(a >> 7, NEGATIVE);
//  UPDATE_STATUS(!a, ZERO);
// }

void ins_php(VOID) { // 0x08
    s_push(status);
}

// void ins_ora_imm(u8 m, UNUSED u8 reg) { // 0x09
//  a |= m;
//  UPDATE_STATUS(a >> 7, NEGATIVE);
//  UPDATE_STATUS(!a, ZERO);
// }

// void ins_asl_acc(VOID) { // 0x0A
//  s8 s = mem_read_u8(m);
//  if (a >> 7) set_status(CARRY);
//  a <<= 1;
//  UPDATE_STATUS(a >> 7, NEGATIVE);
//  UPDATE_STATUS(!a, ZERO);
// }

#define TO_U16(h, l) ((h << 16) | l)

// void ins_ora_abs(u8 hi, u8 lo) { // 0x0D
//  a |= mem_read_u8(TO_U16(hi, lo));

//  UPDATE_STATUS(a >> 7, NEGATIVE);
//  UPDATE_STATUS(!a, ZERO);
// }

// void ins_asl_abs(u8 hi, u8 lo) { // 0x0E
//  s8 s = mem_read_u8(TO_U16(hi, lo));
//  if (s >> 7) set_status(CARRY);
//  s <<= 1;
//  mem_write_u8(TO_U16(hi, lo), s);
//  UPDATE_STATUS(s >> 7, NEGATIVE);
//  UPDATE_STATUS(!s, ZERO);
// }

void ins_bpl(u8 offset, UNUSED u8 reg) {
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

// void ins_ora_ind_y(u8 hi, u8 lo) { // 0x0D
//  a |= mem_read_u8(y + mem_read_u8(TO_U16(hi, lo)));

//  UPDATE_STATUS(a >> 7, NEGATIVE);
//  UPDATE_STATUS(!a, ZERO);
// }

// void ins_ora_zpg_x(u8 m, UNUSED u8 reg) { // 0x0D
//  a |= mem_read_u8(x + m);

//  UPDATE_STATUS(a >> 7, NEGATIVE);
//  UPDATE_STATUS(!a, ZERO);
// }

// void ins_asl_zpg_x(u8 m, UNUSED u8 reg) { // 0x0D
//  s8 tmp = mem_read_u8(x + m);
//  UPDATE_STATUS(tmp >> 7, CARRY);
//  tmp <<= 1;
//  mem_write_u8(x + m, tmp);

//  UPDATE_STATUS(a >> 7, NEGATIVE);
//  UPDATE_STATUS(!a, ZERO);
// }


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
};

int run_one_cycle(void) {
    if (cycle_timer > 0) cycle_timer--;
    else {
        printf("%04X: %02X %s %02X\n", pc, vcs_rom[pc], insn_array[vcs_rom[pc]].nm, vcs_rom[pc + 1]);
        if (vcs_rom[pc] == 0) return -1;
        if (insn_array[vcs_rom[pc]].func != NULL)
            insn_array[vcs_rom[pc]].func(
                vcs_rom[pc],
                vcs_rom[pc + 1],
                vcs_rom[pc + 2],
                insn_array[vcs_rom[pc]].status,
                insn_array[vcs_rom[pc]].cycles);
            UPDATE_CYCLE_TIME();
        else {
            printf("Invalid Instruction\n");
        }
        printf("vars: a:%02X x:%02X y:%02X sp:%02X status: %02X\n", a, x, y, sp, status);
        if (get_status(IS_BRANCH)) {
            clear_status(IS_BRANCH);
            // printf("%04X\n", pc);
        } else {
            pc += insn_array[vcs_rom[pc]].len;
        }
    }
    return 0;
}

int main(void) {
    vcs_boot(vcs_rom);
    while (1) {
        if (run_one_cycle() != 0) break;
    }
}




