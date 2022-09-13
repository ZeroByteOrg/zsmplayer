#include "x16sound.h"
#include "zsm.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* filename = "SONIC.ZSM";
char* zsm;

int load_zsm(char** zsm, const char* filename) {
	FILE *fileptr;
	long filelen;
	char* buffer;

	if(!(fileptr = fopen(filename, "rb")))  // Open the file in binary mode
		return 0;
	fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
	filelen = ftell(fileptr);             // Get the current byte offset in the file
	rewind(fileptr);                      // Jump back to the beginning of the file

	buffer = (char *)malloc(filelen);    // Enough memory for the file
	fread(buffer, filelen, 1, fileptr);  // Read in the entire file
	fclose(fileptr); // Close the file
	if(*zsm != NULL) free(*zsm);
	*zsm=buffer;
	return 1;
}

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
