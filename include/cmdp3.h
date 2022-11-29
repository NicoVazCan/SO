#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

//Por compatibilidad; se definen en funcP3.c
int Cmd_setuid(char **);
void Cmd_getuid(char **);

extern int errno;

void cmd_getpriority(char *trozos[]);
void cmd_setpriority(char **);
void cmd_fork(char **);
void cmd_execute(char **);
void cmd_executeas(char **);