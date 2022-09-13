#include "x16sound.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep()  - remove when no longer using sleep()


char* filename;
char* zsm = NULL;
unsigned char delay=0;
int zindex=16;
int playing=0;

int load_zsm(char* buffer, const char* filename) {
	FILE *fileptr;
	long filelen;

	if(!(fileptr = fopen(filename, "rb")))  // Open the file in binary mode
		return 0;
	fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
	filelen = ftell(fileptr);             // Get the current byte offset in the file
	rewind(fileptr);                      // Jump back to the beginning of the file

	if(buffer != NULL) free(buffer);
	buffer = (char *)malloc(filelen);    // Enough memory for the file
	fread(buffer, filelen, 1, fileptr);  // Read in the entire file
	fclose(fileptr); // Close the file
	return 1;
}

void zsm_tick() {
	unsigned char cmd,reg,val;
	if (delay==0) return;
	if (--delay > 0) return;
	while (delay==0) {
		cmd=zsm[zindex++];
		if (cmd<0x40) {
			reg=zsm[zindex++];
			val=zsm[zindex++];
			psg_writereg(reg,val);
		}
		else if (cmd==0x40) {
			zindex += zsm[zindex] & 0x3f;
		}
		else if (cmd<0x80) {
			cmd &= 0x3f;
			while (cmd > 0) {
				reg=zsm[zindex++];
				val=zsm[zindex++];
				--cmd;
				YM_write(reg,val);
			}
		}
		else if (cmd==0x80) {
			playing=0;
			//todo: looping
			break;
		}
		else delay = cmd & 0x7f;
	}
}

void start_zsm() {
	zindex=16;
	float tickrate = 48828/(zsm[0x0c] + (zsm[0x0d]<<8));
	delay=1;
	x16sound_callback(&zsm_tick,tickrate);
}

int main() {
	int i,j;
	x16sound_reset();
	if (!x16sound_init()) {
		return -3;
	}

	if (!load_zsm(zsm,"SONIC.ZSM")) {
		printf ("Unable to load %s. Exiting.\n",filename);
	};
	start_zsm();

	printf ("Playing %s. [press enter to end]",filename);
	getchar();
  x16sound_shutdown();

	return 0;
}
