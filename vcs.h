// ; VCS.H
// ; Version 1.05, 13/November/2003

#define VERSION_VCS         105

// ;-------------------------------------------------------------------------------

// ; TIA_BASE_ADDRESS
// ; The TIA_BASE_ADDRESS defines the base address of access to TIA registers.
// ; Normally 0, the base address should (externally, before including this file)
// ; be set to $40 when creating 3F-bankswitched (and other?) cartridges.
// ; The reason is that this bankswitching scheme treats any access to locations
// ; < $40 as a bankswitch.

//             IFNCONST TIA_BASE_ADDRESS
#define TIA_BASE_ADDRESS     0
            ENDIF

// ; Note: The address may be defined on the command-line using the -D switch, eg:
// ; dasm.exe code.asm -DTIA_BASE_ADDRESS=$40 -f3 -v5 -ocode.bin
// ; *OR* by declaring the label before including this file, eg:
// ; TIA_BASE_ADDRESS = $40
// ;   include "vcs.h"

// ; Alternate read/write address capability - allows for some disassembly compatibility
// ; usage ; to allow reassembly to binary perfect copies).  This is essentially catering
// ; for the mirrored ROM hardware registers.

// ; Usage: As per above, define the TIA_BASE_READ_ADDRESS and/or TIA_BASE_WRITE_ADDRESS
// ; using the -D command-line switch, as required.  If the addresses are not defined, 
// ; they defaut to the TIA_BASE_ADDRESS.

#define TIA_BASE_READ_ADDRESS TIA_BASE_ADDRESS

#define TIA_BASE_WRITE_ADDRESS TIA_BASE_ADDRESS

// ;-------------------------------------------------------------------------------

//             SEG.U TIA_REGISTERS_WRITE
//             ORG TIA_BASE_WRITE_ADDRESS

//     ; DO NOT CHANGE THE RELATIVE ORDERING OF REGISTERS!
    
#define VSYNC       0x00   // 0000 00x0   Vertical Sync Set-Clear
#define VBLANK      0x01   // xx00 00x0   Vertical Blank Set-Clear
#define WSYNC       0x02   // ---- ----   Wait for Horizontal Blank
#define RSYNC       0x03   // ---- ----   Reset Horizontal Sync Counter
#define NUSIZ0      0x04   // 00xx 0xxx   Number-Size player/missle 0
#define NUSIZ1      0x05   // 00xx 0xxx   Number-Size player/missle 1
#define COLUP0      0x06   // xxxx xxx0   Color-Luminance Player 0
#define COLUP1      0x07   // xxxx xxx0   Color-Luminance Player 1
#define COLUPF      0x08   // xxxx xxx0   Color-Luminance Playfield
#define COLUBK      0x09   // xxxx xxx0   Color-Luminance Background
#define CTRLPF      0x0A   // 00xx 0xxx   Control Playfield, Ball, Collisions
#define REFP0       0x0B   // 0000 x000   Reflection Player 0
#define REFP1       0x0C   // 0000 x000   Reflection Player 1
#define PF0         0x0D   // xxxx 0000   Playfield Register Byte 0
#define PF1         0x0E   // xxxx xxxx   Playfield Register Byte 1
#define PF2         0x0F   // xxxx xxxx   Playfield Register Byte 2
#define RESP0       0x10   // ---- ----   Reset Player 0
#define RESP1       0x11   // ---- ----   Reset Player 1
#define RESM0       0x12   // ---- ----   Reset Missle 0
#define RESM1       0x13   // ---- ----   Reset Missle 1
#define RESBL       0x14   // ---- ----   Reset Ball
#define AUDC0       0x15   // 0000 xxxx   Audio Control 0
#define AUDC1       0x16   // 0000 xxxx   Audio Control 1
#define AUDF0       0x17   // 000x xxxx   Audio Frequency 0
#define AUDF1       0x18   // 000x xxxx   Audio Frequency 1
#define AUDV0       0x19   // 0000 xxxx   Audio Volume 0
#define AUDV1       0x1A   // 0000 xxxx   Audio Volume 1
#define GRP0        0x1B   // xxxx xxxx   Graphics Register Player 0
#define GRP1        0x1C   // xxxx xxxx   Graphics Register Player 1
#define ENAM0       0x1D   // 0000 00x0   Graphics Enable Missle 0
#define ENAM1       0x1E   // 0000 00x0   Graphics Enable Missle 1
#define ENABL       0x1F   // 0000 00x0   Graphics Enable Ball
#define HMP0        0x20   // xxxx 0000   Horizontal Motion Player 0
#define HMP1        0x21   // xxxx 0000   Horizontal Motion Player 1
#define HMM0        0x22   // xxxx 0000   Horizontal Motion Missle 0
#define HMM1        0x23   // xxxx 0000   Horizontal Motion Missle 1
#define HMBL        0x24   // xxxx 0000   Horizontal Motion Ball
#define VDELP0      0x25   // 0000 000x   Vertical Delay Player 0
#define VDELP1      0x26   // 0000 000x   Vertical Delay Player 1
#define VDELBL      0x27   // 0000 000x   Vertical Delay Ball
#define RESMP0      0x28   // 0000 00x0   Reset Missle 0 to Player 0
#define RESMP1      0x29   // 0000 00x0   Reset Missle 1 to Player 1
#define HMOVE       0x2A   // ---- ----   Apply Horizontal Motion
#define HMCLR       0x2B   // ---- ----   Clear Horizontal Move Registers
#define CXCLR       0x2C   // ---- ----   Clear Collision Latches
 
// SEG.U TIA_REGISTERS_READ
// ORG TIA_BASE_READ_ADDRESS

#define CXM0P       0x00       // xx00 0000       Read Collision  M0-P1   M0-P0
#define CXM1P       0x01       // xx00 0000                       M1-P0   M1-P1
#define CXP0FB      0x02       // xx00 0000                       P0-PF   P0-BL
#define CXP1FB      0x03       // xx00 0000                       P1-PF   P1-BL
#define CXM0FB      0x04       // xx00 0000                       M0-PF   M0-BL
#define CXM1FB      0x05       // xx00 0000                       M1-PF   M1-BL
#define CXBLPF      0x06       // x000 0000                       BL-PF   -----
#define CXPPMM      0x07       // xx00 0000                       P0-P1   M0-M1
#define INPT0       0x08       // x000 0000       Read Pot Port 0
#define INPT1       0x09       // x000 0000       Read Pot Port 1
#define INPT2       0x0A       // x000 0000       Read Pot Port 2
#define INPT3       0x0B       // x000 0000       Read Pot Port 3
#define INPT4       0x0C       // x000 0000       Read Input (Trigger) 0
#define INPT5       0x0D       // x000 0000       Read Input (Trigger) 1

// ;-------------------------------------------------------------------------------

// SEG.U RIOT
// ORG $280

// ; RIOT MEMORY MAP

#define SWCHA       0x280 //      Port A data register for joysticks:
                          //      Bits 4-7 for player 1.  Bits 0-3 for player 2.

#define SWACNT      0x281 //      Port A data direction register (DDR)
#define SWCHB       0x282 //      Port B data (console switches)
#define SWBCNT      0x283 //      Port B DDR
#define INTIM       0x284 //      Timer output

#define TIMINT      0x285

// Unused/undefined registers ($285-$294)

// #define          286
// #define          287
// #define          288
// #define          289
// #define          28A
// #define          28B
// #define          28C
// #define          28D
// #define          28E
// #define          28F
// #define          290
// #define          291
// #define          292
// #define          293

#define TIM1T       0x294      set 1 clock interval
#define TIM8T       0x295      set 8 clock interval
#define TIM64T      0x296      set 64 clock interval
#define T1024T      0x297      set 1024 clock interval
