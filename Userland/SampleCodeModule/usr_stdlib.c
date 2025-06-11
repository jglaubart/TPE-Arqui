#include "usr_stdlib.h"
#include <syscalls.h>
#include <stdarg.h>
#include <exceptions.h>

#define MAX_CHARS 1000
#define MAX_NUMBER_LENGTH 100
#define REGISTERS 18
static uint64_t fdprintfargs(FileDescriptor fd, const char *fmt, va_list args);

uint64_t puts(const char *string) {
    uint64_t count = strlen(string);
    return sys_call(SYS_WRITE_ID, STDOUT, string, count, 0xFFFFFF);
}

uint64_t putsColored(const char * string, uint32_t color){
    uint64_t count = strlen(string);
    return sys_call(SYS_WRITE_ID, STDOUT, string, count, color);
}

uint64_t putChar(const char c){
    return sys_call(SYS_WRITE_ID, STDOUT, &c, 1, 0xFFFFFF);
}


uint64_t isChar (char c){
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
		return 1;
	}
	return 0;
}

uint64_t isDigit(char c){
		if (c >= '0' && c <= '9'){
		return 1;
	}
	return 0;
}

uint64_t strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

uint64_t strlen(const char *string) {
    uint64_t length = 0;
    while (string[length] != '\0') {
        length++;
    }
    return length;
}

char *strcpy(char *dest, const char *src) {
    uint64_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

static uint64_t fdprintfargs(FileDescriptor fd, const char *fmt, va_list args)
{
    char buffer[MAX_CHARS] = {0};

    char numstr[MAX_NUMBER_LENGTH] = {0};
    uint64_t i, j, k;
    for (i = 0, j = 0; j < MAX_CHARS && fmt[i] != 0; i++)
    {
        if (fmt[i + 1] != 0)
        {
            if (fmt[i] == '%')
            {
                switch (fmt[i + 1])
                {
                case 's':;
                    // Tengo que poner un string
                    const char *s = va_arg(args, const char *);
                    k = 0;
                    while (j < MAX_CHARS && s[k] != 0)
                        buffer[j++] = s[k++];
                    i++; // salteo la s
                    break;
                case 'd':;
                    // Tengo que poner un valor entero con signo
                    int32_t d = va_arg(args, int32_t);
                    signed_num_to_str(d, numstr);
                    k = 0;
                    while (j < MAX_CHARS && numstr[k] != 0)
                        buffer[j++] = numstr[k++];
                    i++; // salteo la d
                    break;
                case 'u':;
                    // Tengo que poner un valor entero sin signo
                    uint32_t u = va_arg(args, uint32_t);
                    unsigned_num_to_str(u, 0, numstr);
                    k = 0;
                    while (j < MAX_CHARS && numstr[k] != 0)
                        buffer[j++] = numstr[k++];
                    i++; // salteo la u
                    break;
                case 'c':;
                    // Tengo que poner un caracter
                    int8_t c = va_arg(args, int);
                    buffer[j++] = c;
                    i++; // salteo la c
                    break;
                case 'x':;
                    // Tengo que poner un valor entero en hexadecimal
                    uint32_t x = va_arg(args, uint32_t);
                    unsigned_num_to_hex_str(x, numstr);
                    k = 0;
                    while (j < MAX_CHARS && numstr[k] != 0)
                        buffer[j++] = numstr[k++];
                    i++; // salteo la x
                    break;
                default:
                    // Si no es ninguno pongo el porcentaje
                    buffer[j++] = '%';
                    break;
                }
            }
            else if (fmt[i] == '\\')
            {
                switch (fmt[i + 1])
                {
                case 'n':
                    buffer[j++] = '\n';
                    i++;
                    break;
                case 't':
                    buffer[j++] = '\t';
                    i++;
                    break;
                default:
                    buffer[j++] = '\\';
                    i++;
                    break;
                }
            }
            else
            {
                buffer[j++] = fmt[i];
            }
        }
        else
        {
            buffer[j++] = fmt[i];
        }
    }
    buffer[j++] = 0;
    return sys_call(SYS_WRITE_ID, fd, buffer, 1, 0xFFFFFF);
}

uint64_t fdprintf(FileDescriptor fd, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    uint64_t toRet = fdprintfargs(fd, fmt, args);
    va_end(args);
    return toRet;
}

uint64_t myprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    uint64_t toRet = fdprintfargs(STDOUT, fmt, args);
    va_end(args);
    return toRet;
}
//static
void unsigned_num_to_str(uint32_t num, uint32_t start, char *buff){
    uint32_t i = start;
    if (num == 0)
        buff[i++] = '0';
    while (i < MAX_NUMBER_LENGTH - 1 && num > 0)
    {
        buff[i++] = (num % 10) + '0';
        num /= 10;
    }
    buff[i] = 0;
    uint32_t revit = start;
    uint32_t revend = i - 1;
    while (revit < revend)
    {
        char aux = buff[revit];
        buff[revit] = buff[revend];
        buff[revend] = aux;
        revit++;
        revend--;
    }
}

//static
void signed_num_to_str(int32_t num, char *buff){
    uint32_t i = 0;
    if (num < 0)
    {
        buff[i++] = '-';
        num = -num;
    }
    unsigned_num_to_str(num, i, buff);
}

//static
uint32_t unsigned_str_to_num(uint64_t *it, uint64_t buff_length, char *buff){
    uint32_t num = 0;
    uint64_t i = *it;
    while (i < buff_length && buff[i] != ' ' && buff[i] != '\t')
    {
        num = num * 10 + (buff[i++] - '0');
    }
    *it = i;
    return num;
}

//static
void unsigned_num_to_hex_str(uint32_t num, char *buff){
    char hexDigits[] = "0123456789ABCDEF";
    uint32_t i = 0;
    if (num == 0)
        buff[i++] = '0';
    while (i < MAX_NUMBER_LENGTH - 1 && num > 0)
    {
        buff[i++] = hexDigits[num % 16];
        num /= 16;
    }
    buff[i] = 0;
    uint32_t revit = 0;
    uint32_t revend = i - 1;
    while (revit < revend)
    {
        char aux = buff[revit];
        buff[revit] = buff[revend];
        buff[revend] = aux;
        revit++;
        revend--;
    }
}

//static
int32_t signed_str_to_num(uint64_t *it, uint64_t buff_length, char *buff){
    int32_t mult = 1;
    if (buff[*it] == '-')
    {
        mult = -1;
        (*it)++;
    }
    return mult * unsigned_str_to_num(it, buff_length, buff);
}

uint8_t readChar(char *cp){
    return sys_call(SYS_READ_ID, STDIN, cp, 1, 0);
}

char getChar(){
    char c;
    uint64_t len;
    while((len = readChar(&c)) < 1);
    return c;
}

uint64_t readLine(char buff[], uint64_t count)
{
    uint64_t i = 0;
    uint8_t c;
    while ((c = getChar()) != '\n')
    {
        if (i < count)
        {
            buff[i] = c;
        }
        if (c == '\b')
        {
            if (i > 0)
            {
                i += putChar(c);
                if (i < count)
                    buff[i] = 0;
            }
        }
        else
        {
            i += putChar(c);
        }
    }
    putChar('\n');
    uint64_t toRet;
    if (i < count)
    {
        buff[i] = 0;
        toRet = i;
    }
    else
    {
        buff[count] = 0;
        toRet = count;
    }
    return i;
}

uint64_t scanf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char buffer[MAX_CHARS + 1] = {0};
    char auxBuffer[100] = {0};
    uint64_t i, j, count_read;
    uint8_t *char_dir;
    int32_t *int_dir;

    uint64_t buffSize = 0;

    for (i = 0, j = 0, count_read = 0; fmt[i] != 0;)
    {
        if (j == buffSize)
        {
            buffSize = readLine(buffer, MAX_CHARS);
            j = 0;
        }
        if (buffer[j] == ' ')
        {
            j++;
        }
        else
        {
            if (fmt[i + 1] != 0 && fmt[i] == '%')
            {
                switch (fmt[i + 1])
                {
                case 's':;
                    char_dir = va_arg(args, char *);
                    while (j < buffSize && buffer[j] != ' ' && buffer[j] != '\t')
                        *char_dir++ = buffer[j++];
                    *char_dir = 0;
                    i++;
                    count_read++;
                    break;
                case 'd':;
                    int_dir = va_arg(args, int32_t *);
                    *int_dir = signed_str_to_num(&j, buffSize, buffer);
                    i++;
                    count_read++;
                    break;
                case 'u':;
                    int_dir = va_arg(args, uint32_t *);
                    *int_dir = unsigned_str_to_num(&j, buffSize, buffer);
                    i++;
                    count_read++;
                    break;
                case 'c':;
                    char_dir = va_arg(args, char *);
                    *char_dir = buffer[j++];
                    count_read++;
                    i++;
                    break;    
                }
            }
            i++;
        }
    }
    return count_read;
}

void beep(uint64_t freq, uint64_t ticks){
    sys_call(SYS_BEEP_ID, freq, ticks, 0, 0);
}

int changeFontSize(unsigned int newSize){
    //putChar('\n');
    int res = sys_call(SYS_CHANGE_FONT_SIZE, newSize, 0, 0, 0);
    return res;
}

void InvalidOpCodeTest(){
    ex_invalid_opcode_exception();
}
void zeroDivTest(){
    ex_zero_division_exception();
}

const char *registerTitles[REGISTERS] = {
    "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP",
    "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15",
    "RIP", "RFLAGS"
};

static void convertToHex(uint64_t number, char buffer[16]) {
    int index = 15;
    do {
        int remainder = number % 16;
        if (remainder < 10)
            buffer[index] = remainder + '0';
        else
            buffer[index] = remainder + 'A' - 10;
        number /= 16;
        index--;
    } while (index != -1);
}
void getRegisters() {
    uint64_t regs[REGISTERS];
    uint64_t retValue = sys_call(SYS_GET_REGS_ID, regs, 0, 0, 0);
    if(retValue == 0) {
        myprintf("Must press CTRL to backup registers.");
    }
    else{
        uint64_t reg;
         // Buffer para convertir a hexadecimal
        char buffer[19];
            buffer[0] = '0';
            buffer[1] = 'x';
            buffer[18] = '\0';

        // Imprimo los registros
        for (int i = 0; i < REGISTERS; i++) {
            myprintf("%s - ",registerTitles[i]);
            convertToHex(regs[i], buffer + 2);
            myprintf(buffer);
            if (i % 4 == 3)
                puts("\n");
            else
                myprintf(" || "); 
        }
        puts("\n");
    }

}

void clearScreen() {
    sys_call(SYS_CLEAR_ID, 0, 0, 0, 0);
}

void sleep(uint64_t ticks) {
    sys_call(SYS_SLEEP_ID, ticks, 0, 0, 0);
}

void getTime() {
    rtc_time_t time = {0};
    sys_call(SYS_TIME_ID, &time, 0, 0, 0);
    myprintf("Time: %d:%d:%d   Date: %d/%d/%d\n", time.hours, time.minutes, time.seconds, time.day, time.month, time.year);
}

uint64_t getScreenWidth() {
    return sys_call(SYS_GET_SCREEN_WIDTH_ID, 0, 0, 0, 0);
}
uint64_t getScreenHeight() {
    return sys_call(SYS_GET_SCREEN_HEIGHT_ID, 0, 0, 0, 0);
}

uint64_t isPressed(char c) {
    return sys_call(SYS_IS_PRESSED_ID, (uint64_t)c, 0, 0, 0);
}

void setDrawBuffer(int buffer) {
    sys_call(SYS_SET_DRAW_BUFFER_ID, buffer, 0, 0, 0);
}

void showBackBuffer() {
    sys_call(SYS_SHOW_BACK_BUFFER_ID, 0, 0, 0, 0);
}

void changeBackgroundColor(uint32_t color){
    sys_call(SYS_CHANGE_BG_COLOR_ID, color, 0, 0, 0);
    clearScreen();
}

/**
 * normalize_angle:
 *   Bring any angle (in radians) into the principal range [-π, +π].
 */
double normalize_angle(double x) {
    // Reduce x mod 2π into (-2π, +2π)
    double two_pi = 2.0 * PI;
    double n = x / two_pi;
    // Round n to nearest integer
    if (n >= 0.0) {
        n = (double)((long)(n + 0.5));
    } else {
        n = (double)((long)(n - 0.5));
    }
    x -= n * two_pi;
    // Now x is in (-2π, +2π). If > π or < -π, shift by ±2π
    if (x > PI) {
        x -= two_pi;
    } else if (x < -PI) {
        x += two_pi;
    }
    return x;
}

/**
 * sin_taylor:
 *   Approximate sin(x) for x in radians, where x ∈ [-π, π], using a
 *   7-term Taylor series. Error is on the order of 10^(-8) for |x| ≤ π.
 *
 *   sin(x) ≈ x
 *            - x^3/6
 *            + x^5/120
 *            - x^7/5040
 *            + x^9/362880
 *            - x^11/39916800
 *            + x^13/6227020800
 */
double sin_taylor(double x) {
    x = normalize_angle(x);
    double x2 = x * x;
    double term = x;           // x
    double sum  = term;

    term *= -x2 / (2.0 * 3.0);  // - x^3/3!
    sum += term;

    term *= -x2 / (4.0 * 5.0);  // + x^5/5!
    sum += term;

    term *= -x2 / (6.0 * 7.0);  // - x^7/7!
    sum += term;

    term *= -x2 / (8.0 * 9.0);  // + x^9/9!
    sum += term;

    term *= -x2 / (10.0 * 11.0); // - x^11/11!
    sum += term;

    term *= -x2 / (12.0 * 13.0); // + x^13/13!
    sum += term;

    return sum;
}

/**
 * cos_taylor:
 *   Approximate cos(x) for x in radians, where x ∈ [-π, π], using a
 *   7-term Taylor series. Error is on the order of 10^(-8) for |x| ≤ π.
 *
 *   cos(x) ≈ 1
 *            - x^2/2
 *            + x^4/24
 *            - x^6/720
 *            + x^8/40320
 *            - x^10/3628800
 *            + x^12/479001600
 */
double cos_taylor(double x) {
    x = normalize_angle(x);
    double x2 = x * x;
    double term = 1.0;       // 1
    double sum  = term;

    term *= -x2 / (1.0 * 2.0);   // - x^2/2!
    sum += term;

    term *= -x2 / (3.0 * 4.0);   // + x^4/4!
    sum += term;

    term *= -x2 / (5.0 * 6.0);   // - x^6/6!
    sum += term;

    term *= -x2 / (7.0 * 8.0);   // + x^8/8!
    sum += term;

    term *= -x2 / (9.0 * 10.0);  // - x^10/10!
    sum += term;

    term *= -x2 / (11.0 * 12.0); // + x^12/12!
    sum += term;

    return sum;
}

/* Absolute value for double */
double my_fabs(double x) {
    return x < 0 ? -x : x;
}

/* Compute square root of x using Newton–Raphson,
   looping only while the change remains ≥ epsilon */
double my_sqrt(double x) {
    if (x < 0.0) {
        /* negative input: return -1 as an error code */
        return -1.0;
    }
    if (x == 0.0 || x == 1.0) {
        return x;
    }

    double guess   = x;                             /* initial guess */
    double epsilon = 1e-12;                         /* desired precision */
    double next    = 0.5 * (guess + x / guess);     /* first iteration */

    /* keep iterating as long as difference ≥ epsilon */
    while (my_fabs(next - guess) >= epsilon) {
        guess = next;
        next  = 0.5 * (guess + x / guess);
    }

    return next;
}

uint64_t moveCursor(uint64_t x, uint64_t y) {
    return sys_call(SYS_MOVE_CURSOR_ID, x, y, 0, 0);
}
