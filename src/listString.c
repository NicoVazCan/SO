/*Autores:
	Nicolas Vazquez Cancela - Login: nicolas.vazquez.cancela
	Iago Valeiro Castrillon - Login: iago.valeiro
*/
#include "listString.h"

bool crearlis(struct lista **plis)
{
	if((*plis = malloc(sizeof(struct lista))) != NULL)
	{
		strcpy((*plis)->stgelem, "Cabeza");
		(*plis)->sig = NULL;

		return true;
	}
	else
	{
		return false;
	}
}

bool estaVaciaLis(struct lista *plis)
{
	return plis == NULL;
}

char *recivelem(struct lista *pnod)
{
	if(pnod != NULL)
	{
		return pnod->stgelem;;
	}
	else
	{
		return NULL;
	}
}

struct lista *signode(struct lista *pnod)
{
	if(pnod != NULL)
	{
		return pnod->sig;
	}
	else
	{
		return NULL;
	}
}

struct lista *ultimolis(struct lista *plis)
{
	struct lista *paux = NULL;
	for(paux = plis; paux->sig != NULL; paux = paux->sig);

	return paux;
}

bool encolarlis(struct lista *plis, char *stgaux)
{
	struct lista *pultimo = ultimolis(plis), *paux;

	if((paux = malloc(sizeof(struct lista))) != NULL)
	{
		strcpy(paux->stgelem, stgaux);
		paux->sig = NULL;
		pultimo->sig = paux;
		return true;
	}
	else
	{
		return false;
	}
}

struct lista *recorrerlis(struct lista *plis, int nstgelems, bool bimprimir)
{
	int k = 1;
	struct lista *paux = plis->sig;
	bimprimir? 0: nstgelems--;

	while(paux != NULL && (nstgelems == -1 || k <= nstgelems))
	{
		if(bimprimir)
		{
			printf("%d->%s\n", k, paux->stgelem);
		}
		paux = paux->sig;
		k++;
	}
	return paux;
}

void freelis(struct lista *lis, bool bfin)
{
	struct lista *aux = NULL, *cur = NULL;
	if(!bfin)
	{
		cur = lis->sig;
		lis->sig = NULL;
	}
	else
	{
		cur = lis;
	}
	while(cur != NULL)
	{
		aux = cur->sig;
		cur->sig = NULL;
		free(cur);
		cur = aux;
	}
}