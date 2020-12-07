#include "vcs.h"

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

void set_status(u32 flag) {
	status |= (1 << flag);
}
void clr_status(u32 flag) {
	status &= ~(1 << flag);
}


void ins_brk(VOID) { // 0x00
	set_status(INTERRUPT);
	cycle_timer = CYC(2);
}

void ins_ora_ind(u8 m, u8 *reg) { // 0x01
	a |= vcs_mem_read_u8(m + *reg);
	if (a >> 7) set_status(NEGATIVE);
	if (!a) set_status(ZERO);
}

void ins_ora_zpg(u8 m, u8 *reg) { // 0x05
	a |= vcs_mem_read_u8(ZPG(m + *reg));
	if (a >> 7) set_status(NEGATIVE);
	if (!a) set_status(ZERO);
}

void ins_asl_zpg(u8 m, UNUSED u8 *reg) { // 0x06
	u32 s = vcs_mem_read_u8(m);
	if (a >> 7) set_status(CARRY);
	a += vcs_mem_read_u8(ZPG(m + *reg));
	if (a >> 7) set_status(NEGATIVE);
	if (!a) set_status(ZERO);
}

void ins_php(VOID) {
	// s_push(status);
}


void (*ins_lookup[])(u8, u8 *) = {
	ins_brk, ins_ora_ind, NULL, NULL, NULL,
	ins_ora_zpg, ins_asl_zpg, NULL,
	ins_php,
};



void 



