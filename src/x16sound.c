#include "x16sound.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

ma_device YM,PSG;

int16_t YMbuffer[BUFFSIZE];
int16_t PSGbuffer[BUFFSIZE];

volatile unsigned int YMhead  = 0;
volatile unsigned int YMtail  = BUFFSIZE-1;
volatile unsigned int PSGhead = 0;
volatile unsigned int PSGtail = BUFFSIZE-1;

bool playing=false;

void x16sound_init() {
	YM_reset();
	psg_reset();
	YMhead  = 0;
	YMtail  = BUFFSIZE-1;
	PSGhead = 0;
	PSGtail = BUFFSIZE-1;
	playing = false;
}

ma_uint32 out(
	int16_t* stream,
	ma_uint32 count,
	int16_t* buffer,
	volatile unsigned int* head,
	volatile unsigned int* tail,
	ma_uint32 skip
) {
	count *= 2;
//	printf ("%u %u %u\n",*tail,*head,*head-*tail);
	while (count>0 && (*tail+1)%BUFFSIZE != *head && (*tail+2)%BUFFSIZE != *head) {
//		printf(".");
//		if (count==1) printf ("how TF did we get an ODD count???\n");
		*tail=(*tail+1)%BUFFSIZE;
		if (skip==0) {
			stream[0]=buffer[*tail];
			*tail=(*tail+1)%BUFFSIZE;
			stream[1]=buffer[*tail];
			count -= 2;
			stream=&stream[2];
		}
		else {
			--skip;
			*tail=(*tail+1)%BUFFSIZE;
		}
	}
	if (count>0) {
		skip+=count/2;
		printf("Buffer Underflow  ");
		if (count%2) printf("with half-sample remaining! CRAP!!!!");
		printf("\n");
	}
	while (count>0) {
		*stream = 0; // buffer underflow
		--count;
		++stream;
	}
	return skip;
}

unsigned int x16sound_render(chipid chip, unsigned int count) {
	unsigned int samples_rendered=0;
	int16_t* buf;
	int16_t tmp[2];
	volatile unsigned int* head;
	volatile unsigned int* tail;
	void (*renderer)(int16_t*,unsigned);
	switch (chip) {
		case CHIP_YM:
			buf  = YMbuffer;
			head = &YMhead;
			tail = &YMtail;
			renderer = &YM_render;
			break;
		case CHIP_PSG:
			buf  = PSGbuffer;
			head = &PSGhead;
			tail = &PSGtail;
			renderer = &psg_render;
			break;
		default:
			return count; // yeah yeah, I rendered it. ;)
	}
	while (count > 0) {
		if (*head == *tail || (*head+1)%BUFFSIZE == *tail) break; // buffer full
		renderer(tmp,1);
		--count;
		++samples_rendered;
		buf[*head]=tmp[0];
		*head = (*head+1) % BUFFSIZE;
		buf[*head]=tmp[1];
		*head = (*head+1) % BUFFSIZE;
	}
	return samples_rendered;
}


void YM_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	static ma_uint32 skip=0;
//	printf("%u --> Streaming YM (%u) --> skip=",skip,frameCount);
	skip = out((int16_t*)pOutput, frameCount, YMbuffer, &YMhead, &YMtail, skip);
//	printf("%u\n",skip);
}

void PSG_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	static ma_uint32 skip=0;
//	printf("%u --> Streaming PSG (%u) --> skip=",skip,frameCount);
	skip = out((int16_t*)pOutput, frameCount, PSGbuffer, &PSGhead, &PSGtail, skip);
//	printf("%u\n",skip);
}

char x16sound_start_audio() {
	playing=false;
	ma_device_config deviceConfig;

	deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format   = ma_format_s16;
	deviceConfig.playback.channels = 2;
	deviceConfig.sampleRate        = YM_samplerate(3579545);
	deviceConfig.dataCallback      = YM_callback;

	if (ma_device_init(NULL, &deviceConfig, &YM) != MA_SUCCESS) {
			printf("Failed to open YM2151 playback device.\n");
			return 0;
	}

	deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format   = ma_format_s16;
	deviceConfig.playback.channels = 2;
	deviceConfig.sampleRate        = 48828;
	deviceConfig.dataCallback      = PSG_callback;

	if (ma_device_init(NULL, &deviceConfig, &PSG) != MA_SUCCESS) {
			printf("Failed to open PSG playback device.\n");
			return 0;
	}

	if (ma_device_start(&YM) != MA_SUCCESS) {
		printf("Failed to start YM playback device.\n");
		ma_device_uninit(&YM);
		return 0;
	}
	if (ma_device_start(&PSG) != MA_SUCCESS) {
		printf("Failed to start PSG playback device.\n");
		ma_device_uninit(&YM);
		ma_device_uninit(&PSG);
		return 0;
	}
	playing=true;
	return 1;
}

void x16sound_empty_buffer() {
	if (!playing) return;
	while ( (YMtail+1)%BUFFSIZE != YMhead || (PSGtail+1)%BUFFSIZE != PSGhead ) {}
}

void x16sound_shutdown() {
	x16sound_empty_buffer();
	ma_device_uninit(&YM);
	ma_device_uninit(&PSG);
	playing=false;
}
