#include <iostream>
#include <utility>
#include <cstring>

#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/rmq_support.hpp>



using namespace std;
using namespace sdsl;


/*
 * OLI
*/
int d_Array(int d[], int size_d,int is[],int size_is, int ie[]);
int_vector<> d_Strich(int size_d,csa_wt<> sa, int d[]);
int g_Array(int_vector<> t,int g[], int is[], int ie[]);

/* 
 * SANDRA
 */
void searchPattern(int st,int ed, int_vector<> dStrich, int patternLength, csa_wt<> sa, int g[], int is[], int size_is, int ie[]);
void getFactors(int startIndex, int patternLength, int g[], int is[], int size_is, int ie[]);

int main(){
/*
 * OLI
 */
	int d[9];
	int_vector<> d_1;
	int_vector<> t = {1,1,1,8,5,3,7,4};
	int g[8];// = {1,2,3,6,8,5,7,4};
	int is[8];// = {0,0,0,2,3,4,6,7};
	int ie[] = {1,2,3,3,8,5,7,8};
  
	csa_wt<> sa;
	construct_im(sa, "acgtgatag", 1);	//Eingabe: Referenzstring

	int size_d = sizeof(d) / sizeof(int);
	int size_is = sizeof(is) / sizeof(int);
	d_Array(d,size_d,is,size_is,ie);
	g_Array(t,g,is,ie);
	//d_1 = d_Strich(size_d,SAr,d);
//	for(int i = 0; i< 8; i++){
//	cout << "g: " << is[i] << endl;
//	}
/*
 * SANDRA 
 */


  string pattern = "a";	//Eingabe: Pattern
  
  uint64_t i=0, j=sa.size()-1, startIndex=0, endIndex=0;
  backward_search(sa, i, j, pattern.begin(), pattern.end(), startIndex, endIndex); // Rueckwaertssuche => startIndex, endIndex
  searchPattern(startIndex-1, endIndex-1, d_Strich(size_d,sa,d),pattern.size(), sa, g, is, size_is, ie);



	return 0;
	
}


/*
 * OLI
 */


int d_Array(int d[],int size_d,int is[],int size_is, int ie[]){
	int p = 0; 							// Aktuelle Stelle in d
	int j = 0; 							// Laufvariable für die maximale Distanz
	int j1 = 0;							// Varible für das Rücksetzen von j
	int neuerFaktor = 0;				// Speicher für Faktor an neuer Stelle

	while(p<size_d-1){ 					// solange p kleiner n, p = aktuelle Stelle in d, n = Länge von G, bzw IS und IE
		d[p] = ie[j]-p+1; 				// Berechnung des Faktors an der ersten neuen Stellen, zwingend für den Vergleich
		j++;							// j = Zählvariable in IS, erhöhen, da erste neue Stelle bereits berechnet
		while(is[j]<=p && j<size_is){ 				// solange Startposition des Strings kkleiner p, p = aktuelle Stelle in d
			neuerFaktor = ie[j]-p+1; 	// Berechnung des Faktors an der neuen Stelle, zwingend für den Vergleich
			if(d[p] < neuerFaktor){  	// Abfrage ob der aktuelle Wert keliner ist als der neu Berechnete	
			d[p] = neuerFaktor;			// Falls ja, neuen Wert speichern
			j1 = j;						// aktuelles j Abspeichern, damit nicht jedesmal von Beginn an getestet wird in der while Schleife
			}			
			j++;			
		}
		j = j1;				
		p++;
	
	}
	d[p] = 1;							// An letzter Stelle 1 schreiben, da D nur bis zum vorletzten Element berechnet
										// wird und das letzte immer 1 sein muss, da der letzte String zu Ende ist
	return 0;
}


int_vector<> d_Strich(int size_d,csa_wt<> sa, int d[]){
	int_vector<> d_1(size_d);
	for(int i = 0; i < size_d;i++){
		d_1[i] = d[sa[i+1]];				// Berechnung von d', Länge des längsten Intervalls an der Position SAr[i]
	}
	
	return d_1;
}

int g_Array(int_vector<> t, int g[], int is[], int ie[]){
	
	int minPosition;
	for(int i = 0; i < t.size();i++){ 
		rmq_succinct_sct<> rminq(&t); 		// RMQ von t
		minPosition = rminq(0,t.size()-1);	// start bis endposition
		g[i] = minPosition+1;				// g[i] = j wenn t[j] linkester Faktor
		is[i] = t[minPosition]-1;		
	//	ie[i] = t[minPosition];
		t[minPosition] = 10;				// Anfangspos von Faktor in T ersetzen mit Wert > Länge Referenzstring, damit RMQ keine falschen Werte leifert
	}
	cout << t << endl;
}

/*
 * SANDRA
 */

void searchPattern(int st,int ed, int_vector<> dStrich, int patternLength, csa_wt<> sa, int g[], int is[], int size_is, int ie[]){
  rmq_succinct_sct<false> rmaxq(&dStrich);
  int maxPosition = rmaxq(st,ed);
  if(dStrich[maxPosition] >= patternLength){ //Abbruch, wenn Faktorlänge < Patternlänge
	getFactors(sa[maxPosition+1], patternLength, g, is, size_is, ie);
    if(st < maxPosition) 
      searchPattern(st, maxPosition-1, dStrich, patternLength, sa, g, is, size_is, ie);
    if(ed > maxPosition)
      searchPattern(maxPosition+1, ed, dStrich, patternLength, sa, g, is, size_is, ie);
  }
 }
  
 void getFactors(int startIndex, int patternLength, int g[], int is[], int size_is, int ie[]){	//ToDo: O(1+occ)????
		for(int i = 0; i<size_is; i++){
			if(is[i] <= startIndex){
				if(ie[i] >= startIndex+patternLength-1){
					cout << "Faktor: " << g[i] << endl;
				}
			}
			else{
				break;
			}
		}
}

//ToDo: Faktor -> Startposition im String

