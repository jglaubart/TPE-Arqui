#include <shell.h>
#include <usr_stdlib.h>
#include <commands.h>

#define CANT_INSTRUCTIONS 10
char instructions[CANT_INSTRUCTIONS][50];

char line[MAX_BUFFER] = {0};
char parameter[MAX_BUFFER] = {0};
char command[MAX_BUFFER] = {0};
int linePos = 0;

static int readInput();  // Returns 1 when complete line is ready
static int checkLine();
static void executeCommand();
static void resetInputState();

void startShell(){
    puts("----------------- Shell ---------------\n");
    beep(1000, 100);
    showCommands();
    puts("\n$ User > ");
    
    while(1) {
        if(readInput()) { // Returns 1 when a complete line is ready
            executeCommand();
            resetInputState();
            puts("$ User > ");
        }
    }
}

static int readInput(){ 
    char c = getChar();
    
    if (linePos < MAX_BUFFER || c == '\b'){
        if (isChar(c) || c == ' ' || isDigit(c)){
            line[linePos++] = c;
            putChar(c);
        } else if (c == '\b' && linePos > 0){
            putChar(c);
            line[--linePos] = 0;
        } else if (c == '\n'){
			line[linePos] = 0;
            return 1;
        }
    } else if (c == '\n') {
		line[linePos] = 0;
        return 1; // Complete line ready (even if buffer is full)
    }
    
    return 0; // Line not complete yet
}

/* static void readInput(){
	char c;
	uint64_t i = 0;
	while((c = getChar()) != '\n' && i < MAX_BUFFER - 1){
		if(c == '\b' && i > 0){
			putChar(c);
			i--;
			line[i] = 0;
		}else{
			putChar(c);
			line[i++] = c;
		}
	}
	line[i] = 0;
	putChar('\n');
	return;
} */

//separa comando de parametro
static int checkLine(){
	int i = 0;
	int j = 0;
	int k = 0;
	for ( j = 0 ; j < linePos && line[j] != ' ' ; j ++){ //aca leemos el comando, por eso si encontramos un '' salimos del for
		command[j] = line[j];
	}
	if (j < linePos){ //si hay un parametro
		j++;
		while (j < linePos){ //leemos el parametro en parameter
			parameter[k++] = line[j++];
		}
	}

	for (i = 0 ; i < MAX_COMMANDS ; i++ ){ //buscamos el comando en el array de comandos
		if (strcmp(command,commands[i]) == 0){ //si lo encontramos
			return i; //devolvemos la posicion en el array de comandos
		}
	}

	return -1; //si no encontramos el comando devolvemos 0
}

static void executeCommand() {
    puts("\n"); 
    int i = checkLine(); // Find command in array
    if (i < 0) { // Command not found
        puts("Command not found. Type 'help' for a list of available commands.\n");
    } else {
        (*commands_ptr[i])(); // Execute the command
    }
}

static void resetInputState() {
    // Clear all buffers
    for (int i = 0; line[i] != '\0'; i++) {
        line[i] = 0;
        command[i] = 0;
        parameter[i] = 0;
    }
    linePos = 0; // Reset buffer position
}