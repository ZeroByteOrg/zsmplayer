#include "x16sound.h"
#include "zsm.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* filename = "SONIC.ZSM";

float YM_samplerate;
float PSG_samplerate;

float YM_samples=0;
float PSG_samples=0;

void fill_buffer() {
	
}

int main() {
	int i,j;
	zsm=NULL;
	x16sound_reset();
	//strcpy(filename)
	if (!load_zsm(&zsm,filename)) {
		printf ("Unable to load %s. Exiting.\n",filename);
	};
	YM_samplerate = YM_samplerate(YM_CLOCK)/(*(unsigned short*)&zsm[0x0c]);
	PSG_samplerate = PSG_SAMPLERATE/(*(unsigned short*)&zsm[0x0c]);

	fill_buffer();
	if (!x16sound_init()) {
		return -3;
	}
	printf ("Playing %s.\n",filename);
	while (zsm_tick())
  x16sound_shutdown();

	return 0;
}
