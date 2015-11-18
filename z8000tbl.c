/*****************************************************************************
 *
 *   Z8001 Disassembler
 *   Copyright Matt Knoth 2008 (p8000@earthlink.net)
 *
 *   based on Juergen Buchmueller's Z8002 MAME model
 *
 *   z8000tbl.c
 *   Portable Z8000(2) emulator
 *   Z8000 disassembler; requires the z8000_exec table to be initialized
 *
 *   Copyright Juergen Buchmueller, all rights reserved.
 *   You can contact me at juergen@mame.net or pullmoll@stop1984.com
 *
 *  -  GPL (GNU General Public License)
 *     This program is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU General Public License
 *     as published by the Free Software Foundation; either version 2
 *     of the License, or (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *
 *****************************************************************************/

static const Z8000_init table_segmented[] = {
// begin, end ,st,sz,cy, operation,                               dasm,   dasmflags /////////////////
{0x0000,0x000f, 1,2,  7, OPCALL(Z00_0000_dddd_imm8), 			 "addb    %rb3,%#b3"},
{0x0010,0x00ff, 1,1,  7, OPCALL(Z00_ssN0_dddd_segmented),		 "addb    %rb3,@%rl2"},
{0x0100,0x010f, 1,2,  7, OPCALL(Z01_0000_dddd_imm16),			 "add     %rw3,%#w1"},
{0x0110,0x01ff, 1,1,  7, OPCALL(Z01_ssN0_dddd_segmented),		 "add     %rw3,@%rl2"},
{0x0200,0x020f, 1,2,  7, OPCALL(Z02_0000_dddd_imm8), 			 "subb    %rb3,%#b3"},
{0x0210,0x02ff, 1,1,  7, OPCALL(Z02_ssN0_dddd_segmented),		 "subb    %rb3,@%rl2"},
{0x0300,0x030f, 1,2,  7, OPCALL(Z03_0000_dddd_imm16),			 "sub     %rw3,%#w1"},
{0x0310,0x03ff, 1,1,  7, OPCALL(Z03_ssN0_dddd_segmented),		 "sub     %rw3,@%rl2"},
{0x0400,0x040f, 1,2,  7, OPCALL(Z04_0000_dddd_imm8), 			 "orb     %rb3,%#b3"},
{0x0410,0x04ff, 1,1,  7, OPCALL(Z04_ssN0_dddd_segmented),		 "orb     %rb3,@%rl2"},
{0x0500,0x050f, 1,2,  7, OPCALL(Z05_0000_dddd_imm16),			 "or      %rw3,%#w1"},
{0x0510,0x05ff, 1,1,  7, OPCALL(Z05_ssN0_dddd_segmented),		 "or      %rw3,@%rl2"},
{0x0600,0x060f, 1,2,  7, OPCALL(Z06_0000_dddd_imm8), 			 "andb    %rb3,%#b3"},
{0x0610,0x06ff, 1,1,  7, OPCALL(Z06_ssN0_dddd_segmented),		 "andb    %rb3,@%rl2"},
{0x0700,0x070f, 1,2,  7, OPCALL(Z07_0000_dddd_imm16),			 "and     %rw3,%#w1"},
{0x0710,0x07ff, 1,1,  7, OPCALL(Z07_ssN0_dddd_segmented),		 "and     %rw3,@%rl2"},
{0x0800,0x080f, 1,2,  7, OPCALL(Z08_0000_dddd_imm8), 			 "xorb    %rb3,%#b3"},
{0x0810,0x08ff, 1,1,  7, OPCALL(Z08_ssN0_dddd_segmented),		 "xorb    %rb3,@%rl2"},
{0x0900,0x090f, 1,2,  7, OPCALL(Z09_0000_dddd_imm16),			 "xor     %rw3,%#w1"},
{0x0910,0x09ff, 1,1,  7, OPCALL(Z09_ssN0_dddd_segmented),		 "xor     %rw3,@%rl2"},
{0x0a00,0x0a0f, 1,2,  7, OPCALL(Z0A_0000_dddd_imm8), 			 "cpb     %rb3,%#b3"},
{0x0a10,0x0aff, 1,1,  7, OPCALL(Z0A_ssN0_dddd_segmented),		 "cpb     %rb3,@%rl2"},
{0x0b00,0x0b0f, 1,2,  7, OPCALL(Z0B_0000_dddd_imm16),			 "cp      %rw3,%#w1"},
{0x0b10,0x0bff, 1,1,  7, OPCALL(Z0B_ssN0_dddd_segmented),		 "cp      %rw3,@%rl2"},
{0x0c10,0x0cf0,16,1, 12, OPCALL(Z0C_ddN0_0000_segmented),		 "comb    @%rl2"},
{0x0c11,0x0cf1,16,2, 11, OPCALL(Z0C_ddN0_0001_imm8_segmented), 	 "cpb     @%rl2,%#b3"},
{0x0c12,0x0cf2,16,1, 12, OPCALL(Z0C_ddN0_0010_segmented),		 "negb    @%rl2"},
{0x0c14,0x0cf4,16,1,  8, OPCALL(Z0C_ddN0_0100_segmented),		 "testb   @%rl2"},
{0x0c15,0x0cf5,16,2, 11, OPCALL(Z0C_ddN0_0101_imm8_segmented), 	 "ldb     @%rl2,%#b3"},
{0x0c16,0x0cf6,16,1, 11, OPCALL(Z0C_ddN0_0110_segmented),		 "tsetb   @%rl2"},
{0x0c18,0x0cf8,16,1,  8, OPCALL(Z0C_ddN0_1000_segmented),		 "clrb    @%rl2"},
{0x0d10,0x0df0,16,1, 12, OPCALL(Z0D_ddN0_0000_segmented),		 "com     @%rl2"},
{0x0d11,0x0df1,16,2, 11, OPCALL(Z0D_ddN0_0001_imm16_segmented),	 "cp      @%rl2,%#w1"},
{0x0d12,0x0df2,16,1, 12, OPCALL(Z0D_ddN0_0010_segmented),		 "neg     @%rl2"},
{0x0d14,0x0df4,16,1,  8, OPCALL(Z0D_ddN0_0100_segmented),		 "test    @%rl2"},
{0x0d15,0x0df5,16,2, 11, OPCALL(Z0D_ddN0_0101_imm16_segmented),	 "ld      @%rl2,%#w1"},
{0x0d16,0x0df6,16,1, 11, OPCALL(Z0D_ddN0_0110_segmented),		 "tset    @%rl2"},
{0x0d18,0x0df8,16,1,  8, OPCALL(Z0D_ddN0_1000_segmented),		 "clr     @%rl2"},
{0x0d19,0x0df9,16,2, 12, OPCALL(Z0D_ddN0_1001_imm16_segmented),	 "push    @%rl2,%#w1"},
{0x0e00,0x0eff, 1,2, 11, OPCALL(Z0E_imm16),				 "ext0e   %#l0"},
{0x0f00,0x0fff, 1,2, 11, OPCALL(Z0F_imm16),				 "ext0f   %#l0"},
{0x1000,0x100f, 1,3, 14, OPCALL(Z10_0000_dddd_imm32),			 "cpl     %rl3,%#l1"},
{0x1010,0x10ff, 1,1, 14, OPCALL(Z10_ssN0_dddd_segmented),		 "cpl     %rl3,@%rl2"},
{0x1111,0x11ff, 1,1, 20, OPCALL(Z11_ddN0_ssN0_segmented),		 "pushl   @%rl2,@%rl3"},
{0x1200,0x120f, 1,3, 14, OPCALL(Z12_0000_dddd_imm32),			 "subl    %rl3,%#l1"},
{0x1210,0x12ff, 1,1, 14, OPCALL(Z12_ssN0_dddd_segmented),		 "subl    %rl3,@%rl2"},
{0x1311,0x13ff, 1,1, 13, OPCALL(Z13_ddN0_ssN0_segmented),		 "push    @%rl2,@%rl3"},
{0x1400,0x140f, 1,3, 11, OPCALL(Z14_0000_dddd_imm32),			 "ldl     %rl3,%#l1"},
{0x1410,0x14ff, 1,1, 11, OPCALL(Z14_ssN0_dddd_segmented),		 "ldl     %rl3,@%rl2"},
{0x1511,0x15ff, 1,1, 19, OPCALL(Z15_ssN0_ddN0_segmented),		 "popl    @%rl3,@%rl2"},
{0x1600,0x160f, 1,3, 14, OPCALL(Z16_0000_dddd_imm32),			 "addl    %rl3,%#l1"},
{0x1610,0x16ff, 1,1, 14, OPCALL(Z16_ssN0_dddd_segmented),		 "addl    %rl3,@%rl2"},
{0x1711,0x17ff, 1,1, 12, OPCALL(Z17_ssN0_ddN0_segmented),		 "pop     @%rl3,@%rl2"},
{0x1800,0x180f, 1,3,282, OPCALL(!!!!!!!!!!!!!!!!!!!!!!!),		 "multl   %rq3,%#l1"},
{0x1810,0x18ff, 1,1,282, OPCALL(Z18_ssN0_dddd_segmented),		 "multl   %rq3,@%rl2"},
{0x1900,0x190f, 1,2, 70, OPCALL(Z19_0000_dddd_imm16),			 "mult    %rl3,%#w1"},
{0x1910,0x19ff, 1,1, 70, OPCALL(Z19_ssN0_dddd_segmented),		 "mult    %rl3,@%rl2"},
{0x1a00,0x1a0f, 1,3,744, OPCALL(Z1A_0000_dddd_imm32),			 "divl    %rq3,%#l1"},
{0x1a10,0x1aff, 1,1,744, OPCALL(Z1A_ssN0_dddd_segmented),		 "divl    %rq3,@%rl2"},
{0x1b00,0x1b0f, 1,2,107, OPCALL(Z1B_0000_dddd_imm16),			 "div     %rl3,%#w1"},
{0x1b10,0x1bff, 1,1,107, OPCALL(Z1B_ssN0_dddd_segmented),		 "div     %rl3,@%rl2"},
{0x1c11,0x1cf1,16,2, 11, OPCALL(Z1C_ssN0_0001_0000_dddd_0000_nmin1_segmented), 	 "ldm     %rw5,@%rl2,#%n"},
{0x1c18,0x1cf8,16,1, 13, OPCALL(Z1C_ddN0_1000_segmented),		 "testl   @%rl2"},
{0x1c19,0x1cf9,16,2, 11, OPCALL(Z1C_ddN0_1001_0000_ssss_0000_nmin1_segmented), 	 "ldm     @%rl2,%rw5,#%n"},
{0x1d10,0x1dff, 1,1, 11, OPCALL(Z1D_ddN0_ssss_segmented),		 "ldl     @%rl2,%rl3"},
{0x1e10,0x1eff, 1,1, 10, OPCALL(Z1E_ddN0_cccc_segmented),		 "jp      %c3,@%rl2"},
{0x1f10,0x1ff0,16,1, 10, OPCALL(Z1F_ddN0_0000_segmented),		 "call    @%rl2", DASMFLAG_STEP_OVER},
{0x2000,0x200f, 1,2,  7, OPCALL(Z20_0000_dddd_imm8),     		 "ldb     %rb3,%#b3"},
{0x2010,0x20ff, 1,1,  7, OPCALL(Z20_ssN0_dddd_segmented),		 "ldb     %rb3,@%rl2"},
{0x2100,0x210f, 1,2,  7, OPCALL(Z21_0000_dddd_imm16),			 "ld      %rw3,%#w1"},
{0x2110,0x21ff, 1,1,  7, OPCALL(Z21_ssN0_dddd_segmented),		 "ld      %rw3,@%rl2"},
{0x2200,0x220f, 1,2, 10, OPCALL(Z22_0000_ssss_0000_dddd_0000_0000),	 "resb    %rb5,%rw3"},
{0x2210,0x22ff, 1,1, 11, OPCALL(Z22_ddN0_imm4_segmented),		 "resb    @%rl2,#%3"},
{0x2300,0x230f, 1,2, 10, OPCALL(Z23_0000_ssss_0000_dddd_0000_0000),	 "res     %rw5,%rw3"},
{0x2310,0x23ff, 1,1, 11, OPCALL(Z23_ddN0_imm4_segmented),		 "res     @%rl2,#%3"},
{0x2400,0x240f, 1,2, 10, OPCALL(Z24_0000_ssss_0000_dddd_0000_0000),	 "setb    %rb5,%rw3"},
{0x2410,0x24ff, 1,1, 11, OPCALL(Z24_ddN0_imm4_segmented),		 "setb    @%rl2,#%3"},
{0x2500,0x250f, 1,2, 10, OPCALL(Z25_0000_ssss_0000_dddd_0000_0000),	 "set     %rw5,%rw3"},
{0x2510,0x25ff, 1,1, 11, OPCALL(Z25_ddN0_imm4_segmented),		 "set     @%rl2,#%3"},
{0x2600,0x260f, 1,2, 10, OPCALL(Z26_0000_ssss_0000_dddd_0000_0000),	 "bitb    %rb5,%rw3"},
{0x2610,0x26ff, 1,1,  8, OPCALL(Z26_ddN0_imm4_segmented),		 "bitb    @%rl3,#%3"},
{0x2700,0x270f, 1,2, 10, OPCALL(Z27_0000_ssss_0000_dddd_0000_0000),	 "bit     %rw5,%rw3"},
{0x2710,0x27ff, 1,1,  8, OPCALL(Z27_ddN0_imm4_segmented),		 "bit     @%rl2,#%3"},
{0x2810,0x28ff, 1,1, 11, OPCALL(Z28_ddN0_imm4m1_segmented),		 "incb    @%rl2,#%+3"},
{0x2910,0x29ff, 1,1, 11, OPCALL(Z29_ddN0_imm4m1_segmented),		 "inc     @%rl2,#%+3"},
{0x2a10,0x2aff, 1,1, 11, OPCALL(Z2A_ddN0_imm4m1_segmented),		 "decb    @%rl2,#%+3"},
{0x2b10,0x2bff, 1,1, 11, OPCALL(Z2B_ddN0_imm4m1_segmented),		 "dec     @%rl2,#%+3"},
{0x2c10,0x2cff, 1,1, 12, OPCALL(Z2C_ssN0_dddd_segmented),		 "exb     %rb3,@%rl2"},
{0x2d10,0x2dff, 1,1, 12, OPCALL(Z2D_ssN0_dddd_segmented),		 "ex      %rw3,@%rl2"},
{0x2e10,0x2eff, 1,1,  8, OPCALL(Z2E_ddN0_ssss_segmented),		 "ldb     @%rl2,%rb3"},
{0x2f10,0x2fff, 1,1,  8, OPCALL(Z2F_ddN0_ssss_segmented),		 "ld      @%rl2,%rw3"},
{0x3000,0x300f, 1,2, 14, OPCALL(Z30_0000_dddd_dsp16),			 "ldrb    %rb3,%p1"},
{0x3010,0x30ff, 1,2, 14, OPCALL(Z30_ssN0_dddd_imm16_segmented),	 "ldb     %rb3,%rl2(%#w1)"},
{0x3100,0x310f, 1,2, 14, OPCALL(Z31_0000_dddd_dsp16),			 "ldr     %rw3,%p1"},
{0x3110,0x31ff, 1,2, 14, OPCALL(Z31_ssN0_dddd_imm16_segmented),	 "ld      %rw3,%rl2(%#w1)"},
{0x3200,0x320f, 1,2, 14, OPCALL(Z32_0000_ssss_dsp16),			 "ldrb    %p1,%rb3"},
{0x3210,0x32ff, 1,2, 14, OPCALL(Z32_ddN0_ssss_imm16_segmented),	 "ldb     %rl2(%#w1),%rb3"},
{0x3300,0x330f, 1,2, 14, OPCALL(Z33_0000_ssss_dsp16),			 "ldr     %p1,%rw3"},
{0x3310,0x33ff, 1,2, 14, OPCALL(Z33_ddN0_ssss_imm16_segmented),	 "ld      %rl2(%#w1),%rw3"},
{0x3400,0x340f, 1,2, 15, OPCALL(Z34_0000_dddd_dsp16_segmented),	 "ldar    %rl3,%p1"},
{0x3410,0x34ff, 1,2, 15, OPCALL(Z34_ssN0_dddd_imm16_segmented),	 "lda     %rl3,%rl2(%#w1)"},
{0x3500,0x350f, 1,2, 17, OPCALL(Z35_0000_dddd_dsp16),			 "ldrl    %rl3,%p1"},
{0x3510,0x35ff, 1,2, 17, OPCALL(Z35_ssN0_dddd_imm16_segmented),	 "ldl     %rl3,%rl2(%#w1)"},
{0x3600,0x3600, 1,1,  2, OPCALL(Z36_0000_0000),			 "bpt"},                      //----UNKNOWN break point trap  (used by MAME debugger????)
{0x3700,0x370f, 1,2, 17, OPCALL(Z37_0000_ssss_dsp16),			 "ldrl    %p1,%rl3"},
{0x3710,0x37ff, 1,2, 17, OPCALL(Z37_ddN0_ssss_imm16),			 "ldl     %rl2(%#w1),%rl3"},
{0x3910,0x39f0,16,1, 12, OPCALL(Z39_ssN0_0000_segmented),		 "ldps    @%rl2"},
{0x3a04,0x3af4,16,2, 12, OPCALL(Z3A_dddd_0100_imm16),			 "%R %rb2,%a1"},                      //----INB
{0x3a05,0x3af5,16,2, 12, OPCALL(Z3A_dddd_0101_imm16),			 "%R %rb2,%a1"},                      //---SINB
{0x3a06,0x3af6,16,2, 12, OPCALL(Z3A_ssss_0110_imm16),			 "%R %a1,%rb2"},                      //----OUTB
{0x3a07,0x3af7,16,2, 12, OPCALL(Z3A_ssss_0111_imm16),			 "%R %a1,%rb2"},                      //---SOUTB
{0x3a10,0x3af0,16,2, 21, OPCALL(Z3A_ssss_0000_0000_aaaa_dddd_x000_segmented),	 "%R @%rl6,@%rw2,%rw5"},      //----INIRB  @rr., @r., r.
{0x3a11,0x3af1,16,2, 21, OPCALL(Z3A_ssss_0001_0000_aaaa_dddd_x000_segmented),	 "%R @%rl6,@%rw2,%rw5"},      //---SINIRB  
{0x3a12,0x3af2,16,2, 21, OPCALL(Z3A_ssss_0010_0000_aaaa_dddd_x000_segmented),	 "%R @%rw6,@%rl2,%rw5"},      //----OTIRB  @r.,@rr., r
{0x3a13,0x3af3,16,2, 21, OPCALL(Z3A_ssss_0011_0000_aaaa_dddd_x000_segmented),	 "%R @%rw6,@%rl2,%rw5"},      //---SOTIRB
{0x3a18,0x3af8,16,2, 21, OPCALL(Z3A_ssss_1000_0000_aaaa_dddd_x000_segmented),	 "%R @%rl6,@%rw2,%rw5"},      //----INDRB
{0x3a19,0x3af9,16,2, 21, OPCALL(Z3A_ssss_1001_0000_aaaa_dddd_x000_segmented),	 "%R @%rl6,@%rw2,%rw5"},      //---SINDRB
{0x3a1a,0x3afa,16,2, 21, OPCALL(Z3A_ssss_1010_0000_aaaa_dddd_x000_segmented),	 "%R @%rw6,@%rl2,%rw5"},      //----OUTD
{0x3a1b,0x3afb,16,2, 21, OPCALL(Z3A_ssss_1011_0000_aaaa_dddd_x000_segmented),	 "%R @%rw6,@%rl2,%rw5"},      //---SOUTD
{0x3b04,0x3bf4,16,2, 12, OPCALL(Z3B_dddd_0100_imm16),			 "%R %rw2,%a1"},                      //----IN
{0x3b05,0x3bf5,16,2, 12, OPCALL(Z3B_dddd_0101_imm16),			 "%R %rw2,%a1"},                      //---SIN
{0x3b06,0x3bf6,16,2, 12, OPCALL(Z3B_ssss_0110_imm16),			 "%R %a1,%rw2"},                      //----OUT
{0x3b07,0x3bf7,16,2, 12, OPCALL(Z3B_ssss_0111_imm16),			 "%R %a1,%rw2"},                      //---SOUT
{0x3b10,0x3bf0,16,2, 21, OPCALL(Z3B_ssss_0000_0000_aaaa_dddd_x000_segmented),	 "%R @%rl6,@%rw2,%rw5"},      //----INIR
{0x3b11,0x3bf1,16,2, 21, OPCALL(Z3B_ssss_0001_0000_aaaa_dddd_x000_segmented),	 "%R @%rl6,@%rw2,%rw5"},      //---SINIR
{0x3b12,0x3bf2,16,2, 21, OPCALL(Z3B_ssss_0010_0000_aaaa_dddd_x000_segmented),	 "%R @%rw6,@%rl2,%rw5"},      //----OTIR
{0x3b13,0x3bf3,16,2, 21, OPCALL(Z3B_ssss_0011_0000_aaaa_dddd_x000_segmented),	 "%R @%rw6,@%rl2,%rw5"},      //---SOTIR
{0x3b18,0x3bf8,16,2, 21, OPCALL(Z3B_ssss_1000_0000_aaaa_dddd_x000_segmented),	 "%R @%rl6,@%rw2,%rw5"},      //----INDR
{0x3b19,0x3bf9,16,2, 21, OPCALL(Z3B_ssss_1001_0000_aaaa_dddd_x000_segmented),	 "%R @%rl6,@%rw2,%rw5"},      //---SINDR
{0x3b1a,0x3bfa,16,2, 21, OPCALL(Z3B_ssss_1010_0000_aaaa_dddd_x000_segmented),	 "%R @%rw6,@%rl2,%rb5"},      //----OTDR
{0x3b1b,0x3bfb,16,2, 21, OPCALL(Z3B_ssss_1011_0000_aaaa_dddd_x000_segmented),	 "%R @%rw6,@%rl2,%rb5"},      //---SOTDR
{0x3c00,0x3cff, 1,1, 10, OPCALL(Z3C_ssss_dddd),				 "inb     %rb3,@%rw2"},
{0x3d00,0x3dff, 1,1, 10, OPCALL(Z3D_ssss_dddd),				 "in      %rw3,@%rw2"},
{0x3e00,0x3eff, 1,1, 12, OPCALL(Z3E_dddd_ssss),				 "outb    @%rw2,%rb3"},
{0x3f00,0x3fff, 1,1, 12, OPCALL(Z3F_dddd_ssss),				 "out     @%rw2,%rw3"},
{0x4000,0x400f, 1,2, 10, OPCALL(Z40_0000_dddd_addr_segmented),			 "addb    %rb3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x4010,0x40ff, 1,2, 10, OPCALL(Z40_ssN0_dddd_addr_segmented),			 "addb    %rb3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x4100,0x410f, 1,2, 10, OPCALL(Z41_0000_dddd_addr_segmented),			 "add     %rw3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x4110,0x41ff, 1,2, 10, OPCALL(Z41_ssN0_dddd_addr_segmented),			 "add     %rw3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x4200,0x420f, 1,2, 10, OPCALL(Z42_0000_dddd_addr_segmented),			 "subb    %rb3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x4210,0x42ff, 1,2, 10, OPCALL(Z42_ssN0_dddd_addr_segmented),			 "subb    %rb3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x4300,0x430f, 1,2, 10, OPCALL(Z43_0000_dddd_addr_segmented), 		 "sub     %rw3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x4310,0x43ff, 1,2, 10, OPCALL(Z43_ssN0_dddd_addr_segmented),			 "sub     %rw3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x4400,0x440f, 1,2, 10, OPCALL(Z44_0000_dddd_addr_segmented),			 "orb     %rb3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x4410,0x44ff, 1,2, 10, OPCALL(Z44_ssN0_dddd_addr_segmented),			 "orb     %rb3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x4500,0x450f, 1,2, 10, OPCALL(Z45_0000_dddd_addr_segmented), 		 "or      %rw3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x4510,0x45ff, 1,2, 10, OPCALL(Z45_ssN0_dddd_addr_segmented),			 "or      %rw3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x4600,0x460f, 1,2, 10, OPCALL(Z46_0000_dddd_addr_segmented), 		 "andb    %rb3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x4610,0x46ff, 1,2, 10, OPCALL(Z46_ssN0_dddd_addr_segmented),			 "andb    %rb3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x4700,0x470f, 1,2, 10, OPCALL(Z47_0000_dddd_addr_segmented), 		 "and     %rw3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x4710,0x47ff, 1,2, 10, OPCALL(Z47_ssN0_dddd_addr_segmented),			 "and     %rw3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x4800,0x480f, 1,2, 10, OPCALL(Z48_0000_dddd_addr_segmented), 		 "xorb    %rb3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x4810,0x48ff, 1,2, 10, OPCALL(Z48_ssN0_dddd_addr_segmented),			 "xorb    %rb3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x4900,0x490f, 1,2, 10, OPCALL(Z49_0000_dddd_addr_segmented), 		 "xor     %rw3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x4910,0x49ff, 1,2, 10, OPCALL(Z49_ssN0_dddd_addr_segmented), 		 "xor     %rw3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x4a00,0x4a0f, 1,2, 10, OPCALL(Z4A_0000_dddd_addr_segmented), 		 "cpb     %rb3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x4a10,0x4aff, 1,2, 10, OPCALL(Z4A_ssN0_dddd_addr_segmented), 		 "cpb     %rb3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x4b00,0x4b0f, 1,2, 10, OPCALL(Z4B_0000_dddd_addr_segmented), 		 "cp      %rw3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x4b10,0x4bff, 1,2, 10, OPCALL(Z4B_ssN0_dddd_addr_segmented),			 "cp      %rw3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x4c00,0x4c00, 1,2, 16, OPCALL(Z4C_0000_0000_addr_segmented), 		 "comb    %A1"},              // FIX dasm long address SS/SL, fix cycle count
{0x4c01,0x4c01, 1,3, 15, OPCALL(Z4C_0000_0001_addr_imm8_segmented),		 "cpb     %A1,%Db5"},         // FIX dasm long address SS/SL, fix cycle count
{0x4c02,0x4c02, 1,2, 16, OPCALL(Z4C_0000_0010_addr_segmented),			 "negb    %A1"},              // FIX dasm long address SS/SL, fix cycle count
{0x4c04,0x4c04, 1,2, 12, OPCALL(Z4C_0000_0100_addr_segmented), 		 "testb   %A1"},              // FIX dasm long address SS/SL, fix cycle count
{0x4c05,0x4c05, 1,3, 15, OPCALL(Z4C_0000_0101_addr_imm8_segmented),		 "ldb     %A1,%Db5"},         // FIX dasm long address SS/SL, fix cycle count
{0x4c06,0x4c06, 1,2, 15, OPCALL(Z4C_0000_0110_addr_segmented), 		 "tsetb   %A1"},              // FIX dasm long address SS/SL, fix cycle count
{0x4c08,0x4c08, 1,2, 12, OPCALL(Z4C_0000_1000_addr_segmented), 		 "clrb    %A1"},              // FIX dasm long address SS/SL, fix cycle count
{0x4c10,0x4cf0,16,2, 16, OPCALL(Z4C_ddN0_0000_addr_segmented), 		 "comb    %A1(%rw2)"},        // FIX dasm long address SS/SL, fix cycle count
{0x4c11,0x4cf1,16,3, 15, OPCALL(Z4C_ddN0_0001_addr_imm8_segmented),		 "cpb     %A1(%rw2),%Db5"},   // FIX dasm long address SS/SL, fix cycle count
{0x4c12,0x4cf2,16,2, 16, OPCALL(Z4C_ddN0_0010_addr_segmented),			 "negb    %A1(%rw2)"},        // FIX dasm long address SS/SL, fix cycle count
{0x4c14,0x4cf4,16,2, 12, OPCALL(Z4C_ddN0_0100_addr_segmented), 		 "testb   %A1(%rw2)"},        // FIX dasm long address SS/SL, fix cycle count
{0x4c15,0x4cf5,16,3, 15, OPCALL(Z4C_ddN0_0101_addr_imm8_segmented),		 "ldb     %A1(%rw2),%Db5"},   // FIX dasm long address SS/SL, fix cycle count
{0x4c16,0x4cf6,16,2, 15, OPCALL(Z4C_ddN0_0110_addr_segmented), 		 "tsetb   %A1(%rw2)"},        // FIX dasm long address SS/SL, fix cycle count
{0x4c18,0x4cf8,16,2, 12, OPCALL(Z4C_ddN0_1000_addr_segmented),			 "clrb    %A1(%rw2)"},        // FIX dasm long address SS/SL, fix cycle count
{0x4d00,0x4d00, 1,2, 16, OPCALL(Z4D_0000_0000_addr_segmented),			 "com     %A1"},              // FIX dasm long address SS/SL, fix cycle count
{0x4d01,0x4d01, 1,3, 15, OPCALL(Z4D_0000_0001_addr_imm16_segmented),		 "cp      %A1,%Dw2"},         // FIX dasm long address SS/SL, fix cycle count
{0x4d02,0x4d02, 1,2, 16, OPCALL(Z4D_0000_0010_addr_segmented), 		 "neg     %A1"},              // FIX dasm long address SS/SL, fix cycle count
{0x4d04,0x4d04, 1,2, 12, OPCALL(Z4D_0000_0100_addr_segmented), 		 "test    %A1"},              // FIX dasm long address SS/SL, fix cycle count
{0x4d05,0x4d05, 1,3, 15, OPCALL(Z4D_0000_0101_addr_imm16_segmented),		 "ld      %A1,%Dw2"},         // FIX dasm long address SS/SL, fix cycle count
{0x4d06,0x4d06, 1,2, 14, OPCALL(Z4D_0000_0110_addr_segmented), 		 "tset    %A1"},              // FIX dasm long address SS/SL, fix cycle count
{0x4d08,0x4d08, 1,2, 12, OPCALL(Z4D_0000_1000_addr_segmented), 		 "clr     %A1"},              // FIX dasm long address SS/SL, fix cycle count
{0x4d10,0x4df0,16,2, 16, OPCALL(Z4D_ddN0_0000_addr_segmented), 		 "com     %A1(%rw2)"},        // FIX dasm long address SS/SL, fix cycle count
{0x4d11,0x4df1,16,3, 15, OPCALL(Z4D_ddN0_0001_addr_imm16_segmented),		 "cp      %A1(%rw2),%Dw2"},   // FIX dasm long address SS/SL, fix cycle count
{0x4d12,0x4df2,16,2, 16, OPCALL(Z4D_ddN0_0010_addr_segmented), 		 "neg     %A1(%rw2)"},        // FIX dasm long address SS/SL, fix cycle count
{0x4d14,0x4df4,16,2, 12, OPCALL(Z4D_ddN0_0100_addr_segmented), 		 "test    %A1(%rw2)"},        // FIX dasm long address SS/SL, fix cycle count
{0x4d15,0x4df5,16,3, 15, OPCALL(Z4D_ddN0_0101_addr_imm16_segmented),		 "ld      %A1(%rw2),%Dw2"},   // FIX dasm long address SS/SL, fix cycle count
{0x4d16,0x4df6,16,2, 15, OPCALL(Z4D_ddN0_0110_addr_segmented), 		 "tset    %A1(%rw2)"},        // FIX dasm long address SS/SL, fix cycle count
{0x4d18,0x4df8,16,2, 12, OPCALL(Z4D_ddN0_1000_addr_segmented), 		 "clr     %A1(%rw2)"},        // FIX dasm long address SS/SL, fix cycle count
{0x4e00,0x4e0f, 1,3, 15, OPCALL(Z4E_ssN0_dddd_segmented),                       "ext4e   %#l0,%A2"},         // FIX dasm long address SS/SL, fix cycle count
{0x4e10,0x4eff, 1,3, 15, OPCALL(Z4E_ssss_dddd_segmented),                       "ext4e   %#l0,%A2"},         // FIX dasm long address SS/SL, fix cycle count
{0x4f00,0x4f0f, 1,3, 15, OPCALL(Z4F_ssN0_dddd_segmented),                       "ext4f   %#l0,%A2"},         // FIX dasm long address SS/SL, fix cycle count
{0x4f10,0x4fff, 1,3, 15, OPCALL(Z4F_ssss_dddd_segmented),                       "ext4f   %#l0,%A2"},         // FIX dasm long address SS/SL, fix cycle count
{0x5000,0x500f, 1,2, 16, OPCALL(Z50_0000_dddd_addr_segmented), 		 "cpl     %rl3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x5010,0x50ff, 1,2, 16, OPCALL(Z50_ssN0_dddd_addr_segmented), 		 "cpl     %rl3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x5110,0x51f0,16,2, 21, OPCALL(Z51_ddN0_0000_addr_segmented), 		 "pushl   @%rl2,%A1"},        // FIX dasm long address SS/SL, fix cycle count
{0x5111,0x51f1,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5112,0x51f2,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5113,0x51f3,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5114,0x51f4,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5115,0x51f5,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5116,0x51f6,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5117,0x51f7,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5118,0x51f8,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5119,0x51f9,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x511a,0x51fa,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x511b,0x51fb,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x511c,0x51fc,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x511d,0x51fd,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x511e,0x51fe,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x511f,0x51ff,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr_segmented), 		 "pushl   @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5200,0x520f, 1,2, 16, OPCALL(Z52_0000_dddd_addr_segmented), 		 "subl    %rl3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x5210,0x52ff, 1,2, 16, OPCALL(Z52_ssN0_dddd_addr_segmented), 		 "subl    %rl3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x5310,0x53f0,16,2, 14, OPCALL(Z53_ddN0_0000_addr_segmented), 		 "push    @%rl2,%A1"},        // FIX dasm long address SS/SL, fix cycle count
{0x5311,0x53f1,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5312,0x53f2,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5313,0x53f3,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5314,0x53f4,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5315,0x53f5,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5316,0x53f6,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5317,0x53f7,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5318,0x53f8,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5319,0x53f9,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x531a,0x53fa,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x531b,0x53fb,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x531c,0x53fc,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x531d,0x53fd,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x531e,0x53fe,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x531f,0x53ff,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr_segmented), 		 "push    @%rl2,%A1(%rw3)"},  // FIX dasm long address SS/SL, fix cycle count
{0x5400,0x540f, 1,2, 13, OPCALL(Z54_0000_dddd_addr_segmented), 		 "ldl     %rl3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x5410,0x54ff, 1,2, 13, OPCALL(Z54_ssN0_dddd_addr_segmented), 		 "ldl     %rl3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x5510,0x55f0,16,2, 23, OPCALL(Z55_ssN0_0000_addr_segmented), 		 "popl    %A1,@%rl2"},        // FIX dasm long address SS/SL, fix cycle count
{0x5511,0x55f1,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5512,0x55f2,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5513,0x55f3,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5514,0x55f4,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5515,0x55f5,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5516,0x55f6,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5517,0x55f7,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5518,0x55f8,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5519,0x55f9,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x551a,0x55fa,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x551b,0x55fb,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x551c,0x55fc,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x551d,0x55fd,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x551e,0x55fe,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x551f,0x55ff,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr_segmented), 		 "popl    %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5600,0x560f, 1,2, 16, OPCALL(Z56_0000_dddd_addr_segmented), 		 "addl    %rl3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x5610,0x56ff, 1,2, 16, OPCALL(Z56_ssN0_dddd_addr_segmented), 		 "addl    %rl3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x5710,0x57f0,16,2, 16, OPCALL(Z57_ssN0_0000_addr_segmented), 		 "pop     %A1,@%rl2"},        // FIX dasm long address SS/SL, fix cycle count
{0x5711,0x57f1,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5712,0x57f2,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5713,0x57f3,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5714,0x57f4,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5715,0x57f5,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5716,0x57f6,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5717,0x57f7,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5718,0x57f8,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5719,0x57f9,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x571a,0x57fa,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x571b,0x57fb,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x571c,0x57fc,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x571d,0x57fd,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x571e,0x57fe,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x571f,0x57ff,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr_segmented), 		 "pop     %A1(%rw3),@%rl2"},  // FIX dasm long address SS/SL, fix cycle count
{0x5800,0x580f, 1,2,283, OPCALL(Z58_0000_dddd_addr_segmented), 		 "multl   %rq3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x5810,0x58ff, 1,2,284, OPCALL(Z58_ssN0_dddd_addr_segmented), 		 "multl   %rq3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x5900,0x590f, 1,2, 71, OPCALL(Z59_0000_dddd_addr_segmented), 		 "mult    %rl3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x5910,0x59ff, 1,2, 72, OPCALL(Z59_ssN0_dddd_addr_segmented), 		 "mult    %rl3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x5a00,0x5a0f, 1,2,746, OPCALL(Z5A_0000_dddd_addr_segmented), 		 "divl    %rq3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x5a10,0x5aff, 1,2,746, OPCALL(Z5A_ssN0_dddd_addr_segmented), 		 "divl    %rq3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x5b00,0x5b0f, 1,2,109, OPCALL(Z5B_0000_dddd_addr_segmented), 		 "div     %rl3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x5b10,0x5bff, 1,2,109, OPCALL(Z5B_ssN0_dddd_addr_segmented), 		 "div     %rl3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x5c01,0x5c01, 1,3, 15, OPCALL(Z5C_0000_0001_0000_dddd_0000_nmin1_addr_segmented), "ldm     %rw5,%A2,#%n"},   // FIX dasm long address SS/SL, fix cycle count  ??? %rw5??
{0x5c08,0x5c08, 1,2, 16, OPCALL(Z5C_0000_1000_addr_segmented), 		 "testl   %A1"},              // FIX dasm long address SS/SL, fix cycle count
{0x5c09,0x5c09, 1,3, 15, OPCALL(Z5C_0000_1001_0000_ssss_0000_nmin1_addr_segmented), "ldm     %A2,%rw5,#%n"},   // FIX dasm long address SS/SL, fix cycle count
{0x5c11,0x5cf1,16,3, 15, OPCALL(Z5C_ssN0_0001_0000_dddd_0000_nmin1_addr_segmented), "ldm     %rw5,%A2(%rw2),#%n"}, // FIX dasm long address SS/SL, fix cycle count  ??? %rw5??
{0x5c18,0x5cf8,16,2, 17, OPCALL(Z5C_ddN0_1000_addr_segmented), 		 "testl   %A1(%rw2)"},        // FIX dasm long address SS/SL, fix cycle count
{0x5c19,0x5cf9,16,3, 15, OPCALL(Z5C_ddN0_1001_0000_ssN0_0000_nmin1_addr_segmented), "ldm     %A2(%rw2),%rw5,#%n"}, // FIX dasm long address SS/SL, fix cycle count  ??? %rw5??
{0x5d00,0x5d0f, 1,2, 15, OPCALL(Z5D_0000_ssss_addr_segmented), 		 "ldl     %A1,%rl3"},         // FIX dasm long address SS/SL, fix cycle count
{0x5d10,0x5dff, 1,2, 15, OPCALL(Z5D_ddN0_ssss_addr_segmented), 		 "ldl     %A1(%rw2),%rl3"},   // FIX dasm long address SS/SL, fix cycle count
{0x5e00,0x5e0f, 1,2,  8, OPCALL(Z5E_0000_cccc_addr_segmented), 		 "jp      %c3,%A1"},          // FIX dasm long address SS/SL, fix cycle count
{0x5e10,0x5eff, 1,2,  8, OPCALL(Z5E_ddN0_cccc_addr_segmented), 		 "jp      %c3,%A1(%rw2)"},    // FIX dasm long address SS/SL, fix cycle count
{0x5f00,0x5f00, 1,2, 18, OPCALL(Z5F_0000_0000_addr_segmented), 		 "call    %A1", DASMFLAG_STEP_OVER}, // FIX dasm long address SS/SL, fix cycle count
{0x5f10,0x5ff0,16,2, 18, OPCALL(Z5F_ddN0_0000_addr_segmented), 		 "call    %A1(%rw2)", DASMFLAG_STEP_OVER}, // FIX dasm long address SS/SL, fix cycle count
{0x6000,0x600f, 1,2, 10, OPCALL(Z60_0000_dddd_addr_segmented), 		 "ldb     %rb3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x6010,0x60ff, 1,2, 10, OPCALL(Z60_ssN0_dddd_addr_segmented), 		 "ldb     %rb3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x6100,0x610f, 1,2, 10, OPCALL(Z61_0000_dddd_addr_segmented), 		 "ld      %rw3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x6110,0x61ff, 1,2, 10, OPCALL(Z61_ssN0_dddd_addr_segmented), 		 "ld      %rw3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x6200,0x620f, 1,2, 14, OPCALL(Z62_0000_imm4_addr_segmented), 		 "resb    %A1,#%3"},           // FIX dasm long address SS/SL, fix cycle count
{0x6210,0x62ff, 1,2, 14, OPCALL(Z62_ddN0_imm4_addr_segmented), 		 "resb    %A1(%rw2),#%3"},     // FIX dasm long address SS/SL, fix cycle count
{0x6300,0x630f, 1,2, 14, OPCALL(Z63_0000_imm4_addr_segmented), 		 "res     %A1,#%3"},           // FIX dasm long address SS/SL, fix cycle count
{0x6310,0x63ff, 1,2, 14, OPCALL(Z63_ddN0_imm4_addr_segmented), 		 "res     %A1(%rw2),#%3"},     // FIX dasm long address SS/SL, fix cycle count
{0x6400,0x640f, 1,2, 14, OPCALL(Z64_0000_imm4_addr_segmented), 		 "setb    %A1,#%3"},           // FIX dasm long address SS/SL, fix cycle count
{0x6410,0x64ff, 1,2, 14, OPCALL(Z64_ddN0_imm4_addr_segmented), 		 "setb    %A1(%rw2),#%3"},     // FIX dasm long address SS/SL, fix cycle count
{0x6500,0x650f, 1,2, 14, OPCALL(Z65_0000_imm4_addr_segmented), 		 "set     %A1,#%3"},           // FIX dasm long address SS/SL, fix cycle count
{0x6510,0x65ff, 1,2, 14, OPCALL(Z65_ddN0_imm4_addr_segmented), 		 "set     %A1(%rw2),#%3"},     // FIX dasm long address SS/SL, fix cycle count
{0x6600,0x660f, 1,2, 11, OPCALL(Z66_0000_imm4_addr_segmented), 		 "bitb    %A1,#%3"},           // FIX dasm long address SS/SL, fix cycle count
{0x6610,0x66ff, 1,2, 11, OPCALL(Z66_ddN0_imm4_addr_segmented), 		 "bitb    %A1(%rw2),#%3"},     // FIX dasm long address SS/SL, fix cycle count
{0x6700,0x670f, 1,2, 11, OPCALL(Z67_0000_imm4_addr_segmented), 		 "bit     %A1,#%3"},           // FIX dasm long address SS/SL, fix cycle count
{0x6710,0x67ff, 1,2, 11, OPCALL(Z67_ddN0_imm4_addr_segmented), 		 "bit     %A1(%rw2),#%3"},     // FIX dasm long address SS/SL, fix cycle count
{0x6800,0x680f, 1,2, 14, OPCALL(Z68_0000_imm4m1_addr_segmented),		 "incb    %A1,#%+3"},          // FIX dasm long address SS/SL, fix cycle count
{0x6810,0x68ff, 1,2, 14, OPCALL(Z68_ddN0_imm4m1_addr_segmented),		 "incb    %A1(%rw2),#%+3"},    // FIX dasm long address SS/SL, fix cycle count
{0x6900,0x690f, 1,2, 14, OPCALL(Z69_0000_imm4m1_addr_segmented),		 "inc     %A1,#%+3"},          // FIX dasm long address SS/SL, fix cycle count
{0x6910,0x69ff, 1,2, 14, OPCALL(Z69_ddN0_imm4m1_addr_segmented),		 "inc     %A1(%rw2),#%+3"},    // FIX dasm long address SS/SL, fix cycle count
{0x6a00,0x6a0f, 1,2, 14, OPCALL(Z6A_0000_imm4m1_addr_segmented),		 "decb    %A1,#%+3"},          // FIX dasm long address SS/SL, fix cycle count
{0x6a10,0x6aff, 1,2, 14, OPCALL(Z6A_ddN0_imm4m1_addr_segmented),		 "decb    %A1(%rw2),#%+3"},    // FIX dasm long address SS/SL, fix cycle count
{0x6b00,0x6b0f, 1,2, 14, OPCALL(Z6B_0000_imm4m1_addr_segmented),		 "dec     %A1,#%+3"},          // FIX dasm long address SS/SL, fix cycle count
{0x6b10,0x6bff, 1,2, 14, OPCALL(Z6B_ddN0_imm4m1_addr_segmented),		 "dec     %A1(%rw2),#%+3"},    // FIX dasm long address SS/SL, fix cycle count
{0x6c00,0x6c0f, 1,2, 16, OPCALL(Z6C_0000_dddd_addr_segmented), 		 "exb     %rb3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x6c10,0x6cff, 1,2, 16, OPCALL(Z6C_ssN0_dddd_addr_segmented), 		 "exb     %rb3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x6d00,0x6d0f, 1,2, 16, OPCALL(Z6D_0000_dddd_addr_segmented), 		 "ex      %rw3,%A1"},         // FIX dasm long address SS/SL, fix cycle count
{0x6d10,0x6dff, 1,2, 16, OPCALL(Z6D_ssN0_dddd_addr_segmented), 		 "ex      %rw3,%A1(%rw2)"},   // FIX dasm long address SS/SL, fix cycle count
{0x6e00,0x6e0f, 1,2, 12, OPCALL(Z6E_0000_ssss_addr_segmented), 		 "ldb     %A1,%rb3"},         // FIX dasm long address SS/SL, fix cycle count
{0x6e10,0x6eff, 1,2, 12, OPCALL(Z6E_ddN0_ssss_addr_segmented), 		 "ldb     %A1(%rw2),%rb3"},   // FIX dasm long address SS/SL, fix cycle count
{0x6f00,0x6f0f, 1,2, 12, OPCALL(Z6F_0000_ssss_addr_segmented), 		 "ld      %A1,%rw3"},         // FIX dasm long address SS/SL, fix cycle count
{0x6f10,0x6fff, 1,2, 12, OPCALL(Z6F_ddN0_ssss_addr_segmented), 		 "ld      %A1(%rw2),%rw3"},   // FIX dasm long address SS/SL, fix cycle count
{0x7010,0x70ff, 1,2, 14, OPCALL(Z70_ssN0_dddd_0000_xxxx_0000_0000_segmented), 	 "ldb     %rb3,%rl2(%rw5)"},  
{0x7110,0x71ff, 1,2, 14, OPCALL(Z71_ssN0_dddd_0000_xxxx_0000_0000_segmented),	 "ld      %rw3,%rl2(%rw5)"},
{0x7210,0x72ff, 1,2, 14, OPCALL(Z72_ddN0_ssss_0000_xxxx_0000_0000_segmented),	 "ldb     %rl2(%rw5),%rb3"},
{0x7310,0x73ff, 1,2, 14, OPCALL(Z73_ddN0_ssss_0000_xxxx_0000_0000_segmented),	 "ld      %rl2(%rw5),%rw3"},
{0x7410,0x74ff, 1,2, 15, OPCALL(Z74_ssN0_dddd_0000_xxxx_0000_0000_segmented),	 "lda     %rl3,%rl2(%rw5)"},
{0x7510,0x75ff, 1,2, 17, OPCALL(Z75_ssN0_dddd_0000_xxxx_0000_0000_segmented),	 "ldl     %rl3,%rl2(%rw5)"},
{0x7600,0x760f, 1,2, 13, OPCALL(Z76_0000_dddd_addr_segmented), 		 "lda     %rl3,%A1"},        // FIX dasm long address SS/SL, fix cycle count
{0x7610,0x76ff, 1,2, 13, OPCALL(Z76_ssN0_dddd_addr_segmented), 		 "lda     %rl3,%A1(%rw2)"},  // FIX dasm long address SS/SL, fix cycle count
{0x7710,0x77ff, 1,2, 17, OPCALL(Z77_ddN0_ssss_0000_xxxx_0000_0000_segmented),	 "ldl     %rl2(%rw5),%rl3"},
{0x7900,0x7900, 1,2, 20, OPCALL(Z79_0000_0000_addr_segmented), 		 "ldps    %A1"},              // FIX dasm long address SS/SL, fix cycle count
{0x7910,0x79f0,16,2, 20, OPCALL(Z79_ssN0_0000_addr_segmented), 		 "ldps    %A1(%rw2)"},        // FIX dasm long address SS/SL, fix cycle count
{0x7a00,0x7a00, 1,1,  8, OPCALL(Z7A_0000_0000),				 "halt", DASMFLAG_STEP_OVER},
{0x7b00,0x7b00, 1,1, 16, OPCALL(Z7B_0000_0000_segmented),			 "iret", DASMFLAG_STEP_OUT},
{0x7b08,0x7b08, 1,1,  5, OPCALL(Z7B_0000_1000),				 "mset"},
{0x7b09,0x7b09, 1,1,  5, OPCALL(Z7B_0000_1001),				 "mres"},
{0x7b0a,0x7b0a, 1,1,  7, OPCALL(Z7B_0000_1010),				 "mbit"},
{0x7b0d,0x7bfd,16,1, 12, OPCALL(Z7B_dddd_1101),				 "mreq    %rw2"},
{0x7c00,0x7c03, 1,1,  7, OPCALL(Z7C_0000_00ii),				 "di      %i3"},
{0x7c04,0x7c07, 1,1,  7, OPCALL(Z7C_0000_01ii),				 "ei      %i3"},
{0x7d02,0x7df2,16,1,  7, OPCALL(Z7D_dddd_0ccc),          			 "ldctl   %rw2,fcw"},
{0x7d03,0x7df3,16,1,  7, OPCALL(Z7D_dddd_0ccc),          			 "ldctl   %rw2,refresh"},
{0x7d04,0x7df4,16,1,  7, OPCALL(Z7D_dddd_0ccc),          			 "ldctl   %rw2,psapseg"},
{0x7d05,0x7df5,16,1,  7, OPCALL(Z7D_dddd_0ccc),          			 "ldctl   %rw2,psap"},
{0x7d06,0x7df6,16,1,  7, OPCALL(Z7D_dddd_0ccc),          			 "ldctl   %rw2,nspseg"},
{0x7d07,0x7df7,16,1,  7, OPCALL(Z7D_dddd_0ccc),          			 "ldctl   %rw2,nsp"},
{0x7d0a,0x7dfa,16,1,  7, OPCALL(Z7D_ssss_1ccc),          			 "ldctl   fcw,%rw2"},
{0x7d0b,0x7dfb,16,1,  7, OPCALL(Z7D_ssss_1ccc),          			 "ldctl   refresh,%rw2"},
{0x7d0c,0x7dfc,16,1,  7, OPCALL(Z7D_ssss_1ccc),          			 "ldctl   psapseg,%rw2"},
{0x7d0d,0x7dfd,16,1,  7, OPCALL(Z7D_ssss_1ccc),          			 "ldctl   psap,%rw2"},
{0x7d0e,0x7dfe,16,1,  7, OPCALL(Z7D_ssss_1ccc),          			 "ldctl   nspseg,%rw2"},
{0x7d0f,0x7dff,16,1,  7, OPCALL(Z7D_ssss_1ccc),          			 "ldctl   nsp,%rw2"},
{0x7f00,0x7fff, 1,1, 39, OPCALL(Z7F_imm8_segmented),				 "sc      %#b1"},
{0x8000,0x80ff, 1,1,  4, OPCALL(Z80_ssss_dddd),				 "addb    %rb3,%rb2"},
{0x8100,0x81ff, 1,1,  4, OPCALL(Z81_ssss_dddd),				 "add     %rw3,%rw2"},
{0x8200,0x82ff, 1,1,  4, OPCALL(Z82_ssss_dddd),				 "subb    %rb3,%rb2"},
{0x8300,0x83ff, 1,1,  4, OPCALL(Z83_ssss_dddd),				 "sub     %rw3,%rw2"},
{0x8400,0x84ff, 1,1,  4, OPCALL(Z84_ssss_dddd),				 "orb     %rb3,%rb2"},
{0x8500,0x85ff, 1,1,  4, OPCALL(Z85_ssss_dddd),				 "or      %rw3,%rw2"},
{0x8600,0x86ff, 1,1,  4, OPCALL(Z86_ssss_dddd),				 "andb    %rb3,%rb2"},
{0x8700,0x87ff, 1,1,  4, OPCALL(Z87_ssss_dddd),				 "and     %rw3,%rw2"},
{0x8800,0x88ff, 1,1,  4, OPCALL(Z88_ssss_dddd),				 "xorb    %rb3,%rb2"},
{0x8900,0x89ff, 1,1,  4, OPCALL(Z89_ssss_dddd),				 "xor     %rw3,%rw2"},
{0x8a00,0x8aff, 1,1,  4, OPCALL(Z8A_ssss_dddd),				 "cpb     %rb3,%rb2"},
{0x8b00,0x8bff, 1,1,  4, OPCALL(Z8B_ssss_dddd),				 "cp      %rw3,%rw2"},
{0x8c00,0x8cf0,16,1,  7, OPCALL(Z8C_dddd_0000),				 "comb    %rb2"},
{0x8c01,0x8cf1,16,1,  7, OPCALL(Z7D_dddd_0ccc),                           "ldctlb  %rb2,flags"},
{0x8c02,0x8cf2,16,1,  7, OPCALL(Z8C_dddd_0010),				 "negb    %rb2"},
{0x8c04,0x8cf4,16,1,  7, OPCALL(Z8C_dddd_0100),				 "testb   %rb2"},
{0x8c06,0x8cf6,16,1,  7, OPCALL(Z8C_dddd_0110),				 "tsetb   %rb2"},
{0x8c08,0x8cf8,16,1,  7, OPCALL(Z8C_dddd_1000),				 "clrb    %rb2"},
{0x8c09,0x8cf9,16,1,  7, OPCALL(Z7D_ssss_1ccc),                           "ldctlb  flags,%rb2"},
{0x8d00,0x8df0,16,1,  7, OPCALL(Z8D_dddd_0000),				 "com     %rw2"},
{0x8d01,0x8df1,16,1,  7, OPCALL(Z8D_imm4_0001),				 "setflg  %f2"},
{0x8d02,0x8df2,16,1,  7, OPCALL(Z8D_dddd_0010),				 "neg     %rw2"},
{0x8d03,0x8df3,16,1,  7, OPCALL(Z8D_imm4_0011),				 "resflg  %f2"},
{0x8d04,0x8df4,16,1,  7, OPCALL(Z8D_dddd_0100),				 "test    %rw2"},
{0x8d05,0x8df5,16,1,  7, OPCALL(Z8D_imm4_0101),				 "comflg  %f2"},
{0x8d06,0x8df6,16,1,  7, OPCALL(Z8D_dddd_0110),				 "tset    %rw2"},
{0x8d07,0x8d07, 1,1,  7, OPCALL(Z8D_0000_0111),				 "nop"},
{0x8d08,0x8df8,16,1,  7, OPCALL(Z8D_dddd_1000),				 "clr     %rw2"},
{0x8e00,0x8eff, 1,2, 11, OPCALL(Z8E_imm16),				 "ext8e   %#l0"},
{0x8f00,0x8fff, 1,2, 11, OPCALL(Z8F_imm16),				 "ext8f   %#l0"},
{0x9000,0x90ff, 1,1,  8, OPCALL(Z90_ssss_dddd),				 "cpl     %rl3,%rl2"},
{0x9110,0x91ff, 1,1, 12, OPCALL(Z91_ddN0_ssss_segmented),			 "pushl   @%rl2,%rl3"},
{0x9200,0x92ff, 1,1,  8, OPCALL(Z92_ssss_dddd),				 "subl    %rl3,%rl2"},
{0x9310,0x93ff, 1,1,  9, OPCALL(Z93_ddN0_ssss_segmented),			 "push    @%rl2,%rw3"},
{0x9400,0x94ff, 1,1,  5, OPCALL(Z94_ssss_dddd),				 "ldl     %rl3,%rl2"},
{0x9510,0x95ff, 1,1, 12, OPCALL(Z95_ssN0_dddd_segmented),			 "popl    %rl3,@%rl2"},
{0x9600,0x96ff, 1,1,  8, OPCALL(Z96_ssss_dddd),				 "addl    %rl3,%rl2"},
{0x9710,0x97ff, 1,1,  8, OPCALL(Z97_ssN0_dddd_segmented),			 "pop     %rw3,@%rl2"},
{0x9800,0x98ff, 1,1,282, OPCALL(Z98_ssss_dddd),				 "multl   %rq3,%rl2"},
{0x9900,0x99ff, 1,1, 70, OPCALL(Z99_ssss_dddd),				 "mult    %rl3,%rw2"},
{0x9a00,0x9aff, 1,1,744, OPCALL(Z9A_ssss_dddd),				 "divl    %rq3,%rl2"},
{0x9b00,0x9bff, 1,1,107, OPCALL(Z9B_ssss_dddd),				 "div     %rl3,%rw2"},
{0x9c08,0x9cf8,16,1, 13, OPCALL(Z9C_dddd_1000),				 "testl   %rl2"},
{0x9e00,0x9e0f, 1,1, 10, OPCALL(Z9E_0000_cccc_segmented),			 "ret     %c3", DASMFLAG_STEP_OUT},
{0xa000,0xa0ff, 1,1,  3, OPCALL(ZA0_ssss_dddd),				 "ldb     %rb3,%rb2"},
{0xa100,0xa1ff, 1,1,  3, OPCALL(ZA1_ssss_dddd),				 "ld      %rw3,%rw2"},
{0xa200,0xa2ff, 1,1,  4, OPCALL(ZA2_dddd_imm4),				 "resb    %rb2,#%3"},
{0xa300,0xa3ff, 1,1,  4, OPCALL(ZA3_dddd_imm4),				 "res     %rw2,#%3"},
{0xa400,0xa4ff, 1,1,  4, OPCALL(ZA4_dddd_imm4),				 "setb    %rb2,#%3"},
{0xa500,0xa5ff, 1,1,  4, OPCALL(ZA5_dddd_imm4),				 "set     %rw2,#%3"},
{0xa600,0xa6ff, 1,1,  4, OPCALL(ZA6_dddd_imm4),				 "bitb    %rb2,#%3"},
{0xa700,0xa7ff, 1,1,  4, OPCALL(ZA7_dddd_imm4),				 "bit     %rw2,#%3"},
{0xa800,0xa8ff, 1,1,  4, OPCALL(ZA8_dddd_imm4m1),			 "incb    %rb2,#%+3"},
{0xa900,0xa9ff, 1,1,  4, OPCALL(ZA9_dddd_imm4m1),			 "inc     %rw2,#%+3"},
{0xaa00,0xaaff, 1,1,  4, OPCALL(ZAA_dddd_imm4m1),			 "decb    %rb2,#%+3"},
{0xab00,0xabff, 1,1,  4, OPCALL(ZAB_dddd_imm4m1),			 "dec     %rw2,#%+3"},
{0xac00,0xacff, 1,1,  6, OPCALL(ZAC_ssss_dddd),				 "exb     %rb3,%rb2"},
{0xad00,0xadff, 1,1,  6, OPCALL(ZAD_ssss_dddd),				 "ex      %rw3,%rw2"},
{0xae00,0xaeff, 1,1,  5, OPCALL(ZAE_dddd_cccc),				 "tccb    %c3,%rb2"},
{0xaf00,0xafff, 1,1,  5, OPCALL(ZAF_dddd_cccc),				 "tcc     %c3,%rw2"},
{0xb000,0xb0f0,16,1,  5, OPCALL(ZB0_dddd_0000),				 "dab     %rb2"},
{0xb100,0xb1f0,16,1, 11, OPCALL(ZB1_dddd_0000),				 "extsb   %rw2"},
{0xb107,0xb1f7,16,1, 11, OPCALL(ZB1_dddd_0111),				 "extsl   %rq2"},
{0xb10a,0xb1fa,16,1, 11, OPCALL(ZB1_dddd_1010),				 "exts    %rl2"},
{0xb200,0xb2f0,16,1,  6, OPCALL(ZB2_dddd_00I0),				 "rlb     %rb2,#%?3"},
{0xb201,0xb2f1,16,2, 13, OPCALL(ZB2_dddd_0001_imm8), 			 "s%*lb    %rb2,%$3"},
{0xb202,0xb2f2,16,1,  6, OPCALL(ZB2_dddd_00I0),				 "rlb     %rb2,#%?3"},
{0xb203,0xb2f3,16,2, 15, OPCALL(ZB2_dddd_0011_0000_ssss_0000_0000),	 "sdlb    %rb2,%rw5"},
{0xb204,0xb2f4,16,1,  6, OPCALL(ZB2_dddd_01I0),				 "rrb     %rb2,#%?3"},
{0xb206,0xb2f6,16,1,  6, OPCALL(ZB2_dddd_01I0),				 "rrb     %rb2,#%?3"},
{0xb208,0xb2f8,16,1,  6, OPCALL(ZB2_dddd_10I0),				 "rlcb    %rb2,#%?3"},
{0xb209,0xb2f9,16,2, 13, OPCALL(ZB2_dddd_1001_imm8), 			 "s%*ab    %rb2,%$3"},
{0xb20a,0xb2fa,16,1,  6, OPCALL(ZB2_dddd_10I0),				 "rlcb    %rb2,#%?3"},
{0xb20b,0xb2fb,16,2, 15, OPCALL(ZB2_dddd_1011_0000_ssss_0000_0000),	 "sdab    %rb2,%rw5"},
{0xb20c,0xb2fc,16,1,  6, OPCALL(ZB2_dddd_11I0),				 "rrcb    %rb2,#%?3"},
{0xb20e,0xb2fe,16,1,  6, OPCALL(ZB2_dddd_11I0),				 "rrcb    %rb2,#%?3"},
{0xb300,0xb3f0,16,1,  6, OPCALL(ZB3_dddd_00I0),				 "rl      %rw2,#%?3"},
{0xb301,0xb3f1,16,2, 13, OPCALL(ZB3_dddd_0001_imm8), 			 "s%*l     %rw2,%$3"},
{0xb302,0xb3f2,16,1,  6, OPCALL(ZB3_dddd_00I0),				 "rl      %rw2,#%?3"},
{0xb303,0xb3f3,16,2, 15, OPCALL(ZB3_dddd_0011_0000_ssss_0000_0000),	 "sdl     %rw2,%rw5"},
{0xb304,0xb3f4,16,1,  6, OPCALL(ZB3_dddd_01I0),				 "rr      %rw2,#%?3"},
{0xb305,0xb3f5,16,2, 13, OPCALL(ZB3_dddd_0101_imm8), 			 "s%*ll    %rl2,%$3"},
{0xb306,0xb3f6,16,1,  6, OPCALL(ZB3_dddd_01I0),				 "rr      %rw2,#%?3"},
{0xb307,0xb3f7,16,2, 15, OPCALL(ZB3_dddd_0111_0000_ssss_0000_0000),	 "sdll    %rl2,%rw5"},
{0xb308,0xb3f8,16,1,  6, OPCALL(ZB3_dddd_10I0),				 "rlc     %rw2,#%?3"},
{0xb309,0xb3f9,16,2, 13, OPCALL(ZB3_dddd_1001_imm8), 			 "s%*a     %rw2,%$3"},
{0xb30a,0xb3fa,16,1,  6, OPCALL(ZB3_dddd_10I0),				 "rlc     %rw2,#%?3"},
{0xb30b,0xb3fb,16,2, 15, OPCALL(ZB3_dddd_1011_0000_ssss_0000_0000),	 "sda     %rw2,%rw5"},
{0xb30c,0xb3fc,16,1,  6, OPCALL(ZB3_dddd_11I0),				 "rrc     %rw2,#%?3"},
{0xb30d,0xb3fd,16,2, 13, OPCALL(ZB3_dddd_1101_imm8), 			 "s%*al    %rl2,%$3"},
{0xb30e,0xb3fe,16,1,  6, OPCALL(ZB3_dddd_11I0),				 "rrc     %rw2,#%?3"},
{0xb30f,0xb3ff,16,2, 15, OPCALL(ZB3_dddd_1111_0000_ssss_0000_0000),	 "sdal    %rl2,%rw5"},
{0xb400,0xb4ff, 1,1,  5, OPCALL(ZB4_ssss_dddd),				 "adcb    %rb3,%rb2"},
{0xb500,0xb5ff, 1,1,  5, OPCALL(ZB5_ssss_dddd),				 "adc     %rw3,%rw2"},
{0xb600,0xb6ff, 1,1,  5, OPCALL(ZB6_ssss_dddd),				 "sbcb    %rb3,%rb2"},
{0xb700,0xb7ff, 1,1,  5, OPCALL(ZB7_ssss_dddd),				 "sbc     %rw3,%rw2"},
{0xb810,0xb8f0,16,2, 25, OPCALL(ZB8_ddN0_0000_0000_rrrr_ssN0_0000_segmented),	 "trib    @%rl2,@%rl6,%rw5"},
{0xb812,0xb8f2,16,2, 25, OPCALL(ZB8_ddN0_0010_0000_rrrr_ssN0_0000_segmented),	 "trtib   @%rl2,@%rl6,%rw5"},
{0xb814,0xb8f4,16,2, 25, OPCALL(ZB8_ddN0_0100_0000_rrrr_ssN0_0000_segmented),	 "trirb   @%rl2,@%rl6,%rw5"},
{0xb816,0xb8f6,16,2, 25, OPCALL(ZB8_ddN0_0110_0000_rrrr_ssN0_1110_segmented),	 "trtirb  @%rl2,@%rl6,%rw5"},
{0xb818,0xb8f8,16,2, 25, OPCALL(ZB8_ddN0_1000_0000_rrrr_ssN0_0000_segmented),	 "trdb    @%rl2,@%rl6,%rw5"},
{0xb81a,0xb8fa,16,2, 25, OPCALL(ZB8_ddN0_1010_0000_rrrr_ssN0_0000_segmented),	 "trtdb   @%rl2,@%rl6,%rw5"},
{0xb81c,0xb8fc,16,2, 25, OPCALL(ZB8_ddN0_1100_0000_rrrr_ssN0_0000_segmented),	 "trdrb   @%rl2,@%rl6,%rw5"},
{0xb81e,0xb8fe,16,2, 25, OPCALL(ZB8_ddN0_1110_0000_rrrr_ssN0_1110_segmented),	 "trtdrb  @%rl2,@%rl6,%rw5"},
{0xba10,0xbaf0,16,2, 20, OPCALL(ZBA_ssN0_0000_0000_rrrr_dddd_cccc_segmented),	 "cpib    %rb6,@%rl2,%rw5,%c7"},
{0xba11,0xbaf1,16,2,  9, OPCALL(ZBA_ssN0_0001_0000_rrrr_ddN0_x000_segmented),	 "ldi%^b   @%rl6,@%rl2,%rw5", DASMFLAG_STEP_OVER},
{0xba12,0xbaf2,16,2, 25, OPCALL(ZBA_ssN0_0010_0000_rrrr_ddN0_cccc_segmented),	 "cpsib   @%rl6,@%rl2,%rw5,%c7"},
{0xba14,0xbaf4,16,2,  9, OPCALL(ZBA_ssN0_0100_0000_rrrr_dddd_cccc_segmented),	 "cpirb   %rb6,@%rl2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xba16,0xbaf6,16,2, 14, OPCALL(ZBA_ssN0_0110_0000_rrrr_ddN0_cccc_segmented),	 "cpsirb  @%rl6,@%rl2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xba18,0xbaf8,16,2, 20, OPCALL(ZBA_ssN0_1000_0000_rrrr_dddd_cccc_segmented),	 "cpdb    %rb6,@%rl2,%rw5,%c7"},
{0xba19,0xbaf9,16,2,  9, OPCALL(ZBA_ssN0_1001_0000_rrrr_ddN0_x000_segmented),	 "ldd%^b  @%rl2,@%rl6,%rw5", DASMFLAG_STEP_OVER},
{0xba1a,0xbafa,16,2, 25, OPCALL(ZBA_ssN0_1010_0000_rrrr_ddN0_cccc_segmented),	 "cpsdb   @%rl6,@%rl2,%rw5,%c7"},
{0xba1c,0xbafc,16,2,  9, OPCALL(ZBA_ssN0_1100_0000_rrrr_dddd_cccc_segmented),	 "cpdrb   %rb6,@%rl2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xba1e,0xbafe,16,2, 14, OPCALL(ZBA_ssN0_1110_0000_rrrr_ddN0_cccc_segmented),	 "cpsdrb  @%rl6,@%rl2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xbb10,0xbbf0,16,2, 20, OPCALL(ZBB_ssN0_0000_0000_rrrr_dddd_cccc_segmented),	 "cpi     %rw6,@%rl2,%rw5,%c7"},
{0xbb11,0xbbf1,16,2,  9, OPCALL(ZBB_ssN0_0001_0000_rrrr_ddN0_x000_segmented),	 "ldi%^   @%rl6,@%rl2,%rw5", DASMFLAG_STEP_OVER},
{0xbb12,0xbbf2,16,2, 25, OPCALL(ZBB_ssN0_0010_0000_rrrr_ddN0_cccc_segmented),	 "cpsi    @%rl6,@%rl2,%rw5,%c7"},
{0xbb14,0xbbf4,16,2,  9, OPCALL(ZBB_ssN0_0100_0000_rrrr_dddd_cccc_segmented),	 "cpir    %rw6,@%rl2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xbb16,0xbbf6,16,2, 14, OPCALL(ZBB_ssN0_0110_0000_rrrr_ddN0_cccc_segmented),	 "cpsir   @%rl6,@%rl2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xbb18,0xbbf8,16,2, 20, OPCALL(ZBB_ssN0_1000_0000_rrrr_dddd_cccc_segmented),	 "cpd     %rw6,@%rl2,%rw5,%c7"},
{0xbb19,0xbbf9,16,2,  9, OPCALL(ZBB_ssN0_1001_0000_rrrr_ddN0_x000_segmented),	 "ldd%^   @%rl2,@%rl6,%rw5", DASMFLAG_STEP_OVER},
{0xbb1a,0xbbfa,16,2, 25, OPCALL(ZBB_ssN0_1010_0000_rrrr_ddN0_cccc_segmented),	 "cpsd    @%rl6,@%rl2,%rw5,%c7"},
{0xbb1c,0xbbfc,16,2,  9, OPCALL(ZBB_ssN0_1100_0000_rrrr_dddd_cccc_segmented),	 "cpdr    %rw6,@%rl2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xbb1e,0xbbfe,16,2, 14, OPCALL(ZBB_ssN0_1110_0000_rrrr_ddN0_cccc_segmented),	 "cpsdr   @%rl6,@%rl2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xbc00,0xbcff, 1,1,  9, OPCALL(ZBC_aaaa_bbbb),				 "rrdb    %rb3,%rb2"},
{0xbd00,0xbdff, 1,1,  5, OPCALL(ZBD_dddd_imm4),				 "ldk     %rw2,#%3"},
{0xbe00,0xbeff, 1,1,  9, OPCALL(ZBE_aaaa_bbbb),				 "rldb    %rb3,%rb2"},
{0xc000,0xcfff, 1,1,  5, OPCALL(ZC_dddd_imm8),				 "ldb     %rb1,%#b1"},
{0xd000,0xdfff, 1,1, 10, OPCALL(ZD_dsp12_segmented),			 "calr    %d2", DASMFLAG_STEP_OVER},
{0xe000,0xefff, 1,1,  6, OPCALL(ZE_cccc_dsp8),				 "jr      %c1,%d1"},
{0xf000,0xf07f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf100,0xf17f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf200,0xf27f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf300,0xf37f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf400,0xf47f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf500,0xf57f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf600,0xf67f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf700,0xf77f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf800,0xf87f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf900,0xf97f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xfa00,0xfa7f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xfb00,0xfb7f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xfc00,0xfc7f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xfd00,0xfd7f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xfe00,0xfe7f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xff00,0xff7f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf080,0xf0ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf180,0xf1ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf280,0xf2ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf380,0xf3ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf480,0xf4ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf580,0xf5ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf680,0xf6ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf780,0xf7ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf880,0xf8ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf980,0xf9ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xfa80,0xfaff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xfb80,0xfbff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xfc80,0xfcff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xfd80,0xfdff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xfe80,0xfeff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xff80,0xffff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0, 	0,	   0,	0,	0,0,									 0},
};

static const Z8000_init table[] = {
{0x0000,0x000f, 1,2,  7, OPCALL(Z00_0000_dddd_imm8), 			 "addb    %rb3,%#b3"},
{0x0010,0x00ff, 1,1,  7, OPCALL(Z00_ssN0_dddd),				 "addb    %rb3,@%rw2"},
{0x0100,0x010f, 1,2,  7, OPCALL(Z01_0000_dddd_imm16),			 "add     %rw3,%#w1"},
{0x0110,0x01ff, 1,1,  7, OPCALL(Z01_ssN0_dddd),				 "add     %rw3,@%rw2"},
{0x0200,0x020f, 1,2,  7, OPCALL(Z02_0000_dddd_imm8), 			 "subb    %rb3,%#b3"},
{0x0210,0x02ff, 1,1,  7, OPCALL(Z02_ssN0_dddd),				 "subb    %rb3,@%rw2"},
{0x0300,0x030f, 1,2,  7, OPCALL(Z03_0000_dddd_imm16),			 "sub     %rw3,%#w1"},
{0x0310,0x03ff, 1,1,  7, OPCALL(Z03_ssN0_dddd),				 "sub     %rw3,@%rw2"},
{0x0400,0x040f, 1,2,  7, OPCALL(Z04_0000_dddd_imm8), 			 "orb     %rb3,%#b3"},
{0x0410,0x04ff, 1,1,  7, OPCALL(Z04_ssN0_dddd),				 "orb     %rb3,@%rw2"},
{0x0500,0x050f, 1,2,  7, OPCALL(Z05_0000_dddd_imm16),			 "or      %rw3,%#w1"},
{0x0510,0x05ff, 1,1,  7, OPCALL(Z05_ssN0_dddd),				 "or      %rw3,@%rw2"},
{0x0600,0x060f, 1,2,  7, OPCALL(Z06_0000_dddd_imm8), 			 "andb    %rb3,%#b3"},
{0x0610,0x06ff, 1,1,  7, OPCALL(Z06_ssN0_dddd),				 "andb    %rb3,@%rw2"},
{0x0700,0x070f, 1,2,  7, OPCALL(Z07_0000_dddd_imm16),			 "and     %rw3,%#w1"},
{0x0710,0x07ff, 1,1,  7, OPCALL(Z07_ssN0_dddd),				 "and     %rw3,@%rw2"},
{0x0800,0x080f, 1,2,  7, OPCALL(Z08_0000_dddd_imm8), 			 "xorb    %rb3,%#b3"},
{0x0810,0x08ff, 1,1,  7, OPCALL(Z08_ssN0_dddd),				 "xorb    %rb3,@%rw2"},
{0x0900,0x090f, 1,2,  7, OPCALL(Z09_0000_dddd_imm16),			 "xor     %rw3,%#w1"},
{0x0910,0x09ff, 1,1,  7, OPCALL(Z09_ssN0_dddd),				 "xor     %rw3,@%rw2"},
{0x0a00,0x0a0f, 1,2,  7, OPCALL(Z0A_0000_dddd_imm8), 			 "cpb     %rb3,%#b3"},
{0x0a10,0x0aff, 1,1,  7, OPCALL(Z0A_ssN0_dddd),				 "cpb     %rb3,@%rw2"},
{0x0b00,0x0b0f, 1,2,  7, OPCALL(Z0B_0000_dddd_imm16),			 "cp      %rw3,%#w1"},
{0x0b10,0x0bff, 1,1,  7, OPCALL(Z0B_ssN0_dddd),				 "cp      %rw3,@%rw2"},
{0x0c10,0x0cf0,16,1, 12, OPCALL(Z0C_ddN0_0000),				 "comb    @%rw2"},
{0x0c11,0x0cf1,16,2, 11, OPCALL(Z0C_ddN0_0001_imm8), 			 "cpb     @%rw2,%#b3"},
{0x0c12,0x0cf2,16,1, 12, OPCALL(Z0C_ddN0_0010),				 "negb    @%rw2"},
{0x0c14,0x0cf4,16,1,  8, OPCALL(Z0C_ddN0_0100),				 "testb   @%rw2"},
{0x0c15,0x0cf5,16,2, 11, OPCALL(Z0C_ddN0_0101_imm8), 			 "ldb     @%rw2,%#b3"},
{0x0c16,0x0cf6,16,1, 11, OPCALL(Z0C_ddN0_0110),				 "tsetb   @%rw2"},
{0x0c18,0x0cf8,16,1,  8, OPCALL(Z0C_ddN0_1000),				 "clrb    @%rw2"},
{0x0d10,0x0df0,16,1, 12, OPCALL(Z0D_ddN0_0000),				 "com     @%rw2"},
{0x0d11,0x0df1,16,2, 11, OPCALL(Z0D_ddN0_0001_imm16),			 "cp      @%rw2,%#w1"},
{0x0d12,0x0df2,16,1, 12, OPCALL(Z0D_ddN0_0010),				 "neg     @%rw2"},
{0x0d14,0x0df4,16,1,  8, OPCALL(Z0D_ddN0_0100),				 "test    @%rw2"},
{0x0d15,0x0df5,16,2, 11, OPCALL(Z0D_ddN0_0101_imm16),			 "ld      @%rw2,%#w1"},
{0x0d16,0x0df6,16,1, 11, OPCALL(Z0D_ddN0_0110),				 "tset    @%rw2"},
{0x0d18,0x0df8,16,1,  8, OPCALL(Z0D_ddN0_1000),				 "clr     @%rw2"},
{0x0d19,0x0df9,16,2, 12, OPCALL(Z0D_ddN0_1001_imm16),			 "push    @%rw2,%#w1"},
{0x0e00,0x0eff, 1,2, 11, OPCALL(Z0E_imm16),				 "ext0e   %#l0"},
{0x0f00,0x0fff, 1,2, 11, OPCALL(Z0F_imm16),				 "ext0f   %#l0"},
{0x1000,0x100f, 1,3, 14, OPCALL(Z10_0000_dddd_imm32),			 "cpl     %rl3,%#l1"},
{0x1010,0x10ff, 1,1, 14, OPCALL(Z10_ssN0_dddd),				 "cpl     %rl3,@%rw2"},
{0x1111,0x11ff, 1,1, 20, OPCALL(Z11_ddN0_ssN0),				 "pushl   @%rw2,@%rw3"},
{0x1200,0x120f, 1,3, 14, OPCALL(Z12_0000_dddd_imm32),			 "subl    %rl3,%#l1"},
{0x1210,0x12ff, 1,1, 14, OPCALL(Z12_ssN0_dddd),				 "subl    %rl3,@%rw2"},
{0x1311,0x13ff, 1,1, 13, OPCALL(Z13_ddN0_ssN0),				 "push    @%rw2,@%rw3"},
{0x1400,0x140f, 1,3, 11, OPCALL(Z14_0000_dddd_imm32),			 "ldl     %rl3,%#l1"},
{0x1410,0x14ff, 1,1, 11, OPCALL(Z14_ssN0_dddd),				 "ldl     %rl3,@%rw2"},
{0x1511,0x15ff, 1,1, 19, OPCALL(Z15_ssN0_ddN0),				 "popl    @%rw3,@%rw2"},
{0x1600,0x160f, 1,3, 14, OPCALL(Z16_0000_dddd_imm32),			 "addl    %rl3,%#l1"},
{0x1610,0x16ff, 1,1, 14, OPCALL(Z16_ssN0_dddd),				 "addl    %rl3,@%rw2"},
{0x1711,0x17ff, 1,1, 12, OPCALL(Z17_ssN0_ddN0),				 "pop     @%rw3,@%rw2"},
{0x1800,0x180f, 1,3,282, OPCALL(!!!!!!!!!!!!!!!!!!!!!!!),		 "multl   %rq3,%#l1"}, 
{0x1810,0x18ff, 1,1,282, OPCALL(Z18_ssN0_dddd),				 "multl   %rq3,@%rw2"},
{0x1900,0x190f, 1,2, 70, OPCALL(Z19_0000_dddd_imm16),			 "mult    %rl3,%#w1"},
{0x1910,0x19ff, 1,1, 70, OPCALL(Z19_ssN0_dddd),				 "mult    %rl3,@%rw2"},
{0x1a00,0x1a0f, 1,3,744, OPCALL(Z1A_0000_dddd_imm32),			 "divl    %rq3,%#l1"},
{0x1a10,0x1aff, 1,1,744, OPCALL(Z1A_ssN0_dddd),				 "divl    %rq3,@%rw2"},
{0x1b00,0x1b0f, 1,2,107, OPCALL(Z1B_0000_dddd_imm16),			 "div     %rl3,%#w1"},
{0x1b10,0x1bff, 1,1,107, OPCALL(Z1B_ssN0_dddd),				 "div     %rl3,@%rw2"},
{0x1c11,0x1cf1,16,2, 11, OPCALL(Z1C_ssN0_0001_0000_dddd_0000_nmin1), 	 "ldm     %rw5,@%rw2,#%n"},
{0x1c18,0x1cf8,16,1, 13, OPCALL(Z1C_ddN0_1000),				 "testl   @%rw2"},
{0x1c19,0x1cf9,16,2, 11, OPCALL(Z1C_ddN0_1001_0000_ssss_0000_nmin1), 	 "ldm     @%rw2,%rw5,#%n"},
{0x1d10,0x1dff, 1,1, 11, OPCALL(Z1D_ddN0_ssss),				 "ldl     @%rw2,%rl3"},
{0x1e10,0x1eff, 1,1, 10, OPCALL(Z1E_ddN0_cccc),				 "jp      %c3,@%rw2"},
{0x1f10,0x1ff0,16,1, 10, OPCALL(Z1F_ddN0_0000),				 "call    @%rw2", DASMFLAG_STEP_OVER},
{0x2000,0x200f, 1,2,  7, OPCALL(Z20_0000_dddd_imm8),     		 "ldb     %rb3,%#b1"},
{0x2010,0x20ff, 1,1,  7, OPCALL(Z20_ssN0_dddd),				 "ldb     %rb3,@%rw2"},
{0x2100,0x210f, 1,2,  7, OPCALL(Z21_0000_dddd_imm16),			 "ld      %rw3,%#w1"},
{0x2110,0x21ff, 1,1,  7, OPCALL(Z21_ssN0_dddd),				 "ld      %rw3,@%rw2"},
{0x2200,0x220f, 1,2, 10, OPCALL(Z22_0000_ssss_0000_dddd_0000_0000),	 "resb    %rb5,%rw3"},
{0x2210,0x22ff, 1,1, 11, OPCALL(Z22_ddN0_imm4),				 "resb    @%rw2,#%3"},
{0x2300,0x230f, 1,2, 10, OPCALL(Z23_0000_ssss_0000_dddd_0000_0000),	 "res     %rw5,%rw3"},
{0x2310,0x23ff, 1,1, 11, OPCALL(Z23_ddN0_imm4),				 "res     @%rw2,#%3"},
{0x2400,0x240f, 1,2, 10, OPCALL(Z24_0000_ssss_0000_dddd_0000_0000),	 "setb    %rb5,%rw3"},
{0x2410,0x24ff, 1,1, 11, OPCALL(Z24_ddN0_imm4),				 "setb    @%rw2,#%3"},
{0x2500,0x250f, 1,2, 10, OPCALL(Z25_0000_ssss_0000_dddd_0000_0000),	 "set     %rw5,%rw3"},
{0x2510,0x25ff, 1,1, 11, OPCALL(Z25_ddN0_imm4),				 "set     @%rw2,#%3"},
{0x2600,0x260f, 1,2, 10, OPCALL(Z26_0000_ssss_0000_dddd_0000_0000),	 "bitb    %rb5,%rw3"},
{0x2610,0x26ff, 1,1,  8, OPCALL(Z26_ddN0_imm4),				 "bitb    @%rw3,#%3"},
{0x2700,0x270f, 1,2, 10, OPCALL(Z27_0000_ssss_0000_dddd_0000_0000),	 "bit     %rw5,%rw3"},
{0x2710,0x27ff, 1,1,  8, OPCALL(Z27_ddN0_imm4),				 "bit     @%rw2,#%3"},
{0x2810,0x28ff, 1,1, 11, OPCALL(Z28_ddN0_imm4m1),			 "incb    @%rw2,#%+3"},
{0x2910,0x29ff, 1,1, 11, OPCALL(Z29_ddN0_imm4m1),			 "inc     @%rw2,#%+3"},
{0x2a10,0x2aff, 1,1, 11, OPCALL(Z2A_ddN0_imm4m1),			 "decb    @%rw2,#%+3"},
{0x2b10,0x2bff, 1,1, 11, OPCALL(Z2B_ddN0_imm4m1),			 "dec     @%rw2,#%+3"},
{0x2c10,0x2cff, 1,1, 12, OPCALL(Z2C_ssN0_dddd),				 "exb     %rb3,@%rw2"},
{0x2d10,0x2dff, 1,1, 12, OPCALL(Z2D_ssN0_dddd),				 "ex      %rw3,@%rw2"},
{0x2e10,0x2eff, 1,1,  8, OPCALL(Z2E_ddN0_ssss),				 "ldb     @%rw2,%rb3"},
{0x2f10,0x2fff, 1,1,  8, OPCALL(Z2F_ddN0_ssss),				 "ld      @%rw2,%rw3"},
{0x3000,0x300f, 1,2, 14, OPCALL(Z30_0000_dddd_dsp16),			 "ldrb    %rb3,%p1"},
{0x3010,0x30ff, 1,2, 14, OPCALL(Z30_ssN0_dddd_imm16),			 "ldb     %rb3,%rw2(%#w1)"},
{0x3100,0x310f, 1,2, 14, OPCALL(Z31_0000_dddd_dsp16),			 "ldr     %rw3,%p1"},
{0x3110,0x31ff, 1,2, 14, OPCALL(Z31_ssN0_dddd_imm16),			 "ld      %rw3,%rw2(%#w1)"},
{0x3200,0x320f, 1,2, 14, OPCALL(Z32_0000_ssss_dsp16),			 "ldrb    %p1,%rb3"},
{0x3210,0x32ff, 1,2, 14, OPCALL(Z32_ddN0_ssss_imm16),			 "ldb     %rw2(%#w1),%rb3"},
{0x3300,0x330f, 1,2, 14, OPCALL(Z33_0000_ssss_dsp16),			 "ldr     %p1,%rw3"},
{0x3310,0x33ff, 1,2, 14, OPCALL(Z33_ddN0_ssss_imm16),			 "ld      %rw2(%#w1),%rw3"},
{0x3400,0x340f, 1,2, 15, OPCALL(Z34_0000_dddd_dsp16),			 "ldar    %rw3,%p1"},
{0x3410,0x34ff, 1,2, 15, OPCALL(Z34_ssN0_dddd_imm16),			 "lda     %rw3,%rw2(%#w1)"},
{0x3500,0x350f, 1,2, 17, OPCALL(Z35_0000_dddd_dsp16),			 "ldrl    %rl3,%p1"},
{0x3510,0x35ff, 1,2, 17, OPCALL(Z35_ssN0_dddd_imm16),			 "ldl     %rl3,%rw2(%#w1)"},
{0x3600,0x3600, 1,1,  2, OPCALL(Z36_0000_0000),				 "bpt"},     //----UNKNOWN break point trap  (used by MAME debugger????)
{0x3700,0x370f, 1,2, 17, OPCALL(Z37_0000_ssss_dsp16),			 "ldrl    %p1,%rl3"},
{0x3710,0x37ff, 1,2, 17, OPCALL(Z37_ddN0_ssss_imm16),			 "ldl     %rw2(%#w1),%rl3"},
{0x3910,0x39f0,16,1, 12, OPCALL(Z39_ssN0_0000),				 "ldps    @%rw2"},
{0x3a04,0x3af4,16,2, 12, OPCALL(Z3A_dddd_0100_imm16),			 "%R %rb2,%a1"},
{0x3a05,0x3af5,16,2, 12, OPCALL(Z3A_dddd_0101_imm16),			 "%R %rb2,%a1"},
{0x3a06,0x3af6,16,2, 12, OPCALL(Z3A_ssss_0110_imm16),			 "%R %a1,%rb2"},
{0x3a07,0x3af7,16,2, 12, OPCALL(Z3A_ssss_0111_imm16),			 "%R %a1,%rb2"},
{0x3a10,0x3af0,16,2, 21, OPCALL(Z3A_ssss_0000_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3a11,0x3af1,16,2, 21, OPCALL(Z3A_ssss_0001_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3a12,0x3af2,16,2, 21, OPCALL(Z3A_ssss_0010_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3a13,0x3af3,16,2, 21, OPCALL(Z3A_ssss_0011_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3a18,0x3af8,16,2, 21, OPCALL(Z3A_ssss_1000_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3a19,0x3af9,16,2, 21, OPCALL(Z3A_ssss_1001_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3a1a,0x3afa,16,2, 21, OPCALL(Z3A_ssss_1010_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3a1b,0x3afb,16,2, 21, OPCALL(Z3A_ssss_1011_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3b04,0x3bf4,16,2, 12, OPCALL(Z3B_dddd_0100_imm16),			 "%R %rw2,%a1"},
{0x3b05,0x3bf5,16,2, 12, OPCALL(Z3B_dddd_0101_imm16),			 "%R %rw2,%a1"},
{0x3b06,0x3bf6,16,2, 12, OPCALL(Z3B_ssss_0110_imm16),			 "%R %a1,%rw2"},
{0x3b07,0x3bf7,16,2, 12, OPCALL(Z3B_ssss_0111_imm16),			 "%R %a1,%rw2"},
{0x3b10,0x3bf0,16,2, 21, OPCALL(Z3B_ssss_0000_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3b11,0x3bf1,16,2, 21, OPCALL(Z3B_ssss_0001_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3b12,0x3bf2,16,2, 21, OPCALL(Z3B_ssss_0010_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3b13,0x3bf3,16,2, 21, OPCALL(Z3B_ssss_0011_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3b18,0x3bf8,16,2, 21, OPCALL(Z3B_ssss_1000_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3b19,0x3bf9,16,2, 21, OPCALL(Z3B_ssss_1001_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rw5"},
{0x3b1a,0x3bfa,16,2, 21, OPCALL(Z3B_ssss_1010_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rb5"},
{0x3b1b,0x3bfb,16,2, 21, OPCALL(Z3B_ssss_1011_0000_aaaa_dddd_x000),	 "%R @%rw6,@%rw2,%rb5"},
{0x3c00,0x3cff, 1,1, 10, OPCALL(Z3C_ssss_dddd),				 "inb     %rb3,@%rw2"},
{0x3d00,0x3dff, 1,1, 10, OPCALL(Z3D_ssss_dddd),				 "in      %rw3,@%rw2"},
{0x3e00,0x3eff, 1,1, 12, OPCALL(Z3E_dddd_ssss),				 "outb    @%rw2,%rb3"},
{0x3f00,0x3fff, 1,1, 12, OPCALL(Z3F_dddd_ssss),				 "out     @%rw2,%rw3"},
{0x4000,0x400f, 1,2,  9, OPCALL(Z40_0000_dddd_addr), 			 "addb    %rb3,%a1"},
{0x4010,0x40ff, 1,2, 10, OPCALL(Z40_ssN0_dddd_addr), 			 "addb    %rb3,%a1(%rw2)"},
{0x4100,0x410f, 1,2,  9, OPCALL(Z41_0000_dddd_addr), 			 "add     %rw3,%a1"},
{0x4110,0x41ff, 1,2, 10, OPCALL(Z41_ssN0_dddd_addr), 			 "add     %rw3,%a1(%rw2)"},
{0x4200,0x420f, 1,2,  9, OPCALL(Z42_0000_dddd_addr), 			 "subb    %rb3,%a1"},
{0x4210,0x42ff, 1,2, 10, OPCALL(Z42_ssN0_dddd_addr), 			 "subb    %rb3,%a1(%rw2)"},
{0x4300,0x430f, 1,2,  9, OPCALL(Z43_0000_dddd_addr), 			 "sub     %rw3,%a1"},
{0x4310,0x43ff, 1,2, 10, OPCALL(Z43_ssN0_dddd_addr), 			 "sub     %rw3,%a1(%rw2)"},
{0x4400,0x440f, 1,2,  9, OPCALL(Z44_0000_dddd_addr), 			 "orb     %rb3,%a1"},
{0x4410,0x44ff, 1,2, 10, OPCALL(Z44_ssN0_dddd_addr), 			 "orb     %rb3,%a1(%rw2)"},
{0x4500,0x450f, 1,2,  9, OPCALL(Z45_0000_dddd_addr), 			 "or      %rw3,%a1"},
{0x4510,0x45ff, 1,2, 10, OPCALL(Z45_ssN0_dddd_addr), 			 "or      %rw3,%a1(%rw2)"},
{0x4600,0x460f, 1,2,  9, OPCALL(Z46_0000_dddd_addr), 			 "andb    %rb3,%a1"},
{0x4610,0x46ff, 1,2, 10, OPCALL(Z46_ssN0_dddd_addr), 			 "andb    %rb3,%a1(%rw2)"},
{0x4700,0x470f, 1,2,  9, OPCALL(Z47_0000_dddd_addr), 			 "and     %rw3,%a1"},
{0x4710,0x47ff, 1,2, 10, OPCALL(Z47_ssN0_dddd_addr), 			 "and     %rw3,%a1(%rw2)"},
{0x4800,0x480f, 1,2,  9, OPCALL(Z48_0000_dddd_addr), 			 "xorb    %rb3,%a1"},
{0x4810,0x48ff, 1,2, 10, OPCALL(Z48_ssN0_dddd_addr), 			 "xorb    %rb3,%a1(%rw2)"},
{0x4900,0x490f, 1,2,  9, OPCALL(Z49_0000_dddd_addr), 			 "xor     %rw3,%a1"},
{0x4910,0x49ff, 1,2, 10, OPCALL(Z49_ssN0_dddd_addr), 			 "xor     %rw3,%a1(%rw2)"},
{0x4a00,0x4a0f, 1,2,  9, OPCALL(Z4A_0000_dddd_addr), 			 "cpb     %rb3,%a1"},
{0x4a10,0x4aff, 1,2, 10, OPCALL(Z4A_ssN0_dddd_addr), 			 "cpb     %rb3,%a1(%rw2)"},
{0x4b00,0x4b0f, 1,2,  9, OPCALL(Z4B_0000_dddd_addr), 			 "cp      %rw3,%a1"},
{0x4b10,0x4bff, 1,2, 10, OPCALL(Z4B_ssN0_dddd_addr), 			 "cp      %rw3,%a1(%rw2)"},
{0x4c00,0x4c00, 1,2, 15, OPCALL(Z4C_0000_0000_addr), 			 "comb    %a1"},
{0x4c01,0x4c01, 1,3, 14, OPCALL(Z4C_0000_0001_addr_imm8),		 "cpb     %a1,%#b5"},
{0x4c02,0x4c02, 1,2, 15, OPCALL(Z4C_0000_0010_addr), 			 "negb    %a1"},
{0x4c04,0x4c04, 1,2, 11, OPCALL(Z4C_0000_0100_addr), 			 "testb   %a1"},
{0x4c05,0x4c05, 1,3, 14, OPCALL(Z4C_0000_0101_addr_imm8),		 "ldb     %a1,%#b5"},
{0x4c06,0x4c06, 1,2, 14, OPCALL(Z4C_0000_0110_addr), 			 "tsetb   %a1"},
{0x4c08,0x4c08, 1,2, 11, OPCALL(Z4C_0000_1000_addr), 			 "clrb    %a1"},
{0x4c10,0x4cf0,16,2, 16, OPCALL(Z4C_ddN0_0000_addr), 			 "comb    %a1(%rw2)"},
{0x4c11,0x4cf1,16,3, 15, OPCALL(Z4C_ddN0_0001_addr_imm8),		 "cpb     %a1(%rw2),%#b5"},
{0x4c12,0x4cf2,16,2, 16, OPCALL(Z4C_ddN0_0010_addr), 			 "negb    %a1(%rw2)"},
{0x4c14,0x4cf4,16,2, 12, OPCALL(Z4C_ddN0_0100_addr), 			 "testb   %a1(%rw2)"},
{0x4c15,0x4cf5,16,3, 15, OPCALL(Z4C_ddN0_0101_addr_imm8),		 "ldb     %a1(%rw2),%#b5"},
{0x4c16,0x4cf6,16,2, 15, OPCALL(Z4C_ddN0_0110_addr), 			 "tsetb   %a1(%rw2)"},
{0x4c18,0x4cf8,16,2, 12, OPCALL(Z4C_ddN0_1000_addr), 			 "clrb    %a1(%rw2)"},
{0x4d00,0x4d00, 1,2, 15, OPCALL(Z4D_0000_0000_addr), 			 "com     %a1"},
{0x4d01,0x4d01, 1,3, 14, OPCALL(Z4D_0000_0001_addr_imm16),		 "cp      %a1,%#w2"},
{0x4d02,0x4d02, 1,2, 15, OPCALL(Z4D_0000_0010_addr), 			 "neg     %a1"},
{0x4d04,0x4d04, 1,2, 11, OPCALL(Z4D_0000_0100_addr), 			 "test    %a1"},
{0x4d05,0x4d05, 1,3, 14, OPCALL(Z4D_0000_0101_addr_imm16),		 "ld      %a1,%#w2"},
{0x4d06,0x4d06, 1,2, 14, OPCALL(Z4D_0000_0110_addr), 			 "tset    %a1"},
{0x4d08,0x4d08, 1,2, 11, OPCALL(Z4D_0000_1000_addr), 			 "clr     %a1"},
{0x4d10,0x4df0,16,2, 16, OPCALL(Z4D_ddN0_0000_addr), 			 "com     %a1(%rw2)"},
{0x4d11,0x4df1,16,3, 15, OPCALL(Z4D_ddN0_0001_addr_imm16),		 "cp      %a1(%rw2),%#w2"},
{0x4d12,0x4df2,16,2, 16, OPCALL(Z4D_ddN0_0010_addr), 			 "neg     %a1(%rw2)"},
{0x4d14,0x4df4,16,2, 12, OPCALL(Z4D_ddN0_0100_addr), 			 "test    %a1(%rw2)"},
{0x4d15,0x4df5,16,3, 15, OPCALL(Z4D_ddN0_0101_addr_imm16),		 "ld      %a1(%rw2),%#w2"},
{0x4d16,0x4df6,16,2, 15, OPCALL(Z4D_ddN0_0110_addr), 			 "tset    %a1(%rw2)"},
{0x4d18,0x4df8,16,2, 12, OPCALL(Z4D_ddN0_1000_addr), 			 "clr     %a1(%rw2)"},
{0x4e00,0x4e0f, 1,3, 15, OPCALL(Z4E_ssN0_dddd),                         "ext4e   %#l0,%a2"},
{0x4e10,0x4eff, 1,3, 15, OPCALL(Z4E_ssss_dddd),                         "ext4e   %#l0,%a2"},
{0x4f00,0x4f0f, 1,3, 15, OPCALL(Z4F_ssN0_dddd),                         "ext4f   %#l0,%a2"},
{0x4f10,0x4fff, 1,3, 15, OPCALL(Z4F_ssss_dddd),                         "ext4f   %#l0,%a2"},
{0x5000,0x500f, 1,2, 15, OPCALL(Z50_0000_dddd_addr), 			 "cpl     %rl3,%a1"},
{0x5010,0x50ff, 1,2, 16, OPCALL(Z50_ssN0_dddd_addr), 			 "cpl     %rl3,%a1(%rw2)"},
{0x5110,0x51f0,16,2, 21, OPCALL(Z51_ddN0_0000_addr), 			 "pushl   @%rw2,%a1"},
{0x5111,0x51f1,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x5112,0x51f2,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x5113,0x51f3,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x5114,0x51f4,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x5115,0x51f5,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x5116,0x51f6,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x5117,0x51f7,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x5118,0x51f8,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x5119,0x51f9,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x511a,0x51fa,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x511b,0x51fb,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x511c,0x51fc,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x511d,0x51fd,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x511e,0x51fe,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x511f,0x51ff,16,2, 21, OPCALL(Z51_ddN0_ssN0_addr), 			 "pushl   @%rw2,%a1(%rw3)"},
{0x5200,0x520f, 1,2, 15, OPCALL(Z52_0000_dddd_addr), 			 "subl    %rl3,%a1"},
{0x5210,0x52ff, 1,2, 16, OPCALL(Z52_ssN0_dddd_addr), 			 "subl    %rl3,%a1(%rw2)"},
{0x5310,0x53f0,16,2, 14, OPCALL(Z53_ddN0_0000_addr), 			 "push    @%rw2,%a1"},
{0x5311,0x53f1,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x5312,0x53f2,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x5313,0x53f3,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x5314,0x53f4,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x5315,0x53f5,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x5316,0x53f6,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x5317,0x53f7,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x5318,0x53f8,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x5319,0x53f9,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x531a,0x53fa,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x531b,0x53fb,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x531c,0x53fc,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x531d,0x53fd,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x531e,0x53fe,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x531f,0x53ff,16,2, 14, OPCALL(Z53_ddN0_ssN0_addr), 			 "push    @%rw2,%a1(%rw3)"},
{0x5400,0x540f, 1,2, 12, OPCALL(Z54_0000_dddd_addr), 			 "ldl     %rl3,%a1"},
{0x5410,0x54ff, 1,2, 13, OPCALL(Z54_ssN0_dddd_addr), 			 "ldl     %rl3,%a1(%rw2)"},
{0x5510,0x55f0,16,2, 23, OPCALL(Z55_ssN0_0000_addr), 			 "popl    %a1,@%rw2"},
{0x5511,0x55f1,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x5512,0x55f2,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x5513,0x55f3,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x5514,0x55f4,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x5515,0x55f5,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x5516,0x55f6,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x5517,0x55f7,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x5518,0x55f8,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x5519,0x55f9,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x551a,0x55fa,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x551b,0x55fb,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x551c,0x55fc,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x551d,0x55fd,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x551e,0x55fe,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x551f,0x55ff,16,2, 23, OPCALL(Z55_ssN0_ddN0_addr), 			 "popl    %a1(%rw3),@%rw2"},
{0x5600,0x560f, 1,2, 15, OPCALL(Z56_0000_dddd_addr), 			 "addl    %rl3,%a1"},
{0x5610,0x56ff, 1,2, 16, OPCALL(Z56_ssN0_dddd_addr), 			 "addl    %rl3,%a1(%rw2)"},
{0x5710,0x57f0,16,2, 16, OPCALL(Z57_ssN0_0000_addr), 			 "pop     %a1,@%rw2"},
{0x5711,0x57f1,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x5712,0x57f2,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x5713,0x57f3,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x5714,0x57f4,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x5715,0x57f5,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x5716,0x57f6,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x5717,0x57f7,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x5718,0x57f8,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x5719,0x57f9,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x571a,0x57fa,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x571b,0x57fb,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x571c,0x57fc,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x571d,0x57fd,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x571e,0x57fe,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x571f,0x57ff,16,2, 16, OPCALL(Z57_ssN0_ddN0_addr), 			 "pop     %a1(%rw3),@%rw2"},
{0x5800,0x580f, 1,2,283, OPCALL(Z58_0000_dddd_addr), 			 "multl   %rq3,%a1"},
{0x5810,0x58ff, 1,2,284, OPCALL(Z58_ssN0_dddd_addr), 			 "multl   %rq3,%a1(%rw2)"},
{0x5900,0x590f, 1,2, 71, OPCALL(Z59_0000_dddd_addr), 			 "mult    %rl3,%a1"},
{0x5910,0x59ff, 1,2, 72, OPCALL(Z59_ssN0_dddd_addr), 			 "mult    %rl3,%a1(%rw2)"},
{0x5a00,0x5a0f, 1,2,745, OPCALL(Z5A_0000_dddd_addr), 			 "divl    %rq3,%a1"},
{0x5a10,0x5aff, 1,2,746, OPCALL(Z5A_ssN0_dddd_addr), 			 "divl    %rq3,%a1(%rw2)"},
{0x5b00,0x5b0f, 1,2,108, OPCALL(Z5B_0000_dddd_addr), 			 "div     %rl3,%a1"},
{0x5b10,0x5bff, 1,2,109, OPCALL(Z5B_ssN0_dddd_addr), 			 "div     %rl3,%a1(%rw2)"},
{0x5c01,0x5c01, 1,3, 14, OPCALL(Z5C_0000_0001_0000_dddd_0000_nmin1_addr), "ldm     %rw5,%a2,#%n"},
{0x5c08,0x5c08, 1,2, 16, OPCALL(Z5C_0000_1000_addr), 			 "testl   %a1"},
{0x5c09,0x5c09, 1,3, 14, OPCALL(Z5C_0000_1001_0000_ssss_0000_nmin1_addr), "ldm     %a2,%rw5,#%n"},
{0x5c11,0x5cf1,16,3, 15, OPCALL(Z5C_ssN0_0001_0000_dddd_0000_nmin1_addr), "ldm     %rw5,%a2(%rw2),#%n"},
{0x5c18,0x5cf8,16,2, 17, OPCALL(Z5C_ddN0_1000_addr), 			 "testl   %a1(%rw2)"},
{0x5c19,0x5cf9,16,3, 15, OPCALL(Z5C_ddN0_1001_0000_ssN0_0000_nmin1_addr), "ldm     %a2(%rw2),%rw5,#%n"},
{0x5d00,0x5d0f, 1,2, 15, OPCALL(Z5D_0000_ssss_addr), 			 "ldl     %a1,%rl3"},
{0x5d10,0x5dff, 1,2, 14, OPCALL(Z5D_ddN0_ssss_addr), 			 "ldl     %a1(%rw2),%rl3"},
{0x5e00,0x5e0f, 1,2,  7, OPCALL(Z5E_0000_cccc_addr), 			 "jp      %c3,%a1"},
{0x5e10,0x5eff, 1,2,  8, OPCALL(Z5E_ddN0_cccc_addr), 			 "jp      %c3,%a1(%rw2)"},
{0x5f00,0x5f00, 1,2, 12, OPCALL(Z5F_0000_0000_addr), 			 "call    %a1", DASMFLAG_STEP_OVER},
{0x5f10,0x5ff0,16,2, 13, OPCALL(Z5F_ddN0_0000_addr), 			 "call    %a1(%rw2)", DASMFLAG_STEP_OVER},
{0x6000,0x600f, 1,2,  9, OPCALL(Z60_0000_dddd_addr), 			 "ldb     %rb3,%a1"},
{0x6010,0x60ff, 1,2, 10, OPCALL(Z60_ssN0_dddd_addr), 			 "ldb     %rb3,%a1(%rw2)"},
{0x6100,0x610f, 1,2,  9, OPCALL(Z61_0000_dddd_addr), 			 "ld      %rw3,%a1"},
{0x6110,0x61ff, 1,2, 10, OPCALL(Z61_ssN0_dddd_addr), 			 "ld      %rw3,%a1(%rw2)"},
{0x6200,0x620f, 1,2, 13, OPCALL(Z62_0000_imm4_addr), 			 "resb    %a1,#%3"},
{0x6210,0x62ff, 1,2, 14, OPCALL(Z62_ddN0_imm4_addr), 			 "resb    %a1(%rw2),#%3"},
{0x6300,0x630f, 1,2, 13, OPCALL(Z63_0000_imm4_addr), 			 "res     %a1,#%3"},
{0x6310,0x63ff, 1,2, 14, OPCALL(Z63_ddN0_imm4_addr), 			 "res     %a1(%rw2),#%3"},
{0x6400,0x640f, 1,2, 13, OPCALL(Z64_0000_imm4_addr), 			 "setb    %a1,#%3"},
{0x6410,0x64ff, 1,2, 14, OPCALL(Z64_ddN0_imm4_addr), 			 "setb    %a1(%rw2),#%3"},
{0x6500,0x650f, 1,2, 13, OPCALL(Z65_0000_imm4_addr), 			 "set     %a1,#%3"},
{0x6510,0x65ff, 1,2, 14, OPCALL(Z65_ddN0_imm4_addr), 			 "set     %a1(%rw2),#%3"},
{0x6600,0x660f, 1,2, 10, OPCALL(Z66_0000_imm4_addr), 			 "bitb    %a1,#%3"},
{0x6610,0x66ff, 1,2, 11, OPCALL(Z66_ddN0_imm4_addr), 			 "bitb    %a1(%rw2),#%3"},
{0x6700,0x670f, 1,2, 10, OPCALL(Z67_0000_imm4_addr), 			 "bit     %a1,#%3"},
{0x6710,0x67ff, 1,2, 11, OPCALL(Z67_ddN0_imm4_addr), 			 "bit     %a1(%rw2),#%3"},
{0x6800,0x680f, 1,2, 13, OPCALL(Z68_0000_imm4m1_addr),			 "incb    %a1,#%+3"},
{0x6810,0x68ff, 1,2, 14, OPCALL(Z68_ddN0_imm4m1_addr),			 "incb    %a1(%rw2),#%+3"},
{0x6900,0x690f, 1,2, 13, OPCALL(Z69_0000_imm4m1_addr),			 "inc     %a1,#%+3"},
{0x6910,0x69ff, 1,2, 14, OPCALL(Z69_ddN0_imm4m1_addr),			 "inc     %a1(%rw2),#%+3"},
{0x6a00,0x6a0f, 1,2, 13, OPCALL(Z6A_0000_imm4m1_addr),			 "decb    %a1,#%+3"},
{0x6a10,0x6aff, 1,2, 14, OPCALL(Z6A_ddN0_imm4m1_addr),			 "decb    %a1(%rw2),#%+3"},
{0x6b00,0x6b0f, 1,2, 13, OPCALL(Z6B_0000_imm4m1_addr),			 "dec     %a1,#%+3"},
{0x6b10,0x6bff, 1,2, 14, OPCALL(Z6B_ddN0_imm4m1_addr),			 "dec     %a1(%rw2),#%+3"},
{0x6c00,0x6c0f, 1,2, 15, OPCALL(Z6C_0000_dddd_addr), 			 "exb     %rb3,%a1"},
{0x6c10,0x6cff, 1,2, 16, OPCALL(Z6C_ssN0_dddd_addr), 			 "exb     %rb3,%a1(%rw2)"},
{0x6d00,0x6d0f, 1,2, 15, OPCALL(Z6D_0000_dddd_addr), 			 "ex      %rw3,%a1"},
{0x6d10,0x6dff, 1,2, 16, OPCALL(Z6D_ssN0_dddd_addr), 			 "ex      %rw3,%a1(%rw2)"},
{0x6e00,0x6e0f, 1,2, 12, OPCALL(Z6E_0000_ssss_addr), 			 "ldb     %a1,%rb3"},
{0x6e10,0x6eff, 1,2, 12, OPCALL(Z6E_ddN0_ssss_addr), 			 "ldb     %a1(%rw2),%rb3"},
{0x6f00,0x6f0f, 1,2, 12, OPCALL(Z6F_0000_ssss_addr), 			 "ld      %a1,%rw3"},
{0x6f10,0x6fff, 1,2, 12, OPCALL(Z6F_ddN0_ssss_addr), 			 "ld      %a1(%rw2),%rw3"},
{0x7010,0x70ff, 1,2, 14, OPCALL(Z70_ssN0_dddd_0000_xxxx_0000_0000), 	 "ldb     %rb3,%rw2(%rw5)"},
{0x7110,0x71ff, 1,2, 14, OPCALL(Z71_ssN0_dddd_0000_xxxx_0000_0000),	 "ld      %rw3,%rw2(%rw5)"},
{0x7210,0x72ff, 1,2, 14, OPCALL(Z72_ddN0_ssss_0000_xxxx_0000_0000),	 "ldb     %rw2(%rw5),%rb3"},
{0x7310,0x73ff, 1,2, 14, OPCALL(Z73_ddN0_ssss_0000_xxxx_0000_0000),	 "ld      %rw2(%rw5),%rw3"},
{0x7410,0x74ff, 1,2, 15, OPCALL(Z74_ssN0_dddd_0000_xxxx_0000_0000),	 "lda     %rw3,%rw2(%rw5)"},
{0x7510,0x75ff, 1,2, 17, OPCALL(Z75_ssN0_dddd_0000_xxxx_0000_0000),	 "ldl     %rl3,%rw2(%rw5)"},
{0x7600,0x760f, 1,2, 12, OPCALL(Z76_0000_dddd_addr), 			 "lda     %rw3,%a1"},
{0x7610,0x76ff, 1,2, 13, OPCALL(Z76_ssN0_dddd_addr), 			 "lda     %rw3,%a1(%rw2)"},
{0x7710,0x77ff, 1,2, 17, OPCALL(Z77_ddN0_ssss_0000_xxxx_0000_0000),	 "ldl     %rw2(%rw5),%rl3"},
{0x7900,0x7900, 1,2, 16, OPCALL(Z79_0000_0000_addr), 			 "ldps    %a1"},
{0x7910,0x79f0,16,2, 17, OPCALL(Z79_ssN0_0000_addr), 			 "ldps    %a1(%rw2)"},
{0x7a00,0x7a00, 1,1,  8, OPCALL(Z7A_0000_0000),				 "halt", DASMFLAG_STEP_OVER},
{0x7b00,0x7b00, 1,1, 13, OPCALL(Z7B_0000_0000),				 "iret", DASMFLAG_STEP_OUT},
{0x7b08,0x7b08, 1,1,  5, OPCALL(Z7B_0000_1000),				 "mset"},
{0x7b09,0x7b09, 1,1,  5, OPCALL(Z7B_0000_1001),				 "mres"},
{0x7b0a,0x7b0a, 1,1,  7, OPCALL(Z7B_0000_1010),				 "mbit"},
{0x7b0d,0x7bfd,16,1, 12, OPCALL(Z7B_dddd_1101),				 "mreq    %rw2"},
{0x7c00,0x7c03, 1,1,  7, OPCALL(Z7C_0000_00ii),				 "di      %i3"},
{0x7c04,0x7c07, 1,1,  7, OPCALL(Z7C_0000_01ii),				 "ei      %i3"},
{0x7d02,0x7df2,16,1,  7, OPCALL(Z7D_dddd_0ccc),				 "ldctl   %rw2,fcw"},
{0x7d03,0x7df3,16,1,  7, OPCALL(Z7D_dddd_0ccc),				 "ldctl   %rw2,refresh"},
{0x7d05,0x7df5,16,1,  7, OPCALL(Z7D_dddd_0ccc),				 "ldctl   %rw2,psap"},
{0x7d07,0x7df7,16,1,  7, OPCALL(Z7D_dddd_0ccc),				 "ldctl   %rw2,nsp"},
{0x7d0a,0x7dfa,16,1,  7, OPCALL(Z7D_ssss_1ccc),				 "ldctl   fcw,%rw2"},
{0x7d0b,0x7dfb,16,1,  7, OPCALL(Z7D_ssss_1ccc),				 "ldctl   refresh,%rw2"},
{0x7d0d,0x7dfd,16,1,  7, OPCALL(Z7D_ssss_1ccc),				 "ldctl   psap,%rw2"},
{0x7d0f,0x7dff,16,1,  7, OPCALL(Z7D_ssss_1ccc),				 "ldctl   nsp,%rw2"},
{0x7f00,0x7fff, 1,1, 33, OPCALL(Z7F_imm8),				 "sc      %#b1"},
{0x8000,0x80ff, 1,1,  4, OPCALL(Z80_ssss_dddd),				 "addb    %rb3,%rb2"},
{0x8100,0x81ff, 1,1,  4, OPCALL(Z81_ssss_dddd),				 "add     %rw3,%rw2"},
{0x8200,0x82ff, 1,1,  4, OPCALL(Z82_ssss_dddd),				 "subb    %rb3,%rb2"},
{0x8300,0x83ff, 1,1,  4, OPCALL(Z83_ssss_dddd),				 "sub     %rw3,%rw2"},
{0x8400,0x84ff, 1,1,  4, OPCALL(Z84_ssss_dddd),				 "orb     %rb3,%rb2"},
{0x8500,0x85ff, 1,1,  4, OPCALL(Z85_ssss_dddd),				 "or      %rw3,%rw2"},
{0x8600,0x86ff, 1,1,  4, OPCALL(Z86_ssss_dddd),				 "andb    %rb3,%rb2"},
{0x8700,0x87ff, 1,1,  4, OPCALL(Z87_ssss_dddd),				 "and     %rw3,%rw2"},
{0x8800,0x88ff, 1,1,  4, OPCALL(Z88_ssss_dddd),				 "xorb    %rb3,%rb2"},
{0x8900,0x89ff, 1,1,  4, OPCALL(Z89_ssss_dddd),				 "xor     %rw3,%rw2"},
{0x8a00,0x8aff, 1,1,  4, OPCALL(Z8A_ssss_dddd),				 "cpb     %rb3,%rb2"},
{0x8b00,0x8bff, 1,1,  4, OPCALL(Z8B_ssss_dddd),				 "cp      %rw3,%rw2"},
{0x8c00,0x8cf0,16,1,  7, OPCALL(Z8C_dddd_0000),				 "comb    %rb2"},
{0x8c01,0x8cf1,16,1,  7, OPCALL(Z7D_dddd_0ccc),                           "ldctlb  %rb2,flags"},              // check function+encoding!! ADD missing functions BUGGY!
{0x8c02,0x8cf2,16,1,  7, OPCALL(Z8C_dddd_0010),				 "negb    %rb2"},
{0x8c04,0x8cf4,16,1,  7, OPCALL(Z8C_dddd_0100),				 "testb   %rb2"},
{0x8c06,0x8cf6,16,1,  7, OPCALL(Z8C_dddd_0110),				 "tsetb   %rb2"},
{0x8c08,0x8cf8,16,1,  7, OPCALL(Z8C_dddd_1000),				 "clrb    %rb2"},
{0x8c09,0x8cf9,16,1,  7, OPCALL(Z7D_ssss_1ccc),                           "ldctlb  flags,%rb2"},              // check function+encoding!! ADD missing functions BUGGY!
{0x8d00,0x8df0,16,1,  7, OPCALL(Z8D_dddd_0000),				 "com     %rw2"},
{0x8d01,0x8df1,16,1,  7, OPCALL(Z8D_imm4_0001),				 "setflg  %f2"},
{0x8d02,0x8df2,16,1,  7, OPCALL(Z8D_dddd_0010),				 "neg     %rw2"},
{0x8d03,0x8df3,16,1,  7, OPCALL(Z8D_imm4_0011),				 "resflg  %f2"},
{0x8d04,0x8df4,16,1,  7, OPCALL(Z8D_dddd_0100),				 "test    %rw2"},
{0x8d05,0x8df5,16,1,  7, OPCALL(Z8D_imm4_0101),				 "comflg  %f2"},
{0x8d06,0x8df6,16,1,  7, OPCALL(Z8D_dddd_0110),				 "tset    %rw2"},
{0x8d07,0x8d07, 1,1,  7, OPCALL(Z8D_0000_0111),				 "nop"},
{0x8d08,0x8df8,16,1,  7, OPCALL(Z8D_dddd_1000),				 "clr     %rw2"},
{0x8e00,0x8eff, 1,2, 11, OPCALL(Z8E_imm16),				 "ext8e   %#l0"},
{0x8f00,0x8fff, 1,2, 11, OPCALL(Z8F_imm16),				 "ext8f   %#l0"},
{0x9000,0x90ff, 1,1,  8, OPCALL(Z90_ssss_dddd),				 "cpl     %rl3,%rl2"},
{0x9110,0x91ff, 1,1, 12, OPCALL(Z91_ddN0_ssss),				 "pushl   @%rw2,%rl3"},
{0x9200,0x92ff, 1,1,  8, OPCALL(Z92_ssss_dddd),				 "subl    %rl3,%rl2"},
{0x9310,0x93ff, 1,1,  9, OPCALL(Z93_ddN0_ssss),				 "push    @%rw2,%rw3"},
{0x9400,0x94ff, 1,1,  5, OPCALL(Z94_ssss_dddd),				 "ldl     %rl3,%rl2"},
{0x9510,0x95ff, 1,1, 12, OPCALL(Z95_ssN0_dddd),				 "popl    %rl3,@%rw2"},
{0x9600,0x96ff, 1,1,  8, OPCALL(Z96_ssss_dddd),				 "addl    %rl3,%rl2"},
{0x9710,0x97ff, 1,1,  8, OPCALL(Z97_ssN0_dddd),				 "pop     %rw3,@%rw2"},
{0x9800,0x98ff, 1,1,282, OPCALL(Z98_ssss_dddd),				 "multl   %rq3,%rl2"},
{0x9900,0x99ff, 1,1, 70, OPCALL(Z99_ssss_dddd),				 "mult    %rl3,%rw2"},
{0x9a00,0x9aff, 1,1,744, OPCALL(Z9A_ssss_dddd),				 "divl    %rq3,%rl2"},
{0x9b00,0x9bff, 1,1,107, OPCALL(Z9B_ssss_dddd),				 "div     %rl3,%rw2"},
{0x9c08,0x9cf8,16,1, 13, OPCALL(Z9C_dddd_1000),				 "testl   %rl2"},
{0x9e00,0x9e0f, 1,1, 10, OPCALL(Z9E_0000_cccc),				 "ret     %c3", DASMFLAG_STEP_OUT},
{0xa000,0xa0ff, 1,1,  3, OPCALL(ZA0_ssss_dddd),				 "ldb     %rb3,%rb2"},
{0xa100,0xa1ff, 1,1,  3, OPCALL(ZA1_ssss_dddd),				 "ld      %rw3,%rw2"},
{0xa200,0xa2ff, 1,1,  4, OPCALL(ZA2_dddd_imm4),				 "resb    %rb2,#%3"},
{0xa300,0xa3ff, 1,1,  4, OPCALL(ZA3_dddd_imm4),				 "res     %rw2,#%3"},
{0xa400,0xa4ff, 1,1,  4, OPCALL(ZA4_dddd_imm4),				 "setb    %rb2,#%3"},
{0xa500,0xa5ff, 1,1,  4, OPCALL(ZA5_dddd_imm4),				 "set     %rw2,#%3"},
{0xa600,0xa6ff, 1,1,  4, OPCALL(ZA6_dddd_imm4),				 "bitb    %rb2,#%3"},
{0xa700,0xa7ff, 1,1,  4, OPCALL(ZA7_dddd_imm4),				 "bit     %rw2,#%3"},
{0xa800,0xa8ff, 1,1,  4, OPCALL(ZA8_dddd_imm4m1),			 "incb    %rb2,#%+3"},
{0xa900,0xa9ff, 1,1,  4, OPCALL(ZA9_dddd_imm4m1),			 "inc     %rw2,#%+3"},
{0xaa00,0xaaff, 1,1,  4, OPCALL(ZAA_dddd_imm4m1),			 "decb    %rb2,#%+3"},
{0xab00,0xabff, 1,1,  4, OPCALL(ZAB_dddd_imm4m1),			 "dec     %rw2,#%+3"},
{0xac00,0xacff, 1,1,  6, OPCALL(ZAC_ssss_dddd),				 "exb     %rb3,%rb2"},
{0xad00,0xadff, 1,1,  6, OPCALL(ZAD_ssss_dddd),				 "ex      %rw3,%rw2"},
{0xae00,0xaeff, 1,1,  5, OPCALL(ZAE_dddd_cccc),				 "tccb    %c3,%rb2"},
{0xaf00,0xafff, 1,1,  5, OPCALL(ZAF_dddd_cccc),				 "tcc     %c3,%rw2"},
{0xb000,0xb0f0,16,1,  5, OPCALL(ZB0_dddd_0000),				 "dab     %rb2"},
{0xb100,0xb1f0,16,1, 11, OPCALL(ZB1_dddd_0000),				 "extsb   %rw2"},
{0xb107,0xb1f7,16,1, 11, OPCALL(ZB1_dddd_0111),				 "extsl   %rq2"},
{0xb10a,0xb1fa,16,1, 11, OPCALL(ZB1_dddd_1010),				 "exts    %rl2"},
{0xb200,0xb2f0,16,1,  6, OPCALL(ZB2_dddd_00I0),				 "rlb     %rb2,#%?3"},
{0xb201,0xb2f1,16,2, 13, OPCALL(ZB2_dddd_0001_imm8), 			 "s%*lb    %rb2,%$3"},
{0xb202,0xb2f2,16,1,  6, OPCALL(ZB2_dddd_00I0),				 "rlb     %rb2,#%?3"},
{0xb203,0xb2f3,16,2, 15, OPCALL(ZB2_dddd_0011_0000_ssss_0000_0000),	 "sdlb    %rb2,%rw5"},
{0xb204,0xb2f4,16,1,  6, OPCALL(ZB2_dddd_01I0),				 "rrb     %rb2,#%?3"},
{0xb206,0xb2f6,16,1,  6, OPCALL(ZB2_dddd_01I0),				 "rrb     %rb2,#%?3"},
{0xb208,0xb2f8,16,1,  6, OPCALL(ZB2_dddd_10I0),				 "rlcb    %rb2,#%?3"},
{0xb209,0xb2f9,16,2, 13, OPCALL(ZB2_dddd_1001_imm8), 			 "s%*ab    %rb2,%$3"},
{0xb20a,0xb2fa,16,1,  6, OPCALL(ZB2_dddd_10I0),				 "rlcb    %rb2,#%?3"},
{0xb20b,0xb2fb,16,2, 15, OPCALL(ZB2_dddd_1011_0000_ssss_0000_0000),	 "sdab    %rb2,%rw5"},
{0xb20c,0xb2fc,16,1,  6, OPCALL(ZB2_dddd_11I0),				 "rrcb    %rb2,#%?3"},
{0xb20e,0xb2fe,16,1,  6, OPCALL(ZB2_dddd_11I0),				 "rrcb    %rb2,#%?3"},
{0xb300,0xb3f0,16,1,  6, OPCALL(ZB3_dddd_00I0),				 "rl      %rw2,#%?3"},
{0xb301,0xb3f1,16,2, 13, OPCALL(ZB3_dddd_0001_imm8), 			 "s%*l     %rw2,%$3"},
{0xb302,0xb3f2,16,1,  6, OPCALL(ZB3_dddd_00I0),				 "rl      %rw2,#%?3"},
{0xb303,0xb3f3,16,2, 15, OPCALL(ZB3_dddd_0011_0000_ssss_0000_0000),	 "sdl     %rw2,%rw5"},
{0xb304,0xb3f4,16,1,  6, OPCALL(ZB3_dddd_01I0),				 "rr      %rw2,#%?3"},
{0xb305,0xb3f5,16,2, 13, OPCALL(ZB3_dddd_0101_imm8), 			 "s%*ll    %rl2,%$3"},
{0xb306,0xb3f6,16,1,  6, OPCALL(ZB3_dddd_01I0),				 "rr      %rw2,#%?3"},
{0xb307,0xb3f7,16,2, 15, OPCALL(ZB3_dddd_0111_0000_ssss_0000_0000),	 "sdll    %rl2,%rw5"},
{0xb308,0xb3f8,16,1,  6, OPCALL(ZB3_dddd_10I0),				 "rlc     %rw2,#%?3"},
{0xb309,0xb3f9,16,2, 13, OPCALL(ZB3_dddd_1001_imm8), 			 "s%*a     %rw2,%$3"},
{0xb30a,0xb3fa,16,1,  6, OPCALL(ZB3_dddd_10I0),				 "rlc     %rw2,#%?3"},
{0xb30b,0xb3fb,16,2, 15, OPCALL(ZB3_dddd_1011_0000_ssss_0000_0000),	 "sda     %rw2,%rw5"},
{0xb30c,0xb3fc,16,1,  6, OPCALL(ZB3_dddd_11I0),				 "rrc     %rw2,#%?3"},
{0xb30d,0xb3fd,16,2, 13, OPCALL(ZB3_dddd_1101_imm8), 			 "s%*al    %rl2,%$3"},
{0xb30e,0xb3fe,16,1,  6, OPCALL(ZB3_dddd_11I0),				 "rrc     %rw2,#%?3"},
{0xb30f,0xb3ff,16,2, 15, OPCALL(ZB3_dddd_1111_0000_ssss_0000_0000),	 "sdal    %rl2,%rw5"},
{0xb400,0xb4ff, 1,1,  5, OPCALL(ZB4_ssss_dddd),				 "adcb    %rb3,%rb2"},
{0xb500,0xb5ff, 1,1,  5, OPCALL(ZB5_ssss_dddd),				 "adc     %rw3,%rw2"},
{0xb600,0xb6ff, 1,1,  5, OPCALL(ZB6_ssss_dddd),				 "sbcb    %rb3,%rb2"},
{0xb700,0xb7ff, 1,1,  5, OPCALL(ZB7_ssss_dddd),				 "sbc     %rw3,%rw2"},
{0xb810,0xb8f0,16,2, 25, OPCALL(ZB8_ddN0_0000_0000_rrrr_ssN0_0000),	 "trib    @%rw2,@%rw6,%rw5"},
{0xb812,0xb8f2,16,2, 25, OPCALL(ZB8_ddN0_0010_0000_rrrr_ssN0_0000),	 "trtib   @%rw2,@%rw6,%rw5"},
{0xb814,0xb8f4,16,2, 25, OPCALL(ZB8_ddN0_0100_0000_rrrr_ssN0_0000),	 "trirb   @%rw2,@%rw6,%rw5"},
{0xb816,0xb8f6,16,2, 25, OPCALL(ZB8_ddN0_0110_0000_rrrr_ssN0_1110),	 "trtirb  @%rw2,@%rw6,%rw5"},
{0xb818,0xb8f8,16,2, 25, OPCALL(ZB8_ddN0_1000_0000_rrrr_ssN0_0000),	 "trdb    @%rw2,@%rw6,%rw5"},
{0xb81a,0xb8fa,16,2, 25, OPCALL(ZB8_ddN0_1010_0000_rrrr_ssN0_0000),	 "trtdb   @%rw2,@%rw6,%rw5"},
{0xb81c,0xb8fc,16,2, 25, OPCALL(ZB8_ddN0_1100_0000_rrrr_ssN0_0000),	 "trdrb   @%rw2,@%rw6,%rw5"},
{0xb81e,0xb8fe,16,2, 25, OPCALL(ZB8_ddN0_1110_0000_rrrr_ssN0_1110),	 "trtdrb  @%rw2,@%rw6,%rw5"},
{0xba10,0xbaf0,16,2, 20, OPCALL(ZBA_ssN0_0000_0000_rrrr_dddd_cccc),	 "cpib    %rb6,@%rw2,%rw5,%c7"},
{0xba11,0xbaf1,16,2,  9, OPCALL(ZBA_ssN0_0001_0000_rrrr_ddN0_x000),	 "ldi%^b  @%rw6,@%rw2,%rw5", DASMFLAG_STEP_OVER},
{0xba12,0xbaf2,16,2, 25, OPCALL(ZBA_ssN0_0010_0000_rrrr_ddN0_cccc),	 "cpsib   @%rw6,@%rw2,%rw5,%c7"},
{0xba14,0xbaf4,16,2,  9, OPCALL(ZBA_ssN0_0100_0000_rrrr_dddd_cccc),	 "cpirb   %rb6,@%rw2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xba16,0xbaf6,16,2, 14, OPCALL(ZBA_ssN0_0110_0000_rrrr_ddN0_cccc),	 "cpsirb  @%rw6,@%rw2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xba18,0xbaf8,16,2, 20, OPCALL(ZBA_ssN0_1000_0000_rrrr_dddd_cccc),	 "cpdb    %rb6,@%rw2,%rw5,%c7"},
{0xba19,0xbaf9,16,2,  9, OPCALL(ZBA_ssN0_1001_0000_rrrr_ddN0_x000),	 "ldd%^b  @%rw2,@%rw6,%rw5", DASMFLAG_STEP_OVER},
{0xba1a,0xbafa,16,2, 25, OPCALL(ZBA_ssN0_1010_0000_rrrr_ddN0_cccc),	 "cpsdb   @%rw6,@%rw2,%rw5,%c7"},
{0xba1c,0xbafc,16,2,  9, OPCALL(ZBA_ssN0_1100_0000_rrrr_dddd_cccc),	 "cpdrb   %rb6,@%rw2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xba1e,0xbafe,16,2, 14, OPCALL(ZBA_ssN0_1110_0000_rrrr_ddN0_cccc),	 "cpsdrb  @%rw6,@%rw2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xbb10,0xbbf0,16,2, 20, OPCALL(ZBB_ssN0_0000_0000_rrrr_dddd_cccc),	 "cpi     %rw6,@%rw2,%rw5,%c7"},
{0xbb11,0xbbf1,16,2,  9, OPCALL(ZBB_ssN0_0001_0000_rrrr_ddN0_x000),	 "ldi%^   @%rw6,@%rw2,%rw5", DASMFLAG_STEP_OVER},
{0xbb12,0xbbf2,16,2, 25, OPCALL(ZBB_ssN0_0010_0000_rrrr_ddN0_cccc),	 "cpsi    @%rw6,@%rw2,%rw5,%c7"},
{0xbb14,0xbbf4,16,2,  9, OPCALL(ZBB_ssN0_0100_0000_rrrr_dddd_cccc),	 "cpir    %rw6,@%rw2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xbb16,0xbbf6,16,2, 14, OPCALL(ZBB_ssN0_0110_0000_rrrr_ddN0_cccc),	 "cpsir   @%rw6,@%rw2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xbb18,0xbbf8,16,2, 20, OPCALL(ZBB_ssN0_1000_0000_rrrr_dddd_cccc),	 "cpd     %rw6,@%rw2,%rw5,%c7"},
{0xbb19,0xbbf9,16,2,  9, OPCALL(ZBB_ssN0_1001_0000_rrrr_ddN0_x000),	 "ldd%^   @%rw2,@%rw6,%rw5", DASMFLAG_STEP_OVER},
{0xbb1a,0xbbfa,16,2, 25, OPCALL(ZBB_ssN0_1010_0000_rrrr_ddN0_cccc),	 "cpsd    @%rw6,@%rw2,%rw5,%c7"},
{0xbb1c,0xbbfc,16,2,  9, OPCALL(ZBB_ssN0_1100_0000_rrrr_dddd_cccc),	 "cpdr    %rw6,@%rw2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xbb1e,0xbbfe,16,2, 14, OPCALL(ZBB_ssN0_1110_0000_rrrr_ddN0_cccc),	 "cpsdr   @%rw6,@%rw2,%rw5,%c7", DASMFLAG_STEP_OVER},
{0xbc00,0xbcff, 1,1,  9, OPCALL(ZBC_aaaa_bbbb),				 "rrdb    %rb3,%rb2"},
{0xbd00,0xbdff, 1,1,  5, OPCALL(ZBD_dddd_imm4),				 "ldk     %rw2,#%3"},
{0xbe00,0xbeff, 1,1,  9, OPCALL(ZBE_aaaa_bbbb),				 "rldb    %rb3,%rb2"},
{0xc000,0xcfff, 1,1,  5, OPCALL(ZC_dddd_imm8),				 "ldb     %rb1,%#b1"},
{0xd000,0xdfff, 1,1, 10, OPCALL(ZD_dsp12),				 "calr    %d2", DASMFLAG_STEP_OVER},
{0xe000,0xefff, 1,1,  6, OPCALL(ZE_cccc_dsp8),				 "jr      %c1,%d1"},
{0xf000,0xf07f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf100,0xf17f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf200,0xf27f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf300,0xf37f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf400,0xf47f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf500,0xf57f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf600,0xf67f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf700,0xf77f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf800,0xf87f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf900,0xf97f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xfa00,0xfa7f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xfb00,0xfb7f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xfc00,0xfc7f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xfd00,0xfd7f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xfe00,0xfe7f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xff00,0xff7f, 1,1, 11, OPCALL(ZF_dddd_0dsp7),				 "dbjnz   %rb1,%d0", DASMFLAG_STEP_OVER},
{0xf080,0xf0ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf180,0xf1ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf280,0xf2ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf380,0xf3ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf480,0xf4ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf580,0xf5ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf680,0xf6ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf780,0xf7ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf880,0xf8ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xf980,0xf9ff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xfa80,0xfaff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xfb80,0xfbff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xfc80,0xfcff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xfd80,0xfdff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xfe80,0xfeff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0xff80,0xffff, 1,1, 11, OPCALL(ZF_dddd_1dsp7),				 "djnz    %rw1,%d0", DASMFLAG_STEP_OVER},
{0, 	0,	   0,	0,	0,0,									 0},
};

void z8000_init(int index, int clock, const void *config, int (*irqcallback)(int))
{
    int i;
	const Z8000_init *init;

	/* already initialized? */
	if( z8000_exec && z8000_exec_seg)
		return;

	/* allocate the opcode execution and disassembler array */
	z8000_exec     = (Z8000_exec *)malloc(0x10000 * sizeof(Z8000_exec));
	z8000_exec_seg = (Z8000_exec *)malloc(0x10000 * sizeof(Z8000_exec));
	if( !z8000_exec  || !z8000_exec_seg)
	{
		fprintf(stderr,"cannot allocate Z8000 execution table\n");
                exit(1);
	}

	/* set up the zero, sign, parity lookup table */
    for (i = 0; i < 256; i++)
		z8000_zsp[i] = ((i == 0) ? F_Z : 0) |
                       ((i & 128) ? F_S : 0) |
                       ((((i>>7)^(i>>6)^(i>>5)^(i>>4)^(i>>3)^(i>>2)^(i>>1)^i) & 1) ? F_PV : 0);

    /* first set all 64K opcodes to invalid */
	for (i = 0; i < 0x10000; i++)
	{
		z8000_exec[i].opcode = zinvalid;
		z8000_exec[i].cycles = 4;
		z8000_exec[i].size	 = 1;
		z8000_exec[i].dasm	 = ".word   %#w0";
		z8000_exec[i].dasmflags = 0;

                z8000_exec_seg[i].opcode = zinvalid;
                z8000_exec_seg[i].cycles = 4;
                z8000_exec_seg[i].size       = 1;
                z8000_exec_seg[i].dasm       = ".word   %#w0";
                z8000_exec_seg[i].dasmflags = 0;

	}

    /* now decompose the initialization table */
	for (init = table; init->size; init++)
	{
		for (i = init->beg; i <= init->end; i += init->step)
		{
			if (z8000_exec[i].opcode != zinvalid) {
				fprintf(stderr,"Z8000 opcode %04x clash '%s'\n", i, z8000_exec[i].dasm);
                                exit(2);
                        }

			z8000_exec[i].opcode = init->opcode;
			z8000_exec[i].cycles = init->cycles;
			z8000_exec[i].size	 = init->size;
			z8000_exec[i].dasm	 = init->dasm;
			z8000_exec[i].dasmflags = init->dasmflags;
		}
	}

    /* now decompose the segmented initialization table */
        for (init = table_segmented; init->size; init++)
        {
                for (i = init->beg; i <= init->end; i += init->step)
                {
                        if (z8000_exec_seg[i].opcode != zinvalid) {
                                fprintf(stderr,"Z8000 opcode %04x clash '%s'\n", i, z8000_exec_seg[i].dasm);
                                exit(2);
                        }

                        z8000_exec_seg[i].opcode = init->opcode;
                        z8000_exec_seg[i].cycles = init->cycles;
                        z8000_exec_seg[i].size       = init->size;
                        z8000_exec_seg[i].dasm       = init->dasm;
                        z8000_exec_seg[i].dasmflags = init->dasmflags;
                }
        }
#ifndef Z8K1_DASM
  	Z.irq_callback = irqcallback;
#endif
}

void z8000_deinit(void)
{
        if(z8000_exec) {
	   free( z8000_exec );
	   z8000_exec = 0;
        }

        if(z8000_exec_seg) {
	   free( z8000_exec_seg );
	   z8000_exec_seg = 0;
        }
}

