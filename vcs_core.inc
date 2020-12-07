void vcs_boot(u8 *rom) {
	pc = (rom[0xFFC] << 16) | rom[0xFFD];
}


u8 vcs_tia[0x80];
u8 vcs_ram[0x80];
u8 vcs_riot[0x80];
u8 vcs_rom[0x1000];

u8 vcs_mem_read_u8(u32 addr) {
	if (addr < 0x80) {
		return vcs_tia[addr];
	}
	else if (addr < 0x100) {
		return vcs_ram[addr - 0x80];
	}
	else if (addr >= 0x280 && addr < 0x300){
		return vcs_riot[addr - 0x280];
	}
	else if (addr >= 0x1000) {
		return vcs_rom[addr - 0x1000];
	}
}

u16 vcs_mem_read_u16(u32 addr) {
	if (addr < 0x80) {
		return ((u16*)vcs_tia)[addr / 2];
	}
	else if (addr < 0x100) {
		return ((u16*)vcs_ram)[(addr - 0x40) / 2];
	}
	else if (addr >= 0x280 && addr < 0x300){
		return ((u16*)vcs_riot)[(addr - 0x280/2) / 2];
	}
	else if (addr >= 0x1000) {
		return ((u16*)vcs_rom)[(addr - 0x800) / 2];
	}
}


void vcs_mem_write(u32 addr, u8 dat) {
	if (addr < 0x80) {
		vcs_tia[addr] = dat;
	}
	else if (addr < 0x100) {
		vcs_ram[addr - 0x80] = dat;
	}
	else if (addr >= 0x280 && addr < 0x300){
		vcs_riot[addr - 0x280] = dat;
	}
	else if (addr >= 0x1000) {
		vcs_rom[addr - 0x1000] = dat;
	}
}

