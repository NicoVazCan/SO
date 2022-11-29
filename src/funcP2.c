/*Autores:
	Nicolas Vazquez Cancela - Login: nicolas.vazquez.cancela
	Iago Valeiro Castrillon - Login: iago.valeiro
*/
#include "funcP2.h"

void Cmd_AllocateMalloc(stcLisM *plis, char *args[])
{
	size_t size;
	void *pMem = NULL;

	if(args[0] == NULL)
	{
		listarLisM(plis, eqtypM, "malloc");
		return;
	}

	if((size = atoi(args[0])) > 0)
	{
		if((pMem = malloc(size)) != NULL)
		{
			encolarLisM(plis, pMem, size, "malloc", "", INULL, INULL);
			printf("Asignados %ld bytes en %p\n", size, pMem);
		}
		else { printf("Imposible obtener memoria con malloc: %s.\n", strerror(errno)); }
	}
	else { printf("No se asignan bloques menores de 1 byte.\n"); }
}

void *ObtenerMemoriaShmget(stcLisM *plis, key_t clave, size_t tam)
{
	void *p;
	int aux, id, flags = 0777;
	struct shmid_ds s;

	if(tam)			/*si tam no es 0 la crea en modo exclusivo */
		flags = flags | IPC_CREAT | IPC_EXCL;
							/*si tam es 0 intenta acceder a una ya creada*/
	if(clave == IPC_PRIVATE) /*no nos vale*/
	{
		errno = EINVAL;
		return NULL;
	}
	if((id = shmget(clave, tam, flags)) == -1)
		return (NULL);
	if((p = shmat(id, NULL, 0)) == (void *) -1)
	{
		aux = errno;	/*si se ha creado y no se puede mapear*/
		if(tam)				/*se borra */
			shmctl(id, IPC_RMID, NULL);
		errno = aux;
		return (NULL);
	}
	shmctl(id, IPC_STAT, &s);

	/* Guardar En Direcciones de Memoria Shared (p, s.shm_segsz, clave.....);*/
	encolarLisM(plis, p, s.shm_segsz, "shared memory", "", INULL, clave);
	return (p);
}

void Cmd_AllocateShared(stcLisM *plis, char *args[])
{
	void *pMem;
	key_t cl;
	if(args[0] == NULL)
	{
		listarLisM(plis, eqtypM, "shared memory");
		return;
	}
	else { cl = atoi(args[0]); }
	if((pMem = ObtenerMemoriaShmget(plis, cl, 0)) == NULL)
	{
		perror("Imposible obtener memoria shmget.");
	}
	else
	{
		printf("Memoria de shmget de clave %d asignada en %p\n", cl, pMem);
	}
}

void Cmd_AllocateCreateShared(stcLisM *plis, char *arg[]) /*arg[0] is the key
																			and arg[1] is the size*/
{
	key_t k;
	size_t tam = 0;
	void *p;

	if(arg[0] == NULL || arg[1] == NULL)
	{/*Listar Direcciones de Memoria Shared */
		listarLisM(plis, eqtypM, "shared memory");
		return;
	}

	k = (key_t) atoi(arg[0]);

	if(arg[1] != NULL)
			tam = (size_t) atoll(arg[1]);
	if((p = ObtenerMemoriaShmget(plis, k, tam)) == NULL)
		perror("Imposible obtener memoria shmget.");
	else
		printf("Memoria de shmget de clave %d asignada en %p\n", k, p);
}

void *MmapFichero(stcLisM *plis, char * fichero, int protection)
{
	int fd, map = MAP_PRIVATE, modo = O_RDONLY;
	struct stat s;
	void *p;

	if(protection & PROT_WRITE)
		modo = O_RDWR;
	if(stat(fichero, &s) == -1 || (fd = open(fichero, modo)) == -1)
		return NULL;
	if((p = mmap(NULL, s.st_size, protection, map, fd, 0)) == MAP_FAILED)
		return NULL;

	/*Guardar Direccion de Mmap (p, s.st_size,fichero,fd......);*/
	encolarLisM(plis, p, s.st_size, "mmap", fichero, fd, INULL);
	return p;
}

void Cmd_AllocateMmap(stcLisM *plis, char *arg[]) /*arg[0] is the file name
															and arg[1] is the permissions*/
{
	char *perm;
	void *p;
	int protection = 0;
	if(arg[0] == NULL)
	{/*Listar Direcciones de Memoria mmap;*/
		listarLisM(plis, eqtypM, "mmap");
		return;
	}
	if((perm = arg[1]) != NULL && strlen(perm) < 4)
	{
		if(strchr(perm,'r')!=NULL) protection |= PROT_READ;
		if(strchr(perm,'w')!=NULL) protection |= PROT_WRITE;
		if(strchr(perm,'x')!=NULL) protection |= PROT_EXEC;
	}
	if((p = MmapFichero(plis, arg[0], protection)) == NULL)
		perror("Imposible mapear fichero.");
	else
		printf("fichero %s mapeado en %p\n", arg[0], p);
}

#define LEERCOMPLETO ((ssize_t)-1)
ssize_t LeerFichero (char *fich, void *p, ssize_t n)
{
	//n=-1 indica que se lea todo
	ssize_t nleidos, tam = n;
	int fd, aux;
	struct stat s;

	if(stat(fich, &s) == -1 || (fd = open(fich, O_RDONLY)) == -1)
		return ((ssize_t) - 1);

	if(n == LEERCOMPLETO)
		tam = (ssize_t) s.st_size;

	if((nleidos = read(fd, p, tam)) == -1)
	{
		aux = errno;
		close(fd);
		errno = aux;
		return ((ssize_t) - 1);
	}
	close(fd);
	return (nleidos);
}

void Cmd_deletekey(char *args[]) /*arg[0] points to a str containing the key*/
{
	key_t clave;
	int id;
	char *key=args[0];
	
	if(key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE)
	{
		printf ("   rmkey clave_valida.\n");
		return;
	}
	if ((id=shmget(clave,0,0666))==-1)
	{
		perror("shmget: imposible obtener memoria compartida.");
		return;
	}
	if (shmctl(id,IPC_RMID,NULL)==-1)
		perror ("shmctl: imposible eliminar memoria compartida.\n");
}

void Cmd_dopmap(char *args[]) /*no arguments necessary*/
{
	pid_t pid;
	char elpid[32];
	char *argv[3] = {"pmap", elpid, NULL};
	
	sprintf(elpid, "%d", (int) getpid());
	if((pid = fork()) == -1)
	{
		perror("Imposible crear proceso.");
		return;
	}
	if(pid == 0)
	{
		if(execvp(argv[0], argv) == -1)
			perror("cannot execute pmap.");
		exit(1);
	}
	waitpid(pid, NULL, 0);
}

void Cmd_Dealloc(stcLisM *plis, void liberar(stcLisM*, stcLisM*), char *args[])
{
	stcLisM *pnodo;
	char *typM;
	void *igual;

	if(liberar == freeM)
	{
		typM = "malloc";
		igual = eqsize;
	}
	else if(liberar == closeFile)
	{
		typM = "mmap";
		igual = eqfName;
	}
	else if(liberar == separarSM)
	{
		typM = "shared memory";
		igual = eqcl;
	}
	else
	{
		printf("Error: tipo de reserva no registrada.\n");
		return;
	}

	if(args[0] == NULL)
	{
		listarLisM(plis, eqtypM, typM);
		return;
	}
	pnodo = buscarLisM(plis, igual, args[0]);

	if(pnodo != NULL)
	{
		liberar(plis, pnodo);
	}
	else { printf("No se encontro ningun %s %s.\n", typM, args[0]); }
}

void Cmd_DeallocAddr(stcLisM *plis, char *args[])
{
	stcLisM *pnodo = buscarLisM(plis, eqpMem, str2pntr(args[0]));

	if(pnodo != NULL)
	{
		freeGeneric(plis, pnodo);
		freeNodo(plis, pnodo);
	}
	else { printf("Direccion %s no asignada con malloc, shared o mmap.\n", args[0]); }
}