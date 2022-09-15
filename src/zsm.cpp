#include "zsm.h"
#include "x16sound.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <math.h>

char* zsm = NULL;
char delay=1;
unsigned int ptr;
char loops;

int load_zsm(const char* filename) {
	FILE *fileptr;
	long filelen;

	if(!(fileptr = fopen(filename, "rb")))  // Open the file in binary mode
		return 0;
	fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
	filelen = ftell(fileptr);             // Get the current byte offset in the file
	rewind(fileptr);                      // Jump back to the beginning of the file

  if (zsm != NULL) free(zsm);
	zsm = (char *)malloc(filelen);    // Enough memory for the file
	fread(zsm, filelen, 1, fileptr);  // Read in the entire file
	fclose(fileptr);                  // Close the file
	delay=1;
	ptr=16;
	loops=2;
	return 1;
}

bool zsm_tick() {
  unsigned char cmd,reg,val;

	if (delay==0) return false;
  if (zsm==NULL) return false;
	if (--delay > 0) return true;
	while (delay==0) {
//printf("[%06x] : ",i);
		cmd=zsm[ptr];
		if (cmd<0x40) {
			reg=cmd;
			val=zsm[++ptr];
			psg_writereg(reg,val);
//printf("PSG Write: %02x %02x\n",reg,val);
		}
		else if (cmd==0x40) {
//printf("EXTCMD: %02x (skipping %d bytes)\n",zsm[i+1],zsm[i+1]&0x3f);
			ptr += zsm[++ptr] & 0x3f;
		}
		else if (cmd<0x80) {
			cmd &= 0x3f;
//printf ("YM Write (%d)\n             : ",cmd);
			while (cmd > 0) {
				reg=zsm[++ptr];
				val=zsm[++ptr];
				--cmd;
				YM_write(reg,val);
//printf ("%02x:%02x ",reg,val);
			}
//printf("\n");
		}
		else if (cmd==0x80) {
//printf("END\n");
			uint32_t l = (*(uint32_t*)&zsm[0x03]) & 0xFFFFFF;
			if (--loops && l) {
				ptr=l;
				continue;
			}
			break;
		}
		else {
      delay = cmd & 0x7f;
//printf("Delay %d ticks\n",delay);
    }
    ++ptr;
	}
  return (delay > 0);
}
