#include "x16sound.h"
#include "zsm.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char* filename = "SONIC.ZSM";

float YMrate;
float PSGrate;

float YM_samples=0;
float PSG_samples=0;

// returns -1 = music not playing, 0=buffer full, 1=done and music is playing
int tick() {
	if (!zsm_tick()) return -1;
	int buffer_full=0;
	static int ym_remain = 0;
	static int psg_remain = 0;
	ym_remain += YMrate;
	psg_remain += PSGrate;
	while (ym_remain >= 1 || psg_remain >= 1) {
		ym_remain -= x16sound_render(CHIP_YM, floor(ym_remain));
		psg_remain -= x16sound_render(CHIP_PSG, floor(psg_remain));
		buffer_full |=  (ym_remain >= 1 || psg_remain >= 1);
	}
	return !buffer_full;
}

int fill_buffer() {
	while (tick()==1) {

	}
}


int main() {
	int i,j;
	zsm=NULL;
	x16sound_reset();
	//strcpy(filename)
	if (!load_zsm(filename)) {
		printf ("Unable to load %s. Exiting.\n",filename);
	};
	YMrate = YM_samplerate(YM_CLOCK)/(*(unsigned short*)&zsm[0x0c]);
	PSGrate = PSG_SAMPLERATE/(*(unsigned short*)&zsm[0x0c]);

	fill_buffer();
	if (!x16sound_init()) {
		return -3;
	}
	printf ("Playing %s.\n",filename);
	while (zsm_tick())
  x16sound_shutdown();

	return 0;
}
