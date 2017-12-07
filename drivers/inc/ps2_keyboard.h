#ifndef __PS2_KEYBOARD
#define __PS2_KEYBOARD

	extern int read_ps2_data_ASM(char* data);
	extern void enable_ps2_int_ASM(void);
	extern void disable_ps2_int_ASM(void);
	
	extern char ps2_fifo_data[256];
	extern int ps2_fifo_ravail;
#endif
