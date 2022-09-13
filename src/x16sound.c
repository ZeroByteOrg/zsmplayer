#include "x16sound.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

ma_device YM,PSG;

int16_t YMbuffer[BUFFSIZE];
int16_t PSGbuffer[BUFFSIZE];

volatile int YMhead  = 0;
volatile int YMtail  = BUFFSIZE-1;
volatile int PSGhead = 0;
volatile int PSGtail = BUFFSIZE-1;

void x16sound_reset() {
	YM_reset();
	psg_reset();
}

void out(int16_t* stream, ma_uint32 count, int16_t* buffer, int* head, int* tail) {
	if (*tail > *head) {
		while (count>0 && *tail < BUFFSIZE) {
			*stream=buffer[*tail];
			++*tail;
			--count;
			++stream;
		}
	}
	if (*tail >= BUFFSIZE) *tail=0;
	while (count>0 && *tail<*head) {
		*stream=buffer[*tail];
		++*tail;
		--count;
		++stream;
	}
	while (count>0) {
		*stream = 0; // buffer underflow
		--count;
		++stream;
	}
}

void YM_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	out((int16_t*)pOutput, frameCount, YMbuffer, &YMhead, &YMtail);
}

void PSG_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	out((int16_t*)pOutput, frameCount, PSGbuffer, &PSGhead, &PSGtail);
}

char x16sound_init() {
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
	YM_reset();
	psg_reset();

	return 1;
}

void x16sound_shutdown() {
	ma_device_uninit(&YM);
	ma_device_uninit(&PSG);
}
