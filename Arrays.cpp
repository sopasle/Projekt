#include <iostream>
#include <utility>
#include <cstring>

#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/rmq_support.hpp>

#include <vector>
#include <algorithm>
#include <string>
#include <tuple>

using namespace std;
using namespace sdsl;


/*
 * OLI
*/
int d_Array(int d[], int size_d,int is[],int size_is, int ie[]);
int_vector<> d_Strich(int size_d,csa_wt<> sa, int d[]);
int g_Array(int_vector<> t,int g[], int is[], int ie[]);
int g_Array_Sort(vector<pair<int,int> > t_array, int size_t,int g[], int is[], int ie[]);

/* 
 * SANDRA
 */
void searchPattern(int st,int ed, int_vector<> dStrich, int patternLength, csa_wt<> sa, int g[], int is[], int size_is, int ie[]);
void getFactors(int startIndex, int patternLength, int g[], int is[], int size_is, int ie[]);

/*
 * HARRY
*/
static vector<string> getLempelZivFactors(string R, vector<string> S);
static vector<pair<int,int>> getFactorPosInR(string R, vector<string> factors);

int main(){
/*
 * OLI
 */
	int d[9];
	int_vector<> d_1;
	int_vector<> t = {1,1,1,8,5,3,7,4};
	int g[8];// = {1,2,3,6,8,5,7,4};
	int is[8] = {0,0,0,2,3,4,6,7};
	int ie[] = {1,2,3,3,8,5,7,8};
  
	//csa_wt<> sa;
	//construct_im(sa, "acgtgatag", 1);	//Eingabe: Referenzstring

	int size_d = sizeof(d) / sizeof(int);
	int size_is = sizeof(is) / sizeof(int);
	d_Array(d,size_d,is,size_is,ie);
	//g_Array(t,g,is,ie);

	//d_1 = d_Strich(size_d,SAr,d);
	for(int i = 0; i< 9; i++){
	cout << "d: " << d[i] << endl;
	}
/*
 * SANDRA 
 */

/*
  string pattern = "a";	//Eingabe: Pattern
  
  uint64_t i=0, j=sa.size()-1, startIndex=0, endIndex=0;
  backward_search(sa, i, j, pattern.begin(), pattern.end(), startIndex, endIndex); // Rueckwaertssuche => startIndex, endIndex
  searchPattern(startIndex-1, endIndex-1, d_Strich(size_d,sa,d),pattern.size(), sa, g, is, size_is, ie);
*/


/*
 * HARRY
*/
/*
	string R = "ACGTGATAG"; // Eingabestring
	vector<string> S; // hat alle S-Strings
	
	S.push_back("TGATAGACG");// hier mal mit richtiger Eingabe
	S.push_back("GAGTACTA");
	S.push_back("GTACGT");
	S.push_back("AGGA");
	vector<string> factors = getLempelZivFactors(R, S);
	vector<pair<int, int>> positionen = getFactorPosInR(R, factors);
	// Junge, mach ne for schleife rein :-)
	// sonst kann es zu fehlern kommen 
	/* cout << factors[0] << endl;
	cout << factors[1] << endl;
	cout << factors[2] << endl;
	cout << factors[3] << endl;
	cout << factors[4] << endl;
	cout << factors[5] << endl;
	cout << factors[6] << endl;
	cout << factors[7] << endl; */
	
	// mit nem Debugger sieht man die Werte auch :-)
	//cout << factors[8] << endl;

	/*std::cout << std::get<0>(positionen[0]) << "\n";
	std::cout << std::get<0>(positionen[1]) << "\n";
	std::cout << std::get<0>(positionen[2]) << "\n";
	std::cout << std::get<0>(positionen[3]) << "\n";
	std::cout << std::get<0>(positionen[4]) << "\n";
	std::cout << std::get<0>(positionen[5]) << "\n";
	std::cout << std::get<0>(positionen[6]) << "\n";
	std::cout << std::get<0>(positionen[7]) << "\n";
	std::cout << std::get<0>(positionen[8]) << "\n";
	std::cout << std::get<0>(positionen[9]) << "\n";	
*/
/*
 / Test g_array
	int g[8];
	int is[8];
	int ie[8];

	vector<pair<int,int>> pos(8); 
	for(int i = 0; i<8;i++){
	pos[i] = {std::get<0>(positionen[i]),std::get<1>(positionen[i])}; // tupel in pair umwandeln
	}*/

	//g_Array_Sort(positionen,8,g,is,ie);
	
	return EXIT_SUCCESS; // So macht man das Heute richtig



	
}


/*
 * OLI
 */


int d_Array(int d[],int size_d,int is[],int size_is, int ie[]){
	int p = 0; 							// Aktuelle Stelle in d
	int j = 0; 							// Laufvariable f�r die maximale Distanz
	int j1 = 0;							// Varible f�r das R�cksetzen von j
	int neuerFaktor = 0;				// Speicher f�r Faktor an neuer Stelle
	int neuerFaktor2 = 0;
	while(p<size_d-1){ 					// solange p kleiner n, p = aktuelle Stelle in d, n = L�nge von G, bzw IS und IE
		while(p<is[j]){					// Falls der erste String nicht an erster Stelle anf�ngt, schreibe 0 in d
			d[p] = 0;					// evtl unn�tig, da mit 0 initialisiert
			p++;
		}
		neuerFaktor = ie[j]; 					// Berechnung des Faktors an der ersten neuen Stellen, zwingend f�r den Vergleich		
												// j = Z�hlvariable in IS, erh�hen, da erste neue Stelle bereits berechnet
		while(is[j]<=p && j<size_is){ 			// solange Startposition des Strings kkleiner p, p = aktuelle Stelle in d
			neuerFaktor2 = ie[j];	 			// Berechnung des Faktors an der neuen Stelle, zwingend f�r den Vergleich
			if(neuerFaktor2 > neuerFaktor){  	// Abfrage ob der aktuelle Wert keliner ist als der neu Berechnete	
			neuerFaktor = neuerFaktor2;			// Falls ja, neuen Wert speichern
			j1 = j;									// aktuelles j Abspeichern, damit nicht jedesmal von Beginn an getestet wird in der while Schleife
			}			
			j++;			
		}
		while(p<j){
		d[p] = neuerFaktor-p+1;			
		p++;
		}
		j = j1;
	
	}
	d[p] = 1;							// An letzter Stelle 1 schreiben, da D nur bis zum vorletzten Element berechnet
			
								// wird und das letzte immer 1 sein muss, da der letzte String zu Ende ist
	return 0;
}


int_vector<> d_Strich(int size_d,csa_wt<> sa, int d[]){
	int_vector<> d_1(size_d);
	for(int i = 0; i < size_d;i++){
		d_1[i] = d[sa[i+1]];				// Berechnung von d', L�nge des l�ngsten Intervalls an der Position SAr[i]
	}
	
	return d_1;
}

/*int g_Array(int_vector<> t, int g[], int is[], int ie[]){
	
	int minPosition;
	for(int i = 0; i < t.size();i++){ 
		rmq_succinct_sct<> rminq(&t); 		// RMQ von t
		minPosition = rminq(0,t.size()-1);	// start bis endposition
		g[i] = minPosition+1;				// g[i] = j wenn t[j] linkester Faktor
		is[i] = t[minPosition]-1;		
	//	ie[i] = t[minPosition];
		t[minPosition] = 10;				// Anfangspos von Faktor in T ersetzen mit Wert > L�nge Referenzstring, damit RMQ keine falschen Werte leifert
	}
	cout << t << endl;
}*/


int g_Array_Sort(vector <pair<int,int>> t_array, int size_t,int g[], int is[], int ie[]){	
	vector<pair<pair<int,int>,int > > r(size_t); 		// Vektor als ((is,ie),g)
	vector<pair<int,int> > r1(size_t); 					// Vektor (is,ie)
	for(int i = 0; i<size_t;i++){						
		r1[i] = {t_array[i].first, t_array[i].second};							// r1 wird aus is,ie zusammengesetzt, entspricht nacher dem r Vektor, der aus der Faktorensuche entsteht
		r[i] = {r1[i],i+1};								// r wird aus r1,g zusammengesetzt, inhalt in g entspricht dem index des t arrays, das aus der Faktorensuche entsteht
	}
	sort(r.begin(), r.end());							// r wird nach dem ersten Faktor sortiert, der erste Faktor ist r1, da es wieder ein pair ist wird nach is stabil sortiert
	for(int i=0; i<size_t; ++i)
	{
		cout << r[i].first.first << ", " << r[i].first.second << ", " << r[i].second << endl; // Ausgabe r
	}
	
	cout << "----" << endl;
		for(int i=0; i<size_t; ++i)
	{
		g[i] = r[i].second;								// da der index �ber den Schleifenindex erzeugt wird muss er noch in g geschrieben werden
		cout << is[i] << ", " << ie[i] << ", " << g[i] << endl;	// Ausgabe is,ie,g
	}
}



	


/*
 * SANDRA
 */

void searchPattern(int st,int ed, int_vector<> dStrich, int patternLength, csa_wt<> sa, int g[], int is[], int size_is, int ie[]){
  rmq_succinct_sct<false> rmaxq(&dStrich);
  int maxPosition = rmaxq(st,ed);
  if(dStrich[maxPosition] >= patternLength){ //Abbruch, wenn Faktorl�nge < Patternl�nge
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


// Harry done
static vector<pair<int,int>> getFactorPosInR(string R, vector<string> factors){
	vector < pair<int, int> > posInR(8);
// da mir keiner gesagt hat, wie ihr das Gericht haben wollt,
// habe ich mich für Tupel entschieden,
// der erste Wert gibt die startposition
// der zweite Wert die Länge
// Aupassen muss man, da meine Werte bei 0 anfangen
// nicht wie im Paper bei 1, aber sonst stimmen die Werte

// Verwendendungsbeispiel
/*
vector<string> factors = getLempelZivFactors(R, S);
vector<tuple<int, int>> positionen = getFactorPosInR(R, factors);
*/


/*
 / Edit: abgeändert von tuple zu pair
*/

	int startpos = -1;
	int length = -1;
	for (int i = 0; i < factors.size(); i++){
		startpos = R.find(factors[i]);
		//length = factors[i].length;
		posInR[i] = {startpos, length};
	}
	return posInR;
}

// Harry

// Achtung, wird noch optimiert, hatte gerade einen guten Einfall :-)
static vector<string> getLempelZivFactors(string R, vector<string> S){
	vector<string> factors;
	string tempFactor;
	for (int i = 0; i < S.size(); i++){
		tempFactor = "";
		for (int z = 0; z < S[i].size(); z++){
			tempFactor += S[i][z];// mache präfix immer länger
			if (R.find(tempFactor) != string::npos){
				//cout << tempFactor << R.find(tempFactor) << endl;
			}
			else{
				z--;
				tempFactor.erase(tempFactor.size() - 1, 1);
				factors.push_back(tempFactor);
				tempFactor = "";
			}
		}

		// Muss gemacht werden, da der letzte substring sonst nicht mitgenommen wird...
		factors.push_back(tempFactor);
	}

	// so einfach geht es
	sort(factors.begin(), factors.end());
	factors.erase(unique(factors.begin(), factors.end()), factors.end());

	// ich schmeiße die duplette raus
	// da wir die ja nicht brauchen
	// erst danach ermittle ich die Positionen
	// so werden ein paar durchläufe bei der positionsermittelung
	// gespart


	return factors;
}

