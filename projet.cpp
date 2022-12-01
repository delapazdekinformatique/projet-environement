#include <iostream>
#include <fstream>
#include "liste.hpp"
using namespace std;


// STRUCTURES DE DONNEES


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

struct Couts{
	int cout_thermique, cout_nucleaire, cout_eolien, cout_solaire, cout_hydraulique, cout_bioenergie;
};






// FONCTIONS ET PROCEDURES





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




float couts_moyen(Production p_r, Couts cout) {
	float resultats = (cout.cout_thermique * (p_r.thermique.taux_production/100)) + (cout.cout_nucleaire* (p_r.nucleaire.taux_production/100)) + (cout.cout_eolien*(p_r.eolien.taux_production/100)) + (cout.cout_solaire * (p_r.solaire.taux_production/100)) + (cout.cout_hydraulique *  (p_r.hydraulique.taux_production/100)) + (cout.cout_bioenergie *  (p_r.bioenergie.taux_production/100));
	return resultats;
}




int cout_marginal_regional(Production regionale,tache_calcul tache_de_calcul, Couts couts){
	
	int cout_marginale = 0;
	
	if(regionale.thermique.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
		cout_marginale = couts.cout_thermique; // 560
		}
	else{
		if(regionale.bioenergie.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
			cout_marginale = couts.cout_bioenergie; // 230
			}
		else{
			if(regionale.solaire.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
				cout_marginale = couts.cout_solaire; // 27
				}
			else{
				if(regionale.hydraulique.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
					cout_marginale = couts.cout_hydraulique; // 24
					}
				else{
					if(regionale.nucleaire.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
						cout_marginale = couts.cout_nucleaire; // 12
						}
					else{
						if(regionale.eolien.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
							cout_marginale = couts.cout_eolien; // 11
							}
						}
					}
				}
			}
		}
	
	return cout_marginale;
}


// ALGORITHMES POUR LIRE LES FICHIERS


liste<Production> lire_production (string fichier,Couts couts,tache_calcul tache_de_calcul){

	/* Paramètres :
	
	fichier: permet de lire le fichier des productions (celui de 100 000+ lignes)
	couts : c'est l'enregistrement permettant des lire les couts
	tache_de_calcul : c'est l'enegistrement permettant de lire la feuille de calcul.
	
	*/


    fstream flux;

    Production production_region;
    liste<Production> liste_production = {};


    int prod_totale_nation = 0; // la production tôtale des 12 régions
    int echanges_totaux = 0; // les échanges totaux des 12 régions
	int cout_marginal = 0; // le cout marginal d'une région
	float cout_moyen = 0; //le cout moyen des productions d'une region 

    flux.open(fichier, ios::in);
    if (flux.is_open()) {
        int prod_totale_region = 0;
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

        taux_de_production_energie(production_region,prod_totale_region); // cette procedure permet d'avoirs les taux de productions et de récuperer la production totale
        prod_totale_nation += prod_totale_region; // c'est la production totale de toutes les régions, pas seulement d'une seule région
        echanges_totaux += production_region.importation.production; // pour calculer les echanges physiques totaux

		cout_moyen = couts_moyen(production_region,couts);
		cout << cout_moyen << " - ";

		cout_marginal = cout_marginal_regional(production_region,tache_de_calcul,couts); // cette fonction permet de calculer le cout marginal de la region.
		cout << cout_marginal << endl;
        
		//cout << echanges_totaux << endl;

		// on a le cout moyen , le cout marginal , le taux de production regional et il manque que le taux de production national. 
		// après ça il faudra créer une fonction qui s'assure que toutes les contraintes sont respectées. SI c'est le cas on renvoit true et on insere la région dans une liste 
		// prévue pour elle avec le mois, le jour, l'heure, l'id de la région et le cout moyen. SI c'est pas le cas on renvoie false et on passe simplement à la région suivante.
		
        


        while (flux.good()) { // vérification que la lecture a été effectuée correctement
       
       
	        inserer(production_region,liste_production,taille(liste_production)+1); // liste
            int prod_totale_region = 0;
 
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

            taux_de_production_energie(production_region,prod_totale_region); // cette procedure permet aussi de récuperer la production totale
            prod_totale_nation += prod_totale_region;
            echanges_totaux += production_region.importation.production;
			
			cout_moyen = couts_moyen(production_region,couts);
			cout << cout_moyen << " - ";

			cout_marginal = cout_marginal_regional(production_region,tache_de_calcul,couts); // cette fonction permet de calculer le cout marginal de la region.
			cout << cout_marginal << endl;
            
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







Couts lire_couts(string fichier){
	Couts couts;
	fstream flux;
	flux.open(fichier, ios::in);
	
	if (flux.is_open()) {
		
		flux >> couts.cout_thermique;
		flux >> couts.cout_nucleaire;
		flux >> couts.cout_eolien;
		flux >> couts.cout_solaire;
		flux >> couts.cout_hydraulique;
		flux >> couts.cout_bioenergie;

		while (flux.good()) {

			flux >> couts.cout_thermique;
			flux >> couts.cout_nucleaire;
			flux >> couts.cout_eolien;
			flux >> couts.cout_solaire;
			flux >> couts.cout_hydraulique;
			flux >> couts.cout_bioenergie;
		}
		flux.close();
	}
	else {
		cout << "Erreur : impossible d'ouvrir " << fichier << endl;
	}
	return couts;
	
}



// AFFICHAGES



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
	Couts couts_productions = lire_couts("couts.txt");



    string nom_fichier = "tache_deb.txt";
    tache_calcul t = lire_tache_calcul(nom_fichier);
   // afficher_tache_calcul(t);

    liste_p = lire_production("t6.txt",couts_productions,t);
    afficher(liste_p);
    

    return 0;

}

