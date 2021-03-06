#include "FRLZSI.hpp"
#include <iostream>
#include <algorithm>

using namespace sdsl;
using namespace std;

/*Default Konstruktor*/
FRLZSI::FRLZSI(string &pattern){
}

/*Konstruktor*/
FRLZSI::FRLZSI(string &r, vector<string> &s){
	string filename = "sa";
	store_to_file(r.c_str(),filename);
	construct(m_sa,filename, 1); // 0=Serialisierter int_vector<>
	remove("sa");

	string R_r(r.rbegin(), r.rend());	//R reverse

	string filename1 = "csa";
	store_to_file(R_r.c_str(),filename1);
	construct(m_csa_bwd,filename1, 1); // 0=Serialisierter int_vector<>
	remove("csa");
	R_r.clear();	//R_r loeschen

	vector<int_vector<>> m_s(s.size());
	int_vector<> t_to_t_reverse = LZ_factorization(r, s, m_s);		//m_t_array, m_s initialisieren
	r.clear();	// r loeschen
	uint64_t max = 0;			// maximale Gr��e f�r int_vector in m_l herausfinden, um Speicherplatz zu sparen und s loeschen
	for(int i = 0; i<s.size();i++){
		if(max < s[i].size()){
			max = s[i].size();
		}
		s[i].clear();
	}
	max = (int)(log(max)/log(2))+1;	

	f_array(max, m_s);				//m_f und m_v initialisieren
	m_s.clear();	//m_s leeren

	initialize_m(t_to_t_reverse);	//m_m_array initialisieren

	g_Array(); 			//m_g, m_is, m_ie_rmaxq() initialisieren

	d_Strich(d_Array());		//m_ds initialisieren

	bcl_erzeugen();			// Datenstruktur X(T) f�llen
}

/*Destruktor*/
FRLZSI::~FRLZSI(){
}

int anzahl_treffer = 0;

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

	uint64_t posmax = 0;	// Position des Maximums bis zur aktuellen Stelle
	while(p<d.size()){
		while(m_is[j]<=p && j<m_is.size()){ 
			j++;
		}
		if(j > 0){
			posmax = m_ie_rmaxq(0,j-1); // Maxpos berechnen mit m_ie
			if(m_t_array[m_g[posmax]-1].second < p){ // Pr�fen, ob aktueller Faktor bis zur Stelle p geht oder nicht
				d[p] = 0;
			}else{
				d[p] = m_t_array[m_g[posmax]-1].second - p+1;
			}
		}else{
			d[p] = 0;
		}

		p++;
	}
	return d;	
}

void FRLZSI::d_Strich(int_vector<> d){
	/*
	 * Erwartet Initialisierung von m_d, m_sa
	 * erstellt d-strich
	 */
	int_vector<> d_1(d.size());
	for(int i = 0; i < d.size();i++){
		d_1[i]=(d[m_sa[i+1]]);	// Berechnung von d', Laenge des laengsten Intervalls an der Position SAr[i]
	}
	rmq_succinct_sct<false> rmaxq(&d_1);
	m_ds_rmaxq = std::move(rmaxq);
}

void FRLZSI::bcl_erzeugen(){
	/*
	 * Erwartet Initialisierung von m_t_array, m_sa
	 * erstellt m_b,m_c,m_gamma jeweils f�r t und tq
	 */
	m_b_t.resize(m_sa.size());
	m_c_t.resize(m_t_array.size());
	m_b_tq.resize(m_sa.size());
	m_c_tq.resize(m_t_array.size());
	vector<vector <uint64_t> > gamma_t(m_sa.size()-1);
	vector<vector <uint64_t> > gamma_tq(m_sa.size()-1);

	int i = 0;

	while(i < m_t_array.size()){
		gamma_t[m_t_array[i].first].push_back(m_t_array[i].second-m_t_array[i].first +1); // Vektor mit L�nge aller an der aktuellen Stelle beginnenden Strings
		gamma_tq[m_t_reverse_array[i].first].push_back(m_t_reverse_array[i].second-m_t_reverse_array[i].first +1);

		/*m_c_t und m_c_tq auf 0 inizialisieren*/
		m_c_t[i] = 0;
		m_c_tq[i] = 0;

		i++;
	}

	i = 0;
	while(i < gamma_t.size()){
		if(!gamma_t[m_sa[i+1]].empty()){
			int_vector<> gamma_help(gamma_t[m_sa[i+1]].size());
			for(int j = 0; j < gamma_t[m_sa[i+1]].size();j++){
				gamma_help[j] = gamma_t[m_sa[i+1]][j]; // Umtragen in die Membervariable im Zusammenhang mit SA
			}
			m_gamma_t.push_back(gamma_help);

			m_b_t[i] = 1;		// Faktor f�ngt an dieser Stelle an
		}else{
			m_b_t[i] = 0;
		}
		i++;
	}

	gamma_t.clear();
	i= 0;
	while(i < gamma_tq.size()){
		if(!gamma_tq[m_csa_bwd[i+1]].empty()){
			int_vector<> gammaq_help(gamma_tq[m_csa_bwd[i+1]].size());
			for(int j = 0; j < gamma_tq[m_csa_bwd[i+1]].size();j++){
				gammaq_help[j] = gamma_tq[m_csa_bwd[i+1]][j]; // Umtragen in die Membervariable im Zusammenhang mit SA
			}
			m_gamma_tq.push_back(gammaq_help);

			m_b_tq[i] = 1;		// Faktor f�ngt an dieser Stelle an
		}else{
			m_b_tq[i] = 0;
		}

		i++;
	}


	gamma_tq.clear();

	i = 0;
	int j = 0,k=0;
	while(i<m_gamma_t.size()){
		j += m_gamma_t[i].size();  // Anzahl an Faktoren in Gamma um c zu berechnen
		if(j != 0){
			m_c_t[j-1] = 1;
		}
		i++;
	}

	i = 0;
	while(i < m_gamma_tq.size()){
		k += m_gamma_tq[i].size();  
		if(k != 0){
			m_c_tq[k-1] = 1;
		}
		i++;
	}
}


void FRLZSI::p_zu_t(uint64_t st, uint64_t ed, uint64_t& p, uint64_t& q, uint64_t c){
	/*
	 * Erwartet Initialisierung von m_b_t, m_c_t bzw rank und select Strukturen
	 * erstellt q und p, Umrechnung von Range in m_sa zu Faktoren in t
	 */
	rank_support_v<1> m_b_t_rank(&m_b_t);	//exklusiv
	select_support_mcl<1> m_c_t_select(&m_c_t);	//inklusiv	
	uint64_t rank_helper_ed = m_b_t_rank(ed);
	if(rank_helper_ed == 0){
		p = 0;
		q = 0;
	}else{
		uint64_t rank_helper_st_1 = m_b_t_rank(st-1);
		uint64_t rank_helper_st_2 = m_b_t_rank(st);
		if(rank_helper_st_1 == 0){
			if(rank_helper_st_2 == 0){
				p = 1;
			}else{
				p = 1+ 0 + binaere_suche(m_gamma_t[rank_helper_st_2-1],c);
			}
		}else{
			if(rank_helper_st_2 != rank_helper_st_1){	// Es existiert ein Faktor, der an Stelle st beginnt
				p = 1+ m_c_t_select(rank_helper_st_1)+1 + binaere_suche(m_gamma_t[rank_helper_st_2-1],c);
			}else{
				p = 1+ m_c_t_select(rank_helper_st_1)+1;
			}
		}
		q = m_c_t_select(rank_helper_ed)+1;	//+1, da m_c_tq 0-indiziert	
	}
}

void FRLZSI::p_zu_tq(uint64_t st, uint64_t ed, uint64_t& p, uint64_t& q, uint64_t c){
	/*
	 * Erwartet Initialisierung von m_b_tq, m_c_tq bzw rank und select Strukturen
	 * erstellt q und p, Umrechnung von Range in m_sa zu Faktoren in tq
	 */
	rank_support_v<1> m_b_tq_rank(&m_b_tq);	//exklusiv
	select_support_mcl<1> m_c_tq_select(&m_c_tq);	//inklusiv
	uint64_t rank_helper_ed = m_b_tq_rank(ed);
	if(rank_helper_ed == 0){
		p = 0;
		q = 0;
	}else{
		uint64_t rank_helper_st_1 = m_b_tq_rank(st-1);
		uint64_t rank_helper_st_2 = m_b_tq_rank(st);
		if(rank_helper_st_1 == 0){
			if(rank_helper_st_2 == 0){
				p = 1;
			}else{
				p = 1+ 0 + binaere_suche(m_gamma_tq[rank_helper_st_2-1],c);
			}
		}else{
			if(rank_helper_st_2 != rank_helper_st_1){	// Es existiert ein Faktor, der an Stelle st beginnt
				p = 1+ m_c_tq_select(rank_helper_st_1)+1 + binaere_suche(m_gamma_tq[rank_helper_st_2-1],c);
			}else{
				p = 1+ m_c_tq_select(rank_helper_st_1)+1;
			}
		}
		q = m_c_tq_select(rank_helper_ed)+1;	//+1, da m_c_tq 0-indiziert	
	}
}

uint64_t FRLZSI::binaere_suche(int_vector<> &gamma, uint64_t c) {
	int_vector<>::iterator low;
	low = lower_bound(gamma.begin(), gamma.end(), c);

	if(low-gamma.begin() <= 0){
		return 0;
	}else{
		return (low-gamma.begin());
	}
}


void FRLZSI::y_array(string &pattern,vector<pair<uint64_t,uint64_t>> &y){
	/*
	 * Erwartet Initialisierung von m_sa
	 * erstellt y, Anfang und Ende der Range in m_f
	 */
	y.resize(pattern.size()); // jeweils pattern.size()-1
	vector<pair<uint64_t,uint64_t>> yq(pattern.size(),{0,0});
	/* Yq */

	uint64_t st_r = 0, ed_r = m_sa.size()-1;
	uint64_t l_res, r_res;
	for(uint64_t i = pattern.size()-1; i<pattern.size(); i--){
		// Idee zur Laufzeitverbesserung, rmaxq von ds auf l�ngsten Faktor, wenn pattern.size()-i gr��er als der l�ngste Faktor, muss der Rest 0 sein
		backward_search(m_sa, st_r, ed_r, pattern[i], l_res, r_res);
		if(l_res <= r_res){	//Sub-Pattern existiert
			st_r = l_res;
			ed_r = r_res;

			uint64_t st_t,ed_t;
			p_zu_t(st_r, ed_r, st_t, ed_t,pattern.size()-i);
			yq[i].first = st_t;
			yq[i].second = ed_t;
		}
		else{
			break;
		}
	}

	for(uint64_t i = 0; i<yq.size();i++){
		if(yq[i].first != 0 && yq[i].first <= yq[i].second){

			if(yq[i].first-1 == 0){
				y[i].first = 1;
			}else{
				y[i].first = m_v(yq[i].first)+1;
			}
			if(yq[i].second+1 > m_t_array.size()){
				y[i].second = m_v_array.size();
			}else{
				y[i].second =m_v(yq[i].second+1);
			}
		}
	}

}


void FRLZSI::q_array(string &pattern,int_vector<> &q_first , int_vector<> &q_second){
	/*
	 * Erwartet Initialisierung von y, a, m_f
	 * erstellt q, Anfang und Ende der Range in m_f, Zusammensetzung von a und y
	 */
	vector<pair<uint64_t,uint64_t>> y;
	y_array(pattern,y);

	for(uint64_t i = q_first.size()-1; i <q_first.size();i--){
		if(y[i].first != 0){ // �bernehmen der Werte von y, falls ungleich 0
			q_first[i] = y[i].first;
			q_second[i] = y[i].second;
		}else{
			uint64_t a_length;
			uint64_t a = a_array(pattern, i, a_length);		//Berechnung des Wertes a[i]
			if(a != 0 && q_first[i+a_length] != 0 && q_second[i+a_length] != 0){ // �bernehmen der Werte von a, falls ungleich 0
				uint64_t st_a_res=0, ed_a_res=0;
				backward_search(m_f,q_first[i+a_length],q_second[i+a_length],a,st_a_res, ed_a_res); // backward_search um den Faktor von a vorne in m_f zu finden, anhand von q
				if(st_a_res > ed_a_res){
					q_first[i] = 0;
					q_second[i] = 0;
				}else{
					q_first[i] = st_a_res;
					q_second[i] = ed_a_res;
				}
			}else{
				q_first[i] = 0;
				q_second[i] = 0;
			}
		}
	}	

}

void FRLZSI::m_array(string &pattern){
	/*
	 * Erwartet Initialisierung von m_m_array, q, m_csa_bwd,
	 * erstellt m f�r die Suche2
	 */
	int_vector<> q_first(pattern.size());
	int_vector<> q_second(pattern.size());
	q_array(pattern,q_first,q_second);
	uint64_t j = 0;
	uint64_t st_r_reverse = 0, ed_r_reverse = m_csa_bwd.size()-1;


	while(j < pattern.size()-1){
		uint64_t st_r_reverse_res, ed_r_reverse_res;
		backward_search(m_csa_bwd, st_r_reverse, ed_r_reverse,pattern[j], st_r_reverse_res, ed_r_reverse_res);
		if(st_r_reverse_res <= ed_r_reverse_res){
			st_r_reverse = st_r_reverse_res;
			ed_r_reverse = ed_r_reverse_res;
			uint64_t st_t,ed_t;
			p_zu_tq(st_r_reverse, ed_r_reverse, st_t, ed_t,j+1);
			j++;
			if(st_t <= ed_t && st_t != 0){	//T~ exitstiert
				if(q_first[j] > 0){
					auto res = m_m.range_search_2d(q_first[j]-1,q_second[j]-1,st_t,ed_t); // 2d Suche um m_t_array und m_f zu verkn�pfen und Treffer zu finden
					for(auto point : res.second){
						phase_2(point.first+1,(-j));
					}
				}
			}
		}
		else{

			break;
		}
	}
}

void FRLZSI::phase_1(uint64_t factor,uint64_t st_pos){
	/*
	 * Erwartet Initialisierung von m_v
	 * Berechnet m_f range aus m_t_array von Suche 1
	 */
	uint64_t st,ed;
	st = m_v(factor)+1;
	if(factor != m_t_array.size()){
		ed = m_v(factor+1)+1;
	}else{
		ed = m_v_array.size()+1;
	}
	for(int i = 0; i< ed-st; i++){
		phase_2(st+i,st_pos);
	}


}
int exist = 0; // 0, es wurde kein pattern gefunden, 1 mindestens ein pattern wurde gefunden
vector<pair<int,int>> projekt_treffer;
void FRLZSI::phase_2(uint64_t factor,uint64_t st_pos){
	/*
	 * Erwartet Initialisierung von m_c_rank,m_f
	 * Berechnet die Positionen der Treffer
	 */
	uint64_t i;
	i = m_c_rank(m_f[factor]+1);
	exist = 1;
	anzahl_treffer++;
	if(m_f[factor] == 0){
		i = 1;
		projekt_treffer.push_back(std::make_pair(i,st_pos));
	}else{
		projekt_treffer.push_back(std::make_pair(i,m_l[m_f[factor]-1]+st_pos));
	}
}


void FRLZSI::return_treffer(vector<pair<int,int>> &treffer){
	treffer = std::move(projekt_treffer);	
}


/*
 * SANDRA
 */
/*oeffentlich aufrufbare Methode zum auffinden eines Patterns in S*/
void FRLZSI::search_pattern(string &pattern){

	select_support_mcl<1> v_select(&m_v_array);
	m_v = std::move(v_select);

	rank_support_v<1> c_rank(&m_c);
	m_c_rank = std::move(c_rank);

	uint64_t i=0, j=m_sa.size()-1, l_res=0, r_res=0,l_res_help,r_res_help;
	backward_search(m_sa, i, j, pattern.begin(), pattern.end(), l_res, r_res);	// Rueckwaertssuche => startIndex, endIndex

	if(l_res <= r_res && r_res <= m_sa.size()-1){	//Pattern existiert in R
		searchPattern(l_res-1,r_res-1, pattern.size());
	}
	m_array(pattern);
	if(exist != 1){
		cout << "Das Pattern wurde nicht gefunden!" << endl;
	}	
}

/*sucht solange das Maximum in d_Strich bis Faktorlaenge < Patternlaenge und berechnet jeweils die zugehoerigen Faktoren mittels getFactors.*/
void FRLZSI::searchPattern(uint64_t st,uint64_t ed, uint64_t patternLength){
	uint64_t maxPosition = m_ds_rmaxq(st,ed);	//Maximum von d_Strich innerhalb des Bereichst st,ed
	//Berechnung des Wertes d_Strich[maxPosition]
	int_vector<>::iterator up;
	up = upper_bound(m_is.begin(), m_is.end(), m_sa[maxPosition+1]);	//endIndex f�r rmaxq in ie (binaere Suche)
	if(up-m_is.begin()-1 >= 0){	//Abbruch, wenn kein Pattern an der Stelle beginnt
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
}

/*sucht das Maximum in ie und gibt den Faktor aus, falls ie-Wert >= startIndex+Patternlaenge-1*/
void FRLZSI::getFactors(uint64_t startIndex, uint64_t patternLength, uint64_t ieStartIndex, uint64_t ieEndIndex){
	uint64_t factorPosition = m_ie_rmaxq(ieStartIndex,ieEndIndex);	//naechster moeglicher Faktor beim Maximum von ie
	if(m_t_array[m_g[factorPosition]-1].second >= startIndex+patternLength-1){	//Abbruch falls Pattern nicht mehr im Faktor liegt
		phase_1(m_g[factorPosition],startIndex-m_is[factorPosition]);

		if(factorPosition < ieEndIndex){
			getFactors(startIndex, patternLength, factorPosition+1, ieEndIndex);
		}
		if(factorPosition > ieStartIndex){
			getFactors(startIndex, patternLength, ieStartIndex, factorPosition-1);
		}
	}
}


/*a-Array fuer die Suche im 2.Fall*/
uint64_t FRLZSI::a_array(string &pattern, uint64_t pos, uint64_t &length){
	/*Berechnet den Wert des A-Arrays an Position pos. Da kein delete_back() existiert ist es schneller nicht alle A-Array-Werte zu berechnen.*/
	uint64_t a = 0;
	length = 0;
	uint64_t j = pos;
	uint64_t st_r = 0, ed_r = m_csa_bwd.size()-1, st_r_reverse = 0, ed_r_reverse = m_csa_bwd.size()-1;
	while(j < pattern.size()){
		uint64_t st_r_res, ed_r_res, st_r_reverse_res, ed_r_reverse_res;
		bidirectional_search(m_csa_bwd, st_r_reverse, ed_r_reverse, st_r, ed_r, pattern[j], st_r_reverse_res, ed_r_reverse_res, st_r_res, ed_r_res);
		if(st_r_res <= ed_r_res){	//P[i..j] existiert in R
			st_r = st_r_res;
			ed_r = ed_r_res;
			st_r_reverse = st_r_reverse_res;
			ed_r_reverse = ed_r_reverse_res;			
			uint64_t st_t, ed_t, c;
			p_zu_t(st_r, ed_r, st_t, ed_t,j-pos+1);
			if(st_t <= ed_t && st_t != 0){	//P[i..j] ist moeglicherweise ein Faktor in T
				if(j-pos+1 == m_t_array[st_t-1].second - m_t_array[st_t-1].first + 1){ //Faktor existiert (Nur ersten Faktor ueberpruefen, da die Faktoren in T lexikographisch sortiert sind)
					length = j-pos+1;
					a = st_t;	
				}
			}else{
				break;
			}
			j++;
		}else{
			break;
		}
	}
	return a;
}


/*Zerlegt die einzelnen Strings in Faktoren relativ zum Referenzstring R*/
int_vector<> FRLZSI::LZ_factorization(string &R, vector<string> &S, vector<int_vector<>> &m_s){
	vector<tuple<int,pair<int,int>,int,int,int>> factors;	//tuple(ISA,is,ie,Si,faktornummer,T~)
	vector<tuple<int,int,pair<int,int>,int>> factors_reverse;		//tuple(ISA_reverse,faktorlaenge,pair(is,ie),pos)

	uint64_t count = 0;
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
				factors.push_back(std::make_tuple (l_bwd,temp,i,number_of_factors,count));

				temp.first = m_csa_bwd[l_fwd];	//factors_reverse
				temp.second = temp.first + factorLength-1;
				factors_reverse.push_back(std::make_tuple (l_fwd,factorLength,temp,count));
				count++;

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
		factors.push_back(std::make_tuple (l_bwd,temp,i,number_of_factors,count));

		temp.first = m_csa_bwd[l_fwd];	//factors_reverse
		temp.second = temp.first + factorLength-1;
		factors_reverse.push_back(std::make_tuple (l_fwd,factorLength,temp,count));
		count++;

		int_vector<> s(number_of_factors+1);
		m_s[i] = s;
	}

	sort(factors_reverse.begin(), factors_reverse.end());	//nach SA sortieren
	vector<tuple<int,int,pair<int,int>,int>>::iterator f_reverse_iter = factors_reverse.begin();
	vector<tuple<int,int,pair<int,int>,int>>::iterator f_reverse_iter2;
	uint64_t pos = 0;
	get<4>(factors[get<3>(*f_reverse_iter)]) = pos;
	f_reverse_iter2 = factors_reverse.begin();
	f_reverse_iter++;
	while(f_reverse_iter != factors_reverse.end()){
		if(get<2>(*f_reverse_iter) != get<2>(*f_reverse_iter2)){
			pos++;
		}
		get<4>(factors[get<3>(*f_reverse_iter)]) = pos;
		f_reverse_iter++;
		f_reverse_iter2++;
	}

	sort(factors.begin(), factors.end());	//nach SA sortieren


	//m_s[i] initialisieren
	if(!factors.empty()){
		vector<tuple<int,pair<int,int>,int,int,int>>::iterator iter;
		vector<tuple<int,pair<int,int>,int,int,int>>::iterator iter2;
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

	vector<tuple<int,int,pair<int,int>,int>>::iterator iter_reverse = factors_reverse.begin();
	for(vector<tuple<int,pair<int,int>,int,int,int>>::iterator iter = factors.begin(); iter != factors.end(); iter++){  	
		m_t_array.push_back(get<1>(*iter));
		m_t_reverse_array.push_back(get<2>(*iter_reverse));
		iter_reverse++;
	}

	m_t_array.erase(unique(m_t_array.begin(), m_t_array.end()), m_t_array.end());	//gleiche Faktoren loeschen
	m_t_reverse_array.erase(unique(m_t_reverse_array.begin(), m_t_reverse_array.end()), m_t_reverse_array.end());

	vector<tuple<int,pair<int,int>,int,int,int>>::iterator iter = factors.begin();
	uint64_t vector_entry_size = (int) (log(m_t_array.size())/log(2)) + 1;
	int_vector<> t_to_t_reverse(m_t_array.size(),0,vector_entry_size);
	for(int i=0; i<t_to_t_reverse.size(); i++){
		t_to_t_reverse[i]= get<4>(*iter);
		while(get<4>(*iter)==t_to_t_reverse[i]){
			iter++;
		}
	}

	return t_to_t_reverse;
}

/*Erzeugt das F-Array*/
void FRLZSI::f_array(uint64_t max, vector<int_vector<>> &m_s){
	uint64_t eos = m_t_array.size()+1;
	uint64_t length = 0;
	//csa der einzelnen S-Zerlegungen -> int_vectoren zusammenfassen
	for(int i = 0; i< m_s.size(); i++){
		length += m_s[i].size();
	}
	int_vector<> seg(length+m_s.size());
	m_l.width(max);
	m_c.resize(length+m_s.size());	//m_c => pos. der Trennsymbole
	m_l.resize(length+m_s.size());	//Laenge des Strings S bis zum Beginn des i-ten Faktors
	uint64_t counter = 0;
	for(int i = 0; i< m_s.size(); i++){
		for(int j = 0; j<m_s[i].size(); j++){
			seg[counter] = m_s[i][j];
			if(j == 0){
				m_c[counter] = 1;
				m_l[counter] = m_t_array[m_s[i][j]-1].second - m_t_array[m_s[i][j]-1].first+1;
			}else{
				m_c[counter] = 0;
				m_l[counter] = m_l[counter-1] + m_t_array[m_s[i][j]-1].second - m_t_array[m_s[i][j]-1].first+1;
			}
			counter++;
		}
		seg[counter] = eos;
		m_c[counter] = 0;
		m_l[counter] = 0;
		counter++;
	}	

	string filename = "m_f";
	store_to_file(seg,filename);
	construct(m_f,filename, 0); // 0=Serialisierter int_vector<>
	remove("m_f");

	//m_v initialisieren
	bit_vector v(length);
	v[0] = 1;
	for(int i=2; i<length+2; i++){
		if(seg[m_f[i]] != seg[m_f[i-1]]){
			v[i-1] = 1;
		}
		else{
			v[i-1] = 0;
		}
	}
	m_v_array = std::move(v);
}

/*Erzeugt den M-Vektor*/
void FRLZSI::initialize_m(int_vector<> &t_to_t_reverse){
	uint64_t vector_entry_size = (int) (log(m_t_array.size())/log(2)) + 1;
	int_vector<> m_array(m_v_array.size(),0,vector_entry_size);
	for(int i=0; i<m_array.size(); i++){
		int t = m_f.bwt[i+1];
		if(t == m_t_array.size()+1 || t == 0){	//$
			m_array[i] = 0;
		}else{
			m_array[i] = t_to_t_reverse[t-1]+1;
		}
	}
	string filename = "int_vector";
	store_to_file(m_array,filename);
	construct(m_m,filename, 0); // 0=Serialisierter int_vector<>
	remove("int_vector");
}
