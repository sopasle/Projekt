#include <vector>
#include <cmath>
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
int_vector<> d_Strich(int size_d,int SAr[], int d[]);

/* 
 * SANDRA
 *
void searchPattern(int st,int ed, int_vector<> dStrich, int patternLength, csa_wt<> sa, int g[], int is[], int size_is, int ie[]);
void getFactors(int startIndex, int patternLength, int g[], int is[], int size_is, int ie[]);
*/
int main(){
/*
 * OLI
 */
	int d[9];
	int SAr[9];
	
	int_vector<> d_1;
	int is[8];	
	int ie[8];	

	is[0] = 0;	ie[0] = 1;
	is[1] = 0;	ie[1] = 2;
	is[2] = 0;	ie[2] = 3;
	is[3] = 2;	ie[3] = 3;
	is[4] = 3;	ie[4] = 8;
	is[5] = 4;	ie[5] = 5;
	is[6] = 6;	ie[6] = 7;
	is[7] = 7;	ie[7] = 8;
	
	SAr[0] = 0;
	SAr[1] = 7;
	SAr[2] = 5;
	SAr[3] = 1;
	SAr[4] = 8;
	SAr[5] = 4;
	SAr[6] = 2;
	SAr[7] = 6;
	SAr[8] = 3;
	
	int size_d = sizeof(d) / sizeof(int);
	int size_is = sizeof(is) / sizeof(int);
	d_Array(d,size_d,is,size_is,ie);
	d_1 = d_Strich(size_d,SAr,d);
	for(int i = 0; i< size_d; i++){
	cout << "d: " << d[i]<< " SAr: " << SAr[i]+1 << " d_1: " << d_1[i] << endl;
}
}

/*
 * SANDRA 
 *
  int_vector<> dStrich = {4,2,4,3,1,5,2,3,6};
  int g[] = {1,2,3,6,8,5,7,4};
  int is[] = {0,0,0,2,3,4,6,7};
  //int size_is = sizeof(is)/sizeof(is[0]);
  int ie[] = {1,2,3,3,8,5,7,8};
  
  csa_wt<> sa;
  construct_im(sa, "acgtgatag", 1);	//Eingabe: Referenzstring

  string pattern = "a";	//Eingabe: Pattern
  
  uint64_t i=0, j=sa.size()-1, startIndex=0, endIndex=0;
  backward_search(sa, i, j, pattern.begin(), pattern.end(), startIndex, endIndex); // Rueckwaertssuche => startIndex, endIndex
  searchPattern(startIndex-1, endIndex-1, dStrich,pattern.size(), sa, g, is, size_is, ie);



	return 0;
	
}

*/
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


int_vector<> d_Strich(int size_d,int SAr[], int d[]){
	int_vector<> d_1(size_d);
	for(int i = 0; i < size_d;i++){
		d_1[i] = d[SAr[i]];				// Berechnung von d', Länge des längsten Intervalls an der Position SAr[i]
	}
	
	return d_1;
}


/*
 * SANDRA
 *

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

*/
