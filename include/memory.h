//Fichero auxiliar para ser incluido en el shell.c
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void memdump(char **);
void memfill(char **);
void readfile(char **);
void writefile(char **);
void recursiva(int); //La del enunciado reformateada
void recurse(char **); //El comando en s'i
