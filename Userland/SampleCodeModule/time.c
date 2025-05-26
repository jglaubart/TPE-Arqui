#include <sys_calls.h>
#include <usr_stdlib.h>

#define GMT -3

int getHours(){ // te consigue la hora, pero en hexa
	return sys_getHours();
}

int getMinutes(){
	return sys_getMinutes();
}

int getSeconds(){
	return sys_getSeconds();
}

int pow(double base, int exponent) {
    int result = 1;
    for(int i = 0; i < exponent; i++) {
        result *= base;
    }
    return result;
}

void display_time(){
	int hours, minutes, seconds;

	hours = getHours();
	int realHour = 0;
	int gmt_offset = GMT;
	for(int i=0 ; hours > 0 ; i++){
		realHour += (hours % 16) * pow(10, i);
		hours /= 16;
	}
	realHour += gmt_offset;
	if (realHour < 0) {
        realHour += 24;
    } else if (realHour >= 24) {
        realHour -= 24;
    }
	hours = realHour;
	minutes = getMinutes();
	seconds = getSeconds();

	printChar('\n');
	printDec(hours);
	printChar(':');
	printHex(minutes);
	printChar(':');
	printHex(seconds);
}


