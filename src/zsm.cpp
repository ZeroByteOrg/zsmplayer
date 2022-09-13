#include "zsm.h"
#include "x16sound.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <math.h>

unsigned char delay=0;
int zindex=16;
int playing=0;

void zsm_tick(char*);

void player_start(char* zsm, std::function<void(char*)> func, unsigned int interval)
{
  std::thread([func, interval, zsm]()
  {
    while (true)
    {
      auto x = std::chrono::steady_clock::now() + std::chrono::microseconds(interval);
      func(zsm);
      std::this_thread::sleep_until(x);
    }
  }).detach();
}

void zsm_tick(char* zsm) {
	unsigned char cmd,reg,val;
	if (delay==0) return;
	if (--delay > 0) return;
	while (delay==0) {
//    printf("[%06x] : ",zindex);
		cmd=zsm[zindex];
		if (cmd<0x40) {
			reg=cmd;
			val=zsm[++zindex];
			psg_writereg(reg,val);
//      printf("PSG Write: %02x %02x\n",reg,val);
		}
		else if (cmd==0x40) {
//      printf("EXTCMD: %02x (skipping %d bytes)\n",zsm[zindex+1],zsm[zindex+1]&0x3f);
			zindex += zsm[++zindex] & 0x3f;
		}
		else if (cmd<0x80) {
			cmd &= 0x3f;
//      printf ("YM Write (%d)\n             : ",cmd);
			while (cmd > 0) {
				reg=zsm[++zindex];
				val=zsm[++zindex];
				--cmd;
				YM_write(reg,val);
//        printf ("%02x:%02x ",reg,val);
			}
//      printf("\n");
		}
		else if (cmd==0x80) {
//      printf("END\n");
			playing=0;
			//todo: looping
			break;
		}
		else {
      delay = cmd & 0x7f;
//      printf("Delay %d ticks\n",delay);
    }
    ++zindex;
	}
}

void start_zsm(char* zsm) {
//  unsigned int rate,hz;
  unsigned int hz;
  float rate;
  if (zsm==NULL) { printf("zsm is null\n"); return; }
  zindex=16;
  delay=1;
  hz=(*(unsigned short*)&zsm[0x0c]);
  rate=(float)1/(float)hz * 1000000;
  //player_start(zsm, zsm_tick,rate);
  x16sound_set_music_rate(hz);
}
