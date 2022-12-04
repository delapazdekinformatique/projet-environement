#include <iostream>
#include <fstream>
#include "liste.hpp"
using namespace std;


// STRUCTURES DE DONNEES //


struct Production{

    struct Energie{                      // enregistrement imbriqué à Production qui permet d'avoir la production ainsi que le taux de production 

        double production;               // un double c'est comme un flottant mais en plus précis
        double taux_production = 0;
    };

    int region, mois, jour, heure;  
    Energie thermique, nucleaire, eolien, solaire, hydraulique, bioenergie,importation; // ce qui donne {production,taux_production} pour chaque type de production
};


struct tache_calcul{ // enregistrement qui contient les données de la feuille de calcul (nombre de régions, cout marginal , etc)

	int identifiant,mois_depart,jour_depart,horaire_depart,mois_terminaison,jour_terminaison,horaire_terminaison;
	string nom;
	float duree,cout_moyen_maximum,cout_marginal_maximum,pourcentage_minimum_production_marginale,pourcentage_maximal_importation,pourcentage_maximal_importation_nationale;
    liste<int> region;	

};

struct Couts{  // enregistrement qui contient les couts de productions de chaque énergie
	int cout_thermique, cout_nucleaire, cout_eolien, cout_solaire, cout_hydraulique, cout_bioenergie;
};

struct Regions{ // enregistrement qui contient des liste d'enregistrement Production. chaque liste correspond à une des 12 régions

	liste<Production> ile_de_france = {};
	liste<Production> centre_val_de_loire = {};
	liste<Production> bourgogne_franche_comte = {};
	liste<Production> normandie = {};
	liste<Production> hauts_de_france = {};
	liste<Production> grand_est = {};
	liste<Production> pays_de_la_loire = {};
	liste<Production> bretagne = {};
	liste<Production> nouvelle_aquitaine = {};
	liste<Production> occitanie = {};
	liste<Production> auvergne_rhone_alpes = {};
	liste<Production> provence_alpes_cote_d_azur = {};

};






// FONCTIONS ET PROCEDURES //





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

	// le if-else sert à pour le taux importation, en fonction de si c'est de l'exportation ou de l'importation, on change le dénominateur

    if (p_r.importation.production >= 0){
        p_r.importation.taux_production = (p_r.importation.production * 100 ) / production_totale_echanges; // importation
    }
    else {
         p_r.importation.taux_production = (p_r.importation.production * 100 ) / production_totale; // exportation
    }
}


float echanges_nationaux (int echanges_totaux, int production_totale) {
	float resultats = (echanges_totaux*100) / production_totale;
	return resultats;
}


float couts_moyen(Production p_r, Couts cout) { // renvoie le cout de production moyen d'une region 
	float resultats = (cout.cout_thermique * (p_r.thermique.taux_production/100)) + (cout.cout_nucleaire* (p_r.nucleaire.taux_production/100)) + (cout.cout_eolien*(p_r.eolien.taux_production/100)) + (cout.cout_solaire * (p_r.solaire.taux_production/100)) + (cout.cout_hydraulique *  (p_r.hydraulique.taux_production/100)) + (cout.cout_bioenergie *  (p_r.bioenergie.taux_production/100));
	return resultats;
}




int cout_marginal_regional(Production regionale,tache_calcul tache_de_calcul, Couts couts){ // renvoie le cout marginal d'une region en fonction de la tache de calcul
	
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


bool contraintes(Production production ,tache_calcul tache_de_calcul, int cout_marg, float cout_moy,int production_region){

	// les différentes contraintes sont : 
	// 1. le cout moyen
	// 2. le cout marginal
	// 3. le taux d'importation
	// 4. la date, jour et heure
	// Si toutes les contraintes sont respectées, la fonction renvoie true

	bool conditions = false;

	if (cout_moy < tache_de_calcul.cout_moyen_maximum){

		if (cout_marg < tache_de_calcul.cout_marginal_maximum){

			if (production_region >= tache_de_calcul.pourcentage_maximal_importation){

				if (production.mois >= tache_de_calcul.mois_depart and production.mois <= tache_de_calcul.mois_terminaison){

					if (production.jour >= tache_de_calcul.jour_depart and production.jour <= tache_de_calcul.jour_terminaison){
	
						if (production.heure >= tache_de_calcul.horaire_depart and production.heure <= tache_de_calcul.horaire_terminaison){

							conditions = true;
					
						}
					}	
				}
			}
		}
	}

	return conditions;
}

void insere_region (Production p_r, Regions & r, tache_calcul tache_de_calcul){

	switch(p_r.region){ // en fonction de l'id de la région, on a un cas différent, à savoir s'inserer dans une des 12 listes des régions

		case 1 :
			if (taille(r.ile_de_france) < tache_de_calcul.duree){ // ce if permet de stoper l'insertion lorsqu'on a dépasse la limite de calcul pour cette région
				inserer(p_r, r.ile_de_france, taille(r.ile_de_france)+1);
			}
			break;

		case 2:
			if (taille(r.centre_val_de_loire) < tache_de_calcul.duree){
				inserer(p_r, r.centre_val_de_loire, taille(r.centre_val_de_loire)+1);
			}
			break;

		case 3:
			if (taille(r.bourgogne_franche_comte) < tache_de_calcul.duree){
				inserer(p_r, r.bourgogne_franche_comte, taille(r.bourgogne_franche_comte)+1);
			}
			break;

		case 4:
			if (taille(r.normandie) < tache_de_calcul.duree){
				inserer(p_r, r.normandie, taille(r.normandie)+1);
			}
			break;

		case 5:
			if (taille(r.hauts_de_france) < tache_de_calcul.duree){
				inserer(p_r, r.hauts_de_france, taille(r.hauts_de_france)+1);
			}
			break;
			
		case 6:
			if (taille(r.grand_est) < tache_de_calcul.duree){
				inserer(p_r, r.grand_est, taille(r.grand_est)+1);
			}
			break;

		case 7:
			if (taille(r.pays_de_la_loire) < tache_de_calcul.duree){
				inserer(p_r, r.pays_de_la_loire, taille(r.pays_de_la_loire)+1);
			}
			break;

		case 8:
			if (taille(r.bretagne) < tache_de_calcul.duree){
				inserer(p_r, r.bretagne, taille(r.bretagne)+1);
			}
			break;

		case 9:
			if (taille(r.nouvelle_aquitaine) < tache_de_calcul.duree){
				inserer(p_r, r.nouvelle_aquitaine, taille(r.nouvelle_aquitaine)+1);
			}
			break;

		case 10:
			if (taille(r.occitanie) < tache_de_calcul.duree){
				inserer(p_r, r.occitanie, taille(r.occitanie)+1);
			}
			break;

		case 11:
			if (taille(r.auvergne_rhone_alpes) < tache_de_calcul.duree){
				inserer(p_r, r.auvergne_rhone_alpes, taille(r.auvergne_rhone_alpes)+1);
			}
			break;

		case 12:
			if (taille(r.provence_alpes_cote_d_azur) < tache_de_calcul.duree){
				inserer(p_r, r.provence_alpes_cote_d_azur, taille(r.provence_alpes_cote_d_azur)+1);
			}
			break;

	}


}

// ALGORITHMES POUR LIRE LES FICHIERS //


Regions lire_production (string fichier,Couts couts,tache_calcul tache_de_calcul){

	/* Paramètres :
	
	fichier: permet de lire le fichier des productions (celui de 100 000+ lignes)
	couts : c'est l'enregistrement permettant des lire les couts
	tache_de_calcul : c'est l'enegistrement permettant de lire la feuille de calcul.
	
	*/


    fstream flux;
    Production production_region;
	Regions regions;


    int prod_totale_nation = 0; // la production tôtale des 12 régions
    int echanges_totaux = 0; // les échanges totaux des 12 régions
	int cout_marginal = 0; // le cout marginal d'une région
	float cout_moyen = 0; //le cout moyen des productions d'une region 
	
	float importation_nationale = 0; // importation nationale 
	int nombre_regions = taille(tache_de_calcul.region);
    int prod_totale_region = 0; // la production totale d'une région qui est initialisée à 0
	int region_compteur = 1 ; // il va s'incrementer à chaque fois qu'on calcul une nouvelle region jusqu'a ce qu'on fasse toutes les régions

	bool depassement_date = false;

	// Ce qu'on va faire c'est d'augmenter le importation_nationale autant de fois que la valeur de nombre_regions (ex: si il y a 6 régions, on va calculer l'importation nationale 6 fois)

    flux.open(fichier, ios::in);
    if (flux.is_open()) {

		

        flux >>production_region.region;  // première lecture avant le tant que
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
		region_compteur ++;

		//cout << prod_totale_nation << endl;

        echanges_totaux += production_region.importation.production; // pour calculer les echanges physiques totaux

		cout_moyen = couts_moyen(production_region,couts); // calcul du cout moyen

		cout_marginal = cout_marginal_regional(production_region,tache_de_calcul,couts); // cette fonction permet de calculer le cout marginal de la region.


        while (flux.good()) { // vérification que la lecture a été effectuée correctement
       
			
			for (long unsigned int region_id : tache_de_calcul.region){ // on vérifie que l'id de la région est présent dans la liste des régions de la feuille de calcul

				if (region_id == production_region.region and contraintes(production_region,tache_de_calcul,cout_marginal,cout_moyen,prod_totale_region)){ 
					// si c'est le cas, on fait passer le test des contraintes.
					insere_region(production_region, regions, tache_de_calcul);

				}

			}			

			if (region_compteur > nombre_regions){ // la production nationale repasse à 0 quand on a fait le tout de toutes le regions
				prod_totale_nation = 0;
				region_compteur = 1;
				//cout << "---" << endl;
			}

			
            int prod_totale_region = 0;
            flux >>production_region.region; 

	        	flux >>production_region.mois; 
	        	flux >>production_region.jour; 
	        	flux >>production_region.heure;



				if (production_region.mois >= tache_de_calcul.mois_terminaison){
					if(production_region.jour >= tache_de_calcul.mois_terminaison){
						if(production_region.heure > tache_de_calcul.horaire_terminaison){

							depassement_date = true;
							
						}
					}
				}

            	flux >>production_region.thermique.production; //pour acceder au taux : -->production_region.thermique.taux_production (le definir)
            	flux >>production_region.nucleaire.production; // les productions de chaque moyen de production
            	flux >>production_region.eolien.production;
            	flux >>production_region.solaire.production;
            	flux >>production_region.hydraulique.production;
            	flux >>production_region.bioenergie.production;

            	flux >>production_region.importation.production;

            	taux_de_production_energie(production_region,prod_totale_region); // cette procedure permet aussi de récuperer la production totale
            	prod_totale_nation += prod_totale_region;
				region_compteur ++;
            	echanges_totaux += production_region.importation.production;
			
				cout_moyen = couts_moyen(production_region,couts);

				cout_marginal = cout_marginal_regional(production_region,tache_de_calcul,couts); // cette fonction permet de calculer le cout marginal de la region.

				//cout << prod_totale_nation << endl;
		}
			
			
			for (long unsigned int region_id : tache_de_calcul.region){ // on refait ça après le while pour pouvoir inserer la dernière Production.

				if (region_id == production_region.region and contraintes(production_region,tache_de_calcul,cout_marginal,cout_moyen,prod_totale_region)){
					insere_region(production_region, regions, tache_de_calcul);
				}

			if (depassement_date){
				flux.close();	
			}

			}
        flux.close();   
    }
    else {
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
    }
    

    return regions;
}



tache_calcul lire_tache_calcul(string nom_fichier){
	tache_calcul tache_de_calcul;
	fstream flux;
	liste<int> li =  {};
	int nb;
	
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
		flux >> tache_de_calcul.cout_marginal_maximum;
		flux >> tache_de_calcul.pourcentage_minimum_production_marginale;
		flux >> tache_de_calcul.pourcentage_maximal_importation;
		flux >> tache_de_calcul.pourcentage_maximal_importation_nationale;
		flux >> nb;

		while (flux.good()){

			inserer(nb, li, taille(li)+1);

			flux >> nb ;
		}

			tache_de_calcul.region = li;

		
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

		flux.close();
	}
	else {
		cout << "Erreur : impossible d'ouvrir " << fichier << endl;
	}
	return couts;
	
}



// AFFICHAGES //

int afficher(liste<int>l){
	cout<<"<";
	for(int c:l){
		cout<<c<<' ';
	};
	cout<<">"<<endl;

	return 0;
}


/*void afficher_tache_calcul(tache_calcul tache_de_calcul){

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
	cout<<"le cout_marginal_maximum est: "<<tache_de_calcul.cout_marginal_maximum<<endl;
	cout<<"le pourcentage_minimum_production_marginale est: "<<tache_de_calcul.pourcentage_minimum_production_marginale<<endl;
	cout<<"le pourcentage_maximal_importation est: "<<tache_de_calcul.pourcentage_maximal_importation<<endl;
	cout<<"le pourcentage_maximal_importation_nationale est: "<<tache_de_calcul.pourcentage_maximal_importation_nationale<<endl;
	cout<<"les régions concernées sont: ";
	afficher (tache_de_calcul.region);
}
*/

/*int afficher (liste<Production> t){

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

}*/

int afficher_contenu_region (liste<Production> region, int id, Couts couts){ // l'affichage des régions et de leurs valeurs, id sert à determiner la region

	switch (id){

		case 1 :
			if (taille(region) > 0){
			cout << "Ile-de-France" << " " << taille(region) << endl;
			}
			break;

		case 2 :
			if (taille(region) > 0){
			cout << "Centre-Val_de_Loire" << " " << taille(region) << endl;
			}
			break;

		case 3 :
			if (taille(region) > 0){
			cout << "Bourgogne-Franche-Comte" << " " << taille(region) << endl;
			}
			break;

		case 4 :
			if (taille(region) > 0){
			cout << "Normandie" << " " << taille(region) << endl;
			}
			break;

		case 5 :
			if (taille(region) > 0){
			cout << "Hauts-de-France" << " " << taille(region) << endl;
			}
			break;

		case 6 :
			if (taille(region) > 0){
			cout << "Grand_Est" << " " << taille(region) << endl;
			}
			break;

		case 7 :
			if (taille(region) > 0){
			cout << "Pays_de_la_Loire" << " " << taille(region) << endl;
			}
			break;

		case 8 :
			if (taille(region) > 0){
			cout << "Bretagne" << " " << taille(region) << endl;
			}
			break;

		case 9 :
			if (taille(region) > 0){
			cout << "Nouvelle-Aquitaine" << " " << taille(region) << endl;
			}
			break;

		case 10 :
			if (taille(region) > 0){
			cout << "Occitanie" << " " << taille(region) << endl;
			}
			break;

		case 11 :
			if (taille(region) > 0){
			cout << "Auvergne-Rhone-Alpes" << " " << taille(region) << endl;
			}
			break;

		case 12 :
			if (taille(region) > 0){
			cout << "Provence-Alpes-Cote_d'Azur" << " " << taille(region) << endl;
			}
			break;

	}

	for (Production ele : region){

		cout << ele.mois << " " << ele.jour << " " << ele.heure << " " << ele.region << " " << couts_moyen(ele, couts) << endl;

	}	

	return 0;
}


int afficher_regions (Regions r,Couts couts){ // on utilise la fonction precedente 

	afficher_contenu_region(r.ile_de_france,1,couts);
	afficher_contenu_region(r.centre_val_de_loire,2,couts);
	afficher_contenu_region(r.bourgogne_franche_comte,3,couts);
	afficher_contenu_region(r.normandie,4,couts);
	afficher_contenu_region(r.hauts_de_france,5,couts);
	afficher_contenu_region(r.grand_est,6,couts);
	afficher_contenu_region(r.pays_de_la_loire,7,couts);
	afficher_contenu_region(r.bretagne,8,couts);
	afficher_contenu_region(r.nouvelle_aquitaine,9,couts);
	afficher_contenu_region(r.occitanie,10,couts);
	afficher_contenu_region(r.auvergne_rhone_alpes,11,couts);
	afficher_contenu_region(r.provence_alpes_cote_d_azur,12,couts);

	return 0;

}

int main(){

    Regions mes_regions ;
	Couts couts_productions = lire_couts("couts.txt");

    string nom_fichier = "tache_deb.txt";
    tache_calcul t = lire_tache_calcul(nom_fichier);

    mes_regions = lire_production("t5.ssv",couts_productions,t);
    afficher_regions(mes_regions,couts_productions);
    

    return 0;

}

