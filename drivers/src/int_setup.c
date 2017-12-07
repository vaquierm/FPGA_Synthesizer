#include "../inc/int_setup.h"

void disable_A9_interrupts() {
	int status = 0b11010011;
	asm("msr cpsr, %[ps]" : : [ps]"r"(status));
}

void enable_A9_interrupts() {
	int status = 0b01010011;
	asm("msr cpsr, %[ps]" : : [ps]"r"(status));
}

void set_A9_IRQ_stack() {
	int stack, mode;
	stack = 0xFFFFFFFF - 7;
	mode = 0b11010010;
	asm("msr cpsr, %[ps]" : : [ps] "r" (mode));
	asm("mov sp, %[ps]" : : [ps] "r" (stack));
	
	mode = 0b11010011;
	asm("msr cpsr, %[ps]" : : [ps] "r" (mode));
}

void config_interrupt(int ID, int CPU) {
	int reg_offset;
	int index;
	int value;
	int address;
	
	reg_offset = (ID>>3) & 0xFFFFFFFC;
	index = ID & 0x1F;
	value = 1<<index;
	address = MPCORE_GIC_DIST + ICDISER + reg_offset;
	*(int *)address |= value;
	
	reg_offset = (ID & 0xFFFFFFFC);
	index = ID & 3;
	address = MPCORE_GIC_DIST + ICDIPTR + reg_offset + index;
	*(char *)address = (char)CPU;
}

void config_GIC(int len, int* IDs) {
	int i;
	for(i=0 ; i<len ; i++)
		config_interrupt(IDs[i],1);
	*((int *) (MPCORE_GIC_CPUIF + ICCPMR)) = 0xFFFF;
	*((int *) (MPCORE_GIC_CPUIF)) = 1;
	*((int *) (MPCORE_GIC_DIST)) = 1;
}

void __attribute__ ((interrupt)) __cs3_isr_irq() {
	int interrupt_ID = *((int *) (MPCORE_GIC_CPUIF + ICCIAR));
	
	switch(interrupt_ID) {
		
		case 29:  A9_PRIV_TIM_ISR(); break;
		case 197: HPS_GPIO1_ISR(); break;
		case 199: HPS_TIM0_ISR(); break;
		case 200: HPS_TIM1_ISR(); break;
		case 201: HPS_TIM2_ISR(); break;
		case 202: HPS_TIM3_ISR(); break;
		case 72:  FPGA_INTERVAL_TIM_ISR(); break;
		case 73:  FPGA_PB_KEYS_ISR(); break;
		case 78:  FPGA_Audio_ISR(); break;
		case 79:  FPGA_PS2_ISR(); break;
		case 80:  FPGA_JTAG_ISR(); break;
		case 81:  FPGA_IrDA_ISR(); break;
		case 83:  FPGA_JP1_ISR(); break;
		case 84:  FPGA_JP2_ISR(); break;
		case 89:  FPGA_PS2_DUAL_ISR(); break;
	
		default: while(1); break;
	}
	
	*((int *) (MPCORE_GIC_CPUIF + ICCEOIR)) = interrupt_ID;
}

void __attribute__ ((interrupt)) __cs3_reset (void) {
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_undef (void) {
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_swi (void) {
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_pabort (void) {
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_dabort (void) {
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_fiq (void) {
	while(1);
}

void fix_bug() {
	volatile int * addr = (int *)0xFFFED198;
	*addr = 0x000C0000;
}

void int_setup(int len, int* IDs) {
	disable_A9_interrupts();
	set_A9_IRQ_stack();
	fix_bug();
	config_GIC(len, IDs);
	enable_A9_interrupts();
}
