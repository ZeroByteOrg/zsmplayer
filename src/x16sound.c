#include "x16sound.h"

void x16sound_reset() {
	YM_reset();
	psg_reset();
}
