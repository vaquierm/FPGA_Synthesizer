#include <stdio.h>

#include "./drivers/inc/vga.h"
#include "./drivers/inc/ISRs.h"
#include "./drivers/inc/LEDs.h"
#include "./drivers/inc/audio.h"
#include "./drivers/inc/HPS_TIM.h"
#include "./drivers/inc/int_setup.h"
#include "./drivers/inc/wavetable.h"
#include "./drivers/inc/pushbuttons.h"
#include "./drivers/inc/ps2_keyboard.h"
#include "./drivers/inc/HEX_displays.h"
#include "./drivers/inc/slider_switches.h"

//Frequency in Hz, time in sampling periods
float signal(float f, int t){
	float FTproduct = f*t;
	int FTcasted = (int) FTproduct;
	float remainder = FTproduct - FTcasted;
	int indexfirst = FTcasted % 48000;
	int indexsecond = (indexfirst + 1) % 48000;
	float output = (((1 - remainder) * sine[indexfirst]) + (remainder * sine[indexsecond]));
	return output;
}


int main() {
	
	//enable interrupts for the timers
	int_setup(1, (int[]){199});

	//Configure timer
	HPS_TIM_config_t hps_tim;
	hps_tim.tim = TIM0;
	hps_tim.timeout = 20;
	hps_tim.LD_en = 1;
	hps_tim.INT_en = 1;
	hps_tim.enable = 1;
	
	HPS_TIM_config_ASM(&hps_tim);

	//enable keyboard
	enable_ps2_int_ASM();

	//clear screen
	VGA_clear_charbuff_ASM();
	VGA_clear_pixelbuff_ASM();

	int time = 0; //in sampling period
	int breakCode = 0; //Flag indicating if a break code was received
	char keyboardInput; //char buffer storing kezboard input
	int audioOut = 0; //amplitude to be writtn to audio port
	int keyCount = 0; //Variable to keep track of how many keys are pressed
	float volumeScaling = 0.5; //scalig used for volume
	float noteMappingFrequency[8] = {130.813 , 146.832, 164.814, 174.614, 195.998, 220.0, 246.942, 261.626}; //table keeping all frequencies
	int rowCollumnPosition[320]; //array keeping track of the y position of the vga last written
	int noteMappingOnOff[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //mapping keeping track of which keys are pressed
	int vgaOutCounter = 0; //counter to slow down the writting to the screen
	int cursorX = 0; //X cursor
	int cursorY = 0; //Y cursor
	int volumeChange[2] = {0, 0}; //Array keeping track of which volume key was pressed
	int octaveChange[2] = {0, 0}; //Array keeping track of which octave key is pressed
	int currentOctave = 3; //Curent octave
	int i; //all purpose couning index
	int x = 0; //pixel cursor X
	int y = 0; //pixel cursor Y

	for(i = 0; i < 320; i++){
		rowCollumnPosition[i] = 0;
	}
	while(1) {
		

		//Read the keyboard, if the read is successful update whitch key is pressed
		if (read_ps2_data_ASM(&keyboardInput)) {
			if(cursorX + 3 > 79) {
				cursorY = (cursorY + 1) % 60;
				cursorX = 0;
			} else {
				cursorX = (cursorX + 3) % 80;
			}
			//Update status flags depending on input
			switch (keyboardInput) {
				case 0xF0: //endkey
					breakCode = 1;
					break;
				case 0x1C: //A
					if(breakCode == 1) {
						noteMappingOnOff[0] = 0;
						breakCode = 0;
					} else {
						noteMappingOnOff[0] = 1;
					}
					break;
				case 0x1B: //S
					if(breakCode == 1) {
						noteMappingOnOff[1] = 0;
						breakCode = 0;
					} else {
						noteMappingOnOff[1] = 1;
					}
					break;
				case 0x23: //D
					if(breakCode == 1) {
						noteMappingOnOff[2] = 0;
						breakCode = 0;
					} else {
						noteMappingOnOff[2] = 1;
					}
					break;
				case 0x2B: //F
					if(breakCode == 1) {
						noteMappingOnOff[3] = 0;
						breakCode = 0;
					} else {
						noteMappingOnOff[3] = 1;
					}
					break;
				case 0x3B: //J
					if(breakCode == 1) {
						noteMappingOnOff[4] = 0;
						breakCode = 0;
					} else {
						noteMappingOnOff[4] = 1;
					}
					break;
				case 0x42: //K
					if(breakCode == 1) {
						noteMappingOnOff[5] = 0;
						breakCode = 0;
					} else {
						noteMappingOnOff[5] = 1;
					}
					break;
				case 0x4B: //L
					if(breakCode == 1) {
						noteMappingOnOff[6] = 0;
						breakCode = 0;
					} else {
						noteMappingOnOff[6] = 1;
					}
					break;
				case 0x4C: //;
					if(breakCode == 1) {
						noteMappingOnOff[7] = 0;
						breakCode = 0;
					} else {
						noteMappingOnOff[7] = 1;
					}
					break;
				case 0x75: //up arrow
					if(breakCode == 1) {
						volumeChange[0] = 0;
						breakCode = 0;
					} else {
						volumeChange[0] = 1;
					}
					break;
				case 0x72: //down arrow
					if(breakCode == 1) {
						volumeChange[1] = 0;
						breakCode = 0;
					} else {
						volumeChange[1] = -1;
					}
					break;
				case 0x74: //right arrow
					if(breakCode == 1) {
						octaveChange[0] = 0;
						breakCode = 0;
					} else {
						octaveChange[0] = 1;
					}
					break;
				case 0x6B: //left arrow
					if(breakCode == 1) {
						octaveChange[1] = 0;
						breakCode = 0;
					} else {
						octaveChange[1] = -1;
					}
					break;
				default:
					break;
			}

			if(keyboardInput != 0xF0 && keyboardInput != 0xE0) {
				//Calculate the volume change needed
				int volumeAdd = volumeChange[0] + volumeChange[1];
				if(volumeAdd > 0 && volumeScaling < 1) {
					volumeScaling += 0.1;
				}
				else if(volumeAdd < 0 && volumeScaling > 0) {
					volumeScaling -= 0.1;
				}
				//Calculate the octave change needed
				int octaveAdd = octaveChange[0] + octaveChange[1];
				if ((octaveAdd > 0 && currentOctave < 5) || (octaveAdd < 0 && currentOctave > 0)) {
					if (octaveAdd > 0) {
						currentOctave++;
					}
					else {
						currentOctave--;
					}
					for (i = 0; i < 8; i++) {
						if (octaveAdd > 0) {
							noteMappingFrequency[i] = noteMappingFrequency[i] * 2;
						}
						else {
							noteMappingFrequency[i] = noteMappingFrequency[i] / 2;
						}
					}
				}
			}
		}

		//If the timer has timed out, write the correct value to the audio port
		if(hps_tim0_int_flag != 0) {
			//Restet the timer
			hps_tim0_int_flag = 0;

			//increment the time
			time = (time + 1) % 48000;

			//Reset audio out to recalculate it.
			audioOut = 0;
			keyCount = 0;

			//Calculate the next value to be written to the FIFO
			for(i = 0; i < 8; i++) {
				if(noteMappingOnOff[i] > 0) {
					audioOut += (int) signal(noteMappingFrequency[i], time);
					keyCount++;
				}
			}
			
			//Scale the output written to the audio port depending on the volume and keycount
			audioOut = (int) (audioOut * volumeScaling / keyCount);
			
			while(!audio_write_data_ASM(audioOut, audioOut));

			//Update the counter for VGA, if it is required to write,
			//Draw a point to the screen offset from the center and save the position written
			//Draw the last point from the last itteration
			vgaOutCounter++;
			if(vgaOutCounter > 10) {
				vgaOutCounter = 0;
				x = (x + 1) % 320;
				y = (int)((float) audioOut / 9000000 * 120);
				VGA_draw_point_ASM(x, rowCollumnPosition[x], 0);
				VGA_draw_point_ASM(x, 120 + y, rand());
				rowCollumnPosition[x] = 120 + y;
			}
		}
	}

	return 0;
}
