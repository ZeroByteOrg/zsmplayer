#ifndef _x16audio_h_
#define _x16audio_h_

#include <stdint.h>
#include "opm.h"
#include "vera_psg.h"

#define PSG_SAMPLERATE 48828
#define YM_CLOCK 3579545

#define BUFFSIZE 10000

extern int16_t YMbuffer[BUFFSIZE];
extern int16_t PSGbuffer[BUFFSIZE];

extern volatile int YMhead  = 0;
extern volatile int YMtail  = BUFFSIZE-1;
extern volatile int PSGhead = 0;
extern volatile int PSGtail = BUFFSIZE-1;


extern void psg_reset(void);
extern void psg_writereg(uint8_t reg, uint8_t val);
extern void psg_render(int16_t *buf, unsigned num_samples);

extern void YM_render(int16_t *stream, uint32_t samples);
extern void YM_write(uint8_t reg, uint8_t val);
extern void YM_reset();
extern uint32_t YM_samplerate(uint32_t clock);

extern void x16sound_reset();
extern char x16sound_init();
extern void x16sound_shutdown();

extern void x16sound_set_music_rate(float hz);

/*
void ym_render(int16_t *stream, uint32_t samples) {
	YM_render(stream, samples);
}

void ym_write(uint8_t reg, uint8_t val) {
	YM_write(reg, val);
}

void ym_reset() {
	YM_reset();
}

*/

#endif
