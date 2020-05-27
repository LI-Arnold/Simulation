#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXEVENT 10000
#define nbSlot 150
#define K 111 //nbr de stations dans l'anneau
#define EPSILON 12


/* ********* Variables globales ********** */
int N; // total conteneurs qui ont été produit
int temps;
int Nservi; // conteneurs sortis du système
int compteur= 0; //condition d'arrêt <=> manchon
int delai1 = 0;  //delai d'attente pour la station 1
int delai10 = 0; //delai d'attente pour la station 2

/* ********* Création de structure ********** */

struct elem{
	int attente;
	struct elem *suiv;
};typedef struct elem* liste;

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
	int duree; //duree d'attente entre chaque nouveau conteneur produit 
	liste l; // liste d'attente de conteneurs
}station;

typedef struct Anneau {
	station Stati[K];
	slot Slo[nbSlot];
	conteneur Cont[nbSlot];
	int Nanneau; //nombre de conteneurs dans l'anneau
}anneau;




/* ***** Fonctions liste ******* */


liste creer_liste(){
	return NULL;
}

int est_vide(liste l){
	if(l == NULL)
		return 1;
	return 0;
}

void affiche_liste(liste l){
	
	if(est_vide(l))
	{
		printf("Aucun conteneur en attente dans la station\n");
		return;
	}
	while(l)
	{
		printf("%d / ",l->attente);
		l = l->suiv;
	}
	printf("\n");
}

liste libere_liste(liste l){
	liste tmp;
	while(l)
	{
		tmp = l->suiv;
		//printf("libération de %d\n",l->val);
		free(l);
		l = tmp;
	}
	return NULL;
}

liste ajoute_elem_fin(liste l){
	liste new = malloc(sizeof(struct elem));
	new->attente = 0;
	new->suiv = NULL;
	if(est_vide(l))
	{
		return new;
	}
	liste debut = l;
	//on place le pointeur sur le dernier element de la liste
	while(l->suiv)
		l = l->suiv;
	l->suiv = new;
	return debut;
}


liste supprime_elem(liste l){
	if(est_vide(l))return l;
	//Premier element
	liste tmp;
	tmp = l->suiv;
	free(l);
	return tmp;
}


liste attente_liste(liste l){
	if(est_vide(l)) return l;
	liste debut = l;
	while(l){
		l->attente = l->attente + 1;
		l = l->suiv;
	}
	return debut;
}




/* ******* Fonctions d'initialisations ******** */

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
		A->Stati[i].l = creer_liste();
	}
}

void Init_Slot(anneau *A){
	int i;
	for(i = 0; i<nbSlot; i++){
		A->Slo[i].occupe = 0;
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
	for(int i=0;i<K;i++){
		A->Stati[i].duree = 0;
	}
	
	//Initialisation variables
	temps = 0;
	N = 0;
	Nservi = 0;
}



/* ******* Fonctions Affichage ******** */
void affiche_Conteneur(anneau *A){
	int i;
	for(i = 0; i < A->Nanneau; i++){
		 printf("\nConteneur %d \t position %d \t deplacement restants %d \n", i, A->Cont[i].position, A->Cont[i].nbdeplacement);
	}
	
}

void affiche_Station(anneau *A){
	int i;
	for(i = 0; i < K; i++){
		printf("\nStation N° %d contient : %d conteneur(s)  delta = %d et durée = %d\n",A->Stati[i].position, A->Stati[i].Nstation, A->Stati[i].delta, A->Stati[i].duree);
		affiche_liste(A->Stati[i].l);
	}
}

void affiche_Slots(anneau *A){
	int i;
	printf("\n");
	for(i = 0; i < nbSlot; i++){
		if(A->Slo[i].occupe == 1){
			printf("Slots N° %d  Etat : %d \t",i, A->Slo[i].occupe);
			if((i%10) == 0) printf("\n");
		}
	}
}

void affiche_total_slots_occupes(anneau *A){
	int i;
	int compte = 0;
	for(i = 0; i < nbSlot; i++){
		if(A->Slo[i].occupe == 1){
			compte++;
		}
	}
	printf("\ni = %d ==> %d conteneurs\n",temps, compte);
}






/* ***** Fonctions utiles pour la simulation */

//je decalle les conteneurs d'un slot à chaque tic d'horloge
void Decale_Anneau(anneau *A){
	int i;
	temps ++;
	for(i = 0; i < A->Nanneau; i++){
		A->Slo[A->Cont[i].position].occupe = 0;  //je mets tous les slots libre
		A->Cont[i].position = (A->Cont[i].position + 1) % nbSlot;
		A->Slo[A->Cont[i].position].occupe = 1;  //je regarde où sont les nouveaux conteneur et je mets les slots correspondqnt dans l'état :  occupé
		A->Cont[i].nbdeplacement -=1;
		if(A->Cont[i].nbdeplacement <= 0) A->Cont[i].nbdeplacement = 0;
	}
	
}


//j'avance le delta de chaque stations à chaque tic d'horloge
//je progresse sur la création des conteneurs dans chaque stations à chaque tic d'horloge
void Avance_Delta(anneau *A){
	int i;
	for(i = 0; i < K; i++){ // quand je décale l'anneau toutes les stations vont avoir leur delta qui augmente de 1
		A->Stati[i].delta += 1;
		A->Stati[i].duree -= 1; //création des conteneurs
		if(A->Stati[i].duree <= 0) A->Stati[i].duree = 0;
	}
		
}

//ajout d'un conteneur nouvellement créé dans la file d'attente d'une station
void Ajout_Conteneur_File(anneau *A, int k){
	A->Stati[k].l = ajoute_elem_fin(A->Stati[k].l);
}

//Création d'un conteneur dans l'anneau <=> début service
void Ajout_Conteneur(anneau *A, int k){
	A->Nanneau += 1;
	A->Cont[A->Nanneau-1].nbdeplacement = nbSlot;
	A->Cont[A->Nanneau-1].position = A->Stati[k].position;	
}


//pour toutes les stations vérification à chaque tic d'horolge si je peux émettre un conteneur dans l'anneau 
void Ajout_Conteneur_Anneau(anneau *A, FILE *f2, FILE *f3){
	int i; //indice pour parcours toutes les stations;
	temps ++;
	for(i = 0; i < K; i++){
		if(A->Stati[i].Nstation > 0) { //je regarde si la station possede des conteneurs à ajouter
			if(A->Stati[i].delta >= 10){ //je regarde si la station i a bien attendu 10 slots avant de vouloi insérer un conteneur dans l'anneau
				if(A->Slo[A->Stati[i].position].occupe == 0) { //Je regarde si le slot dans l'anneau est bien vide 
					A->Stati[i].delta = 0; //Si j'emet un conteneur je réinitialsie le delta 
					A->Stati[i].Nstation -= 1; //je diminue le nombre de conteneur dans la station
					A->Slo[A->Stati[i].position].occupe = 1; //je dis qu'un slot de l'anneau est occupé 
					Ajout_Conteneur(A, i);
					
					A->Cont[A->Nanneau-1].position = A->Stati[i].position;
					
					//Je sauvegarde pour les stations 1 et 10 le temps d'attente de chaque conteneur
					if(i == 1) {
						delai1 = A->Stati[i].l->attente;
						fprintf(f2, "%d    %d \n", temps, delai1);
					}
					else if(i == 10) {
						delai10 = A->Stati[i].l->attente;
						fprintf(f3, "%d    %d \n", temps, delai10);
					}
					
					//Comme le conteneur vient dêtre emis dans l'anneau de le supprime de la file d'attente de la station
					A->Stati[i].l = supprime_elem(A->Stati[i].l);
				}
			}
			else{ //temps d'attente augmente car le conteneur est encore dans la file d'attente
				A->Stati[i].l = attente_liste(A->Stati[i].l);
			}
		}
	}
	
}


//Une fois que ke conteneur a passé un tour dans l'anneau (150 slots) je le supprime 
// <=> Fin de service
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



int Condition_arret(int slotAncien, int slotNouveau){	
	if ( abs(slotAncien-slotNouveau) < EPSILON){
		compteur ++;
		if(compteur >1e3) return 1;
	}
	else compteur = 0;
	if (temps >= 25000) {
		printf("\narret forcé au bout de %d itérations\n",temps);
		return 1;   // Si le système ne se stabilise pas, arrêter le programme au bout
									// d'un certains temps.
	}
	return 0;
}


/* ******* Fonctions générer temps de fabrication des conteneurs ******** */

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

void remplir_conteneur(anneau *A){
	int i;
	for(i=0;i<K;i++){
		
		if(A->Stati[i].duree == 0){
				A->Stati[i].Nstation ++;
				A->Stati[i].duree = Generer_Duree();
				A->Stati[i].duree -= 50;
				if(A->Stati[i].duree <=0){
					A->Stati[i].duree = 0;
					A->Stati[i].duree +=7;
				 }
					Ajout_Conteneur_File(A, i);
					N++;
			}
	}
}





void Simulation(FILE *f1, anneau *A){
	FILE *f2 = fopen("Simulation_attente1.data","w");
	FILE *f3 = fopen("Simulation_attente10.data","w");
	int slotAncien;
	int slotNouveau = 0;
	int  i= 0;
	
	do {
		slotAncien = slotNouveau;
		remplir_conteneur(A);
		
		printf("\n\n**************** i = %d ****************\n\n",i);
		Ajout_Conteneur_Anneau(A, f2, f3);
		Decale_Anneau(A);
		Avance_Delta(A);
		Supprime_Conteneur_Anneau(A);
	
		//slotNouveau = A->Nanneau;
		slotNouveau = delai1;
		
		affiche_Station(A);
		//affiche_Slots(A);
		//affiche_total_slots_occupes(A);
		//affiche_Conteneur(A);
		
		//courbe du nombre de slot rempli dans l’anneau durant la simulation
		if(temps == 0) fprintf(f1, "0    0 \n");
		else fprintf(f1, "%d    %d \n", temps, A->Nanneau);
		
		i++;
	}while(Condition_arret(slotAncien,slotNouveau) == 0);
	
}


int main (){
	FILE *f1 = fopen("Simulation_nb_conteneurs.data","w");
	srand(time(NULL));
	anneau A;
	Init_Anneau(&A);
	Simulation(f1, &A);
	return 0;
	
}
