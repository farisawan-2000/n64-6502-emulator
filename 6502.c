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

#define VOID UNUSED u8 m, UNUSED u8 *reg
#define CYC(x) (x - 1)
#define ZPG(x) ((x) & 0xFF)
#include "6502.h"
#include "vcs_core.inc"

int ins_cycle_lookup[] = {
	7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2
};


// status helpers
void set_status(u32 flag) {
	status |= (1 << flag);
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

void ins_ora_ind(u8 m, u8 *reg) { // 0x01
	a |= vcs_mem_read_u8(m + *reg);
	if (a >> 7) set_status(NEGATIVE);
	if (!a) set_status(ZERO);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_ora_zpg(u8 m, u8 *reg) { // 0x05
	a |= vcs_mem_read_u8(ZPG(m + *reg));
	if (a >> 7) set_status(NEGATIVE);
	if (!a) set_status(ZERO);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_asl_zpg(u8 m, UNUSED u8 *reg) { // 0x06
	u32 s = vcs_mem_read_u8(m);
	if (a >> 7) set_status(CARRY);
	a <<= vcs_mem_read_u8(ZPG(m + *reg));
	if (a >> 7) set_status(NEGATIVE);
	if (!a) set_status(ZERO);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_php(VOID) { // 0x08
	s_push(status);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_ora_imm(u8 m, UNUSED u8 *reg) { // 0x09
	a |= m;
	if (a >> 7) set_status(NEGATIVE);
	if (!a) set_status(ZERO);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void ins_asl_acc(VOID) { // 0x06
	u32 s = vcs_mem_read_u8(m);
	if (a >> 7) set_status(CARRY);
	a <<= 1;
	if (a >> 7) set_status(NEGATIVE);
	if (!a) set_status(ZERO);
	cycle_timer = CYC(ins_cycle_lookup[vcs_rom[pc]]);
}

void (*ins_lookup[])(u8, u8 *) = {
	ins_brk, ins_ora_ind, NULL, NULL, NULL,	ins_ora_zpg, ins_asl_zpg, NULL,	ins_php, ins_ora_imm, ins_asl_acc
};

char ins_str[][5] = {
	"BRK", "ORA", "NIL", "NIL", "NIL", "ORA", "ASL", "NIL",	"PHP", "ORA", "ASL"
};

int ins_len_lookup[] = {
	1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1
};


int run_one_cycle(void) {
	if (cycle_timer > 0) cycle_timer--;
	else {
		printf("%04X: %02X %s\n", pc, vcs_rom[pc], ins_str[vcs_rom[pc]]);
		if (vcs_rom[pc] == 0) return -1;
		ins_lookup[vcs_rom[pc]](vcs_rom[pc + 1], &vcs_rom[pc + 2]);
		printf("vars: a:%02X x:%02X y:%02X sp:%02X\n", a, x, y, sp);
		pc += ins_len_lookup[vcs_rom[pc]];
	}
	return 0;
}

int main(void) {
	vcs_boot(vcs_rom);
	while (1) {
		if (run_one_cycle() != 0) break;
	}
}




