#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>


#define TMEMAX 25
#define STGMAX 256
#define SIGMAX 11

typedef struct 
{
	pid_t pid;
	char prog[STGMAX];
	char time[TMEMAX];
	char stad[STGMAX];
	char rtrn[STGMAX];
} stcProc;

void crearLisP(stcProc *lis[], int max);
int getPosP(stcProc *lis[], pid_t pid);
int lastPosP(stcProc *lis[]);
void addProc(stcProc *lis[], pid_t pid, char *prog[]);
void removeProc(stcProc *lis[], int pos);
void freeLisP(stcProc *lis[]);