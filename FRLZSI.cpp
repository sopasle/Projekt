#include "FRLZSI.hpp"
#include <iostream>
#include <algorithm>

using namespace sdsl;
using namespace std;


/*Default Konstruktor*/

FRLZSI::FRLZSI(){
}


/*Konstruktor*/
FRLZSI::FRLZSI(string &r, vector<string> &s){
	construct_im(m_sa, r.c_str(), 1);	//m_sa initialisieren
	LZ_factorization(r, s);		//m_t_array initialisieren
	g_Array(); 			//m_g, m_is, m_ie_rmaxq() initialisieren
	d_Strich(d_Array());		//m_ds initialisieren
}

/*Destruktor*/
FRLZSI::~FRLZSI(){
}


/*Methoden*/

/*
 * OLI
*/
void FRLZSI::g_Array(){
	/*
	* Erwartet Initialisierung von m_t_array
	*	erstellt g
	*/
	vector<tuple<uint64_t,uint64_t,uint64_t>> r; // tupel(is,ie,g)
	int_vector<> g(m_t_array.size());
	int_vector<> is(m_t_array.size());
	int_vector<> ie(m_t_array.size());
	for(int i = 0; i<m_t_array.size();i++){	
		r.push_back(std::make_tuple (m_t_array[i].first,m_t_array[i].second,i+1)); // anhängen des nächsten tupels
	}
	sort(r.begin(), r.end());		// sortieren nach is,ie
	int i = 0;
	for(vector<tuple<uint64_t,uint64_t,uint64_t>>::iterator iter = r.begin(); iter != r.end(); iter++){  	
	is[i] = get<0>(*iter);
	ie[i] = get<1>(*iter);
	g[i] = get<2>(*iter);
	i++;
  	}
	
	m_g = std::move(g);
	m_is = std::move(is);
	rmq_succinct_sct<false> rmaxq(&ie);
	m_ie_rmaxq = std::move(rmaxq);
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
	/*cout << "d:  ";
	for(int i = 0;i<d.size();i++){
		cout << d[i] << ",";
	}
	cout << endl;*/
	return d;	
}

void FRLZSI::d_Strich(int_vector<> d){
	int_vector<> d_1(d.size());
	for(int i = 0; i < d.size();i++){
		d_1[i]=(d[m_sa[i+1]]);	// Berechnung von d', Laenge des laengsten Intervalls an der Position SAr[i]
	}
	m_ds = std::move(d_1);
	rmq_succinct_sct<false> rmaxq(&d_1);
	m_ds_rmaxq = std::move(rmaxq);
}


/*
 * SANDRA
*/
/*oeffentlich aufrufbare Methode zum auffinden eines Patterns in S*/
void FRLZSI::search_pattern(string &pattern){
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


/*Zerlegt die einzelnen Strings in Faktoren relativ zum Referenzstring R*/
void FRLZSI::LZ_factorization(string &R, vector<string> &S){
	vector<pair<int,pair<int,int>>> factors;
	string R_r(R.rbegin(), R.rend());	//R reverse
	csa_wt<wt_hutu<>> csa_bwd;
	construct_im(csa_bwd, R_r.c_str(), 1);	//SA von R reverse

	for(int i = 0; i<S.size(); i++){	//Faktorisierung der einzelnen Strings S
		uint64_t l_fwd = 0, r_fwd = m_sa.size()-1, l_bwd = 0, r_bwd = csa_bwd.size()-1, l_fwd_res = 0, r_fwd_res = 0, r_bwd_res = 0, l_bwd_res = 0;
		uint64_t factorLength = 0;

		for(int j = 0; j<S[i].size(); j++){	//String S wird durchlaufen
			bidirectional_search(csa_bwd, l_fwd, r_fwd, l_bwd, r_bwd, S[i][j], l_fwd_res, r_fwd_res, l_bwd_res, r_bwd_res);
			if(l_bwd_res <= r_bwd_res){	//Teilstring existiert in R
				l_fwd = l_fwd_res;
				r_fwd = r_fwd_res;
				l_bwd = l_bwd_res;
				r_bwd = r_bwd_res;
				factorLength++;
			}
			else{	//Teilstring existiert nicht in R (aber Teilstring-1)
				pair<int,pair<int,int>> temp;
				temp.second.first = m_sa[l_bwd];
				temp.second.second = m_sa[l_bwd]+factorLength-1;
				temp.first = l_bwd;
				factors.push_back(temp);
				
				l_fwd = 0;
				r_fwd = m_sa.size()-1;
				l_bwd = 0;
				r_bwd = csa_bwd.size()-1;
				bidirectional_search(csa_bwd, l_fwd, r_fwd, l_bwd, r_bwd, S[i][j], l_fwd_res, r_fwd_res, l_bwd_res, r_bwd_res);	//Da alle Buchstaben von S in R vorkommen muss l_bwd_res <= r_bwd_res sein
				l_fwd = l_fwd_res;
				r_fwd = r_fwd_res;
				l_bwd = l_bwd_res;
				r_bwd = r_bwd_res;
				factorLength = 1;
			}
		
		}
		//letzter Faktor des Strings:
		pair<int,pair<int,int>> temp;
		temp.second.first = m_sa[l_bwd];
		temp.second.second = m_sa[l_bwd]+factorLength-1;
		temp.first = l_bwd;
		factors.push_back(temp);
	}
	
	sort(factors.begin(), factors.end());	//nach SA sortieren

	for(uint64_t i = 0; i<factors.size(); i++){	//im m_t_array speichern
		m_t_array.push_back(factors[i].second);
	}
	m_t_array.erase(unique(m_t_array.begin(), m_t_array.end()), m_t_array.end());	//gleiche Faktoren loeschen
}




/*
 * Testmethoden
*/

void FRLZSI::test_LZ_factorization(string &R, vector<string> &S){
	vector<pair<string,pair<int,int>>> factors;
	for(uint64_t i = 0; i< S.size(); i++){
		uint64_t start = 0;
		uint64_t length = 1;
		uint64_t is = 0;
		while(start+length-1<S[i].length()){
			uint64_t found = R.find(S[i].substr(start, length));
  			if (found!=string::npos){	//Teilstring in R enthalten
				is = found;
				length++;
			}
			else{	//Teilstring nicht in R enthalten (aber Teilstring-1)
				pair<string,pair<int,int>> temp;
				temp.second.first = is;
				temp.second.second = is+length-2;
				temp.first = S[i].substr(start,length-1);
				factors.push_back(temp);
				start = start + length-1;
				length = 1;
			}
		}
		pair<string,pair<int,int>> temp;	//letzter Faktor
		temp.second.first = is;
		temp.second.second = is+length-2;
		temp.first = S[i].substr(start,length-1);
		factors.push_back(temp);
	}
	sort(factors.begin(), factors.end());	//sortieren
	factors.erase(unique(factors.begin(), factors.end()), factors.end()); //doppelte Eintraege loeschen
	
	int failure = 1;	//Ueberpruefen
	if(m_t_array.size() == factors.size()){	
		for(uint64_t i = 0; i<factors.size(); i++){
			if(m_t_array[i].first != factors[i].second.first || m_t_array[i].second != factors[i].second.second){
				failure = 0;
				cout << "Fehler an Positon: " << i << endl;
			}
		}
		if(failure){
			cout << "m_t_array korrekt" << endl;
		}
	}
	else{
		cout << "Unterschiedliche Anzahl an Faktoren!" << endl;
	}

	
	
}

void FRLZSI::test_search(string &pattern){
	uint64_t i=0, j=m_sa.size()-1, l_res=0, r_res=0;
	backward_search(m_sa, i, j, pattern.begin(), pattern.end(), l_res, r_res);	//Rueckwaertssuche => startIndex, endIndex
	if(l_res <= r_res && r_res <= m_sa.size()-1){	//Pattern existiert in R
		while(l_res <= r_res){
			for(int i = 0; i< m_t_array.size(); i++){
				if(m_t_array[i].first <= m_sa[l_res] && m_t_array[i].second >= m_sa[l_res]+pattern.size()-1){
					cout << "Faktor: " << i+1 << " " << m_sa[l_res]-m_t_array[i].first << "-" << m_sa[l_res]-m_t_array[i].first+pattern.size()-1 << endl;
				}
			}
			l_res++;
		}
	}else{
		cout << pattern << " existiert nicht" << endl;	//Vorlaeufig fuer Case1
	}
}


void FRLZSI::test_ausgabe(){
	for(int i = 0;i<m_g.size();i++){
		cout << "g: " << m_g[i] << endl;
	}
	
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
