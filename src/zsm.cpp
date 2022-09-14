#include "zsm.h"
#include "x16sound.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <math.h>

char* zsm = NULL;

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
	fclose(fileptr); // Close the file
	return 1;
}

bool zsm_tick() {
  unsigned char cmd,reg,val;

  static unsigned char delay=1;
  static unsigned int i=16;

	if (delay==0) return false;
  if (zsm==NULL) return false;
	if (--delay > 0) return true;
	while (delay==0) {
//    printf("[%06x] : ",i);
		cmd=zsm[i];
		if (cmd<0x40) {
			reg=cmd;
			val=zsm[++i];
			psg_writereg(reg,val);
//      printf("PSG Write: %02x %02x\n",reg,val);
		}
		else if (cmd==0x40) {
//      printf("EXTCMD: %02x (skipping %d bytes)\n",zsm[i+1],zsm[i+1]&0x3f);
			i += zsm[++i] & 0x3f;
		}
		else if (cmd<0x80) {
			cmd &= 0x3f;
//      printf ("YM Write (%d)\n             : ",cmd);
			while (cmd > 0) {
				reg=zsm[++i];
				val=zsm[++i];
				--cmd;
				YM_write(reg,val);
//        printf ("%02x:%02x ",reg,val);
			}
//      printf("\n");
		}
		else if (cmd==0x80) {
//      printf("END\n");
			//todo: looping
			break;
		}
		else {
      delay = cmd & 0x7f;
//      printf("Delay %d ticks\n",delay);
    }
    ++i;
	}
  return (delay > 0);
}
