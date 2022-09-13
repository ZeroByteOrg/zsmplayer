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

/*
char note[9] = { 0,0x3e,0x41,0x44,0x45,0x48,0x4a,0x4d,0x4e};

void zsm_tick(char* dummy) {
  static char i=0;
  static char delay=1;

  if (delay==0) return;
  if (--delay>0) return;
  delay=30;
  if (i==0) {
    YM_write(0x20,0xC7);
    YM_write(0xe0,0x0f);
    YM_write(0xe8,0x0f);
    YM_write(0xf0,0x0f);
    YM_write(0xf8,0x0f);
    YM_write(0x08,0);
    YM_write(0x40,1);
    YM_write(0x48,1);
    YM_write(0x50,1);
    YM_write(0x58,1);
    YM_write(0x80,0x1f);
    YM_write(0x88,0x1f);
    YM_write(0x90,0x1f);
    YM_write(0x98,0x1f);
    i=1;
  }
  else if (i<9) {
    YM_write(0x08,0);
    YM_write(0x28,note[i]);
    YM_write(0x08,0x78);
    ++i;
  }
}
*/

void zsm_tick(char* zsm) {
	unsigned char cmd,reg,val;
	if (delay==0) return;
	if (--delay > 0) return;
	while (delay==0) {
		cmd=zsm[zindex];
		if (cmd<0x40) {
			reg=zsm[++zindex];
			val=zsm[++zindex];
			psg_writereg(reg,val);
		}
		else if (cmd==0x40) {
			zindex += zsm[++zindex] & 0x3f;
		}
		else if (cmd<0x80) {
			cmd &= 0x3f;
			while (cmd > 0) {
				reg=zsm[++zindex];
				val=zsm[++zindex];
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
    ++zindex;
	}
}

void start_zsm(char* zsm) {
  unsigned int rate,hz;
  if (zsm==NULL) { printf("zsm is null\n"); return; }
  zindex=16;
  delay=1;
  hz=(*(unsigned short*)&zsm[0x0c]);
  rate=floor((float)1/(float)hz * 1000000);
  printf ("playing at tick rate of %d (%dhz)", rate, hz);
  player_start(zsm, zsm_tick,rate);
}

/*
int main()
{
  timer_start(do_something, 1000);
  while (true)
    ;
}
*/
