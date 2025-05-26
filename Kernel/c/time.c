#include <stdint.h>
#include "time.h"

static unsigned long ticks = 0;
extern int _hlt();

int secondsPassed = 0;

void timer_handler() {
	ticks++;
	secondsPassed += 55;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}


int ms_elapsed() {
    return ticks*5000/91;
}

void timer_wait(int delta) { 
	int initialTicks = ticks;
	while(ticks - initialTicks < delta) {
		_hlt();
	}
}

void sleep(int millis){
	secondsPassed = 0;
	while (secondsPassed<millis)
	{
		_hlt();
	}
}