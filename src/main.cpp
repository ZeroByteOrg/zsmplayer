#include "x16sound.h"
#include "zsm.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char filename[] = "SONIC.ZSM";

float YMrate;
float PSGrate;

// returns -1 = music not playing, 0=buffer full, 1=done and music is playing
int tick() {
	int buffer_full=0;
	unsigned int r;
	static float ym_remain = 0;
	static float psg_remain = 0;
//	static float lym = 0;
//	static float lpg = 0;
	if (ym_remain < 1 && psg_remain < 1) {
//printf("ZSM tick ------------------\n");
		if (!zsm_tick()) return -1;
		ym_remain += YMrate;
		psg_remain += PSGrate;
	}
//if (lym != ym_remain || lpg != psg_remain) {
//printf ("YMremain=%1.3f PSGremain=%1.3f\n",ym_remain,psg_remain);
//lym=ym_remain ; lpg = psg_remain;
//}
//	while (ym_remain >= 1 || psg_remain >= 1) {
		r = x16sound_render(CHIP_YM, floor(ym_remain));
//		printf ("rendered YM:%4u PSG:",r);
		ym_remain -= r;
		r = x16sound_render(CHIP_PSG, floor(psg_remain));
//		printf ("%4u\n",r);
		psg_remain -= r;
		buffer_full = (ym_remain >= 1 || psg_remain >= 1) ? 1 : 0;
		//if (!buffer_full) break;
//	}
//	printf ("buffer filled %s\n",buffer_full?"(full)":"(not yet full)");

		return !buffer_full;
//	  return 1;
}

// returns 0=music not playing, 1=music still playing
int fill_buffer() {
	int result;
	do {
		result=tick();
	} while (result==1);
	return (result==0);
}

int main() {
	int i,j;
	zsm=NULL;
	x16sound_init();
	//strcpy(filename)
	if (!load_zsm(filename)) {
		printf ("Unable to load %s. Exiting.\n",filename);
	};
	YMrate = (float)YM_samplerate(YM_CLOCK)/(float)(*(unsigned short*)&zsm[0x0c]);
	PSGrate = (float)(PSG_SAMPLERATE)/(float)(*(unsigned short*)&zsm[0x0c]);
	fill_buffer();
	if (!x16sound_start_audio()) {
		return -3;
	}
	printf ("Playing %s.\n",filename);
	while (tick()>=0) {}
  x16sound_shutdown();

	return 0;
}
