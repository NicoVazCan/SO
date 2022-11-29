/*Autores:
	Nicolas Vazquez Cancela - Login: nicolas.vazquez.cancela
	Iago Valeiro Castrillon - Login: iago.valeiro
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define INULL -1
#define STYMAX 14
#define TMEMAX 25

typedef struct aux
{
	void *pMem;
	size_t size;
	char typM[STYMAX];
	//Opcional
	char fName[32];	//mmap
	int fd;				//mmap
	key_t cl;			//share

	char time[TMEMAX];

	struct aux *sig;
}
stcLisM;

void *str2pntr(char*);
bool vaciaLisM(stcLisM*);
bool crearLisM(stcLisM**);
stcLisM *ultimoLisM(stcLisM*);
void encolarLisM(stcLisM*, void*, size_t, char*, char*, int, key_t);
void freeNodo(stcLisM*, stcLisM*);
void freeM(stcLisM*, stcLisM*);
void separarSM(stcLisM*, stcLisM*);
void closeFile(stcLisM*, stcLisM*);
void freeGeneric(stcLisM*, stcLisM*);
void freelisM(stcLisM*);
stcLisM *buscarLisM(stcLisM*, bool igual(stcLisM*, char*), char*);
void listarLisM(stcLisM*, bool igual(stcLisM*, char*), char*);
bool eqpMem(stcLisM*, char*);
bool eqsize(stcLisM*, char*);
bool eqfName(stcLisM*, char*);
bool eqcl(stcLisM*, char*);
bool eqtypM(stcLisM*, char*);
