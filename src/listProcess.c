#include "listProcess.h"

void crearLisP(stcProc *lis[], int max)
{
	for(int k = 0; k < max; k++) { lis[k] = NULL; }
}

int getPosP(stcProc *lis[], pid_t pid)
{
	int k;

	for(k = 0; lis[k] != NULL && lis[k]->pid != pid; k++);

	return(lis[k] == NULL? -1: k);
}

int lastPosP(stcProc *lis[])
{
	int k;

	for(k = 0; lis[k] != NULL; k++);

	return k-1;
}

void addProc(stcProc *lis[], pid_t pid, char *prog[])
{
	stcProc *proc = malloc(sizeof(stcProc));
	char aux[STGMAX];
	time_t t;
	struct tm *tl;
   time(&t);
	tl = localtime(&t);

	proc->pid = pid;
	
	strcpy(aux, prog[0]);
	for(int k = 1; prog[k] != NULL; k++)
	{
		strcat(aux, " ");
		strcat(aux, prog[k]);
	}
	strcpy(proc->prog, aux);
	strcpy(proc->time, strtok(asctime(tl), "\n"));
	strcpy(proc->stad, "ACTIVO");
	strcpy(proc->rtrn, "000");

	lis[lastPosP(lis)+1] = proc;
}

void removeProc(stcProc *lis[], int pos)
{
	free(lis[pos]);
	for(int k = pos; lis[k] != NULL; k++) { lis[k] = lis[k+1]; }
}

void freeLisP(stcProc *lis[])
{
	for(int k = lastPosP(lis); k >= 0; k--) { removeProc(lis, k); }
}