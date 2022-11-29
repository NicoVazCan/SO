#include "funcP3.h"

static struct SEN sigstrnum[] =
{
	{"HUP", SIGHUP},
	{"INT", SIGINT},
	{"QUIT", SIGQUIT},
	{"ILL", SIGILL},
	{"TRAP", SIGTRAP},
	{"ABRT", SIGABRT},
	{"IOT", SIGIOT},
	{"BUS", SIGBUS},
	{"FPE", SIGFPE},
	{"KILL", SIGKILL},
	{"USR1", SIGUSR1},
	{"SEGV", SIGSEGV},
	{"USR2", SIGUSR2},
	{"PIPE", SIGPIPE},
	{"ALRM", SIGALRM},
	{"TERM", SIGTERM},
	{"CHLD", SIGCHLD},
	{"CONT", SIGCONT},
	{"STOP", SIGSTOP},
	{"TSTP", SIGTSTP},
	{"TTIN", SIGTTIN},
	{"TTOU", SIGTTOU},
	{"URG", SIGURG},
	{"XCPU", SIGXCPU},
	{"XFSZ", SIGXFSZ},
	{"VTALRM", SIGVTALRM},
	{"PROF", SIGPROF},
	{"WINCH", SIGWINCH},
	{"IO", SIGIO},
	{"SYS", SIGSYS},
	/*senales que no hay en todas partes*/
	#ifdef SIGPOLL
		{"POLL", SIGPOLL},
	#endif
	#ifdef SIGPWR
		{"PWR", SIGPWR},
	#endif
	#ifdef SIGEMT
		{"EMT", SIGEMT},
	#endif
	#ifdef SIGINFO
		{"INFO", SIGINFO},
	#endif
	#ifdef SIGSTKFLT
		{"STKFLT", SIGSTKFLT},
	#endif
	#ifdef SIGCLD
		{"CLD", SIGCLD},
	#endif
	#ifdef SIGLOST
		{"LOST", SIGLOST},
	#endif
	#ifdef SIGCANCEL
		{"CANCEL", SIGCANCEL},
	#endif
	#ifdef SIGTHAW
		{"THAW", SIGTHAW},
	#endif
	#ifdef SIGFREEZE
		{"FREEZE", SIGFREEZE},
	#endif
	#ifdef SIGLWP
		{"LWP", SIGLWP},
	#endif
	#ifdef SIGWAITING
		{"WAITING", SIGWAITING},
	#endif
	{NULL,-1},
};

char *NombreUsuario(uid_t uid)
{
	struct passwd *p;

	if((p = getpwuid(uid)) == NULL)
	{
		return (" ??????");
	}
	else { return p->pw_name; }
}

uid_t UidUsuario(char *nombre)
{
	struct passwd *p;

	if((p = getpwnam(nombre)) == NULL)
	{
		return (uid_t) -1;
	}
	else { return p->pw_uid; }
}

void Cmd_getuid(char *tr[])
{
	uid_t real=getuid(), efec=geteuid();

	printf ("Credencial real: %d, (%s)\n", real, NombreUsuario (real));
	printf ("Credencial efectiva: %d, (%s)\n", efec, NombreUsuario (efec));
}

//Tipo de retorno modificado para comprobar errores en cmd_executeas
int Cmd_setuid(char *tr[])
{
	uid_t uid;
	int u;

	if(tr[0]==NULL || (!strcmp(tr[0],"-l") && tr[1]==NULL))
	{
		Cmd_getuid(tr);
		return -1;
	}
	if(!strcmp(tr[0],"-l"))
	{
		if((uid = UidUsuario(tr[1])) == (uid_t) -1)
		{
			printf ("Usuario no existente %s\n", tr[1]);
			return -1;
		}
	}
	else if((uid=(uid_t) ((u=atoi (tr[0]))<0)? -1: u) == (uid_t) -1)
	{
		printf ("Valor no valido de la credencial %s\n",tr[0]);
		return -1;
	}
	if(setuid (uid) == -1)
	{
		printf ("Imposible cambiar credencial: %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

//Señales
int Senal(char *sen)
/* devuelve el n'umero de señal a partir del nombre */
{
	int i;
	for (i=0; sigstrnum[i].nombre != NULL; i++)
		if (!strcmp(sen, sigstrnum[i].nombre))
		return sigstrnum[i].senal;
	return -1;
}

/* devuelve el nombre de la señal a partir de la señal */
/* para sitios donde no hay sig2str*/
char *NombreSenal(int sen)
{
	int i;
	for (i=0; sigstrnum[i].nombre != NULL; i++)
		if (sen==sigstrnum[i].senal)
			return sigstrnum[i].nombre;
	return ("SIGUNKNOWN");
}

int prioPos(char *tr[])
{
	for(int k = 0; tr[k] != NULL; k++)
	{
		if(tr[k][0] == '@') { return k; }
	}
	return -1;
}

void Cmd_background(stcProc *lis[], char *tr[])
{
	pid_t chld;
	int posA = prioPos(tr);

	if(tr[0] == NULL)
	{
		printf("Debe introducir un programa.\n");
		return;
	}
	chld = fork();
	switch(chld)
	{
		case 0:
			if(posA != -1) { tr[posA] = NULL; }
			execvp(tr[0], tr);
			perror("Imposible ejecutar programa");
			exit(255);
			break;

		case -1:
			perror("Imposible crear proceso");
			break;

		default:
			if(posA != -1)
			{
				if(setpriority(PRIO_PROCESS, chld, atoi(&tr[posA][1])) == -1)
				{
					perror("Imposible cambiar la prioridad.\n");;
				}
			}
			addProc(lis, chld, tr);
	}
}

void getInfo(stcProc *proc, int espe)
{
	int state;

	if(waitpid(proc->pid, &state, 
	           (espe & WNOHANG) | 
	           WUNTRACED | 
	           WCONTINUED) == proc->pid)
	{
		if(WIFEXITED(state))
		{
			strcpy(proc->stad, "TERMINADO");
			sprintf(proc->rtrn, "%03d", WEXITSTATUS(state));
		}
		else if(WIFSIGNALED(state))
		{
			strcpy(proc->stad, "SENALADO");
			strcpy(proc->rtrn, NombreSenal(WTERMSIG(state)));
		}
		else if(WIFSTOPPED(state))
		{
			strcpy(proc->stad, "ESPERANDO");
			strcpy(proc->rtrn, NombreSenal(WSTOPSIG(state)));
		}
		else 
		{
			strcpy(proc->stad, "ACTIVO");
			strcpy(proc->rtrn, "000");
		}
	}
	printf("%d p=%d %s %s (%s) %s\n", proc->pid,
	       getpriority(PRIO_PROCESS, proc->pid),
	       proc->time, proc->stad, proc->rtrn, proc->prog);
}

void Cmd_proc(stcProc *lis[], char *tr[])
{
	int pos, fg;

	if(tr[0] == NULL) { Cmd_listprocs(lis); }
	else 
	{
		fg = strcmp(tr[0], "-fg") == 0;
		if(fg && tr[2] == NULL) 
		{
			printf("Debe introducir la id del proceso.\n");
			return;
		}
		pos = getPosP(lis, atoi(tr[fg]));

		if(pos != -1)
		{
			getInfo(lis[pos], !fg);

			if(fg) { removeProc(lis, pos); }
		}
		else { printf("Proceso %s no encontrado.\n", tr[fg]); }
	}
}

void Cmd_listprocs(stcProc *lis[])
{
	for (int k = 0; lis[k] != NULL; k++)
	{
		getInfo(lis[k], WNOHANG);
	}
}

void Cmd_deleteprocs(stcProc *lis[], char *tr[])
{
	char *stad;

	if(tr[0] == NULL)
	{
		printf("Opciones: -term, -sig.\n");
		return;
	}
	if(strcmp(tr[0], "-term") == 0)
	{
		stad = "TERMINADO";
	}
	else if(strcmp(tr[0], "-sig") == 0)
	{
		stad = "SENALADO";
	}
	else
	{
		printf("Opciones: -term, -sig.\n");
		return;
	}
	for(int k = lastPosP(lis); k >= 0; k--)
	{
		if(strcmp(lis[k]->stad, stad) == 0)
		{
			removeProc(lis, k);
		}
	}
}

void Cmd_foreground(char *tr[])
{
	pid_t chld;
	int posA = prioPos(tr);

	if(tr[0] == NULL)
	{
		printf("Debe introducir un programa.\n");
		return;
	}
	chld = fork();
	switch(chld)
	{
		case 0:
			if(posA != -1) { tr[posA] = NULL; }
			execvp(tr[0], tr);
			perror("Imposible ejecutar programa");
			exit(255);
			break;

		case -1:
			perror("Imposible crear proceso");
			break;

		default:
			if(posA != -1)
			{
				if(setpriority(PRIO_PROCESS, chld, atoi(&tr[posA][1])))
				{
					perror("Imposible cambiar la prioridad.\n");
				}
			}
			waitpid(chld, NULL, 0);
	}
}

void Cmd_anyCommand(stcProc *lis[], char *tr[])
{
	int posY, bAnd = 0;

	for(posY = 0; tr[posY] != NULL && !bAnd; posY++)
	{
		bAnd = strcmp(tr[posY], "&") == 0;
	}
	if(bAnd)
	{
		tr[posY-1] = NULL;
		Cmd_background(lis, tr);
	}
	else { Cmd_foreground(tr); }
}

void Cmd_Run_as(stcProc *lis[], char *tr[])
{
	int posY, posA, bAnd = 0;
	pid_t chld;
	char *trUid[] = {"-l", tr[0], NULL};

	if(tr[0] == NULL || tr[1] == NULL)
	{
		printf("Debe introducir un usuario y un programa.\n");
	}
	else
	{
		posA = prioPos(tr);
		for(posY = 0; tr[posY] != NULL && !bAnd; posY++)
		{
			bAnd = strcmp(tr[posY], "&") == 0;
		}
		if(bAnd) { tr[posY-1] = NULL; }
		chld = fork();

		switch(chld)
		{
			case 0:
				if(posA != -1) { tr[posA] = NULL; }

				if(Cmd_setuid(trUid) == 0) { execvp(tr[1], &tr[1]); }

				perror("Imposible ejecutar programa");
				exit(255);
				break;

			case -1:
				perror("Imposible crear proceso");
				break;

			default:
				if(posA != -1)
				{
					if(setpriority(PRIO_PROCESS, chld, atoi(&tr[posA][1])) == -1)
					{
						perror("Imposible cambiar la prioridad.\n");;
					}
				}
				if(bAnd)
				{
					addProc(lis, chld, &tr[1]);
				}
				else { waitpid(chld, NULL, 0); }
		}
	}
}