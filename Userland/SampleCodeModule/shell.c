#include <usr_stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys_calls.h>
#include <colores.h>
#include "eliminator.h"
#include "shell.h"

#define MAX_BUFFER 254
#define MAX_COMMANDS 10

char line[MAX_BUFFER+1] = {0}; //asi me aseguro que al menos va haber un cero
char parameter[MAX_BUFFER+1] = {0};
char command[MAX_BUFFER+1] = {0};
int linePos = 0;
char lastc;
const char * commands[] = {"undefined","help","time","clear","eliminator","inforeg","zerodiv","invopcode","sizeup","sizedown"};

void showCommands(){
	printString("\n Comandos disponibles:",MAX_BUFFER);
	printString("\n*clear*              Resetea la shell a su estado original",MAX_BUFFER);
	printString("\n*eliminator*         Inicia el juego de eliminator",MAX_BUFFER);
	printString("\n*inforeg*            Imprime los valores de los registros (CTRL + R es necesario previamente)",MAX_BUFFER);
	printString("\n*sizeup*             Aumenta el tamanio de letra",MAX_BUFFER);
	printString("\n*sizedown*           Disminuye el tamanio de letra",MAX_BUFFER);
	printString("\n*time*               Imprime la hora del sistema en pantalla",MAX_BUFFER);
	printString("\n*invopcode*          Prueba la excepcion de codigo de operacion invalido",MAX_BUFFER);
	printString("\n*zerodiv*            Prueba de excepcion de division por cero",MAX_BUFFER);
	printChar('\n');
}



static void newLine();
static void printLine(char c);
static int checkLine();
static void undefinedCommand();
static void helpCommand();
static void timeCommand(); 
static void clearCommand();
static void eliminatorCommand();
static void infoRegCommand();
static void zeroDivisionCommand();
static void invalidOpCodeCommand();
static void upscaleCommand();
static void downscaleCommand();


static void (*commands_ptr[MAX_COMMANDS])() = {undefinedCommand, helpCommand, timeCommand, clearCommand, eliminatorCommand, infoRegCommand, zeroDivisionCommand,invalidOpCodeCommand,
											   upscaleCommand,downscaleCommand};


void shell (){
	char c;
	printString("$ User> ",9);

	while(1){
		c = getChar();
		printLine(c);
	};
}

static void printLine(char c){ 
	if (linePos < MAX_BUFFER && c != lastc){
		if (isChar(c) || c == ' ' ||isDigit(c)){
			line[linePos++] = c;
			printChar(c);
		} else if (c == '\b' && linePos > 0){
			printChar(c);
			line[--linePos] = 0;
		} else if (c == '\n'){
			newLine();
		}
	}
	lastc = c;
}


static void newLine(){ //aca se ejecuta el comando
	int i = checkLine(); //buscamos el comando en el array de comandos

	(*commands_ptr[i])();  //ejecutamos el comando

	for (int i = 0; line[i] != '\0' ; i++){ //limpiamos los buffers
		line[i] = 0;
		command[i] = 0;
		parameter[i] = 0;
	}
	linePos = 0; //reseteamos la posicion del buffer

	if (i != 3 ){ //si no es clear
		printString("\n$ User> ",9); //imprimimos el prompt en la siguiente linea
	} else {
		printString("$ User> ",9); //imprimimos el prompt
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

	for (i = 1 ; i < MAX_COMMANDS ; i++ ){ //buscamos el comando en el array de comandos
		if (strcmp(command,commands[i]) == 0){ //si lo encontramos
			return i; //devolvemos la posicion en el array de comandos
		}
	}

	return 0; //si no encontramos el comando devolvemos 0
}


static void helpCommand(){
	printString("\n---HELP---\n",MAX_BUFFER);
	showCommands();
}

static void undefinedCommand(){
	printString("\n\nNo se reconoce \"",MAX_BUFFER);
	printString(command,MAX_BUFFER);
	printString("\" como un comando valido, para ver los comandos disponibles escribir \"help\"\n",MAX_BUFFER);
}

static void timeCommand(){
	display_time();
}


static void eliminatorCommand(){
	if(!startEliminator(charToInt(parameter))){
		printString("\nParametro invalido. Utilice 'eliminator 1' o 'eliminator 2' para comenzar el juego\n",MAX_BUFFER);
	}
	
}

static void clearCommand(){
	clear_scr();
}

static void infoRegCommand(){
	// Le pasamos cero para que imprima unicamente lo que este capturado (o nada)
	inforeg(0);
}

static void invalidOpCodeCommand(){
	InvalidOpCodeTest();
}

static void zeroDivisionCommand(){
	zeroDivTest();
}

static void upscaleCommand(){
	increaseScale();
}

static void downscaleCommand(){
	decreaseScale();
}
