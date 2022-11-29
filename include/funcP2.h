#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#include "list.h"

void Cmd_AllocateMalloc(stcLisM*, char *[]);
void Cmd_AllocateShared(stcLisM*, char *[]);
void Cmd_AllocateCreateShared(stcLisM*, char *[]);
void Cmd_AllocateMmap(stcLisM*, char *[]);
void Cmd_deletekey(char*[]);
void Cmd_dopmap(char*[]);
void Cmd_Dealloc(stcLisM*, void liberar(stcLisM*, stcLisM*), char*[]);
void Cmd_DeallocAddr(stcLisM*, char *[]);
#define LEERCOMPLETO ((ssize_t)-1)
ssize_t LeerFichero (char *, void *, ssize_t);
