#ifndef _TIME_H_
#define _TIME_H_
#include <stdint.h>
typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_time_t;


void timer_handler();
int ticks_elapsed();
int seconds_elapsed();
void get_time(rtc_time_t *time);
void sleep(uint32_t ticks);
uint64_t msToTicks(uint64_t ms);

#endif
