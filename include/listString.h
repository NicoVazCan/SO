/*Autores:
	Nicolas Vazquez Cancela - Login: nicolas.vazquez.cancela
	Iago Valeiro Castrillon - Login: iago.valeiro
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define STRGMAX 127

struct lista
{
	char stgelem[STRGMAX];
	struct lista *sig;
};

bool crearlis(struct lista **);
bool estaVaciaLis(struct lista *);
struct lista *ultimolis(struct lista *);
char *recivelem(struct lista *);
struct lista *signode(struct lista *);
bool encolarlis(struct lista *, char *);
struct lista *recorrerlis(struct lista *, int , bool);
void freelis(struct lista*, bool);
