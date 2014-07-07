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
int_vector<> d_Array(int_vector<> is,int_vector<> ie);
int_vector<> d_Strich(csa_wt<> sa, int_vector<> d);
int_vector<> g_Array_Sort(vector <pair<int,int>> t_array, int size_t);

/* 
 * SANDRA
 */
void searchPattern(uint64_t st,uint64_t ed, int_vector<>& dStrich, uint64_t patternLength, csa_wt<>& sa, int_vector<>& g, int_vector<>& is, int_vector<>& ie);
void getFactors(uint64_t startIndex, uint64_t patternLength, int_vector<>& g, int_vector<>& is, int_vector<>& ie, uint64_t ieStartIndex, uint64_t ieEndIndex);

/*
 * HARRY
*/
vector<string> getLempelZivFactors(string R, vector<string> S);
vector<pair<int,int>> getFactorPosInR(string R, vector<string> factors);



/*
 * Testmethoden
 */

void test_dArray();


int main(){
/*
 * OLI
 */
 
 
 test_dArray();
/*
	int_vector<> d;
	int_vector<> d_1;
	int_vector<> t = {1,1,1,8,5,3,7,4};
	int_vector<> g(8);// = {1,2,3,6,8,5,7,4};
	int_vector<> is = {0,0,0,2,3,4,6,7};
	int_vector<> ie = {1,2,3,3,8,5,7,8};
	vector<pair<int,int> > t_array(8);
	for(int i = 0; i<8;i++){
	t_array[i] ={is[i],ie[i]};
	}	
	
	csa_wt<> sa;
	construct_im(sa, "acgtgatag", 1);	//Eingabe: Referenzstring

	//int size_d = 9; //sizeof(d) / sizeof(int);
	//int size_is = sizeof(is) / sizeof(int);
	d = d_Array(is,ie);
	g = g_Array_Sort(t_array,8);

	d_1 = d_Strich(sa,d);
	for(int i = 0; i< 9; i++){
	cout << "d: " << d[i] << endl;
	}

*/
/*
 * SANDRA 
 */
/*
  int_vector<> dStrich = {4,2,4,3,1,5,2,3,6};
  int_vector<> g = {1,2,3,6,8,5,7,4};
  int_vector<> is = {0,0,0,2,3,4,6,7};
  int_vector<> ie = {1,2,3,3,8,5,7,8};
  
  csa_wt<> sa;
  construct_im(sa, "acgtgatag", 1);
  string pattern = "a";
  
  uint64_t i=0, j=sa.size()-1, startIndex=0, endIndex=0;
  backward_search(sa, i, j, pattern.begin(), pattern.end(), startIndex, endIndex); // Rueckwaertssuche => startIndex, endIndex
  searchPattern(startIndex-1, endIndex-1, dStrich,pattern.size(), sa, g, is, ie);

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




int_vector<> d_Array(int_vector<> is,int_vector<> ie){
	/*
	 * Erwartet is und ie
	 * leifert d zurück
	*/

	int p = 0; 						// Aktuelle Stelle in d
	int j = 0; 						// Laufvariable fuer die maximale Distanz
	int_vector<> d(ie[ie.size()-1]);	
				
	uint64_t neuerFaktor = 0;				// max aus ie[j], bei is[j] <= p
	uint64_t neuerFaktor2 = 0;
	while(p<d.size()){ 	
		
				if(neuerFaktor < p){				// wenn das max aus ie kleiner ist als die Position in d, ist der aktuell Längste Faktor zu ende
		neuerFaktor = 0; 
								
		};					
		while(p<is[j] && neuerFaktor == 0){					// solange die Position in d echt kleiner ist als der erste Inhalt in is, fülle d mit 0
			d[p] = 0;			
		
			p++;		
		}
									
		while(is[j]<=p && j<=is.size()){		// solange der Start des Faktors kleiner gleich der Stelle in d und j in is
			neuerFaktor2 =ie[j];
			if(neuerFaktor2 > neuerFaktor){  	// wenn die Endposition des aktuellen Faktors größer ist als das bisherige Maximum, verwende diesen als neues Maximum
			neuerFaktor = neuerFaktor2;												
			}		
								cout << p << " " << j << " " << neuerFaktor<< endl;				
			j++;			
		}
		while(p<=j){					// solange die aktuelle Position in d kleiner ist als die aktuelle Stelle in is, schreibe in d
		d[p] = neuerFaktor-p+1;		
	
		p++;
		}	
	}
	return d;							
}


int_vector<> d_Strich(csa_wt<> sa, int_vector<> d){
	int_vector<> d_1(d.size());
	for(int i = 0; i < d.size();i++){
		d_1[i]=(d[sa[i+1]]);				// Berechnung von d', Lï¿½nge des lï¿½ngsten Intervalls an der Position SAr[i]
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
		t[minPosition] = 10;				// Anfangspos von Faktor in T ersetzen mit Wert > Lï¿½nge Referenzstring, damit RMQ keine falschen Werte leifert
	}
	cout << t << endl;
}*/


int_vector<> g_Array_Sort(vector <pair<int,int>> t_array, int size_t){	
	vector<pair<pair<int,int>,int > > r(size_t); 		// Vektor als ((is,ie),g)
	vector<pair<int,int> > r1(size_t); 					// Vektor (is,ie)
	int_vector<> g(size_t);
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
		g[i] = r[i].second;								// da der index ï¿½ber den Schleifenindex erzeugt wird muss er noch in g geschrieben werden
		cout << g[i] << endl;	// Ausgabe g
	}
	return g;
}



	


/*
 * SANDRA
 */

/* sucht solange das Maximum in dStrich bis die Faktorlaenge < Patternlaenge ist und berechnet jeweils die Faktoren mittels getFactors.*/
void searchPattern(uint64_t st,uint64_t ed, int_vector<>& dStrich, uint64_t patternLength, csa_wt<>& sa, int_vector<>& g, int_vector<>& is, int_vector<>& ie){
  rmq_succinct_sct<false> rmaxq(&dStrich);
  uint64_t maxPosition = rmaxq(st,ed); //Maximum in dStrich
  if(dStrich[maxPosition] >= patternLength){ //Abbruch, wenn Faktorlaenge < Patternlaenge
	int_vector<>::iterator up;
	up = upper_bound(is.begin(), is.end(), sa[maxPosition+1]);	//endIndex für rmq in ie
	getFactors(sa[maxPosition+1], patternLength, g, is, ie, 0, up-is.begin()-1);
    if(st < maxPosition) 
      searchPattern(st, maxPosition-1, dStrich, patternLength, sa, g, is, ie);
    if(ed > maxPosition)
      searchPattern(maxPosition+1, ed, dStrich, patternLength, sa, g, is, ie);
  }
 }
 
/* sucht das Maximum in ie und gibt den Faktor aus, falls ie-Wert >= startIndex+Patternlaenge-1*/
void getFactors(uint64_t startIndex, uint64_t patternLength, int_vector<>& g, int_vector<>& is, int_vector<>& ie, uint64_t ieStartIndex, uint64_t ieEndIndex){
   rmq_succinct_sct<false> rmaxq(&ie);
   uint64_t factorPosition = rmaxq(ieStartIndex,ieEndIndex); //naechster moeglicher Faktor beim Maximum in ie
   if(ie[factorPosition] >= startIndex+patternLength-1){ //Abbruch falls Pattern nicht mehr im Faktor liegt
     cout << "Faktor: " << g[factorPosition] << endl;
    if(factorPosition < ieEndIndex){
	getFactors(startIndex, patternLength, g, is, ie, factorPosition+1, ieEndIndex);
     }
     if(factorPosition > ieStartIndex){
	getFactors(startIndex, patternLength, g, is, ie, ieStartIndex, factorPosition-1);
     }
   }
}


// Harry done
vector<pair<int,int>> getFactorPosInR(string R, vector<string> factors){
	vector < pair<int, int> > posInR(8);

/*
vector<string> factors = getLempelZivFactors(R, S);
vector<tuple<int, int>> positionen = getFactorPosInR(R, factors);
*/


/*
 / Edit: abgeÃ¤ndert von tuple zu pair
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

vector<string> getLempelZivFactors(string R, vector<string> S){
	vector<string> factors;
	string tempFactor;
	for (int i = 0; i < S.size(); i++){
		tempFactor = "";
		for (int z = 0; z < S[i].size(); z++){
			tempFactor += S[i][z];// mache prÃ¤fix immer lÃ¤nger
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

	return factors;
}


// So, hier mal alles in einer Methode
// 
vector<pair<int,int>> getFactorsN(string s, string vgl){
	vector<pair<int, int>> values;
	//string sear = "";
	int startpos = 0;
	int stringLength = 1;
	for (int i = 0; i < vgl.length(); i++){
		//sear = vgl.substr(startpos, stringLength);
		if (s.find(vgl.substr(startpos, stringLength)) != string::npos){
			stringLength++;
		}
		else{
			stringLength--;
			values.push_back(make_pair(startpos, stringLength));
			startpos = stringLength;
			stringLength = 1;
		}
	}
	values.push_back(make_pair(startpos, stringLength));
	return values;
}






/*
 * Testmethoden
 */
 
 
void test_dArray(){
	
	int_vector<> is = {1,1,2,2,6,6,7,8,10};
	int_vector<> ie = {5,6,4,5,6,8,8,8,10};
	int_vector<> d; // 0,6,5,4,3,2,3,2,1,0,1
	
	d = d_Array(is,ie);
	for(int i = 0; i< d.size(); i++){
	cout << "d: " << d[i] << endl;
	}
	
	
}

