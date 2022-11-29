/*Autores:
	Nicolas Vazquez Cancela - Login: nicolas.vazquez.cancela
	Iago Valeiro Castrillon - Login: iago.valeiro
*/
#include "list.h"

void *str2pntr(char *num)
{
	return (void*)strtol(num, NULL, 16);
}

int buscarCadenaM(char *entrada, char *trozos[], int max)
{
	if(entrada != NULL)
	{
		for(int k = 0; k < max; k++)
		{
			if(strcmp(trozos[k], entrada) == 0)
			{
				return k;
			}
		}
	}
	return INULL;
}

bool vaciaLisM(stcLisM *plis)
{
	return(plis->sig == NULL);
}

bool crearLisM(stcLisM **plis)
{
	if((*plis = malloc(sizeof(stcLisM))) != NULL)
	{
		(*plis)->pMem = plis;
		(*plis)->size = sizeof(stcLisM);
		strcpy((*plis)->typM, "Cabeza");
		strcpy((*plis)->fName, "Cabeza");
		(*plis)->fd = (*plis)->cl = INULL;

		(*plis)->sig = NULL;

		return true;
	}
	else
	{
		return false;
	}
}

stcLisM *ultimoLisM(stcLisM *plis)
{
	stcLisM *paux = NULL;
	for(paux = plis; paux->sig != NULL; paux = paux->sig);

	return paux;
}

void encolarLisM(stcLisM *plis, void *pMem, size_t size, 
                 char *typM, char *fName, int fd, key_t cl)
{
	stcLisM *pult = ultimoLisM(plis), *paux;
	time_t t;
	struct tm *tl;

   time(&t);
	tl = localtime(&t);

	if((paux = malloc(sizeof(stcLisM))) != NULL)
	{
		paux->pMem = pMem;
		paux->size = size;
		strcpy(paux->typM, typM);
		strcpy(paux->time, asctime(tl));
		strcpy(paux->fName, fName);
		paux->fd = fd;
		paux->cl = cl;
		paux->sig = NULL;

		pult->sig = paux;
	}
	else
	{
		printf("Imposible registrar reserva de memoria en la lista:  %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void freeNodo(stcLisM *plis, stcLisM *pnodo)
{
	stcLisM *pant;
	for(pant = plis; pant != NULL && pant->sig != pnodo; pant = pant->sig);

	if(pant != NULL)
	{
		pant->sig = pnodo->sig;
		pnodo->sig = NULL;

		free(pnodo);
	}
}

void freeM(stcLisM *plis, stcLisM *pnodo)
{
	free(pnodo->pMem);
	freeNodo(plis, pnodo);
}

void separarSM(stcLisM *plis, stcLisM *pnodo)
{
	if(shmdt(pnodo->pMem) != -1)
	{
		freeNodo(plis, pnodo);
	}
	else { printf("Error al separar clave %d: %s\n", pnodo->cl, strerror(errno)); }
}

void closeFile(stcLisM *plis, stcLisM *pnodo)
{
	if(munmap(pnodo->pMem, pnodo->size) == 0 && close(pnodo->fd) == 0)
	{
		freeNodo(plis, pnodo);
	}
	else{ printf("Error al desmapear %s: %s.\n", pnodo->fName, strerror(errno)); }
}

void freeGeneric(stcLisM *plis, stcLisM *pnodo)
{
	char *typesM[3] = {"malloc", "mmap", "shared memory"};

	switch(buscarCadenaM(pnodo->typM, typesM, 3))
	{
		case 0:	//malloc
			freeM(plis, pnodo);
			break;

		case 1:	//mmap
			closeFile(plis, pnodo);
			break;

		case 2:	//shared memory
			separarSM(plis, pnodo);
			break;

		default:
			printf("Tipo de reserva %s no registrado.\n", pnodo->typM);
			freeNodo(plis, pnodo);
	}
}

void freelisM(stcLisM *plis)
{
	stcLisM *pnodo = NULL;

	while(!vaciaLisM(plis))
	{
		pnodo = plis->sig;

		freeGeneric(plis, pnodo);
	}
	free(plis);
}

stcLisM *buscarLisM(stcLisM *plis, bool igual(stcLisM*, char*), char *elem)
{
	stcLisM *paux;

	for(paux = plis; paux != NULL && !igual(paux, elem); paux = paux->sig);

	return paux;
}

void listarLisM(stcLisM *plis, bool igual(stcLisM*, char*), char *elem)
{
	stcLisM *paux;
	if(!vaciaLisM(plis))
	{
		for(paux = plis->sig; paux != NULL; paux = paux->sig)
		{
			if(elem == NULL || igual(paux, elem))
			{
				printf("%p: %ld bytes %s ", paux->pMem, paux->size, paux->typM);
				if(paux->fName[0] != '\0')
				{
					printf("%s (fd: %d) ", paux->fName, paux->fd);
				}
				else if(paux->cl != INULL)
				{
					printf("(key: %d) ", paux->cl);
				}
				printf("%s", paux->time);
			}
		}
	}
}

bool eqpMem(stcLisM *pnodo, char *pMem)
{
	return(pnodo->pMem == pMem);
}

bool eqsize(stcLisM *pnodo, char *sname)
{
	size_t size = atoll(sname);

	return (pnodo->size == size);
}

bool eqfName(stcLisM *pnodo, char *fich)
{
	return(strcmp(pnodo->fName, fich) == 0);
}

bool eqcl(stcLisM *pnodo, char *shmid)
{
	key_t cl = atoi(shmid);

	return(pnodo->cl == cl);
}

bool eqtypM(stcLisM *pnodo, char *typM)
{
	return(strcmp(pnodo->typM, typM) == 0);
}