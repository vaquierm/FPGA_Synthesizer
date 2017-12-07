#ifndef __VGA
#define __VGA

	void VGA_clear_charbuff_ASM();
	void VGA_clear_pixelbuff_ASM();
	
	void VGA_write_char_ASM(int x, int y, char c);
	void VGA_write_byte_ASM(int x, int y,	char byte);
	
	void VGA_draw_point_ASM(int x, int y, short colour);

#endif
