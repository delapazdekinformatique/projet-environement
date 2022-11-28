#include <iostream>
#include <fstream>
#include "liste.hpp"
using namespace std;


struct Production{

    struct Energie{                      // enregistrement imbriqué à Production qui permet d'avoir la production ainsi que le taux de production 

        double production;               // un double c'est comme un flottant mais en plus précis
        double taux_production = 0;
    };

    int region, mois, jour, heure,importation;  
    Energie thermique, nucleaire, eolien, solaire, hydraulique, bioenergie; // ce qui donne {production,taux_production} pour chaque type de production

};

void taux_de_production_energie(Production & p_r){

/*
    permet de calculer le taux de production de chaque type de 
    production d’une région à partir de l’enregistrement Production
*/



    // on créé la production totale qui est la somme des productions.
    
    int production_totale = 0;
    production_totale += p_r.thermique.production + p_r.nucleaire.production + p_r.eolien.production +p_r.solaire.production + p_r.hydraulique.production + p_r.bioenergie.production ;

    // pour faire le taux on fait : (production * 100) / production_totale.

    p_r.thermique.taux_production   = (p_r.thermique.production * 100) / production_totale;
    p_r.nucleaire.taux_production   = (p_r.nucleaire.production * 100) / production_totale;
    p_r.eolien.taux_production      = (p_r.eolien.production * 100) / production_totale;
    p_r.solaire.taux_production     = (p_r.solaire.production * 100) / production_totale;
    p_r.hydraulique.taux_production = (p_r.hydraulique.production * 100) / production_totale;
    p_r.bioenergie.taux_production  = (p_r.bioenergie.production * 100) / production_totale;

}


liste<Production> lire_production (string fichier, string fichier_cout){

    fstream flux;

    Production production_region;

    liste<Production> liste_production = {};
    int cout_moyen = 0;

    flux.open(fichier, ios::in);
    if (flux.is_open()) {
        flux >> production_region.region;  // première lecture avant le tant que
	    flux >>production_region.mois; 
	    flux >>production_region.jour; 
	    flux >>production_region.heure;

        flux >>production_region.thermique.production; // les productions de chaque moyen de production
        flux >>production_region.nucleaire.production;
        flux >>production_region.eolien.production;
        flux >>production_region.solaire.production;
        flux >>production_region.hydraulique.production;
        flux >>production_region.bioenergie.production;

        flux >>production_region.importation;

        taux_de_production_energie(production_region);


        while (flux.good()) { // vérification que la lecture a été effectuée correctement
       
       
	        inserer(production_region,liste_production,taille(liste_production)+1); // liste
 
            flux >>production_region.region;  
	        flux >>production_region.mois; 
	        flux >>production_region.jour; 
	        flux >>production_region.heure;

            flux >>production_region.thermique.production; //pour acceder au taux : -->production_region.thermique.taux_production (le definir)
            flux >>production_region.nucleaire.production; // les productions de chaque moyen de production
            flux >>production_region.eolien.production;
            flux >>production_region.solaire.production;
            flux >>production_region.hydraulique.production;
            flux >>production_region.bioenergie.production;

            flux >>production_region.importation;

            taux_de_production_energie(production_region);
        }
        flux.close();   
    }
    /*else {
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
    }*/
    
    return liste_production;
}


int afficher (liste<Production> t){

		for (long unsigned int  i = 1; i<= taille(t); i++){
			
			cout << t[i].region << " ";
			cout << t[i].mois<< " ";
			cout << t[i].jour<< " ";
			cout << t[i].heure << " ";

            cout << "- " ;

            cout << t[i].thermique.taux_production<< " ";
			cout << t[i].nucleaire.taux_production<< " ";
			cout << t[i].eolien.taux_production << " ";
            cout << t[i].solaire.taux_production<< " ";
			cout << t[i].hydraulique.taux_production<< " ";
			cout << t[i].bioenergie.taux_production << " ";

            cout << "- " ;

            cout << t[i].importation ;
			cout << endl;
			
		
		}

    return 0;

}

int main(){

    liste<Production> liste_p= {};

    liste_p = lire_production("t6.txt","couts.txt");
    afficher(liste_p);

    return 0;

}

