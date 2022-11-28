/*
Programme d'exemple d'utilisation de fichiers 

  
*/
#include <iostream>
#include <fstream>
#include "liste.hpp"
using namespace std;

struct Bizarre{
    string numero, valeur;
    int x, y;
};

liste<int> lire_entiers(string nom_fic){
  fstream flux; 
  
  int nb, total;
  liste<int> liste_entiers = {} ;

  total = 0;

  flux.open(nom_fic, ios::in);
  if (flux.is_open()) {
    flux >> nb;  // première lecture avant le tant que
    while (flux.good()) { // vérification que la lecture a été effectuée correctement
       total = total + nb;  // traitement d'un élément en étant sûr que la lecture s'est bien passée 
	   inserer(nb,liste_entiers,taille(liste_entiers)+1); // liste
       flux >> nb;  // lecture du suivant, déclenchant  possiblement une erreur rendant flux.good() faux 
    }
    flux.close();   
  }
  else {
    cout << "Erreur : impossible d'ouvrir " << nom_fic << endl;
  }

  return liste_entiers;
}

// Rôle : sous algo d'illustration écrivant des entiers dans un fichier

void ecrire_entiers(string nom_fic, liste<int> liste_entiers){
    fstream flux; 
    long unsigned int  nb = 1;
    flux.open(nom_fic, ios::out);
    if (flux.is_open()) {
    
       while (nb <= taille(liste_entiers) ){
           flux << liste_entiers[nb]<< " ";
           nb += 1;
       }
       flux.close();   
   }
   else {
       cout << "Erreur : impossible d'ouvrir " << nom_fic << endl;
   }
}

// Copie de liste entiers


liste<Bizarre> lire_bizarre(string nom_fic){
  fstream flux; 
  
  Bizarre n;
  
  liste<Bizarre> liste_bizarre = {} ;



  flux.open(nom_fic, ios::in);
  if (flux.is_open()) {
    flux >> n.numero;  // première lecture avant le tant que
	  flux >> n.valeur;
	  flux >> n.x;
	  flux >> n.y;
    while (flux.good()) { // vérification que la lecture a été effectuée correctement
       
	    inserer(n,liste_bizarre,taille(liste_bizarre)+1); // liste
 
		flux >> n.numero;  // première lecture avant le tant que
		flux >> n.valeur;
		flux >> n.x;
		flux >> n.y;
    }
    flux.close();   
  }
  else {
    cout << "Erreur : impossible d'ouvrir " << nom_fic << endl;
  }
  return liste_bizarre;
}


void affichage(liste<int> t){
	
			for (long unsigned int i = 1; i<= taille(t); i++){
			
			cout << t[i] ;
			cout << endl;
			
		
		}
	
	
}


void affichage(liste<Bizarre> t){
	
		for (long unsigned int  i = 1; i<= taille(t); i++){
			
			cout << t[i].numero << "-";
			cout << t[i].valeur<< "-";
			cout << t[i].x<< "-";
			cout << t[i].y;
			cout << endl;
			
		
		}

}




// Rôle : programme principal utilisant lire_entiers et ecrire_entiers en en demandant les paramètres à l'utilisateur
int main(int argc, char * argv[]){
  
  liste<string> arguments_programme = arguments(argc, argv);
  
  if (taille(arguments_programme) == 0){
	  
	  string val ;
	  
	  cout << "inserer  fichier entier" << endl;
	  cin >> val ;
	  inserer(val,arguments_programme,taille(arguments_programme)+1);
	  
	  cout << "inserer fichier bizarre" << endl;
	  cin >> val ;
	  inserer(val,arguments_programme,taille(arguments_programme)+1);
	  

	  cout << "inserer ecrire" << endl;
	  cin >> val ;
	  inserer(val,arguments_programme,taille(arguments_programme)+1);
	  
	  
	  
  }
  
  
  //string nom_fichier;
   liste<int> cumul;
   liste<Bizarre> testt;
   liste<int> liste_test = {1,2,3,4,5,6,7,9,2,12,56};
  
  
  cumul = lire_entiers(arguments_programme[1]);
  cout << "Entiers lus : " << endl;
  affichage(cumul);
  cout << endl;
  
  testt = lire_bizarre(arguments_programme[2]);
  cout << "Bizarres lus : "<< endl;
  affichage(testt);
  cout << endl;

  ecrire_entiers(arguments_programme[3],liste_test);
  cout << "Ecriture terminée  "<< endl;
  
  return 0;
}
