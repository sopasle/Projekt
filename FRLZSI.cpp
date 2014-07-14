#include "FRLZSI.hpp"
#include <iostream>
#include <algorithm>

using namespace sdsl;
using namespace std;


/*Default Konstruktor*/

FRLZSI::FRLZSI(){
}


/*Konstruktor*/
FRLZSI::FRLZSI(string r, vector<string> &s){
	getLempelZivFactors(r,s);	//m_t_array initialisieren
	construct_im(m_sa, "ACGTGATAG", 1);	//m_sa initialisieren
	g_Array(); 			//m_g, m_is, m_ie_rmaxq() initialisieren
	d_Array();
	//d_Strich(d_Array());		//m_ds initialisieren
	d_ArrayTest();
	
}

/*Destruktor*/
FRLZSI::~FRLZSI(){
}


/*Methoden*/
/*
 * HARRY
*/
void FRLZSI::getLempelZivFactors(string r, vector<string> &s){
	/*Zum Testen erstmal mit den Werten vom Beispiel initialisieren*/
	int_vector<> is = {0,0,0,7,4,2,6,3}; // Blattbsp
	int_vector<> ie = {1,2,3,8,5,3,7,8}; // Blattbsp
	//int_vector<> is = {0,0,0,0,0,0,1,2,2,2,9,12,14};	// Zufallsbsp
	//int_vector<> ie = {1,2,4,5,7,13,14,5,8,17,10,28,15};	// Zufallsbsp
	vector<pair<int,int>> t_array(is.size());
	for(int i = 0; i<is.size();i++){
		t_array[i] ={is[i],ie[i]};
	} 
	m_t_array = t_array;

}


/*
 * OLI
*/
void FRLZSI::g_Array(){
	vector<pair<pair<int,int>,int > > r(m_t_array.size()); // Vektor als ((is,ie),g)
	vector<pair<int,int> > r1(m_t_array.size()); // Vektor (is,ie)
	int_vector<> g(m_t_array.size());
	int_vector<> is(m_t_array.size());
	int_vector<> ie(m_t_array.size());
	for(int i = 0; i<m_t_array.size();i++){	
		r1[i] = {m_t_array[i].first, m_t_array[i].second};	// r1 wird aus is,ie zusammengesetzt, entspricht nacher dem r Vektor, der aus der Faktorensuche entsteht
		r[i] = {r1[i],i+1};	// r wird aus r1,g zusammengesetzt, inhalt in g entspricht dem index des t arrays, das aus der Faktorensuche entsteht
	}
	sort(r.begin(), r.end());	// r wird nach dem ersten Faktor sortiert, der erste Faktor ist r1, da es wieder ein pair ist wird nach is stabil sortiert
	/*for(int i=0; i<m_t_array.size(); ++i)
	{
		cout << r[i].first.first << ", " << r[i].first.second << ", " << r[i].second << endl; // Ausgabe r
	}*/

	//cout << "----" << endl;
	for(int i=0; i<m_t_array.size(); ++i)
	{
		g[i] = r[i].second;	// da der index ueber den Schleifenindex erzeugt wird muss er noch in g geschrieben werden
		is[i] = r[i].first.first;
		ie[i] = r[i].first.second;
		//cout << g[i] << endl;	// Ausgabe g
	}
	m_g = g;
	m_is = is;
	rmq_succinct_sct<false> rmaxq(&ie);
	m_ie_rmaxq = rmaxq;
}

int_vector<> FRLZSI::d_Array(){
	/*
	* Erwartet Initialisierung von m_is, m_ie_rmaxq und m_t_array
	* erstellt d
	*/
	int p = 0; // Aktuelle Stelle in d
	int j = 0; // Laufvariable fuer die maximale Distanz
	int_vector<> d(m_sa.size()-1);	
	uint64_t posmax = 0;	// Position des Maximums an bis zur aktuellen Stelle
	while(p<d.size()){
		while(m_is[j]<=p && j<m_is.size()){ 
		j++;
		}
		posmax = m_ie_rmaxq(0,j-1);
		//cout << p << " " << j << " " << neuerFaktor << endl;
		d[p] = m_t_array[m_g[posmax]-1].second - p+1;
		p++;	
	}
	cout << "d:  ";
	for(int i = 0;i<d.size();i++){
		cout << d[i] << ",";
	}
	cout << endl;
	return d;	
}

void FRLZSI::d_Strich(int_vector<> d){
	int_vector<> d_1(d.size());
	for(int i = 0; i < d.size();i++){
		d_1[i]=(d[m_sa[i+1]]);	// Berechnung von d', Laenge des laengsten Intervalls an der Position SAr[i]
	}
	m_ds = d_1;
	rmq_succinct_sct<false> rmaxq(&d_1);
	m_ds_rmaxq = rmaxq;
}


/*
 * SANDRA
*/
/*oeffentlich aufrufbare Methode zum auffinden eines Patterns in S*/
void FRLZSI::search_pattern(string pattern){
	uint64_t i=0, j=m_sa.size()-1, l_res=0, r_res=0;
	backward_search(m_sa, i, j, pattern.begin(), pattern.end(), l_res, r_res);	// Rueckwaertssuche => startIndex, endIndex
	if(l_res <= r_res && r_res <= m_sa.size()-1){	//Pattern existiert in R
		searchPattern(l_res-1, r_res-1, pattern.size());
	}else{
		cout << pattern << " existiert nicht" << endl;	//Vorlaeufig fuer Case1
	}
}

/*sucht solange das Maximum in d_Strich bis Faktorlaenge < Patternlaenge und berechnet jeweils die zugehoerigen Faktoren mittels getFactors.*/
void FRLZSI::searchPattern(uint64_t st,uint64_t ed, uint64_t patternLength){
	uint64_t maxPosition = m_ds_rmaxq(st,ed);	//Maximum von d_Strich innerhalb des Bereichst st,ed
	
	//Berechnung des Wertes d_Strich[maxPosition]
	int_vector<>::iterator up;
	up = upper_bound(m_is.begin(), m_is.end(), m_sa[maxPosition+1]);	//endIndex für rmaxq in ie (binaere Suche)
	uint64_t gIndex = m_ie_rmaxq(0,up-m_is.begin()-1);
	uint64_t dsValue = m_t_array[m_g[gIndex]-1].second - m_sa[maxPosition+1] + 1;

	if(dsValue >= patternLength){	//Abbruch, wenn Faktorlaenge < Patternlaenge
		getFactors(m_sa[maxPosition+1], patternLength, 0, up-m_is.begin()-1);
		if(st < maxPosition)
			searchPattern(st, maxPosition-1, patternLength);
		if(ed > maxPosition)
			searchPattern(maxPosition+1, ed, patternLength);
	}
}

/*sucht das Maximum in ie und gibt den Faktor aus, falls ie-Wert >= startIndex+Patternlaenge-1*/
void FRLZSI::getFactors(uint64_t startIndex, uint64_t patternLength, uint64_t ieStartIndex, uint64_t ieEndIndex){
	uint64_t factorPosition = m_ie_rmaxq(ieStartIndex,ieEndIndex);	//naechster moeglicher Faktor beim Maximum von ie
	if(m_t_array[m_g[factorPosition]-1].second >= startIndex+patternLength-1){	//Abbruch falls Pattern nicht mehr im Faktor liegt
		cout << "Faktor: " << m_g[factorPosition] << " " << startIndex-m_is[factorPosition] << "-" << startIndex-m_is[factorPosition]+patternLength-1 << endl;
		if(factorPosition < ieEndIndex){
			getFactors(startIndex, patternLength, factorPosition+1, ieEndIndex);
		}
		if(factorPosition > ieStartIndex){
			getFactors(startIndex, patternLength, ieStartIndex, factorPosition-1);
		}
  	 }
}
/*
 * Testmethoden
*/


void FRLZSI::test_ausgabe(){
	/*for(int i = 0;i<m_g.size();i++){
		cout << "g: " << m_g[i] << endl;
	}*/

	cout << "sa: ";
	for(int i = 1;i<m_sa.size();i++){
		cout << m_sa[i]+1 << ",";
	}
	cout << endl;

	cout << "ds: ";
	for(int i = 0;i<m_ds.size();i++){
		cout << m_ds[i] << ",";
	}
	cout << endl;

}

void FRLZSI::d_ArrayTest(){
/*
	* Erwartet Initialisierung von m_is und m_t_array
	* erstellt d
	*/
	//int_vector<> ie = {1,2,3,3,8,5,7,8};
	int i = 0; // Höchster Wert der Reichweite
 	int j = 0; // Laufvariable fuer die maximale Distanz
	int p = 0; // Aktuelle Stelle in d
	int_vector<> d(m_sa.size()-1);	
	while(p<d.size()){
		while(m_is[j]>p){ // Solange kein Faktor da ist, wird 0 geschrieben
			d[p] = 0;
			p++;
		}
		while(m_is[j] <= p && j < m_t_array.size()){ 	// solange der Anfang der Faktoren kleiner ist als die aktuelle Stelle und kleiner ist als die Gesamtlänge
			if(i < m_t_array[m_g[j]-1].second-p+1){		// Abfrage ob aktuell höchster Wert kleiner ist als der Wert an der neuen Stelle, eins weiter
				i = m_t_array[m_g[j]-1].second-p+1;		// Falls ja, wird der höhere Wert übernommen
				if(i<0){		// Falls der Wert kleiner als 0 wird, muss er auf 0 gesetzt werden, da ansonsten der Vergleich fehlschlägt
					i = 0;
				}
			}
			j++;
			
		}
		j = 0;					// Durch j=0 wird immer vom Anfang an geprüft
		d[p] = i;				// Speichern des höchsten Wertes an der aktuellen Stelle	
		i = 0;					// Rücksetzen des höchsten Werts
		p++;
	}


	cout << "dT: ";
	for(int i = 0;i<d.size();i++){
		cout << d[i] << ",";
	}
	cout << endl;	
}
