#include <shell.h>
#include <usr_stdlib.h>
#include <commands.h>

#define CANT_INSTRUCTIONS 10
char instructions[CANT_INSTRUCTIONS][50];

char line[MAX_BUFFER] = {0};
char parameter[MAX_BUFFER] = {0};
char command[MAX_BUFFER] = {0};

static int checkLine();
static void executeCommand();
static void resetInputState();

void startShell(){
    puts("----------------- Shell ---------------\n");
    beep(1000, 100);
    showCommands();
    
    while(1) {
        puts("\n$ User > ");
        scanf("%s", line);  // Read input using scanf
        executeCommand();
        resetInputState();
    }
}

//separa comando de parametro
static int checkLine(){
	int i = 0;
	int j = 0;
	int k = 0;
	uint64_t lineLen = strlen(line);
	for ( j = 0 ; j < lineLen && line[j] != ' ' ; j ++){ //aca leemos el comando, por eso si encontramos un '' salimos del for
		command[j] = line[j];
	}
	command[j] = '\0'; // Null terminate command
	
	if (j < lineLen){ //si hay un parametro
		j++;
		while (j < lineLen){ //leemos el parametro en parameter
			parameter[k++] = line[j++];
		}
	}
	parameter[k] = '\0'; // Null terminate parameter

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
}