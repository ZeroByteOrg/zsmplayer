#include "x16sound.h"
#include "zsm.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* filename = "SONIC.ZSM";

float PSG_samplerate, YM_samplerate;



/*
void start_zsm() {
	zindex=16;
	float tickrate = 48828/(zsm[0x0c] + (zsm[0x0d]<<8));
	x16sound_callback(zsm_tick,tickrate);
	delay=1;
}
*/

int main() {
	int i,j;
	zsm=NULL;
	x16sound_reset();
	if (!x16sound_init()) {
		return -3;
	}
	//strcpy(filename)
	if (!load_zsm(&zsm,filename)) {
		printf ("Unable to load %s. Exiting.\n",filename);
	};
	start_zsm(zsm);

	printf ("Playing %s. [press enter to end]\n",filename);
	getchar();
  x16sound_shutdown();

	return 0;
}
