#include "FRLZSI.hpp"
#include <iostream>
#include <algorithm>

using namespace sdsl;
using namespace std;

/*Konstruktor*/
FRLZSI::FRLZSI(string r, vector<string> &s){
	getLempelZivFactors(r,s);	//m_t_array initialisieren
	construct_im(m_sa, "ACGTGATAG", 1);	//m_sa initialisieren
	g_Array_Sort(); 			//m_g, m_is, m_ie_rmaxq() initialisieren
	d_Strich(d_Array());		//m_ds initialisieren
	
	cout << "-----------------" << endl;
	for(uint64_t i=0; i<m_sa.size(); ++i){
		cout << m_sa[i];
	}
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
	int_vector<> is = {0,0,0,2,3,4,6,7};
	int_vector<> ie = {1,2,3,3,8,5,7,8};
	vector<pair<int,int>> t_array(8);
	for(int i = 0; i<8;i++){
		t_array[i] ={is[i],ie[i]};
	} 
	m_t_array = t_array;
}


/*
 * OLI
*/
void FRLZSI::g_Array_Sort(){
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
	for(int i=0; i<m_t_array.size(); ++i)
	{
		cout << r[i].first.first << ", " << r[i].first.second << ", " << r[i].second << endl; // Ausgabe r
	}

	cout << "----" << endl;
	for(int i=0; i<m_t_array.size(); ++i)
	{
		g[i] = r[i].second;	// da der index ueber den Schleifenindex erzeugt wird muss er noch in g geschrieben werden
		is[i] = r[i].first.first;
		ie[i] = r[i].first.second;
		cout << g[i] << endl;	// Ausgabe g
	}
	m_g = g;
	m_is = is;
	rmq_succinct_sct<false> rmaxq(&ie);
	m_ie_rmaxq = rmaxq;;
}

int_vector<> FRLZSI::d_Array(){
	/*
	* Erwartet Initialisierung von m_is und m_t_array
	* leifert d zur�ck
	*/

	int p = 0; // Aktuelle Stelle in d
	int j = 0; // Laufvariable fuer die maximale Distanz
	int_vector<> d(m_sa.size()-1);	

	uint64_t neuerFaktor = 0;	// max aus ie[j], bei is[j] <= p
	uint64_t neuerFaktor2 = 0;
	while(p<d.size()){

		if(neuerFaktor < p){	// wenn das max aus ie kleiner ist als die Position in d, ist der aktuell L�ngste Faktor zu ende
			neuerFaktor = 0;
		};	
		while(p<m_is[j] && neuerFaktor == 0){	// solange die Position in d echt kleiner ist als der erste Inhalt in is, f�lle d mit 0
			d[p] = 0;
			p++;	
		}

		while(m_is[j]<=p && j<=m_is.size()){	// solange der Start des Faktors kleiner gleich der Stelle in d und j in is
			neuerFaktor2 = m_t_array[m_g[j]].second; //ie[j];
			if(neuerFaktor2 > neuerFaktor){ // wenn die Endposition des aktuellen Faktors gr��er ist als das bisherige Maximum, verwende diesen als neues Maximum
				neuerFaktor = neuerFaktor2;	
			}	
			//cout << p << " " << j << " " << neuerFaktor<< endl;
			j++;	
		}
		while(p<=j){	// solange die aktuelle Position in d kleiner ist als die aktuelle Stelle in is, schreibe in d
			d[p] = neuerFaktor-p+1;	
			p++;
		}	
	}
	return d;	
}

void FRLZSI::d_Strich(int_vector<> d){
	int_vector<> d_1(d.size());
	for(int i = 0; i < d.size();i++){
		d_1[i]=(d[m_sa[i+1]]);	// Berechnung von d', Laenge des laengsten Intervalls an der Position SAr[i]
	}
	rmq_succinct_sct<false> rmaxq(&d_1);
	m_ds_rmaxq = rmaxq;
}


/*
 * SANDRA
*/
/*oeffentlich aufrufbare Methode zum auffinden eines Patterns in S*/
void FRLZSI::search_pattern(string pattern){
	uint64_t i=0, j=m_sa.size()-1, startIndex=0, endIndex=0;
	backward_search(m_sa, i, j, pattern.begin(), pattern.end(), startIndex, endIndex);	// Rueckwaertssuche => startIndex, endIndex
	searchPattern(startIndex-1, endIndex-1, pattern.size());
}

/*sucht solange das Maximum in d_Strich bis Faktorlaenge < Patternlaenge und berechnet jeweils die zugehoerigen Faktoren mittels getFactors.*/
void FRLZSI::searchPattern(uint64_t st,uint64_t ed, uint64_t patternLength){
	uint64_t maxPosition = m_ds_rmaxq(st,ed);	//Maximum von d_Strich innerhalb des Bereichst st,ed
	
	//Berechnung des Wertes d_Strich[maxPosition]
	int_vector<>::iterator up;
	up = upper_bound(m_is.begin(), m_is.end(), m_sa[maxPosition+1]);	//endIndex f�r rmaxq in ie (binaere Suche)
	uint64_t gIndex = m_ie_rmaxq(0,up-m_is.begin()-1);
	uint64_t dsValue = m_t_array[m_g[gIndex]].second - m_sa[maxPosition+1] + 1;
	
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
	if(m_t_array[m_g[factorPosition]].second >= startIndex+patternLength-1){	//Abbruch falls Pattern nicht mehr im Faktor liegt
		cout << "Faktor: " << m_g[factorPosition] << " " << m_is[factorPosition]-startIndex << "-" << m_is[factorPosition]-startIndex+patternLength-1 << endl;
		if(factorPosition < ieEndIndex){
			getFactors(startIndex, patternLength, factorPosition+1, ieEndIndex);
		}
		if(factorPosition > ieStartIndex){
			getFactors(startIndex, patternLength, ieStartIndex, factorPosition-1);
		}
   }
}
