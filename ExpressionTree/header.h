/*Lupu Andreea 311CB - TEMA3*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

typedef struct nod {		
	char info[15];
	struct nod *st, *dr;
} TNod, *TArb, **AArb;

typedef struct {
	char variabila[15];
	int valoare;
}Asignare, *TAsignare;

typedef struct celula {
	TAsignare asign;
	struct celula *urm;
} TCelula, *TLista, **ALista;

