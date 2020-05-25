#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXEVENT 10000
#define nbSlot 150
#define K 3 //nbr de stations dans l'anneau
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
	//int numero; //numero de chaque slot
}slot;

typedef struct Station {
	int position;
	int Nstation; //nb de conteneurs que la station possede
	int delta;
}station;

typedef struct Anneau {
	station Stati[K];
	slot Slo[nbSlot];
	conteneur Cont[nbSlot];
	int Nanneau; //nombre de conteneurs dans l'anneau
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

double Fct_Repart(){
	FILE *f = fopen("InterArrivee.txt","r");
	double r = (double)random()/RAND_MAX;
	int duree = r*108;
	double p;
	int test=109;
	while(duree != test){
		fscanf(f,"%d %lf",&test,&p);
	}
	fclose(f);
	return p;
}

int Generer_Duree(){
	FILE *f = fopen("InterArrivee.txt","r");
	double p = Fct_Repart(f);
	double test = 1.0;
	int duree;
	while(test != p){
		fscanf(f,"%d %lf",&duree,&test);
	}
	fclose(f);
	return duree;
}

//j'initialise la position de chaque station
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
		//A->Slo[i].numero = i;
	}
}

void Init_Conteneur(anneau *A){
	int i;
	for(i = 0; i<nbSlot; i++) {
		A->Cont[i].nbdeplacement = 150;
		A->Cont[i].position = -1;
	}
}

void Init_Anneau(anneau *A){
	Init_Station(A);
	Init_Slot(A);
	
	A->Nanneau = 0;
	//printf("t : %d",A->Nanneau);
	//Init_Conteneur(A);
	
	
	//Initialisation variables
	temps = 0;
	N = 0;
	Nservi = 0;
	
	//j'ajoute la premiere vague de conteneur
	//Init_Ajout_Conteneur_Anneau(A);
}

void Decale_Anneau(anneau *A){
	int i;
	temps ++;
	//je decalle les conteneurs
	for(i = 0; i < A->Nanneau; i++){
		A->Slo[A->Cont[i].position].occupe = 0;  //je mets tous les slots libre
		A->Cont[i].position = (A->Cont[i].position + 1) % nbSlot;
		A->Slo[A->Cont[i].position].occupe = 1;  //je regarde où sont les nouveaux conteneur et je mets les slots correspondqnt dans l'état :  occupé
		A->Cont[i].nbdeplacement -=1;
		if(A->Cont[i].nbdeplacement <= 0) A->Cont[i].nbdeplacement = 0;
	}
	
}

void Avance_Delta(anneau *A){
	int i;
	for(i = 0; i < K; i++) // quand je décale l'anneau toutes les station vont avoir leur delta qui augmente de 1
		A->Stati[i].delta += 1;
		
}

void Ajout_Conteneur(anneau *A, int k){
	A->Nanneau += 1;
	//A->Cont = malloc(A->Nanneau * sizeof(conteneur));
	A->Cont[A->Nanneau-1].nbdeplacement = nbSlot;
	A->Cont[A->Nanneau-1].position = A->Stati[k].position;	
	
}

void remplir_Station(anneau *A){
	int i;
	int duree = Generer_Duree();
	for(i = 0; i < K; i++){
		if(duree == 0){
		A->Stati[i].Nstation++;
		}
		duree = duree-temps;
	}
}

void affiche_Conteneur(anneau *A){
	int i;
	printf("\n");
	for(i = 0; i < A->Nanneau; i++){
		printf("Conteneur Position: %d  deplacement : %d \t",A->Cont[i].position, A->Cont[i].nbdeplacement);
		//if((i%5) == 0) printf("\n");

	}
	
}

void affiche_Station(anneau *A){
	int i;
	for(i = 0; i < K; i++){
		printf("\nStation N° %d contient : %d conteneur(s) et delta = %d\n",A->Stati[i].position, A->Stati[i].Nstation, A->Stati[i].delta);
	}
}
void affiche_Slots(anneau *A){
	int i;
	printf("\n");
	for(i = 0; i < nbSlot; i++){
		if(A->Slo[i].occupe == 1){
			printf("Slots N° %d  Etat : %d \t",i, A->Slo[i].occupe);
			//if((i%10) == 0) printf("\n");
		}
	}
}

void Ajout_Conteneur_Anneau(anneau *A){
	int i; //indice pour parcours toutes les stations;
	temps ++;
	for(i = 0; i < K; i++){
		if(A->Stati[i].delta >= 10){ //je regarde si la station i a bien attendu 10 slots avant de vouloi insérer un conteneur dans l'anneau
			if(A->Stati[i].Nstation > 0) { //je regarde si la station possede des conteneurs à ajouter
				if(A->Slo[A->Stati[i].position].occupe == 0) { //Je regarde si le slot dans l'anneau est bien vide 
					A->Stati[i].delta = 0;
					A->Stati[i].Nstation -= 1;
					A->Slo[A->Stati[i].position].occupe = 1;
					Ajout_Conteneur(A, i);
					A->Cont[A->Nanneau-1].position = A->Stati[i].position;
					N++;
				}
			}
		}
	}
	
}

void Init_Ajout_Conteneur_Anneau(anneau *A){
	int i; //indice pour parcours toutes les stations;
	temps ++;
	for(i = 0; i < K; i++){
		if(A->Stati[i].Nstation > 0) { //je regarde si la station possede des conteneurs à ajouter
			if(A->Slo[A->Stati[i].position].occupe == 0) { //Je regarde si le slot dans l'anneau est bien vide 
				A->Stati[i].delta = 0;
				A->Stati[i].Nstation -= 1;
				A->Slo[A->Stati[i].position].occupe = 1;
				Ajout_Conteneur(A, i);
				
				N++;
			}
		}
	}
}

void Supprime_Conteneur_Anneau(anneau *A){
	int i; //indice pour parcours toutes les stations;
	int l;
	int j[ A->Nanneau]; //Numero du conteneur à retirer
	int pos = 0; //indice de parcours des conteneurs
	temps ++;
	
	for(i = 0; i < A->Nanneau; i++){
		j[i] = -1;
	}
	
	for(i = 0; i < A->Nanneau; i++){
		if(A->Cont[i].nbdeplacement == 0){ //si le conteneur à fait un tour dans l'anneau, je le supprime 
			j[pos] = i;
			A->Slo[A->Stati[i].position].occupe = 0; //Je libere le slot
			Nservi ++;
			pos ++;
		}
	}
	
	for(l = 0; l < A->Nanneau; l++){
		if(j[l] != -1) {
			for (i = j[l] - 1; i < A->Nanneau - 1; i++){
				A->Cont[i].nbdeplacement = A->Cont[i+1].nbdeplacement;
				A->Cont[i].position = A->Cont[i+1].position;
			}
		}
	}
	
	A->Nanneau -= pos;
	
	
	
}

void Simulation(FILE *f1, anneau *A){
	remplir_Station(A); //Je rempli les stations avec un nobre de conteneur
	affiche_Station(A);
	Init_Ajout_Conteneur_Anneau(A); //j'ajoute la premiere vague de conteneur
	
	int i = 200;
	affiche_Slots(A);
	affiche_Conteneur(A);
	do {
		printf("\n\n**************** i = %d ****************\n\n",i);
		Ajout_Conteneur_Anneau(A);
		Decale_Anneau(A);
		Avance_Delta(A);
		Supprime_Conteneur_Anneau(A);
		
		affiche_Station(A);
		affiche_Slots(A);
		affiche_Conteneur(A);
		if(temps == 0){
			fprintf(f1, "0    0 \n");
		}else{
			fprintf(f1, "%d    %d \n", temps, A->Nanneau);
		}
		i--;
	}while(i != 0);
	
}


int main (){
	FILE *f1 = fopen("Simulation_nb_conteneurs.data","w");
	srand(time(NULL));
	anneau A;
	Init_Anneau(&A);
	Simulation(f1, &A);
	return 0;
	
}
