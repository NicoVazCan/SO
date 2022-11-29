#include "cmdp3.h"

void cmd_getpriority(char *trozos[])
{
	pid_t pid;
	int pri;
	
	if(trozos[1] != NULL)
		pid = (pid_t) strtoul(trozos[1], NULL, 10);
	else pid = getpid();

	errno = 0;
	pri = getpriority(PRIO_PROCESS, pid);
	//getpriority puede devolver -1; v'ease el manual
	if((errno != 0) && (pri != -1))
		perror("getpriority");
	printf("%d\n", pri);
}

void cmd_setpriority(char *trozos[])
{
	char uso[] = "Uso: setpriority [pid] [value]";
	int ntr;
	pid_t pid = getpid();
	int pri;
	
	for(ntr = 0; trozos[ntr+1] != NULL; ntr++);
	errno = 0;
	switch(ntr)
	{
		case 0:
			pri = getpriority(PRIO_PROCESS, pid);
			if((errno == -1) && (pri != -1))
				perror("getpriority");
			printf("%d\n", pri);
			return;
			break;
		case 1:
			pri = (int) strtoul(trozos[1], NULL, 10);
			break;
		case 2:
			pid = (pid_t) strtoul(trozos[1], NULL, 10);
			pri = (int) strtoul(trozos[2], NULL, 10);
			break;
		default:
			fprintf(stderr, "%s\n", uso);
			return;
			break;
	}

	if(setpriority(PRIO_PROCESS, pid, pri) < 0)
		perror("setpriority");
}

void cmd_fork(char *trozos[])
{
	int nargs; for(nargs = 0; trozos[nargs] != NULL; nargs++);
	int wstatus; int p;

	p = fork();
	if(p == 0)
		{ if(nargs <= 1) Cmd_getuid(trozos); }
	else if(p == -1) perror("fork: fork");
	else if(wait(&wstatus) == -1) perror("fork: wait");
}

void cmd_execute(char *trozos[])
{
	char uso[] = "Uso: execute prog arg1 arg2... [@pri]";

	if(trozos[1] == NULL)
		{ fprintf(stderr, "%s\n", uso); return; }
	for(int i = 0; trozos[i] != NULL; i++)
	{
		if((trozos[i])[0] == '@')
		{
			char *trsp[] = { "setpriority", (trozos[i])+1, NULL };
			cmd_setpriority(trsp);
			trozos[i] = NULL;
		}
	}

	execvp(trozos[1], trozos + 1);
	perror("execute");
}

void cmd_executeas(char *trozos[])
{
	char uso[] = "Uso: execute-as login prog arg1 arg2... [@pri]";
	char *trsetuid[] = { "-l", NULL, NULL };
	
	if(trozos[1] == NULL)
		{ fprintf(stderr, "%s\n", uso); return; }
	else if(trozos[2] == NULL)
		{ fprintf(stderr, "%s\n", uso); return; }
	trsetuid[1] = trozos[1];

	if(Cmd_setuid(trsetuid) < 0) return;
	cmd_execute(trozos + 1);
}