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
		FRLZSI(string r, vector<string> &s);
		~FRLZSI();
		void search_pattern(string pattern);
		
	private:
		vector<pair<int,int>> m_t_array;
		csa_wt<> m_sa;
		/* I(T) Datenstruktur */
		int_vector<> m_g;
		int_vector<> m_is;
		rmq_succinct_sct<false> m_ie_rmaxq;
		rmq_succinct_sct<false> m_ds_rmaxq;
		
		/*
		 * HARRY
		*/
		void getLempelZivFactors(string r, vector<string> &s);
		
		/*
		 * OLI
		*/
		void g_Array_Sort();
		int_vector<> d_Array();
		void d_Strich(int_vector<> d);
		
		/*
		 * SANDRA
		*/
		void searchPattern(uint64_t st,uint64_t ed, uint64_t patternLength);
		void getFactors(uint64_t startIndex, uint64_t patternLength, uint64_t ieStartIndex, uint64_t ieEndIndex);
};

#endif
