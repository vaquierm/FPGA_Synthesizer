#ifndef __AUDIO
#define __AUDIO

	extern int audio_read_data_ASM(int *leftdata, int *rightdata);
	extern int audio_read_leftdata_ASM(int *data);
	extern int audio_read_rightdata_ASM(int *data);
	
	extern int audio_write_data_ASM(int leftdata, int rightdata);
	extern int audio_write_leftdata_ASM(int data);
	extern int audio_write_rightdata_ASM(int data);
	
	extern void audio_enable_read_fifo_clear_ASM(void);
	extern void audio_enable_write_fifo_clear_ASM(void);
	extern void audio_disable_read_fifo_clear_ASM(void);
	extern void audio_disable_write_fifo_clear_ASM(void);

	extern void audio_enable_read_int_ASM(void);
	extern void audio_enable_write_int_ASM(void);
	extern void audio_disable_read_int_ASM(void);
	extern void audio_disable_write_int_ASM(void);
	
	extern int audio_read_wslc_ASM(void);
	extern int audio_read_wsrc_ASM(void);
	extern int audio_read_ralc_ASM(void);
	extern int audio_read_rarc_ASM(void);

#endif
