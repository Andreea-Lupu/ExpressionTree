/*Lupu Andreea 311CB - TEMA3*/

#include "header.h"

TLista AlocareCelula(char *nume, int nr) /*aloca celula pentru o lista*/
{
	TLista aux = (TLista) malloc(sizeof(TCelula));
	if (!aux)
		return NULL;
	aux->asign = (TAsignare) malloc(sizeof(Asignare));
	if(!aux->asign) {
		free(aux);
		return NULL;
	} 
	strcpy(aux->asign->variabila, nume);
	aux->asign->valoare = nr;
	aux->urm = NULL;
	return aux;
}

int InitListaVariabile(ALista L, char *nume, int nr)
/*stocheaza asignarile de variabile in celule adaugate la inceputul unei liste*/
/*intoarce 1-succes, 0-eroare*/
{
	TLista aux = AlocareCelula(nume, nr);
	if (!aux) 
		return 0;
	if(*L == NULL) {
		*L = aux;
	} else {
		aux->urm = *L;
		*L = aux;
	}
	return 1;
}

int valoare_numerica(char *caracter)
/*aflare valoarea numerica pentru un sir de caractere care reprezinta un sir de numere*/
{
	int numar = 0;
	int i;
	for (i = 0; i < strlen(caracter); i++)
	{
		if(caracter[i] != '-') {
			numar = numar * 10 + caracter[i] - '0';
		} 
	}
	/*pt numere negative*/
	if(caracter[0] == '-')
    	numar = 0-numar;
	return numar;
}

void DistrugeLista(ALista L)
{
	TLista aux;
	while(*L != NULL)
	{
		aux = *L;
		*L = aux->urm;
		free(aux->asign);
		free(aux);
	}
	
}

TLista StocheazaAsignari(int N, FILE *f)
/*pastrez intr-o lista simplu inlantuita toate */
{
	char sir[15];
	char *variabila;
	char *valoare;
	int valoareNum;
	int i, ok;
	TLista L = NULL;
	for(i = 0; i < N; i++)
	{ 
		fgets(sir,15,f);
		variabila = strtok(sir, " ="); 
		valoare = strtok(NULL, "= \n"); 
		valoareNum = valoare_numerica(valoare);
		ok = InitListaVariabile(&L, variabila, valoareNum);
		if(ok == 0) {
			printf("Eroare adaugare in lista variabile\n");
			DistrugeLista(&L);
			return NULL;
		}
	}
	return L;
}

TLista CautareAsignare(TLista L, char *nume)
/*intoarce adresa celulei in care se afla variabila cautata*/
{
	TLista aux = L;
	for(; aux != NULL; aux = aux-> urm) {
		if(strcmp(aux->asign->variabila, nume) == 0) {
			return aux;	
		}
	}
	return NULL;
}

TArb AlocareNod(char* x)
{
	TArb nod = (TNod *) malloc(sizeof(TNod));
	if (!nod) 
		return NULL;
	strcpy(nod->info, x);
	nod->st = NULL;
	nod->dr = NULL;
	return nod;
}

int verificare (char * sir) /*1-operatie; 0-numar sau variabila*/
{
	if (strcmp(sir, "+") == 0 || strcmp(sir, "-") == 0 || 
		strcmp(sir, "*") == 0 || strcmp(sir, "/") == 0 || 
		strcmp(sir, "sqrt") == 0 || strcmp(sir, "^") == 0 )
			return 1;
	return 0;
}

int verificare_numar (char * sir) /*1-numar;0-altfel*/
{
	int i = 0;
	sir = strtok(sir, "-");
	while(i < strlen(sir))
	{
		if (sir[i] < '0' || sir[i] > '9')
			return 0;
		i++;
	}
	return 1;
}

TArb ConstrArb(char *sir) /*construiesc arbore de expresie*/
{
	char *m=strtok(sir, "  \n");
	TArb nod = AlocareNod(m);
	if (!nod) {
		return NULL;
	}
	if (verificare(m) == 0)
		return nod;
	if (strcmp(m, "sqrt") != 0) {
	sir = strtok(NULL,"\n");
	if(sir != NULL) {
		nod->st = ConstrArb(sir);
	} 
	if (sir != NULL) {
		sir = strtok(NULL,"\n");
	}
	if(sir != NULL) {
		nod->dr = ConstrArb(sir);
	}	
	} else {
		sir = strtok(NULL,"\n");
		if(sir != NULL) {
			nod->st = ConstrArb(sir);
		} 
	}
	return nod;
}

void distruge(TArb r) 	/* functie auxiliara - distruge toate nodurile */
{
	if (!r) return;
	distruge (r->st);     /* distruge subarborele stang */
	distruge (r->dr);     /* distruge subarborele drept */
	free (r);             /* distruge nodul radacina */
}

void DistrArb(TArb *a) /* distruge toate nodurile arborelui de la adresa a */
{
	if (!(*a)) return;       /* arbore deja vid */
	distruge (*a);           /* distruge toate nodurile */
	*a = NULL;               /* arborele este vid */
}

int format_int(char *sir, TLista Asignari)
{
	if(verificare_numar(sir) == 1) /*daca este numar returnez valoarea sa ca int*/
		return valoare_numerica(sir);
	/*daca este variabila caut variabila in lista asignarilor si intorc valoare
		corespunzatoare ei*/
	TLista aux = CautareAsignare(Asignari, sir); 
	return aux->asign->valoare;
}

int evaluare(TArb radacina, TLista Asignari, int *ok, FILE *out_file)
{
	/*intializare rezultat evaluare nod stanga si nod dreapta*/
	int stanga = 0, dreapta = 0; 
	/*daca este frunza*/
	if(!radacina->st && !radacina->dr && *ok == 0) {
		/*daca valoarea este intreaga sau se gaseste in lista asignarilor o 
			returnez*/
		if (CautareAsignare(Asignari, radacina->info) != NULL || 
			verificare_numar(radacina->info) == 1) {
				return format_int(radacina->info, Asignari);
		/*altfel afisez mesaj de eroare*/
		} else {
			fprintf(out_file, "Variable %s undeclared\n", radacina->info);
			*ok = 1;
		}
	} 
	/*daca nu s-a intalnit pana acum un caz de eroare*/
	if(*ok == 0) {  /*calculez valoarea nodului din stanga*/
		stanga = evaluare(radacina->st, Asignari, ok, out_file);
	}
	/*daca exista nodul din dreapta si nu s-a intalnit pana acum un caz de eroare*/
	if (radacina->dr && *ok == 0) {  /*calculez valoarea nodului din dreapta*/
	 	dreapta = evaluare(radacina->dr, Asignari, ok, out_file);
	}
	/*daca nu s-a intalnit pana acum un caz de eroare evaluez expresia*/
	if (*ok == 0) {
		if (strcmp(radacina->info,"+") == 0)
			return stanga+dreapta;
		if (strcmp(radacina->info, "-") == 0)
			return stanga-dreapta;
		if (strcmp(radacina->info, "*") == 0)
			return stanga*dreapta;
		if (strcmp(radacina->info, "^") == 0) 
			return (int)(pow((double)stanga, (double)dreapta));
		if (strcmp(radacina->info, "/") == 0) {
			if (dreapta != 0) {
				return	stanga/dreapta;
			} else { /*intalnire caz eroare*/
				fprintf(out_file, "Invalid expression\n");
				*ok = 1;
			}
		}
		if (strcmp(radacina->info, "sqrt") == 0) { 
			if (stanga >= 0) { 
				return (int)(sqrt((double)stanga));
			} else { /*intalnire caz eroare*/
				fprintf(out_file, "Invalid expression\n");
				*ok = 1;
			}
		}
	}
}

void Rezolvare (FILE *input, FILE *output, int M, TLista Asignari)
{
	int i;
	char sir[300];
	for (i = 0; i < M; i++) 
	{
		fgets(sir, 300,input); /*citesc expresiile care trebuie evaluate*/
		TArb arb = ConstrArb(sir); /*construiesc arborele de expresie*/
		int ok = 0;
		evaluare(arb, Asignari, &ok, output); /*evaluez expresia*/
		/*daca nu se intalnesc cazuri de eroare afisez rezultatul expresiei*/
		if (ok == 0) {  
			fprintf(output, "%d\n",evaluare(arb, Asignari, &ok, output));
		}
		DistrArb(&arb);
	}
}

int main(int argc, char *argv[])
{
	FILE *in = fopen(argv[1], "rt");
	if (in == NULL) {
		fprintf(stderr, "ERROR: Can`t open file %s", argv[1]);
		return -1;
	}
	FILE *out = fopen(argv[2], "wt");
	if (out == NULL) {
		fprintf(stderr, "ERROR: Can`t open file %s", argv[2]);
		return -1;
	}

	TLista ListaAsignari;
	int nr_asignari, nr_expresii;
	char sir[15];
	fscanf(in, "%d", &nr_asignari);
	fgets(sir, 15, in);
	ListaAsignari = StocheazaAsignari(nr_asignari, in);

	fscanf(in, "%d", &nr_expresii);
	fgets(sir, 15, in);
	Rezolvare(in, out, nr_expresii, ListaAsignari);
	
	DistrugeLista(&ListaAsignari);
	
	fclose(in);
	fclose(out);
	return 0;
}	
