#include <iostream>
#include <fstream>
#include "liste.hpp"
using namespace std;

struct Production{

    struct Energie{                      // enregistrement imbriqué à Production qui permet d'avoir la production ainsi que le taux de production 

        double production;               // un double c'est comme un flottant mais en plus précis
        double taux_production = 0;
    };

    int region, mois, jour, heure;  
    Energie thermique, nucleaire, eolien, solaire, hydraulique, bioenergie,importation; // ce qui donne {production,taux_production} pour chaque type de production
};


struct tache_calcul{

	int identifiant,mois_depart,jour_depart,horaire_depart,mois_terminaison,jour_terminaison,horaire_terminaison;
	string nom;
	float duree,cout_moyen_maximum,cout_marginale_maximum,pourcentage_minimum_production_marginale,pourcentage_maximal_importation,pourcentage_maximal_importation_nationale;
    liste<int> region;	

};

void taux_de_production_energie(Production & p_r, int & production_totale){

/*
    permet de calculer le taux de production de chaque type de 
    production d’une région à partir de l’enregistrement Production
*/
    // on créé la production totale qui est la somme des productions.
    
    
    int production_totale_echanges = 0;
    production_totale += p_r.thermique.production + p_r.nucleaire.production + p_r.eolien.production +p_r.solaire.production + p_r.hydraulique.production + p_r.bioenergie.production ;
    production_totale_echanges += p_r.importation.production + production_totale;

    // pour faire le taux on fait : (production * 100) / production_totale.

    p_r.thermique.taux_production   = (p_r.thermique.production * 100) / production_totale;
    p_r.nucleaire.taux_production   = (p_r.nucleaire.production * 100) / production_totale;
    p_r.eolien.taux_production      = (p_r.eolien.production * 100) / production_totale;
    p_r.solaire.taux_production     = (p_r.solaire.production * 100) / production_totale;
    p_r.hydraulique.taux_production = (p_r.hydraulique.production * 100) / production_totale;
    p_r.bioenergie.taux_production  = (p_r.bioenergie.production * 100) / production_totale;

    if (p_r.importation.production >= 0){
        p_r.importation.taux_production = (p_r.importation.production * 100 ) / production_totale_echanges;
    }
    else {
         p_r.importation.taux_production = (p_r.importation.production * 100 ) / production_totale;
    }
}

int cout_marginale_regionale(Production regionale,tache_calcul tache_de_calcul){
	
	int cout_marginale = 0;
	
	if(regionale.thermique.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
		cout_marginale = 530;
		}
	else{
		if(regionale.bioenergie.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
			cout_marginale = 230;
			}
		else{
			if(regionale.solaire.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
				cout_marginale = 27;
				}
			else{
				if(regionale.hydraulique.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
					cout_marginale = 24;
					}
				else{
					if(regionale.nucleaire.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
						cout_marginale = 12;
						}
					else{
						if(regionale.eolien.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
							cout_marginale = 11;
							}
						}
					}
				}
			}
		}
	
	return cout_marginale;
}

liste<Production> lire_production (string fichier, string fichier_cout){

    fstream flux;

    Production production_region;
    liste<Production> liste_production = {};


    int prod_totale = 0; // la production tôtale des 12 régions
    int echanges_totaux = 0; // les échanges totaux des 12 régions
    //liste<double> liste_taux ={};

    flux.open(fichier, ios::in);
    if (flux.is_open()) {
        int prod_temp = 0;
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

        flux >>production_region.importation.production;

        taux_de_production_energie(production_region,prod_temp); // cette procedure permet aussi de récuperer la production totale
        prod_totale += prod_temp;
        echanges_totaux += production_region.importation.production;
        cout << echanges_totaux << endl;
        //inserer(production_region.importation.production,liste_taux,taille(liste_taux)+1); // on mets les taux d'échanges physiques dedans


        while (flux.good()) { // vérification que la lecture a été effectuée correctement
       
       
	        inserer(production_region,liste_production,taille(liste_production)+1); // liste
            int prod_temp = 0;
 
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

            flux >>production_region.importation.production;

            taux_de_production_energie(production_region,prod_temp); // cette procedure permet aussi de récuperer la production totale
            prod_totale += prod_temp;
            echanges_totaux += production_region.importation.production;
            
            //inserer(production_region.importation.production,liste_taux,taille(liste_taux)+1); // on mets les taux d'échanges physiques dedans
        }

            inserer(production_region,liste_production,taille(liste_production)+1); // on met ici la dernière valeure
        flux.close();   
    }
    else {
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
    }
    

    return liste_production;
}

tache_calcul lire_tache_calcul(string nom_fichier){
	tache_calcul tache_de_calcul;
	fstream flux;
	
	flux.open(nom_fichier, ios::in);
	if (flux.is_open()) {
		flux >> tache_de_calcul.identifiant;  // première lecture avant le tant que
		flux >> tache_de_calcul.nom;
		flux >> tache_de_calcul.duree;
		flux >> tache_de_calcul.mois_depart;
		flux >> tache_de_calcul.jour_depart;
		flux >> tache_de_calcul.horaire_depart;
		flux >> tache_de_calcul.mois_terminaison;
		flux >> tache_de_calcul.jour_terminaison;
		flux >> tache_de_calcul.horaire_terminaison;
		flux >> tache_de_calcul.cout_moyen_maximum;
		flux >> tache_de_calcul.cout_marginale_maximum;
		flux >> tache_de_calcul.pourcentage_minimum_production_marginale;
		flux >> tache_de_calcul.pourcentage_maximal_importation;
		flux >> tache_de_calcul.pourcentage_maximal_importation_nationale;
		while (flux.good()) {
			flux >> tache_de_calcul.identifiant;
		    flux >> tache_de_calcul.nom;
		    flux >> tache_de_calcul.duree;
		    flux >> tache_de_calcul.mois_depart;
		    flux >> tache_de_calcul.jour_depart;
		    flux >> tache_de_calcul.horaire_depart;
		    flux >> tache_de_calcul.mois_terminaison;
		    flux >> tache_de_calcul.jour_terminaison;
		    flux >> tache_de_calcul.horaire_terminaison;
		    flux >> tache_de_calcul.cout_moyen_maximum;
		    flux >> tache_de_calcul.cout_marginale_maximum;
		    flux >> tache_de_calcul.pourcentage_minimum_production_marginale;
		    flux >> tache_de_calcul.pourcentage_maximal_importation;
		    flux >> tache_de_calcul.pourcentage_maximal_importation_nationale;
			}
		flux.close();   
	}
	else {
		cout << "Erreur : impossible d'ouvrir " << nom_fichier << endl;
		}
	return tache_de_calcul;
}



void afficher_tache_calcul(tache_calcul tache_de_calcul){
	cout<<"l'identifiant est: "<<tache_de_calcul.identifiant<<endl;
	cout<<"le nom est: "<<tache_de_calcul.nom<<endl;
	cout<<"la duree est: "<<tache_de_calcul.duree<<endl;
	cout<<"le mois_depart est: "<<tache_de_calcul.mois_depart<<endl;
	cout<<"le jour_depart est: "<<tache_de_calcul.jour_depart<<endl;
	cout<<"l'horaire_depart est: "<<tache_de_calcul.horaire_depart<<endl;
	cout<<"le mois_terminaison est: "<<tache_de_calcul.mois_terminaison<<endl;
	cout<<"le jour_terminaison est: "<<tache_de_calcul.jour_terminaison<<endl;
	cout<<"l'horaire_terminaison est: "<<tache_de_calcul.horaire_terminaison<<endl;
	cout<<"le cout_moyen_maximum est: "<<tache_de_calcul.cout_moyen_maximum<<endl;
	cout<<"le cout_marginale_maximum est: "<<tache_de_calcul.cout_marginale_maximum<<endl;
	cout<<"le pourcentage_minimum_production_marginale est: "<<tache_de_calcul.pourcentage_minimum_production_marginale<<endl;
	cout<<"le pourcentage_maximal_importation est: "<<tache_de_calcul.pourcentage_maximal_importation<<endl;
	cout<<"le pourcentage_maximal_importation_nationale est: "<<tache_de_calcul.pourcentage_maximal_importation_nationale<<endl;
}


int afficher (liste<Production> t){

		for (long unsigned int  i = 1; i<= taille(t); i++){
			
			cout << t[i].region << " ";
			cout << t[i].mois<< " ";
			cout << t[i].jour<< " ";
			cout << t[i].heure << " ";

            cout << "| " ;

            cout << t[i].thermique.taux_production<< "| ";
			cout << t[i].nucleaire.taux_production<< "| ";
			cout << t[i].eolien.taux_production << "| ";
            cout << t[i].solaire.taux_production<< "| ";
			cout << t[i].hydraulique.taux_production<< "| ";
			cout << t[i].bioenergie.taux_production << "| ";

            cout << "| " ;

            cout << t[i].importation.taux_production << "| " ;
			cout << endl;
			
		
		}

    return 0;

}

int main(){

    liste<Production> liste_p= {};

    liste_p = lire_production("t6.txt","couts.txt");
    afficher(liste_p);

    string nom_fichier = "tache_deb.txt";
    tache_calcul t = lire_tache_calcul(nom_fichier);
    afficher_tache_calcul(t);
    

    return 0;

}

