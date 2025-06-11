#ifndef COMMANDS_H
#define COMMANDS_H
// ACA IRIAN LOS COMANDOS

#define MAX_COMMANDS 8


static const char * commands[] = {"help","time","clear","golf","inforeg","zerodiv","invopcode", "changeFontSize"};
void showCommands();
void helpCommand();
void timeCommand(); 
void clearCommand();
void golfCommand();
void infoRegCommand();
void zeroDivisionCommand();
void invalidOpCodeCommand();  
void changeFontSizeCommand();

static void (*commands_ptr[MAX_COMMANDS])() = {helpCommand, timeCommand, clearCommand, golfCommand, infoRegCommand, zeroDivisionCommand, invalidOpCodeCommand, changeFontSizeCommand};


#endif