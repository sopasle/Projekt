#ifndef __FRLZSI_HPP
#define __FRLZSI_HPP

#include <sdsl/int_vector.hpp>
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/rmq_support.hpp>
#include <vector>
#include <string>

using namespace sdsl;
using namespace std;


class FRLZSI{
	public: 
		FRLZSI();				// Konstruktor
		FRLZSI(string r, vector<string> &s);	// Konstruktor mit Aufruf Referenzstring, Vektor mit Eingabestrings
		~FRLZSI();				// Destruktor
		void search_pattern(string pattern);	// Patternsuche
		void test_ausgabe();			// Testausgaben
		void test_search(string pattern);
		void test_LZ_factorization(string r, vector<string> s);
		
	private:
		vector<pair<int,int>> m_t_array;	// T-Array, (Faktoranfang,Faktorende) im Referenzstring
		csa_wt<> m_sa;				// Suffix-Array
			int_vector<> m_ds;		// D'-Array, für Ausgabetests
		/* I(T) Datenstruktur */
		int_vector<> m_g;			// G-Array
		int_vector<> m_is;			// IS-Array = Faktoranfang im Referenzstring
		rmq_succinct_sct<false> m_ie_rmaxq;	// RMQ auf IE	
		rmq_succinct_sct<false> m_ds_rmaxq;	// RMQ auf D'
		
		
		/*
		 * OLI
		*/
		void g_Array();				// G-Array erstellen
		int_vector<> d_Array();			// D-Array erstellen
		void d_Strich(int_vector<> d);		// D'-Array erstellen

		void d_ArrayTest();			// D-Array naiv für ein sicheres Ergebnis
		
		/*
		 * SANDRA
		*/
		void searchPattern(uint64_t st,uint64_t ed, uint64_t patternLength);	// Patternsuche
		void getFactors(uint64_t startIndex, uint64_t patternLength, uint64_t ieStartIndex, uint64_t ieEndIndex);
		void LZ_factorization(string R, vector<string> S); // Faktoren herausfinden
};

#endif
