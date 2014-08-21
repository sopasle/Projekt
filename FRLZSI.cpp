#include "FRLZSI.hpp"
#include <iostream>
#include <algorithm>

using namespace sdsl;
using namespace std;


/*Default Konstruktor*/

FRLZSI::FRLZSI(){	
}


/*Konstruktor*/
FRLZSI::FRLZSI(string &r, vector<string> &s) : m_s(s.size()){
	construct_im(m_sa, r.c_str(), 1);	//m_sa initialisieren
	string R_r(r.rbegin(), r.rend());	//R reverse
	construct_im(m_csa_bwd, R_r.c_str(), 1);	//m_csa_bwd initialisieren
	LZ_factorization(r, s);		//m_t_array, m_s initialisieren
	g_Array(); 			//m_g, m_is, m_ie_rmaxq() initialisieren
	d_Strich(d_Array());		//m_ds initialisieren
	bcl_erzeugen();			// Datenstruktur X(T) f�llen
	uint64_t a,b;
	p_zu_t(1,3,a,b,1);
	cout << a << " " << b << endl;
	//f_array();

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
		r.push_back(std::make_tuple (m_t_array[i].first,m_t_array[i].second,i+1)); // anh�ngen des n�chsten tupels
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
	rmq_succinct_sct<false> rmaxq(&m_ds);
	m_ds_rmaxq = std::move(rmaxq);
}

void FRLZSI::bcl_erzeugen(){
	m_b_t.resize(m_sa.size());
	m_c_t.resize(m_t_array.size());
	m_b_tq.resize(m_sa.size());
	m_c_tq.resize(m_t_array.size());
	vector<vector <uint64_t> > gamma_t(m_sa.size()-1);
	vector<vector <uint64_t> > gamma_tq(m_sa.size()-1);
	
	vector<pair<int,int>> t_reverse_array = {{3,4},{1,2},{7,8},{0,1},{0,5},{6,8},{5,6},{5,8}};
	
	int i = 0;
	while(i < m_t_array.size()){
		gamma_t[m_t_array[i].first].push_back(m_t_array[i].second-m_t_array[i].first +1); // Vektor mit L�nge aller an der aktuellen Stelle beginnenden Strings
		gamma_tq[t_reverse_array[i].first].push_back(t_reverse_array[i].second-t_reverse_array[i].first +1);
		i++;
	}
	i = 0;
	while(i < gamma_t.size()){
		if(!gamma_t[m_sa[i+1]].empty()){
			m_gamma_t.push_back(gamma_t[m_sa[i+1]]);		// Umtragen in die Membervariable im Zusammenhang mit SA
			m_b_t[i] = 1;		// Faktor f�ngt an dieser Stelle an
		}else{
			m_b_t[i] = 0;
			}
		if(!gamma_tq[m_sa[i+1]].empty()){
			m_gamma_tq.push_back(gamma_tq[m_sa[i+1]]);		// Umtragen in die Membervariable im Zusammenhang mit SA
			m_b_tq[i] = 1;		// Faktor f�ngt an dieser Stelle an
		}else{
			m_b_tq[i] = 0;
			}
	
	i++;
	}
	i = 0;
	int j = 0,k=0;
	while(i<m_gamma_t.size()){
		j += m_gamma_t[i].size();  // Anzahl an Faktoren in Gamma um c zu berechnen
		m_c_t[j-1] = 1;
		k += m_gamma_tq[i].size();  
		m_c_tq[k-1] = 1;
		i++;
	}
	i = 0;
	while(i < m_sa.size()-1){
	cout  << m_c_t[i] << " ; " << m_sa[i+1] << " |" << m_b_t[i] << endl;
		//cout << i << " " << b[i] << " , " << gamma[i] << " | " << m_sa[i+1] << " : " << m_t_array[i].first << endl;
	i++;
	}
	i = 0;
	while(i < m_gamma_t.size()-1){
	cout  << m_gamma_t[i] << " ; " << m_c_t[i] << endl;
	i++;
	}
	i = 0;
	while(i < m_gamma_tq.size()-1){
	cout  << m_gamma_tq[i] << " - " << m_c_tq[i] << endl;
	i++;
	}
}


void FRLZSI::p_zu_t(uint64_t st, uint64_t ed, uint64_t& p, uint64_t& q, uint64_t c){
	rank_support_v<1> m_b_t_rank(&m_b_t);
	select_support_mcl<1> m_c_t_select(&m_c_t);	
	uint64_t m_b_t_rank_helper;
	m_b_t_rank_helper = m_b_t_rank(st-1);
	if(m_b_t_rank_helper == 0){
		// st =>st-1 if(<0) = 0
	}else{
	p = 1 + m_c_t_select(m_b_t_rank_helper) + binaere_suche(m_b_t_rank(st),c);
	}
	m_b_t_rank_helper = m_b_t_rank(ed);
	if(m_b_t_rank_helper == 0){
		// 
	}else{
	q = m_c_t_select(m_b_t_rank(ed));
	}
	if(p > q){
		//
	}
}

uint64_t FRLZSI::binaere_suche(uint64_t b_rank,uint64_t c) {
   uint64_t l=0;

   uint64_t x;
   vector <uint64_t> gamma_b = m_gamma_t[b_rank];
   cout << gamma_b << " :)" << endl;
	   uint64_t r=7;
   while(r >= l) {
      x=l + ((r-l)/2);
      if(c < gamma_b[x] ) /* kleiner? */
         r=x-1;  /* Rechte Seite ist nicht mehr so interessant. */
       else      /* dann halt gr��er */
         l=x+1;  /* Linke Seite ist nicht mehr so interessant. */
       if(gamma_b[x] == c && x != 0)
          return x;     /* gefunden; x = Position */
   }
	if(c < gamma_b[x])
	return x;
	else
	return x+1;
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
	up = upper_bound(m_is.begin(), m_is.end(), m_sa[maxPosition+1]);	//endIndex f�r rmaxq in ie (binaere Suche)
	uint64_t gIndex = m_ie_rmaxq(0,up-m_is.begin()-1);
	uint64_t dsValue = m_t_array[m_g[gIndex]-1].second - m_sa[maxPosition+1] + 1;

	if(dsValue >= patternLength){	//Abbruch, wenn Faktorlaenge < Patternlaenge
		getFactors(m_sa[maxPosition+1], patternLength, 0, up-m_is.begin()-1);
		if(st < maxPosition){
			searchPattern(st, maxPosition-1, patternLength);
		}
		if(ed > maxPosition){
			searchPattern(maxPosition+1, ed, patternLength);
		}
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


/*a-Array fuer die Suche im 2.Fall*/
vector<vector<int>> FRLZSI::a_array(string pattern){
	vector<vector<int>> a(pattern.size());
	for(uint64_t i = 0; i<pattern.size(); i++){
		uint64_t j = 0;	//in for-Schleife, da kein delet_back()
		uint64_t st_r = 0, ed_r = m_csa_bwd.size()-1, st_r_reverse = 0, ed_r_reverse = m_csa_bwd.size()-1;
		
		string sub_pattern = pattern.substr (i,pattern.size()-i);
		while(j < sub_pattern.size()){
			uint64_t st_r_res, ed_r_res, st_r_reverse_res, ed_r_reverse_res;
			bidirectional_search(m_csa_bwd, st_r_reverse, ed_r_reverse, st_r, ed_r, sub_pattern[j], st_r_reverse_res, ed_r_reverse_res, st_r_res, ed_r_res);
			if(st_r_res <= ed_r_res){	//P[i..j] existiert in R
				st_r = st_r_res;
				ed_r = ed_r_res;
				st_r_reverse = st_r_reverse_res;
				ed_r_reverse = ed_r_reverse_res;
				
				uint64_t st_t, ed_t,c;
				p_zu_t(st_r, ed_r, st_t, ed_t,j+1);
				if(st_t <= ed_t){	//P[i..j] ist moeglicherweise ein Faktor in T
					for(int k = st_t; k<=ed_t; k++){
						if(j+1 == m_t_array[m_g[k]-1].second - m_is[k])
						cout << "Faktor der L�nge: " << j+1 << endl;
					}
				}
				
				j++;
			}
			else{
				break;
			}
		}
		uint64_t st_t, ed_t,c;
		p_zu_t(st_r, ed_r, st_t, ed_t,j);
		if(st_t <= ed_t){	//???
			while(st_t <= ed_t){
				a[i].push_back(st_t+1);
				st_t++;
			}
		} // else a[i] = nil
	}
	
	return a;
}


/*Zerlegt die einzelnen Strings in Faktoren relativ zum Referenzstring R*/
void FRLZSI::LZ_factorization(string &R, vector<string> &S){
	vector<tuple<int,pair<int,int>,int,int>> factors;	//tuple(ISA,is,ie,Si,faktornummer)
	vector<tuple<int,int,pair<int,int>>> factors_reverse;		//tuple(ISA_reverse,faktorlaenge,pair(is,ie))

	for(int i = 0; i<S.size(); i++){	//Faktorisierung der einzelnen Strings S
		uint64_t l_fwd = 0, r_fwd = m_sa.size()-1, l_bwd = 0, r_bwd = m_csa_bwd.size()-1, l_fwd_res = 0, r_fwd_res = 0, r_bwd_res = 0, l_bwd_res = 0;
		uint64_t factorLength = 0;
		uint64_t number_of_factors = 0;
		
		for(int j = 0; j<S[i].size(); j++){	//String S wird durchlaufen
			bidirectional_search(m_csa_bwd, l_fwd, r_fwd, l_bwd, r_bwd, S[i][j], l_fwd_res, r_fwd_res, l_bwd_res, r_bwd_res);
			if(l_bwd_res <= r_bwd_res){	//Teilstring existiert in R
				l_fwd = l_fwd_res;
				r_fwd = r_fwd_res;
				l_bwd = l_bwd_res;
				r_bwd = r_bwd_res;
				factorLength++;
			}
			else{	//Teilstring existiert nicht in R (aber Teilstring-1)
				pair<int,int> temp;	
				temp.first = m_sa[l_bwd];	//factors
				temp.second = temp.first + factorLength-1;
				factors.push_back(std::make_tuple (l_bwd,temp,i,number_of_factors));
				
				temp.first = m_csa_bwd[l_fwd];	//factors_reverse
				temp.second = temp.first + factorLength-1;
				factors_reverse.push_back(std::make_tuple (l_fwd,factorLength,temp));
				
				l_fwd = 0;
				r_fwd = m_sa.size()-1;
				l_bwd = 0;
				r_bwd = m_csa_bwd.size()-1;
				bidirectional_search(m_csa_bwd, l_fwd, r_fwd, l_bwd, r_bwd, S[i][j], l_fwd_res, r_fwd_res, l_bwd_res, r_bwd_res);
				if(l_bwd_res > r_bwd_res || r_bwd_res > m_csa_bwd.size()-1){	//Abbruch falls Buchstabe nicht in R vorkommt
					abort();
				}
				l_fwd = l_fwd_res;
				r_fwd = r_fwd_res;
				l_bwd = l_bwd_res;
				r_bwd = r_bwd_res;
				factorLength = 1;
				number_of_factors++;
			}
		
		}
		//letzter Faktor des Strings:
		pair<int,int> temp;
		temp.first = m_sa[l_bwd];	//factors
		temp.second = m_sa[l_bwd]+factorLength-1;
		factors.push_back(std::make_tuple (l_bwd,temp,i,number_of_factors));
		
		temp.first = m_csa_bwd[l_fwd];	//factors_reverse
		temp.second = temp.first + factorLength-1;
		factors_reverse.push_back(std::make_tuple (l_fwd,factorLength,temp));

		int_vector<> s(number_of_factors+1);
		m_s[i] = s;
	}
	
	sort(factors.begin(), factors.end());	//nach SA sortieren
	sort(factors_reverse.begin(), factors_reverse.end());
	
	//m_s[i] initialisieren
	if(!factors.empty()){
		vector<tuple<int,pair<int,int>,int,int>>::iterator iter;
		vector<tuple<int,pair<int,int>,int,int>>::iterator iter2;
		uint64_t i = 1;
		
		iter = factors.begin();
		m_s[get<2>(*iter)][get<3>(*iter)] = i;
		iter2 = factors.begin();
		iter++;
		while(iter != factors.end()){
			if(get<1>(*iter) != get<1>(*iter2)){	//neuer Faktor
				i++;
			}
			m_s[get<2>(*iter)][get<3>(*iter)] = i;
			iter2++;
			iter++;

		}
	}
	
	vector<tuple<int,int,pair<int,int>>>::iterator iter_reverse = factors_reverse.begin();
	for(vector<tuple<int,pair<int,int>,int,int>>::iterator iter = factors.begin(); iter != factors.end(); iter++){  	
		m_t_array.push_back(get<1>(*iter));
		m_t_reverse_array.push_back(get<2>(*iter_reverse));
		iter_reverse++;
	}

	m_t_array.erase(unique(m_t_array.begin(), m_t_array.end()), m_t_array.end());	//gleiche Faktoren loeschen
	m_t_reverse_array.erase(unique(m_t_reverse_array.begin(), m_t_reverse_array.end()), m_t_reverse_array.end());
	
}

/*Erzeugt das F-Array*/
void FRLZSI::f_array(){
	/*uint64_t eof = m_t_array.size()+1;
	uint64_t length = 0;
	//csa der einzelnen S-Zerlegungen -> int_vectoren zusammenfassen
	for(int i = 0; i< m_s.size(); i++){
		length += m_s[i].size();
	}
	int_vector<> seg(length+m_s.size()-1);
	uint64_t counter = 0;
	for(int i = 0; i< m_s.size(); i++){
		for(int j = 0; j<m_s[i].size(); j++){
			seg[counter] = m_s[i][j];
			counter++;
		}
		if(i != m_s.size()-1){
			seg[counter] = eof;
			counter++;
		}
	}
	cout << seg << endl;
	csa_wt<wt_hutu<>> f;
	construct_im(f, seg, 1);
	
	cout << "i" << "\t" << "text[i]" << "\t" << "sa[i]" << "\t" << "isa[i]" << "\t" << "bwt[i]" << "\t" << "psi[i]" << "\t" << "lf[i]" << "\t" << "Suffix" << endl;
	for(uint64_t i=0; i<f.size(); ++i)
		cout << i << "\t" << f.text[i] << "\t" << f[i] << "\t" << f.isa[i] << "\t" << f.bwt[i] << "\t" << f.psi[i] << "\t" << f.lf[i] << "\t" << extract(f, f[i], f.size()-1) << endl;
	cout << endl;
	
	bit_vector f_v(length);
	for(int i=1; i<f.size(); i++){	//V initialisieren
	}*/
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
		cout << m_ds_rmaxq(0,i) << ",";
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
	int i = 0; // H�chster Wert der Reichweite
 	int j = 0; // Laufvariable fuer die maximale Distanz
	int p = 0; // Aktuelle Stelle in d
	int_vector<> d(m_sa.size()-1);	
	while(p<d.size()){
		while(m_is[j]>p){ // Solange kein Faktor da ist, wird 0 geschrieben
			d[p] = 0;
			p++;
		}
		while(m_is[j] <= p && j < m_t_array.size()){ 	// solange der Anfang der Faktoren kleiner ist als die aktuelle Stelle und kleiner ist als die Gesamtl�nge
			if(i < m_t_array[m_g[j]-1].second-p+1){		// Abfrage ob aktuell h�chster Wert kleiner ist als der Wert an der neuen Stelle, eins weiter
				i = m_t_array[m_g[j]-1].second-p+1;		// Falls ja, wird der h�here Wert �bernommen
				if(i<0){		// Falls der Wert kleiner als 0 wird, muss er auf 0 gesetzt werden, da ansonsten der Vergleich fehlschl�gt
					i = 0;
				}
			}
			j++;
			
		}
		j = 0;					// Durch j=0 wird immer vom Anfang an gepr�ft
		d[p] = i;				// Speichern des h�chsten Wertes an der aktuellen Stelle	
		i = 0;					// R�cksetzen des h�chsten Werts
		p++;
	}


	cout << "dT: ";
	for(int i = 0;i<d.size();i++){
		cout << d[i] << ",";
	}
	cout << endl;	
}
/*
void FRLZSI::test_bcl_erzeugen(){
	vector<bool> b;
	vector<bool> c(m_is.size());
	vector<vector <int> > gamma;
	int i = 0;
	int j = 0;	
	bool b_help;
	vector<int> gamma_help;
	while(i < m_sa.size()-1){
	while(m_is[j] == i){				// �berpr�fen, ob an der aktuellen Stelle ein String anf�ngt
	gamma_help.push_back(m_t_array[m_g[j]-1].second-m_is[j] +1); // Vektor mit L�nge aller an der aktuellen Stelle beginnenden Strings
	j++;
	b_help = 1;					// b wird an dieser Stelle auf 1 gesetzt, da min ein String anf�ngt
	//cout << gamma_help[0] << " ; " << sa[i]-1 << endl;
	}
	gamma.push_back(gamma_help);			// Vektor von Vektoren mit der L�nge der Strings die an der aktuellen Stelle beginnen
	b.push_back(b_help);				// Vektor der anzeigt das an der aktuellen Stelle ein String beginnt
	b_help = 0;
	if(j != 0){
	c[j-1] = 1;					// C Vektor Eintrag, da j die Anzahl an String die bisher gepr�ft wurden
	}
	//cout << gamma[i] << " | " << sa[i] << endl;
	i++;
	gamma_help.erase(gamma_help.begin(),gamma_help.end()); // Hilfsvektor leeren um Platz zu sparen

	}
	m_c = std::move(c);
	i = 1;
	while(i < m_sa.size()){
	m_gamma.push_back(gamma[m_sa[i]]);		// Umtragen in die Membervariable im Zusammenhang mit SA
	m_b.push_back(b[m_sa[i]]);			// -"-----------------"--------------------------"------
	i++;
	}
	i = 0;
	while(i < m_sa.size()-1){
	cout << m_gamma[i] << " - " << m_b[i] << " - " << m_c[i] << " ; " << m_sa[i+1] << endl;
	i++;
	}
}*/
