#include "x16sound.h"
#include <string.h>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


ma_device_config deviceConfig;
ma_device device;


unsigned char vibraphone[] = {
	0xe4,0x00,
	0x19,0x65,0x56,0x61,
	0x1e,0x41,0x23,0x0a,
	0x5f,0x9e,0xdb,0x9e,
	0x10,0x0c,0x07,0x05,
	0x00,0x0b,0x0a,0x0a,
	0xba,0xf6,0x85,0xf5
};

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    YM_render((int16_t*)pOutput, frameCount);
}

char initsound() {
	/* from the Python file:
	device = miniaudio.PlaybackDevice(sample_rate=self.YM.samplerate(), buffersize_msec=50)
	//self.PSGaudio = miniaudio.PlaybackDevice(sample_rate=48828, buffersize_msec=50)
	*/

	deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format   = ma_format_s16;
	deviceConfig.playback.channels = 2;
	deviceConfig.sampleRate        = YM_samplerate(3579545);
	deviceConfig.dataCallback      = data_callback;
//	deviceConfig.pUserData         = &decoder;

	if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
			printf("Failed to open playback device.\n");
			return 0;
	}

	return 1;
}

int main() {
	int i;
	x16sound_reset();
	if (!initsound()) {
		return -3;
	}

	if (ma_device_start(&device) != MA_SUCCESS) {
		printf("Failed to start playback device.\n");
		ma_device_uninit(&device);
		return -4;
	}

	YM_write(0x20,vibraphone[0]);
	for (i=0x38 ; i<0x100 ; i+= 8)
		YM_write(i,vibraphone[i]);
	YM_write(0x28,0x4a);
	YM_write(0x08,0);
	YM_write(0x08,0x78);

	printf("Press Enter to quit...");
	getchar();

	ma_device_uninit(&device);

	return 0;
}
