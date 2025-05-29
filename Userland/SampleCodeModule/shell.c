#include <shell.h>
#include <usr_stdlib.h>

static char *instructions[] = {"help", "registers", "time", "pongis_golf", "change_font_size",  "test_zero_division", "test_invalid_opcode", "echo", "clear", 0};
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

void startShell(){
    

    puts("Hello World\bem!\n");
	putChar('a');
	puts("ahorita la andaba yo buscando, pero no la encontre, busque entre mil botellas, pero no la encontre, me dicen el que la encuentra la va a tomar y la va a beber y la va a beber y la va a beber\b\n");
	for(int i=0; i<25; i++){
		newLine();
	}
	puts("prueba de auto-scroll\n");
}