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
	int_vector<> t_to_t_reverse = LZ_factorization(r, s);		//m_t_array, m_s initialisieren
	g_Array(); 			//m_g, m_is, m_ie_rmaxq() initialisieren
	d_Strich(d_Array());		//m_ds initialisieren
	bcl_erzeugen();			// Datenstruktur X(T) füllen
	f_array();				//m_f und m_v initialisieren
	initialize_m(t_to_t_reverse);	//m_m_array initialisieren
	uint64_t a,b;
	p_zu_t(8,9,a,b,1);
	vector<pair<uint64_t,uint64_t>> ya;
	for(int i = 1; i<10;i++){
		cout << "Stelle: " << i << "V: " << m_v(i) << endl;
	}
	//cout << "27: " <<  a << " " << b << endl;
	//for(int i = 0; i<ya.size();i++){
	//	cout << ya[i].first << " " << ya[i].second << endl;
	//}
	//cout << "a: " << a_array("AGTA") << endl;
	m_array();
	
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
	
	
	int i = 0;
	while(i < m_t_array.size()){
		gamma_t[m_t_array[i].first].push_back(m_t_array[i].second-m_t_array[i].first +1); // Vektor mit Länge aller an der aktuellen Stelle beginnenden Strings
		gamma_tq[m_t_reverse_array[i].first].push_back(m_t_reverse_array[i].second-m_t_reverse_array[i].first +1);
		i++;
	}


	i = 0;
	while(i < gamma_t.size()){
		if(!gamma_t[m_sa[i+1]].empty()){
			m_gamma_t.push_back(gamma_t[m_sa[i+1]]);		// Umtragen in die Membervariable im Zusammenhang mit SA
			m_b_t[i] = 1;		// Faktor fängt an dieser Stelle an
		}else{
			m_b_t[i] = 0;
			}
		if(!gamma_tq[m_csa_bwd[i+1]].empty()){
			m_gamma_tq.push_back(gamma_tq[m_csa_bwd[i+1]]);		// Umtragen in die Membervariable im Zusammenhang mit SA
			m_b_tq[i] = 1;		// Faktor fängt an dieser Stelle an
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
	while(i < m_sa.size()){
	//cout  << m_c_t[i] << " ; " << m_sa[i+1] << " |" << m_b_t[i] << endl;
		//cout << i << " " << b[i] << " , " << gamma[i] << " | " << m_sa[i+1] << " : " << m_t_array[i].first << endl;
	i++;
	}
	i = 0;
	while(i < m_gamma_t.size()){
	cout  << m_gamma_tq[i] << " ; " << m_c_t[i] << endl;
	i++;
	}
	i = 0;
	while(i < m_gamma_tq.size()){
	//cout  << m_gamma_tq[i] << " - " << m_c_tq[i] << endl;
	i++;
	}
}


void FRLZSI::p_zu_t(uint64_t st, uint64_t ed, uint64_t& p, uint64_t& q, uint64_t c){
	rank_support_v<1> m_b_t_rank(&m_b_t);
	select_support_mcl<1> m_c_t_select(&m_c_t);	
	uint64_t rank_helper = m_b_t_rank(st-1);
	if(rank_helper == 0){
		//cout << "lll" << endl;
		p = binaere_suche(m_b_t_rank(st),c)+1;
	}else{
	p = 1+m_c_t_select(m_b_t_rank(st-1)) + binaere_suche(m_b_t_rank(st),c)+1; // +1 Zusatz, da yq es so braucht 
	}
	rank_helper = m_b_t_rank(ed);
	if(rank_helper == 0){
		q = 0;
	}else{
	q = m_c_t_select(m_b_t_rank(ed))+1;
	}
}

void FRLZSI::p_zu_tq(uint64_t st, uint64_t ed, uint64_t& p, uint64_t& q, uint64_t c){
	rank_support_v<1> m_b_tq_rank(&m_b_tq);
	select_support_mcl<1> m_c_tq_select(&m_c_tq);	
	uint64_t rank_helper = m_b_tq_rank(st-1);
	if(rank_helper == 0){
		//cout << "lll" << binaere_suche(m_b_tq_rank(st),c) << endl;
		p = binaere_suche(m_b_tq_rank(st),c)+1;
	}else{

	p = 1+m_c_tq_select(m_b_tq_rank(st-1)) + binaere_suche(m_b_tq_rank(st),c); 
	}
	rank_helper = m_b_tq_rank(ed);
	if(rank_helper == 0){
		q = 0;
	}else{
	q = m_c_tq_select(m_b_tq_rank(ed));
	}
}

uint64_t FRLZSI::binaere_suche(uint64_t b_rank,uint64_t c) {
	vector <int>::iterator low;
	vector <int> gamma_b;
	gamma_b.push_back(0);
		copy (m_gamma_t[b_rank].begin(),m_gamma_t[b_rank].end(),back_inserter(gamma_b));
    //cout << gamma_b << " :)" << '\n';

	low = lower_bound(gamma_b.begin(), gamma_b.end(), c);
	//cout << (low-gamma_b.begin())-1<< "." << gamma_b <<":" << c << '\n';
	return (low-gamma_b.begin())-1;
}
  /* uint64_t l=0;

   uint64_t x;

	   uint64_t r= gamma_b.size()-1;
   while(r >= l) {
      x=l + ((r-l)/2);
      if(c < gamma_b[x] ) 
		if(x == 0){
			return 0;
		}else{
         r=x-1;  
	 }
       else      
       if(x == gamma_b.size()-1){
		   return gamma_b.size();
	   }else{
         l=x+1;  
	 }
       if(gamma_b[x] == c && x != 0)
          return x;     
   }
	if(c < gamma_b[x])
	return x;
	else
	return x+1;
}*/


void FRLZSI::y_array(vector<pair<uint64_t,uint64_t>> &y){
	string pattern = "AGTA";
	y.resize(pattern.size()); // jeweils pattern.size()-1
	vector<pair<uint64_t,uint64_t>> yq(pattern.size());
	//bit_vector v_test = {1,1,1,1,1,0,1,0,1,1};		//f.size()

	/* Yq */
	for(uint64_t i = 0; i<pattern.size(); i++){
		uint64_t j = 0;	//in for-Schleife, da kein delet_back()
		uint64_t st_r = 0, ed_r = m_csa_bwd.size()-1, st_r_reverse = 0, ed_r_reverse = m_csa_bwd.size()-1;
		
		string sub_pattern = pattern.substr (i,pattern.size()-1);
		while(j < sub_pattern.size()){
			uint64_t st_r_res, ed_r_res, st_r_reverse_res, ed_r_reverse_res;
			bidirectional_search(m_csa_bwd, st_r_reverse, ed_r_reverse, st_r, ed_r, sub_pattern[j], st_r_reverse_res, ed_r_reverse_res, st_r_res, ed_r_res);
			if(st_r_res <= ed_r_res){	//P[i..j] existiert in R
				st_r = st_r_res;
				ed_r = ed_r_res;
				st_r_reverse = st_r_reverse_res;
				ed_r_reverse = ed_r_reverse_res;
				uint64_t st_t,ed_t;
				p_zu_t(st_r, ed_r, st_t, ed_t,j+1);
				//cout << "st_t, ed_t: " << st_r << " " << ed_r << endl;
				if(j == sub_pattern.size()-1){
					cout << "SA:" << st_t << " " << ed_t << endl;
					yq[i].first = st_t;
					yq[i].second = ed_t;
				}
				
				j++;
			}
			else{
			
				break;
			}
		}

	}
	for(int i = 0; i<yq.size();i++){
		cout << yq[i].first << " " << yq[i].second << endl;
	}
	/* Y */
	
	//select_support_mcl<1> v_select(&v_test);	
	for(uint64_t i = 0; i<yq.size();i++){
		cout << "a" << endl;
		if(yq[i].first != 0){
			if(yq[i].first-1 == 0){
				y[i].first = 1;
			}else{
				y[i].first = m_v(yq[i].first)+1;
			}
			cout << "b" << endl;
		y[i].second =m_v(yq[i].second+1);
			cout << "c" << endl;
		}
	}
	for(int i = 0; i<y.size();i++){
		cout << "y1: " << y[i].first << " " << y[i].second << endl;
	}

}


void FRLZSI::q_array(string &pattern,int_vector<> &q_first , int_vector<> &q_second){
	//vector<pair<uint64_t,uint64_t>> q(pattern.size());
	vector<pair<uint64_t,uint64_t>> y;
	int_vector<> a_length;
	int_vector<> a = a_array(pattern,a_length);
	y_array(y);
	for(uint64_t i = q_first.size()-1; i <q_first.size();i--){
		if(y[i].first != 0){
			q_first[i] = y[i].first;
			q_second[i] = y[i].second;
			}else if(a[i] != 0){
				uint64_t st_a_res=0, ed_a_res=0;
				backward_search(m_f,q_first[i+a_length[i]],q_second[i+a_length[i]],a[i],st_a_res, ed_a_res);
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
	int i = 0;
	while(i<q_first.size()){
	//cout << "q_array: " << q_first[i] << " " << q_second[i] << endl;
	i++;
}
}

void FRLZSI::m_array(){
	string filename = "int_vector";
	int_vector<> fff = {7,5,7,0,0,4,1,0,3,0};
	store_to_file(fff,filename);
	construct(m_m,filename, 0); // 0=Serialisierter int_vector<>	
	string pattern = "AGTA";
	int_vector<> q_first(pattern.size());
	int_vector<> q_second(pattern.size());
	q_array(pattern,q_first,q_second);

		uint64_t j = 0;
		uint64_t st_r_reverse = 0, ed_r_reverse = m_csa_bwd.size()-1;
		

		while(j < pattern.size()){  // pattern.size() muss später geändert werden, in den linken Teil den die querry findet
			uint64_t st_r_reverse_res, ed_r_reverse_res;
			backward_search(m_csa_bwd, st_r_reverse, ed_r_reverse,pattern[j], st_r_reverse_res, ed_r_reverse_res);
			if(st_r_reverse_res <= ed_r_reverse_res){
				st_r_reverse = st_r_reverse_res;
				ed_r_reverse = ed_r_reverse_res;
				uint64_t st_t,ed_t;
				p_zu_tq(st_r_reverse, ed_r_reverse, st_t, ed_t,j);
				cout << "st_res, ed_res: " << st_r_reverse_res << " " << ed_r_reverse_res << endl;
				cout << "st_t, ed_t: " << st_t << " " << ed_t << endl;
				j++;
				cout << "q: " << q_first[j] << " " << q_second[j] << endl;
					auto res = m_m.range_search_2d(q_first[j]-1,q_second[j]-1,st_t+1,ed_t+1);
			cout << res.first << " Wert 2D" << endl;
			for(auto point : res.second)
				cout << "(" << point.first+1 << "," << point.second << ") ";
			cout << endl;
			cout << endl;
			}
			else{
			
				break;
			}
		}
	
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
	int_vector<> length;
	a_array(pattern, length);
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
int_vector<> FRLZSI::a_array(string pattern, int_vector<> &length){
	length.resize(pattern.size());
	int_vector<> a(pattern.size());
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
				
				uint64_t st_t, ed_t, c;
				p_zu_t(st_r, ed_r, st_t, ed_t,j+1);
				if(st_t <= ed_t){	//P[i..j] ist moeglicherweise ein Faktor in T
					for(int k = st_t; k<=ed_t; k++){
						if(j+1 == m_t_array[k-1].second - m_t_array[k-1].first + 1){		//Faktor existiert
							a[i] = k;
							length[i] = j+1;
						}
					}
				}
				else{
					break;
				}
				j++;
			}
			else{
				break;
			}
		}

	}
	return a;
}


/*Zerlegt die einzelnen Strings in Faktoren relativ zum Referenzstring R*/
int_vector<> FRLZSI::LZ_factorization(string &R, vector<string> &S){
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
	int_vector<> t_to_t_reverse(m_t_array.size());
	for(int i=0; i<t_to_t_reverse.size(); i++){
		t_to_t_reverse[i]= get<4>(*iter);
		while(get<4>(*iter)==t_to_t_reverse[i]){
			iter++;
		}
	}

	return t_to_t_reverse;
}

/*Erzeugt das F-Array*/
void FRLZSI::f_array(){
	uint64_t eos = m_t_array.size()+1;
	uint64_t length = 0;
	//csa der einzelnen S-Zerlegungen -> int_vectoren zusammenfassen
	for(int i = 0; i< m_s.size(); i++){
		length += m_s[i].size();
	}
	int_vector<> seg(length+m_s.size());
	uint64_t counter = 0;
	for(int i = 0; i< m_s.size(); i++){
		for(int j = 0; j<m_s[i].size(); j++){
			seg[counter] = m_s[i][j];
			counter++;
		}
			seg[counter] = eos;
			counter++;
	}	
	construct_im(m_f, seg, 0);
	
	//m_v initialisieren
	bit_vector v(length);
	v[0] = 1;
	for(int i=2; i<m_f.size(); i++){
		if(seg[m_f[i]] != seg[m_f[i-1]]){
			v[i-1] = 1;
		}
		else{
			v[i-1] = 0;
		}
	}
	m_v_array = std::move(v);
	select_support_mcl<1> v_select(&m_v_array);
	m_v = std::move(v_select);
}

/*Erzeugt den M-Vektor*/
void FRLZSI::initialize_m(int_vector<> &t_to_t_reverse){
	m_m_array.resize(m_v_array.size());
	for(int i=0; i<m_m_array.size(); i++){
		int t = m_f.bwt[i+1];
		if(t == m_t_array.size()+1 || t == 0){	//$
			m_m_array[i] = 0;
		}else{
			m_m_array[i] = t_to_t_reverse[t-1]+1;
		}
	}
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
	while(m_is[j] == i){				// überprüfen, ob an der aktuellen Stelle ein String anfängt
	gamma_help.push_back(m_t_array[m_g[j]-1].second-m_is[j] +1); // Vektor mit Länge aller an der aktuellen Stelle beginnenden Strings
	j++;
	b_help = 1;					// b wird an dieser Stelle auf 1 gesetzt, da min ein String anfängt
	//cout << gamma_help[0] << " ; " << sa[i]-1 << endl;
	}
	gamma.push_back(gamma_help);			// Vektor von Vektoren mit der Länge der Strings die an der aktuellen Stelle beginnen
	b.push_back(b_help);				// Vektor der anzeigt das an der aktuellen Stelle ein String beginnt
	b_help = 0;
	if(j != 0){
	c[j-1] = 1;					// C Vektor Eintrag, da j die Anzahl an String die bisher geprüft wurden
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
