#include <rtc_time.h>
#include <stdint.h>
#include <videoDriver.h>
static unsigned long ticks = 0;
static int frequency = 18;

#define SECONDS_REG 0x00
#define MINUTES_REG 0x02
#define HOURS_REG 0x04
#define DAY_REG 0x07
#define MONTH_REG 0x08
#define YEAR_REG 0x09
#define GMT_OFFSET -3
#define NMI_DISABLE_BIT 1
#define REGISTER_AMOUNT 6
#define PIT_FREQ 1193182

void timer_handler()
{
	ticks++;
}

int ticks_elapsed()
{
	return ticks;
}

int seconds_elapsed()
{
	return ticks / frequency;
}
void sleep(uint32_t ticksToWait)
{
	int ticksAtCallTime = ticks_elapsed();
	while (ticks - ticksAtCallTime < ticksToWait)
		;
}
void setTickFrequency(uint16_t freq)
{
	frequency = freq;
	int ticks = PIT_FREQ / freq;
	outp(0x43, 0xb6);
	outp(0x40, (uint8_t)(ticks));
	outp(0x40, (uint8_t)(ticks >> 8));
}

static int isLeap(uint16_t year)
{
	return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

// Auxiliar para get_time
static uint8_t bcd_decimal(uint8_t hex)
{
	int dec = ((hex & 0xF0) >> 4) * 10 + (hex & 0x0F);
	return dec;
}
// Deja en el puntero a time_t todos los parametros de tiempo
// El formato es: segundos, minutos, horas, dia, mes, año
void get_time(rtc_time_t* time)
{
	uint8_t daysPerMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	uint8_t regNeeded[] = {SECONDS_REG, MINUTES_REG, HOURS_REG, DAY_REG, MONTH_REG, YEAR_REG};
	uint8_t data[REGISTER_AMOUNT];
	for (int i = 0; i < REGISTER_AMOUNT; i++)
	{
		outp(0x70, (NMI_DISABLE_BIT << 7) | (regNeeded[i]));
		data[i] = inp(0x71);
	}
	time->seconds = 0xFF;
	time->seconds = bcd_decimal(data[0]);
	time->minutes = bcd_decimal(data[1]);
	time->hours = (bcd_decimal(data[2]) + GMT_OFFSET + 24) % 24;
	if (time->hours < 0)
		time->hours += 24;
	time->day = bcd_decimal(data[3] - ((time->hours > 21) ? 1 : 0));
	time->month = bcd_decimal(data[4]);
	time->year = bcd_decimal(data[5]);
	if (time->day == 0)
	{
		if (time->month == 1)
		{
			time->month = 12;
			time->year--;
		}
		else
		{
			time->month--;
		}
		time->day = daysPerMonth[time->month - 1] + (time->month == 2) * isLeap(time->year);
	}
}
