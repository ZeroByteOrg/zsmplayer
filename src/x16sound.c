#include "x16sound.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

extern char* zsm;
extern void zsm_tick(char*);

ma_device_config deviceConfig;
ma_device YM,PSG;

float frames=0;
float FramesPerTick=0;

void x16sound_reset() {
	YM_reset();
	psg_reset();
}

void x16sound_set_music_rate(float hz) {
	FramesPerTick=48828/hz;
	frames=0;
}

void YM_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    YM_render((int16_t*)pOutput, frameCount);
}

void PSG_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	if (FramesPerTick==0) {
		psg_render((int16_t*)pOutput, frameCount);
		return;
	}
	while (frameCount > 0) {
		unsigned short UntilTick = floor(FramesPerTick-frames);
		unsigned short nFrames;
		if (UntilTick < frameCount)
		 	nFrames=UntilTick;
		else nFrames=frameCount;
		psg_render((int16_t*)pOutput, nFrames);
		zsm_tick(zsm);
		frameCount-=nFrames;
		frames+=nFrames-FramesPerTick;
	}
}

char x16sound_init() {
	/* from the Python file:
	device = miniaudio.PlaybackDevice(sample_rate=self.YM.samplerate(), buffersize_msec=50)
	//self.PSGaudio = miniaudio.PlaybackDevice(sample_rate=48828, buffersize_msec=50)
	*/

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
