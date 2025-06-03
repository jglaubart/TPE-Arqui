#include <shell.h>
#include <commands.h>
#include <usr_stdlib.h>
// IMPLEMENTAION DE LOS COMANDOS
void showCommands(){
	puts("\n Comandos disponibles:");
	puts("\nclear              Resetea la shell a su estado original");
	puts("\ngolf         	   Inicia el juego de Pongis Golf");
	puts("\ninforeg            Imprime los valores de los registros (CTRL + R es necesario previamente)");
	puts("\ntime               Imprime la hora del sistema en pantalla");
	puts("\ninvopcode          Prueba la excepcion de codigo de operacion invalido");
	puts("\nzerodiv            Prueba de excepcion de division por cero\n");
}

void helpCommand(){
	showCommands();
}
void timeCommand(){
	puts("Comando no implementado\n");
}
void clearCommand(){
	clearScreen();
}
void golfCommand(){
	puts("Comando no implementado\n");
}
void infoRegCommand(){
	puts("Comando no implementado\n");
}
void zeroDivisionCommand(){
	zeroDivTest();
}
void invalidOpCodeCommand(){
	InvalidOpCodeTest();
}
