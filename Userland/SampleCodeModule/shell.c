#include <shell.h>
#include <usr_stdlib.h>

#define CANT_INSTRUCTIONS 10
char instructions[CANT_INSTRUCTIONS][50];
//echo y clear son extras

typedef enum
{
    HELP = 0,
    REGISTERS,
    TIME,
    PONGIS_GOLF,
    CHANGE_FONT_SIZE,
    TEST_ZERO_DIVISION,
    TEST_INVALID_OPCODE,
    ECHO,
    CLEAR,
} INSTRUCTION;
void complete_instructions() {
    strcpy(instructions[HELP], "help");
    strcpy(instructions[REGISTERS], "inforeg");
    strcpy(instructions[TIME], "time");
    strcpy(instructions[PONGIS_GOLF], "pongis_golf");
    strcpy(instructions[CHANGE_FONT_SIZE], "font_size");
    strcpy(instructions[TEST_ZERO_DIVISION], "zero_div");
    strcpy(instructions[TEST_INVALID_OPCODE], "inv_opcode");
    strcpy(instructions[ECHO], "echo");
    strcpy(instructions[CLEAR], "clear");
}

void startShell(){
    complete_instructions();

    puts("Hello World\bem!\n");
    //changeFontSize(10);
	puts("ahorita la andaba yo buscando, pero no la encontre, busque entre mil botellas, pero no la encontre, me dicen el que la encuentra la va a tomar y la va a beber y la va a beber y la va a beber\b\n");
	for(int i=0; i<25; i++){
		newLine();
	}
	puts("prueba de auto-scroll\n");
    char *string = "hola";
    puts(string);

    char buffer[100];

    readLine(buffer, 100);

    puts("You typed: ");
    puts(buffer);
    puts("\n");

    puts("tamaño anterior");
    changeFontSize(10);
    puts("tamaño nuevo");
    puts("blablabla");
}