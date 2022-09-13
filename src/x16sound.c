#include "x16sound.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

ma_device_config deviceConfig;
ma_device YM,PSG;

extern void zsm_tick();


void (*tick)(void) = NULL;
float tickrate=0;

void x16sound_reset() {
	YM_reset();
	psg_reset();
}

void YM_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    YM_render((int16_t*)pOutput, frameCount);
}

void PSG_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
		static float frames=0;
		if (tick==NULL) {
    	psg_render((int16_t*)pOutput, frameCount);
			return;
		}
		while (frameCount > 0) {
			if (frameCount < tickrate-frames) {
				psg_render((int16_t*)pOutput, frameCount);
				frames+=frameCount;
				frameCount=0;
			}
			else {
				psg_render((int16_t*)pOutput, floor(tickrate-frames));
				frameCount -= floor(tickrate-frames);
				frames -= floor(tickrate-frames);
				zsm_tick();
			}
		}
}

char x16sound_init() {
	/* from the Python file:
	device = miniaudio.PlaybackDevice(sample_rate=self.YM.samplerate(), buffersize_msec=50)
	//self.PSGaudio = miniaudio.PlaybackDevice(sample_rate=48828, buffersize_msec=50)
	*/

	tick=NULL;
	deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format   = ma_format_s16;
	deviceConfig.playback.channels = 2;
	deviceConfig.sampleRate        = YM_samplerate(3579545);
	deviceConfig.dataCallback      = YM_callback;
//	deviceConfig.pUserData         = &decoder;

	if (ma_device_init(NULL, &deviceConfig, &YM) != MA_SUCCESS) {
			printf("Failed to open YM2151 playback device.\n");
			return 0;
	}

	deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format   = ma_format_s16;
	deviceConfig.playback.channels = 2;
	deviceConfig.sampleRate        = 48828;
	deviceConfig.dataCallback      = PSG_callback;
//	deviceConfig.pUserData         = &decoder;

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

void x16sound_callback(void (*callback)(void), float rate) {
	tick=callback;
	tickrate=rate;
}
