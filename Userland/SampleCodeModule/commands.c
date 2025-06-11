#include <shell.h>
#include <commands.h>
#include <usr_stdlib.h>
#include "pongis.h"
// IMPLEMENTAION DE LOS COMANDOS
void showCommands(){
	puts("\n Comandos disponibles:");
	puts("\nclear              Resetea la shell a su estado original");
	puts("\ngolf         	  Inicia el juego de Pongis Golf");
	puts("\ninforeg            Imprime los valores de los registros (CTRL es necesario previamente)");
	puts("\ntime               Imprime la hora del sistema en pantalla");
	puts("\ninvopcode          Prueba la excepcion de codigo de operacion invalido");
	puts("\nzerodiv            Prueba de excepcion de division por cero");
	puts("\nchangeFontSize     Cambiar tamano de letra. Recibe como argumento un unsigned int\n");
}

void helpCommand(){
	showCommands();
}
void timeCommand(){
	getTime();
}
void clearCommand(){
	clearScreen();
}
void golfCommand(){
	pongisInit();
}
void infoRegCommand(){
	getRegisters();
}
void zeroDivisionCommand(){
	zeroDivTest();
}
void invalidOpCodeCommand(){
	InvalidOpCodeTest();
}
void changeFontSizeCommand(){
	puts("Enter new font size: ");
	int size;
	scanf("%d", &size);
	changeFontSize(size);
}
