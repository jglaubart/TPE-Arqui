#include <shell.h>
#include <usr_stdlib.h>
#include <commands.h>

#define CANT_INSTRUCTIONS 10
char instructions[CANT_INSTRUCTIONS][50];

char line[MAX_BUFFER+1] = {0};
char parameter[MAX_BUFFER+1] = {0};
char command[MAX_BUFFER+1] = {0};
int linePos = 0;
char lastc;

static void newLine();
static void readInput();
static int checkLine();

void startShell(){
    puts("----------------- Shell ---------------\n");
    showCommands();
    puts("\n$ User > ");
	printf("the screen width is %d\n", getScreenWidth());
	printf("the screen height is %d\n", getScreenHeight());
    readInput();
}

static void readInput(){ 
    char c;
    while(1) {
        c = getChar();
        if (linePos < MAX_BUFFER){
            if (isChar(c) || c == ' ' ||isDigit(c)){
                line[linePos++] = c;
                putChar(c);
            } else if (c == '\b' && linePos > 0){
                putChar(c);
                line[--linePos] = 0;
            } else if (c == '\n'){
                newLine();
            }
        }
        lastc = c;
    }
}


static void newLine(){
    puts("\n"); 
	int i = checkLine(); //buscamos el comando en el array de comandos
    if (i < 0){ //si no encontramos el comando
        puts("Command not found. Type 'help' for a list of available commands.\n");
    }else{
	    (*commands_ptr[i])();  //ejecutamos el comando
    }
	for (int i = 0; line[i] != '\0' ; i++){ //limpiamos los buffers
		line[i] = 0;
		command[i] = 0;
		parameter[i] = 0;
	}
	linePos = 0; //reseteamos la posicion del buffer

	if (i != 2){ //si no es clear
		puts("\n$ User > "); //imprimimos el prompt en la siguiente linea
	} else {
		puts("$ User > "); //imprimimos el prompt
	}
}

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