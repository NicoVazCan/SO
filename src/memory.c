//Fichero auxiliar para ser incluido en el shell.c
#include "memory.h"

void memdump(char *trozos[])
{	
	bool trozfin = false; //indica si acabamos de leer los trozos
	void *addr; int cont;
	char *cur;
	char uso[] = "Uso: memdump addr [cont]";

	if(trozos[1] == NULL || trozfin) 
		{ printf("%s\n", uso); return; }
	else addr = (void *) strtoul(trozos[1], NULL, 16);
	if(trozos[2] == NULL || trozfin) 
		{ cont = 25; trozfin = true; }
	else cont = strtoul(trozos[2], NULL, 10);
	

	for(int n = 0; n < cont; n += 25)
	{
		cur = (void *) addr + n;
		putchar(' ');
		for(int i = 0; (i < 25) && (i+n < cont); i++)
		{
			//Comprobamos la imprimibilidad
			printf("%c  ", *(cur+i) < ' ' || *(cur+i) > '~' ? 
			' ' : *(cur+i));
		}
		putchar('\n');
		for(int i = 0; (i < 25) && (i+n < cont); i++)
			printf("%.2hhX ", *(cur+i));
		putchar('\n');
	}
}

void memfill(char *trozos[])
{
	bool trozfin = false;
	void *addr = NULL; int cont; char byte;
	char *cur = addr;
	char uso[] = "Uso: memfill addr [cont] [byte]";

	if(trozos[1] == NULL || trozfin) 
		{ printf("%s\n", uso); return; }
	else  addr = (void *) strtoul(trozos[1], NULL, 16);
	if(trozos[2] == NULL || trozfin) 
		{ cont = 128; trozfin = true; }
	else cont = strtoul(trozos[2], NULL, 10);
	if(trozos[3] == NULL || trozfin) 
		{ byte = 'A'; trozfin = true; }
	else byte = strtoul(trozos[3], NULL, 16);
	

	cur = addr;
	for(int i = 0; i < cont; i++)
		*(cur+i) = byte;
}

void readfile(char *trozos[])
{
	int fich; void *addr = NULL; size_t cont;
	char uso[] = "Uso: readfile fich addr, cont";
	struct stat s;
	
	if(trozos[1] == NULL) 
		{ printf("%s\n", uso); return; }
	else fich = open(trozos[1], O_RDWR);
	if(trozos[2] == NULL) 
		{ printf("%s\n", uso); return; }
	else sscanf(trozos[2], "%i", (int *) addr);
	if(trozos[3] == NULL)
		{ fstat(fich, &s); cont = s.st_size; }
	else cont =atoi(trozos[3]);
	
	read(fich, addr, cont);
	close(fich);
}

void writefile(char *trozos[])
{
	char **trozo = trozos;
	bool overwrite = false;
	int oflags = O_RDWR | O_CREAT;
	int operms = S_IRUSR | S_IWUSR;
	int fich; void *addr; size_t cont;
	char uso[] = "Uso: writefile [-o] fich addr cont";
	struct stat s;
	
	if(trozos[1] != NULL)
	{ 
		if(strcmp(trozos[1], "-o") == 0)
			{ overwrite = true; trozo++; }
	}
	if(trozo[1] == NULL) 
		{ printf("%s\n", uso); return; }
	else 
	{
		if((stat(trozo[1], &s) == 0) && !overwrite)
			{ perror("writefile"); return; }
		else fich = open(trozo[1], oflags, operms);
	}
	if(trozo[2] == NULL) 
		{ printf("%s\n", uso); return; }
	else addr = (void *) strtoul(trozo[2], NULL, 16);
	if(trozo[3] == NULL)
		{ fstat(fich, &s); cont = s.st_size; }
	else cont = atoi(trozo[3]);
	
	write(fich, addr, cont);
	close(fich);
}

//La del enunciado reformateada
void recursiva(int n)
{
	char automatico[4096];
	static char estatico[4096];

	printf ("parametro n:%d en %p\n",n,&n);
	printf ("array estatico en:%p \n",estatico);
	printf ("array automatico en %p\n",automatico);
	
	n--; if (n>0) recursiva(n);
}

//El comando en s'i
void recurse(char *trozos[])
{
	if(trozos[1] != NULL)
		recursiva(atoi(trozos[1]));
}
