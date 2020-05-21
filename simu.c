#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXEVENT 10000
#define nbSlot 150
#define K 10 //nbr de stations dans l'anneau
int N; // total conteneurs qui ont été produit
int temps;
int Nservi; // conteneurs sortis du système


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

typedef struct Conteneur {
	int position;
	int nbdeplacement; // nombre de déplacement à effectuer avant de sortir de l'anneau
}conteneur;

typedef struct Slot {
	int occupe; // 0 quand c'est libre et 1 quand c'est occupe
	int numero; //numero de chaque slot
}slot;

typedef struct Station {
	int position;
	int Nstation; //nb de contenaur que la station possede
	int delta;
}station;

typedef struct Anneau {
	station Stati[K];
	slot Slo[nbSlot];
	conteneur *Cont;
	int Nanneau; //nombre de conteneur dans l'anneau
}anneau;

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

int positionStation(int i){
	return ((150/K)*i);
}

void Init_Station(anneau *A){
	int i; 
	for(i = 0; i<K; i++){
		A->Stati[i].position = positionStation(i);
		A->Stati[i].Nstation = 0;
		A->Stati[i].delta = 0;
	}
}

void Init_Slot(anneau *A){
	int i;
	for(i = 0; i<nbSlot; i++){
		A->Slo[i].occupe = 0;
		A->Slo[i].numero = i;
	}
}

void Init_Conteneur(anneau *A){
	A->Cont = malloc(A->Nanneau * sizeof(conteneur));
}

void Init_Anneau(anneau *A){
	Init_Station(A);
	Init_Slot(A);
	
	A->Nanneau = 0;
	Init_Conteneur(A);
	
	
	//Initialisation variables
	temps = 0;
	N = 0;
	Nservi = 0:
	
	
}

void Ajout_Conteneur(anneau *A){
	A->Cont = malloc(A->Nanneau * sizeof(conteneur));
}






int main (){
	
	return 0;
	
}
