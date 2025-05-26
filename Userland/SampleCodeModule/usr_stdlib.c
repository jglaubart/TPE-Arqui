#include <sys_calls.h>
#include <stdio.h>
#include <exc_test.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define CANT_REGISTERS 18

static char buffer[64] = { '0' };

int scr_height;
int scr_width;



static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

void printChar (char c){
	sys_write(STDOUT, c);
}

void printString (const char * str, int lenght){
	for (int i = 0 ; i < lenght && str[i] != 0 ; i++){
		printChar(str[i]);
	}
}

char getChar(){
	char c;
	sys_read(0,&c);
	return c;
}


int isChar (char c){
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
		return 1;
	}
	return 0;
}

int isDigit(char c){
		if (c >= '0' && c <= '9'){
		return 1;
	}
	return 0;
}

int strlen(const char *str){
	int i = 0;
	while (str[i] != 0){
		i++;
	}

	return i;
}

int strcmp(const char *str1, const char *str2){
	int i = 0;
	int ret = 0;

	while (str1[i] != 0 && str2[i] != 0){
		if (str1[i] - str2[i] != 0){
			return str1[i] - str2[i];
		}
		i++;
	}

	if (str1[i] != 0 || str2[i] != 0){
		return str1[i] - str2[i];
	}

	return ret;
}

uint64_t charToInt(char* str){
	uint64_t ret = 0;

	if (!isDigit(str[0])){
		ret += str[0] - '0';
		return -1;
	}

	for (int i = 0 ; str[i] != 0 ; i++ ){
		if (!isDigit(str[i])){
			return -1;
		}
		ret *= 10;
		ret += str[i] - '0';
	}
	return ret;
}

void printBase(uint64_t value, uint32_t base){
    uintToBase(value, buffer, base);
    for (int i = 0 ; buffer[i] != '\0' ; i++ ){
        printChar(buffer[i]);
    }
}

void printDec(uint64_t value){
    printBase(value, (uint32_t) 10);
}

void printHex(uint64_t value){
    printBase(value, (uint32_t) 16);
}

void printBin(uint64_t value){
    printBase(value, (uint32_t) 2);
}


static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base){
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;

    //Calculate characters for each digit
    do{
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    }while (value /= base);

    // Terminate string in buffer.
    *p = 0;

    //Reverse string in buffer.
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2){
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
    return digits;
}


void clear_scr(){
	sys_clear();
}

int getScreenWidth(){
	return scr_width;
}

int getScreenHeight(){
	return scr_height;
}

static void set_screenSize(){
	scr_width = sys_scrWidth();
	scr_height = sys_scrHeight();
}

void paintRectangle(int x, int y, int x2, int y2, Color color){
	set_screenSize();
	if ( (x >= 0 && x+x2 < scr_width) && ((y >= 0 && y+y2 < scr_height)) ){
		sys_fillRect  (x, y, x2, y2, color);
	}
}

void wait(uint64_t ms){
	sys_wait(ms);
}


static const char* register_names[17] = {
    "RIP", "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15"
};


static void uint64ToHex(uint64_t n, char buf[16]) {
    int i=15;
    do {
        int digit = n % 16;
        buf[i] = (digit < 10 ? '0' : ('A' - 10)) + digit;
        n /= 16;
    } while(i-- != 0);
}

void captureRegisters(){
	uint64_t registers[17];
	sys_inforeg(registers, 1);
}

void inforeg(int capture){
	char hexbuf[19];
    hexbuf[0] = '0';
    hexbuf[1] = 'x';
    hexbuf[18] = '\0';

	uint64_t registers[17];
	int i = sys_inforeg(registers, capture);

	printChar('\n');	
	if (i == 1){
		for (int i = 0; i < 17; i++) {
        	printString(register_names[i],100);
        	printString(": ",100);
        	uint64ToHex(registers[i], hexbuf+2);
        	printString(hexbuf,100);
        	if (i % 4 == 3)
            printChar('\n');
        else
            printString("   ",100);
        }


    } else {
		printString("\nTodavia no se capturaron los registros, presione CTRL + R para capturar\n",100);
	}
}



void InvalidOpCodeTest(){
	ex_invopcode();
}

void zeroDivTest(){
	ex_zerodiv();
}


void increaseScale(){
	syscall_pixelUp();
}

void decreaseScale(){
	syscall_pixelDown();
}


int print_mem(uint64_t mem){
	return sys_printmem(mem);
}