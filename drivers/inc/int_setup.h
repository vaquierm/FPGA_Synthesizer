#ifndef _INT_SETUP
#define _INT_SETUP

	#include "./address_map_arm.h"
	#include "./ISRs.h"

	extern volatile int false_count;

	void disable_A9_interrupts();
	void enable_A9_interrupts();
	void int_setup(int len, int* IDs);
	
#endif
