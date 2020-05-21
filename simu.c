#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXEVENT 10000

int N; // total conteneurs qui ont été produit


typedef struct Event{
	int Et; //état station (T/F)
	int Es; //état de chaque slots : 0 pour slot libre et 1 pour slot non libre
	int ND; //nb de décalage restant pour chaque conteneur (pour savoir quand celui-ci doit partir du système)
}event;

typedef struct Echeancier{
	event Tab[MAXEVENT];
	int taille;
}echeancier;

echeancier E;


void Ajouter_Event(event e){
	if(E.taille < MAXEVENT){
		E.Tab[E.taille] = e;
		E.taille ++;
		printf("Taille = %d\n", E.taille);
	}
	else 
	{
		printf("Echeancier plein \n");
		exit(0);
	}
}

void Init_Echeancier(){
	event e; 
	e.Et = 0;
	e.Es = 0;
	e.ND = 150;
	E.taille = 0;
	Ajouter_Event(e);
}









int main (){
	
	return 0;
	
}
