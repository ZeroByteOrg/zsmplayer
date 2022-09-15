#ifndef _zsm_h_
#define _zsm_h_

extern char* zsm;

extern int load_zsm(const char* filename);
extern bool zsm_tick();
extern void zsm_onloop(void (*func)(void));
#endif
