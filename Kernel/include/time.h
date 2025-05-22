#ifndef _TIME_H_
#define _TIME_H_

void timer_handler();
int ticks_elapsed();
int seconds_elapsed();

void getTimeRTC(unsigned char *hour, unsigned char *min, unsigned char *sec);
void displayCurrentTime();

#endif
