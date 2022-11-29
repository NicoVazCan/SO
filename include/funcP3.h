#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "listProcess.h"

#define STGMAX 256

extern int errno;

//Señales
struct SEN
{
	char *nombre;
	int senal;
};

char *NombreUsuario(uid_t);
uid_t UidUsuario(char *);
void Cmd_getuid(char **);
int Cmd_setuid(char **);
void Cmd_background(stcProc *lis[], char *tr[]);
void Cmd_proc(stcProc *lis[], char *tr[]);
void Cmd_listprocs(stcProc *lis[]);
void Cmd_deleteprocs(stcProc *lis[], char *tr[]);
void Cmd_foreground(char *tr[]);
void Cmd_anyCommand(stcProc *lis[], char *tr[]);
void Cmd_Run_as(stcProc *lis[], char *tr[]);