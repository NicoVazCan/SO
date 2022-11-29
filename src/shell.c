/*Autores:
	Nicolas Vazquez Cancela - Login: nicolas.vazquez.cancela
	Iago Valeiro Castrillon - Login: iago.valeiro
*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>


#define STRGMAX 127
#define ARGUMAX 10
#define COMAMAX 33
#define TEMPMAX 9
#define OPCIMAX 4
#define MARGMAX 7
#define SMRGMAX 5
#define LISPMAX 1024

#define AUTHORS  	0
#define GETPID 	1
#define GETPPID 	2
#define PWD 		3
#define CHDIR 		4
#define DATE 		5
#define TIME 		6
#define HISTORIC 	7
#define QUIT 		8
#define END 		9
#define EXIT 		10
#define CREATE 	11
#define DELETE 	12
#define LIST 		13
#define MEMORY 	14
#define MEMDUMP 	15
#define MEMFILL 	16
#define RECURSE 	17
#define READFILE 	18
#define WRITEFILE 19

//Comandos P3
#define GETPRIORITY	20
#define SETPRIORITY	21
#define GETUID		22
#define SETUID		23
#define FORK		24
#define EXECUTE		25
#define FOREGROUND	26
#define BACKGROUND	27
#define RUNAS		28
#define EXECUTAS	29
#define LISTPROCS	30
#define PROC		31
#define DELETEPROCS	32

#define ALLOCATE 	0
#define DEALLOC 	1
#define DELETEKEY 2
#define SHOW 		3
#define SHOW_VARS	4
#define SHOW_FUNCS 5
#define DOPMAP 	6

#define MALLOC 	0
#define MMAP 		1
#define SHARED 	2
#define CRTSHARED 3
#define ALL 		4




extern int errno;

int trocearCadena(char *, char *[]);
int buscarCadena(char *, char *[], int);
bool tieneAlfanum(char *);

int elimruta(char **, bool);
struct dirent* leerdir(DIR *); //Como readdir, pero se salta "." y ".."
void elimdirrec(char *);
char letraTF(mode_t m);
char *convierteModo(mode_t, char *);
void filDesc(char *, bool);
bool esdir(char *);
void create(char *[]);
void listArch(char *, bool []);
void authors(char *);
char *cwd(char *);
void create(char *[]);
void list(char *[]);

//Funciones para la lista del historico de comandos
#include "listString.h"
//Funciones para el comando memory
#include "funcP2.h"
//Funciones para el resto de comandos de la P2
#include "memory.h"
//Funciones y datos de la P3
#include "funcP3.h"
//Comandos de la P3
#include "cmdp3.h"

int trocearCadena(char *entrada, char *trozos[])
{
	int k = 1;
	if((trozos[0] = strtok(entrada, " \n\t")) == NULL)
	{
		return 0;
	}
	else
	{
		while((trozos[k] = strtok(NULL, " \n\t")) != NULL)
		{
			k++;
		}
		return k;
	}
}

int buscarCadena(char *entrada, char *trozos[], int max)
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
	return -1;
}

bool tieneAlfanum(char *entrada)
{
	for(int k = 0; k < STRGMAX; k++)
	{
		if(isalnum(entrada[k]))
		{
			return true;
		}
		else if(entrada[k] == '\n')
		{
			return false;
		}
	}
	return false;
}



int elimruta(char **argums, bool esrec)
{
	int nerr;
	int argc = 0; for(char** pa = argums; *pa != NULL; pa++) argc++;
	char *args[argc]; for(int i = 0; i <= argc-1; i++) args[i] = argums[i]; //Por si hay que modificarlos, puede que sea redundante
	struct stat stats[argc];
	bool esdir[argc];
	bool esescrible[argc];
	DIR* dir;
	for(int i = 0; i <= argc-1; i++)
	{
		if((lstat(args[i], stats+i) < 0) && ((stats[i].st_mode & S_IFMT) != S_IFLNK)) 
			{ perror("Error: elimruta: stat"); nerr++; }
		if((stats[i].st_mode & S_IFMT) == S_IFDIR) esdir[i] = true;
		else esdir[i] = false;
		//Escritura del usuario y sticky bit
		if((stats[i].st_mode & S_IWUSR) && 
			(!(stats[i].st_mode & S_ISVTX))) esescrible[i] = true;
		else esescrible[i] = false;
	}

	for(int i = 0; i <= argc-1; i++)
	{
		if(esescrible[i])
		{
		if(!esdir[i]) { if(unlink(args[i]) < 0) perror("Error: unlink: "); }
		else
		{
			if((dir = opendir(args[i])) == NULL) { perror("Error: opendir"); nerr++; }
			struct dirent* e = leerdir(dir);
			if(esrec) elimdirrec(args[i]);
			else if(e == NULL) rmdir(args[i]);
			closedir(dir);
		}
		}
		else { fprintf(stderr, "Error: Eliminar: %s: Operacion no permitida\n", args[i]); nerr++; }
	}
	

	return nerr;
}
struct dirent* leerdir(DIR *dir)
{
	struct dirent* e = readdir(dir);
	bool finbucle = false;
	while(e != NULL && !finbucle)
	{	
		if(strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) 
			e = readdir(dir);
		else finbucle = true;
	}
	return e;
}

void elimdirrec(char* ruta)
{
	DIR *dir = opendir(ruta);
	if(dir == NULL)
		{ perror("Error: elimdirrec: opendir"); return; }
	char cwdi[STRGMAX]; getcwd(cwdi, STRGMAX);
	chdir((const char *) ruta);

	for(struct dirent* e = leerdir(dir); e != NULL; e = leerdir(dir))
	{
		char arg[strlen(e->d_name)]; strcpy(arg, e->d_name);
		struct stat se; lstat(arg, &se);
		
		if(esdir(e->d_name)) elimdirrec(arg);
		else if( ((se.st_mode & S_IFMT) == S_IFREG)
		|| ((se.st_mode & S_IFMT) == S_IFLNK) ) unlink(arg);
	}
	chdir(cwdi);
	closedir(dir);
	rmdir(ruta);
}

char letraTF(mode_t m)
{
	switch (m&S_IFMT)
	{ 					//and bit a bit con los bits de formato,0170000
		case S_IFSOCK: 
			return 's'; //socket
		case S_IFLNK:
			return 'l';	//symbolic link
		case S_IFREG:
			return '-';	//fichero normal
		case S_IFBLK:
			return 'b';	//block device
		case S_IFDIR:
			return 'd';	//directorio 
		case S_IFCHR:
			return 'c';	//char device
		case S_IFIFO:
			return 'p';	//pipe
		default: 
			return '?';	//desconocido, no deberia aparecer
	}
}

char *convierteModo(mode_t m, char *permisos)
{
	strcpy(permisos,"---------- ");
	permisos[0] = letraTF(m);

	if (m&S_IRUSR) permisos[1] = 'r'; //propietario
	if (m&S_IWUSR) permisos[2] = 'w';
	if (m&S_IXUSR) permisos[3] = 'x';
	if (m&S_IRGRP) permisos[4] = 'r'; //grupo
	if (m&S_IWGRP) permisos[5] = 'w';
	if (m&S_IXGRP) permisos[6] = 'x';
	if (m&S_IROTH) permisos[7] = 'r';	//resto
	if (m&S_IWOTH) permisos[8] = 'w';
	if (m&S_IXOTH) permisos[9] = 'x';
	if (m&S_ISUID) permisos[3] = 's';	//setuid, setgid y stickybit
	if (m&S_ISGID) permisos[6] = 's';
	if (m&S_ISVTX) permisos[9] = 't';

	return permisos;
}

char *cwd(char *buff)
{
	if(getcwd(buff, sizeof(char[STRGMAX])) != NULL)
	{
		return buff;
	}
	else
	{
		printf("Imposible cargar directorio actual: %s.", strerror(errno));
		exit(EXIT_FAILURE);

		return NULL;
	}
}

void filDesc(char *file, bool larga)
{
	struct stat filstat;
	mode_t m;
	char fechmod[16], permis[12];
	time_t t;
	struct tm lt;
	struct passwd *pwd;
	struct group *grp;
	char link[STRGMAX];

	if(file != NULL)
	{
		if(lstat(file, &filstat) != -1)
		{
			if(larga)
			{
				m = filstat.st_mode;
				t = filstat.st_mtime;
				localtime_r(&t, &lt);
				strftime(fechmod, sizeof fechmod, "%a %d %b %Y", &lt);
				pwd = getpwuid(filstat.st_uid);
				grp = getgrgid(filstat.st_gid);
				convierteModo(m, permis);

				if((m&S_IFMT) == S_IFLNK)
				{
					readlink(file, link, sizeof(char[STRGMAX]));;
					strcpy(link, rindex(link, '/'));
					strcpy(link, strtok(link, "/"));
					strcat(file, " -> ");
					strcat(file, link);
				}

				printf("%15.15s %7ld %12.12s %12.12s %10.10s %7ld byts (%lu) %s\n",
				       fechmod, filstat.st_ino, pwd->pw_name, grp->gr_name,
				       permis, filstat.st_size, filstat.st_nlink, file);
			}
			else
			{
				printf("%7ld byts %s\n", filstat.st_size, file);
			}
		}
		else
		{
			printf("ERROR: %s\n", strerror(errno));
		}
	}
	else
	{
		printf("ERROR: archivo nulo.");
	}
}

bool esdir(char *dir)
{
	struct stat dstat;
	mode_t m;

	if(dir != NULL)
	{
		if(lstat(dir, &dstat) != -1)
		{
			m = dstat.st_mode;

			return((m&S_IFMT) == S_IFDIR);
		}
	}
	return(false);
}

void listArch(char *arch, bool barglist[])
{
	DIR *diraux;
	struct dirent* file;
	char actdir[256];
	struct lista *dirlist = NULL;
	getcwd(actdir, sizeof(actdir));

	if(barglist[1])
	{
		if((diraux = opendir(arch)) != NULL)
		{
			if(chdir(arch) == 0)
			{
				if(barglist[3])
				{
					crearlis(&dirlist)? 0: exit(EXIT_FAILURE);
				}

				printf("Archivos en: '%s'\n", arch);
				for(file = readdir(diraux); file != NULL; file = readdir(diraux))
				{
					if(!(!barglist[2] && (file->d_name[0] == '.')))
					{
						filDesc(file->d_name, barglist[0]);
					}
					if(barglist[3] && !(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) && esdir(file->d_name))
					{
						encolarlis(dirlist, file->d_name)? 0: exit(EXIT_FAILURE);
					}
				}
				printf("\n");

				for(struct lista *paux = signode(dirlist); paux != NULL; paux = signode(paux))
				{
					listArch(recivelem(paux), barglist);
				}
				if (closedir(diraux) != 0)
				{
					printf("ERROR: %s.\n", strerror(errno));
					exit(EXIT_FAILURE);
				}
				if(chdir(actdir) != 0)
				{
					printf("Imposible cambiar directorio '%s': %s.\n", actdir, strerror(errno));
					exit(EXIT_FAILURE);
				}
				if(!estaVaciaLis(dirlist))
				{
					freelis(dirlist, true);
				}
			}
			else
			{
				printf("Imposible cambiar directorio '%s': %s.\n", arch, strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("ERROR:  %s.\n", strerror(errno));
		}
	}
	else
	{
		filDesc(arch, barglist[0]);
	}
}

bool imprPromptLeerEntrada(char *stgaux, char *trozos[], bool biteracion, struct lista *plis)
{
	struct lista *paux = NULL, *pult = NULL;
	int longstg;

	if(biteracion && !estaVaciaLis(plis))
	{
		paux = recorrerlis(plis, atoi(trozos[1]), false);
		pult = ultimolis(plis);

		if(paux != NULL && pult != NULL)
		{
			if(strcmp(recivelem(pult), recivelem(paux)) != 0)
			{
				printf("Ejecutando historic (%d): ", atoi(trozos[1]));
				strcpy(stgaux, recivelem(paux));
				printf("%s\n", stgaux);
				trocearCadena(stgaux, trozos);

				return true;
			}
			else
			{
				strcpy(stgaux, "\n");
				printf("ERROR: recursividad infinita en historic.\n");
			}
		}
	}
	printf("$$ ");
	longstg = strlen(fgets(stgaux, STRGMAX, stdin));
	stgaux[longstg-1] = '\0';

	if(tieneAlfanum(stgaux))
	{
		if(encolarlis(plis, stgaux))
		{
			trocearCadena(stgaux, trozos);

			return true;
		}
	}
	return false;
}

void authors(char *stgarg)
{
	char *stgnombres = {"Nicolas Vazquez Cancela, Iago Valeiro Castrillon."},
		  *stglogins = {"nicolas.vazquez.cancela, iago.valeiro."};

	if(stgarg == NULL)
	{
		printf("%s\n", stgnombres);
		printf("%s\n", stglogins);
	}
	else
	{
		if(strcmp(stgarg, "-l") == 0)
		{
			printf("%s\n", stglogins);
		}
		else if(strcmp(stgarg, "-n") == 0)
		{
			printf("%s\n", stgnombres);
		}
		else
		{
			printf("ERROR: Argumento no valido.\n");
		}
	}
}

bool historic(struct lista *plis, char *trozos[])
{
	if(trozos[1] == NULL)
	{
		recorrerlis(plis, -1, true);
	}
	else
	{
		if(trozos[2] == NULL && trozos[1][1] != '\0')
		{
			trozos[1] = strtok(trozos[1], "-");

			if(trozos[1][0] == 'c')
			{
				freelis(plis, false);
			}
			else if(trozos[1][0] == 'r')
			{
				trozos[1] = strtok(trozos[1], "r");
				return true;
			}
			else
			{
				recorrerlis(plis, atoi(trozos[1]), true);
			}
		}
	}
	return false;
}

void create(char *trozos[])
{
	FILE *filaux;
	bool barglist[OPCIMAX] = {false, true, false, false};

	if(trozos[1] != NULL)
	{
		if(strcmp(trozos[1], "-dir") == 0)
		{
			if(trozos[2] != NULL)
			{
				if(mkdir(trozos[2], 0766) != 0)
				{
					printf("Imposible crear: %s.\n", strerror(errno));
				}
			}
			else
			{
				printf("Imposible crear: directorio nulo.\n");
			}
		}
		else
		{
			if((filaux = fopen(trozos[1], "a")) != NULL)
			{
				if(fclose(filaux) != 0)
				{
					printf("Imposible cerrar: %s.\n", strerror(errno));
				}
			}
			else
			{
				printf("Imposible crear: %s\n.", strerror(errno));
			}
		}
	}
	else
	{
		
		listArch(".", barglist);
	}
}

void list(char *trozos[])
{
	char *arglist[OPCIMAX] = {"-long", "-dir", "-hid", "-rec"};
	int aux;
	bool barglist[OPCIMAX] = {false, false, false, false};

	if(trozos[1] != NULL)
	{
		for(int a = 1; trozos[a] != NULL; a++)
		{
			if(trozos[a][0] == '-')
			{
				aux = buscarCadena(trozos[a], arglist, OPCIMAX);

				if(aux != -1)
				{
					barglist[aux] = true;
				}
				if(trozos[a + 1] == NULL)
				{
					barglist[1] = true;

					listArch(".", barglist);
				}
			}
			else
			{
				listArch(trozos[a], barglist);
			}
		}
	}
	else
	{
		barglist[1] = true;
		listArch(".", barglist);
	}
}

//Show extern var:
int evar1, evar2, evar3;

int main(void)
{
	//Show automatic var:
	int avar1, avar2, avar3;

	bool bsalir = false, biteracion = false;
	struct lista *plis = NULL;
	stcLisM *plisM = NULL;
	stcProc *plisP[LISPMAX];
	char stgaux[STRGMAX], *trozos[ARGUMAX], *comandos[COMAMAX] =
		{"authors", "getpid", "getppid", "pwd", "chdir", "date", "time",
		 "historic", "quit", "end", "exit", "create", "delete", "list", 
		 "memory", "memdump", "memfill", "recurse", "readfile", "writefile",
		/* nombres de los comandos de la P3: */
		"getpriority", "setpriority", "getuid", "setuid", "fork", "execute", 
		"foreground", "background", "run-as", "execute-as", "listprocs", 
		"proc", "deleteprocs"},
		*memArg[MARGMAX] = {"-allocate", "-dealloc", "-deletekey", "-show",
								  "-show-vars", "-show-funcs", "-dopmap"},
		*subMemArg[] = {"-malloc", "-mmap", "-shared", "-createshared", "-all"},
		stgtmebf[TEMPMAX],
		stgcwd[STRGMAX];
	time_t tptiempo;
	struct tm *stclocal;

	if(!crearlis(&plis))
	{
		printf("ERROR: Imposible crear lista de historico.\n");
		bsalir = true;
	}
	if(!crearLisM(&plisM))
	{
		printf("ERROR: Imposible crear lista de memoria.\n");
		bsalir = true;
	}
	crearLisP(plisP, LISPMAX);

	while(!bsalir)
	{
		if(imprPromptLeerEntrada(stgaux, trozos, biteracion, plis))
		{
			biteracion = false;
			time(&tptiempo);
			stclocal = localtime(&tptiempo);
			
			switch(buscarCadena(trozos[0], comandos, COMAMAX))
			{
				case AUTHORS:
					authors(trozos[1]);
					break;

				case GETPID:
					printf("%d\n", getpid());
					break;

				case GETPPID:
					printf("%d\n", getppid());
					break;

				case PWD:
					printf("%s\n", cwd(stgcwd));
					break;

				case CHDIR:
					if(trozos[1] == NULL)
					{
						cwd(stgcwd);
					}
					else if(chdir(trozos[1]) == -1)
					{
						printf("Imposible cambiar directorio '%s': %s.\n", trozos[1], strerror(errno));
					}
					break;

				case DATE:
					strftime(stgtmebf, TEMPMAX, "%D", stclocal);
					printf("%s\n", stgtmebf);
					break;

				case TIME:
					strftime(stgtmebf, TEMPMAX, "%T", stclocal);
					printf("%s\n", stgtmebf);
					break;

				case HISTORIC:
					biteracion = historic(plis, trozos);
					break;

				case QUIT:
				case END:
				case EXIT:
					bsalir = true;
					break;

				case CREATE:
					create(trozos);
					break;

				case DELETE:
					if(trozos[1] != NULL)
					{
						bool esrec = false;
						char **pargelim = trozos + 1;
						if(strcmp(trozos[1], "-rec") == 0)
						{
							esrec = true;
							pargelim++;
						}
						elimruta(pargelim, esrec);
					}
					else
					{
						bool barglist[OPCIMAX] = {false, true, false, false};
						listArch(".", barglist);
					}
					break;

				case LIST:
					list(trozos);
					break;

				case MEMORY:
					switch(buscarCadena(trozos[1], memArg, MARGMAX))
					{
						case ALLOCATE:
							switch(buscarCadena(trozos[2], subMemArg, SMRGMAX))
							{
								case MALLOC:
									Cmd_AllocateMalloc(plisM, &trozos[3]);
									break;

								case MMAP:
									Cmd_AllocateMmap(plisM, &trozos[3]);
									break;

								case SHARED:
									Cmd_AllocateShared(plisM, &trozos[3]);
									break;
									
								case CRTSHARED:
									Cmd_AllocateCreateShared(plisM, &trozos[3]);
									break;

								default:
										if(trozos[2] == NULL)
										{
											listarLisM(plisM, eqtypM, NULL);
										}
										else { printf("Opciones: -malloc, -mmap, -shared, -createshared.\n"); }
									break;
							}
							break;

						case DEALLOC:
							switch(buscarCadena(trozos[2], subMemArg, SMRGMAX))
							{
								case MALLOC:
									Cmd_Dealloc(plisM, freeM, &trozos[3]);
									break;

								case MMAP:
									Cmd_Dealloc(plisM, closeFile, &trozos[3]);
									break;

								case SHARED:
									Cmd_Dealloc(plisM, separarSM, &trozos[3]);
									break;

								default:
									if(trozos[2] != NULL)
									{
										Cmd_DeallocAddr(plisM, &trozos[2]);
									}
									else { listarLisM(plisM, eqtypM, NULL); }
									break;
							}
							break;

						case DELETEKEY:
							Cmd_deletekey(&trozos[1]);
							break;

						case SHOW:
							switch(buscarCadena(trozos[2], subMemArg, SMRGMAX))
							{
								case MALLOC:
									listarLisM(plisM, eqtypM, "malloc");
									break;

								case MMAP:
									listarLisM(plisM, eqtypM, "mmap");
									break;

								case SHARED:
									listarLisM(plisM, eqtypM, "shared memory");
									break;

								case ALL:
									listarLisM(plisM, eqtypM, NULL);
									break;

								default:
									printf("Opciones: -malloc, -mmap, -shared, -all.\n");
									break;
							}
							break;

						case SHOW_VARS:
							printf("Variables locales  %p, %p, %p\nVariables globales %p, %p, %p\n", 
							       &avar1, &avar2, &avar3, &evar1, &evar2, &evar3);
							break;

						case SHOW_FUNCS:
							printf("Funciones programa %p, %p, %p\nFunciones libreria %p, %p, %p\n",
							       &buscarCadena, &trocearCadena, &tieneAlfanum, &printf, &scanf, &open);
							break;

						case DOPMAP:
							Cmd_dopmap(NULL);
							break;

						default:
							printf("Opciones:\n-allocate, -dealloc, -show, -show-vars, -show-funcs, -dopmap.\n");
							break;
					}
					break;

				case MEMDUMP:
					memdump(trozos);
					break;

				case MEMFILL:
					memfill(trozos);
					break;

				case RECURSE:
					recurse(trozos);
					break;

				case READFILE:
					if(trozos[1] != NULL && trozos[2] != NULL)
					{
						if(trozos[3] != NULL)
						{
							LeerFichero(trozos[1], str2pntr(trozos[2]), atoi(trozos[3])); 
						}
						else { LeerFichero(trozos[1], str2pntr(trozos[2]), -1); }
					}
					break;

				case WRITEFILE:
					writefile(trozos);
					break;

				//Comandos P3
				case GETPRIORITY:
					cmd_getpriority(trozos);
					break;

				case SETPRIORITY:
					cmd_setpriority(trozos);
					break;

				case GETUID:
					Cmd_getuid(trozos);
					break;

				case SETUID:
					Cmd_setuid(trozos+1);
					break;

				case FORK:
					cmd_fork(trozos);
					break;

				case EXECUTE:
					cmd_execute(trozos);
					break;

				case FOREGROUND:
					Cmd_foreground(&trozos[1]);
					break;

				case BACKGROUND:
					Cmd_background(plisP, &trozos[1]);
					break;

				case RUNAS: //run-as
					Cmd_Run_as(plisP, &trozos[1]);
					break;

				case EXECUTAS: //execute-as
					cmd_executeas(trozos);
					break;

				case LISTPROCS:
					Cmd_listprocs(plisP);
					break;

				case PROC:
					Cmd_proc(plisP, &trozos[1]);
					break;

				case DELETEPROCS:
					 Cmd_deleteprocs(plisP, &trozos[1]);
					break;

				default:
					if(trozos[0] != NULL) { Cmd_anyCommand(plisP, trozos); }
					break;
			}
		}
		else
		{
			printf("\nERROR: Imposible registrar el comando en el historico.\n");
			bsalir = true;
		}
	}

	freelis(plis, true);
	freelisM(plisM);
	freeLisP(plisP);
	return 0;
}
