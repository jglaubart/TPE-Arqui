#ifndef COMMANDS_H
#define COMMANDS_H
// ACA IRIAN LOS COMANDOS

#define MAX_COMMANDS 7


static const char * commands[] = {"help","time","clear","golf","inforeg","zerodiv","invopcode"};
void showCommands();
void helpCommand();
void timeCommand(); 
void clearCommand();
void golfCommand();
void infoRegCommand();
void zeroDivisionCommand();
void invalidOpCodeCommand();  

static void (*commands_ptr[MAX_COMMANDS])() = {helpCommand, timeCommand, clearCommand, golfCommand, infoRegCommand, zeroDivisionCommand, invalidOpCodeCommand};


#endif