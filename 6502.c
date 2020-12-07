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

#define UPDATE_STATUS(cond, flag) {if(cond) set_status(flag); else clr_status(flag);}

#define CYC(x) (x - 1)
#define ZPG(x) ((x) & 0xFF)
#include "6502.h"
#include "vcs_core.inc.c"

int ins_cycle_lookup[] = {
	7, 6, 0, 0,   0, 3, 5, 0,   3, 2, 2, 0,   0, 4, 6, 0,
	2, 5, 

};

// status helpers
void set_status(u32 flag) {
	status |= (1 << flag);
}
u8 get_status(u32 flag) {
	return (status >> flag) & 1;
}
void clr_status(u32 flag) {
	status &= ~(1 << flag);
}

// stack helpers
void s_push(u8 x) {
	vcs_mem_write_u8(sp--, x);
}

void ins_brk(VOID) { // 0x00
	set_status(INTERRUPT);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_ora_ind(u8 m, u8 reg) { // 0x01
	a |= vcs_mem_read_u8(m + reg);
	UPDATE_STATUS(a >> 7, NEGATIVE);
	UPDATE_STATUS(!a, ZERO);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_ora_zpg(u8 m, u8 reg) { // 0x05
	a |= m;
	UPDATE_STATUS(a >> 7, NEGATIVE);
	UPDATE_STATUS(!a, ZERO);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_asl_zpg(u8 m, UNUSED u8 reg) { // 0x06
	u32 s = vcs_mem_read_u8(m);
	if (a >> 7) set_status(CARRY);
	a <<= vcs_mem_read_u8(ZPG(m + reg));
	UPDATE_STATUS(a >> 7, NEGATIVE);
	UPDATE_STATUS(!a, ZERO);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_php(VOID) { // 0x08
	s_push(status);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_ora_imm(u8 m, UNUSED u8 reg) { // 0x09
	a |= m;
	UPDATE_STATUS(a >> 7, NEGATIVE);
	UPDATE_STATUS(!a, ZERO);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_asl_acc(VOID) { // 0x0A
	s8 s = vcs_mem_read_u8(m);
	if (a >> 7) set_status(CARRY);
	a <<= 1;
	UPDATE_STATUS(a >> 7, NEGATIVE);
	UPDATE_STATUS(!a, ZERO);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

#define TO_U16(h, l) ((h << 16) | l)

void ins_ora_abs(u8 hi, u8 lo) { // 0x0D
	a |= vcs_mem_read_u8(TO_U16(hi, lo));

	UPDATE_STATUS(a >> 7, NEGATIVE);
	UPDATE_STATUS(!a, ZERO);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_asl_abs(u8 hi, u8 lo) { // 0x0E
	s8 s = vcs_mem_read_u8(TO_U16(hi, lo));
	if (s >> 7) set_status(CARRY);
	s <<= 1;
	vcs_mem_write_u8(TO_U16(hi, lo), s);
	UPDATE_STATUS(s >> 7, NEGATIVE);
	UPDATE_STATUS(!s, ZERO);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_bpl(u8 offset, UNUSED u8 reg) {
	if (get_status(NEGATIVE) == 0) {
		if ((offset >> 7) == 1)
			pc -= ((~(offset) - 1) & 0xFF);
		else 
			pc += offset;
		// pc = 0;
		set_status(IS_BRANCH);
		clr_status(NEGATIVE);
	}
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_ora_abs(u8 hi, u8 lo) { // 0x0D
	a |= vcs_mem_read_u8(TO_U16(hi, lo));

	UPDATE_STATUS(a >> 7, NEGATIVE);
	UPDATE_STATUS(!a, ZERO);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

Insn_6502 insn_array[] = {
	/* 0x00 */ {ins_brk, "BRK", 1},
	/* 0x01 */ {ins_ora_ind, "ORA", 2},
	INSN_NULL,
	INSN_NULL,
	INSN_NULL,
	/* 0x05 */ {ins_ora_zpg, "ORA", 2},
	/* 0x06 */ {ins_asl_zpg, "ASL", 2},
	INSN_NULL,
	/* 0x08 */ {ins_php, "PHP", 1},
	/* 0x09 */ {ins_ora_imm, "ORA", 2},
	/* 0x0A */ {ins_asl_acc, "ASL", 1},
	INSN_NULL,
	INSN_NULL,
	/* 0x0D */ {ins_ora_abs, "ORA", 3},
	/* 0x0E */ {ins_asl_abs, "ASL", 3},
	INSN_NULL,
	/* 0x10 */ {ins_bpl, "BPL", 2},
	/* 0x11 */ {ins_ora_ind_y, "ORA", 2},
};

int run_one_cycle(void) {
	if (cycle_timer > 0) cycle_timer--;
	else {
		printf("%04X: %02X %s %02X\n", pc, vcs_rom[pc], insn_array[vcs_rom[pc]].nm, vcs_rom[pc + 1]);
		if (vcs_rom[pc] == 0) return -1;
		if (insn_array[vcs_rom[pc]].func != NULL)
			insn_array[vcs_rom[pc]].func(vcs_rom[pc + 1], vcs_rom[pc + 2]);
		else {
			printf("Invalid Instruction\n");
		}
		printf("vars: a:%02X x:%02X y:%02X sp:%02X status: %02X\n", a, x, y, sp, status);
		if (get_status(IS_BRANCH)) {
			clr_status(IS_BRANCH);
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




