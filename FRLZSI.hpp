#ifndef __FRLZSI_HPP
#define __FRLZSI_HPP

#include <sdsl/int_vector.hpp>
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/rmq_support.hpp>
#include <vector>
#include <string>
#include <sdsl/util.hpp>

using namespace sdsl;
using namespace std;




class FRLZSI{
	public: 
		FRLZSI(string &r);			// Konstruktor
		FRLZSI(string &r, vector<string> &s);	// Konstruktor mit Aufruf Referenzstring, Vektor mit Eingabestrings
		~FRLZSI();				// Destruktor
		void search_pattern(string &pattern);	// Patternsuche
		typedef uint64_t size_type;
		uint64_t projekt_serialize();
		void projekt_load();
		void return_treffer(vector<pair<int,int>> &treffer);

		size_type serialize_vpii(const vector<pair<int,int>>& vpii, std::ostream& out, structure_tree_node* v, std::string name="")const
		{	
   			structure_tree_node* child = structure_tree::add_child(v, name, "vector<pair<int,int>>");
   			uint64_t written_bytes = 0;
			uint64_t size=vpii.size();
			written_bytes += write_member(size, out);
			out.write((char*)vpii.data(), vpii.size()*sizeof(vpii[0]));
			written_bytes += vpii.size()*sizeof(vpii[0]);
    			structure_tree::add_size(child, written_bytes);
    			return written_bytes;
		}	

		size_type serialize_vintv(const vector<int_vector<>>& viv, std::ostream& out, structure_tree_node* v, std::string name="")const
		{
    			structure_tree_node* child = structure_tree::add_child(v, name, "vector<int_vector<>>");
    			uint64_t written_bytes = 0;
			uint64_t size=viv.size();
			written_bytes += write_member(size, out);
			for(int i = 0; i < viv.size();i++){
				written_bytes +=  viv[i].serialize(out, child, "viv");
			}
    			structure_tree::add_size(child, written_bytes);
    			return written_bytes;
		}

		void load_vpii(vector<pair<int,int>>& vpii, std::istream& in) {
			uint64_t size;
			read_member(size, in);
			vpii.resize(size);
			in.read((char*)vpii.data(), size*sizeof(vpii[0]));
		}
		void load_vintv(vector<int_vector<>>& viv, std::istream& in) {
			uint64_t size;
			read_member(size, in);
			viv.resize(size);
			for(int i = 0; i < viv.size();i++){
				viv[i].load(in);
			}
		}



		//! Load the data structure
        	void load(std::istream& in) {	
			m_sa.load(in);
			m_csa_bwd.load(in);
			m_g.load(in);
			m_is.load(in);
			m_ie_rmaxq.load(in);
			m_ds_rmaxq.load(in);
			m_b_t.load(in);
			m_c_t.load(in);
			m_b_tq.load(in);
			m_c_tq.load(in);
			m_f.load(in);
			m_v_array.load(in);
			m_m.load(in);
			m_c.load(in);
			m_l.load(in);
			load_vpii(m_t_array, in);
			load_vpii(m_t_reverse_array, in);
			load_vintv(m_gamma_t, in);	
			load_vintv(m_gamma_tq, in);
		}

		//! Serialize the data structure
		size_type serialize(std::ostream& out, structure_tree_node* v=nullptr, std::string name="")const {
			structure_tree_node* child = structure_tree::add_child(v, name, util::class_name(*this));
			size_type written_bytes = 0;
			written_bytes += m_sa.serialize(out, child, "sa");
			written_bytes += m_csa_bwd.serialize(out, child, "csa_bwd");
			written_bytes += m_g.serialize(out, child, "g");
			written_bytes += m_is.serialize(out, child, "is");
			written_bytes += m_ie_rmaxq.serialize(out, child, "ie RMQ");
			written_bytes += m_ds_rmaxq.serialize(out, child, "ds RMQ");
			written_bytes += m_b_t.serialize(out, child, "b_t");
			written_bytes += m_c_t.serialize(out, child, "c_t");
			written_bytes += m_b_tq.serialize(out, child, "b_tq");
			written_bytes += m_c_tq.serialize(out, child, "c_tq");
			written_bytes += m_f.serialize(out, child, "f");
			written_bytes += m_v_array.serialize(out, child, "v_array");
			written_bytes += m_m.serialize(out, child, "m");
			written_bytes += m_c.serialize(out, child, "c");
			written_bytes += m_l.serialize(out, child, "l");
			string t="t_array";
			written_bytes += serialize_vpii(m_t_array,out, nullptr, t);
			written_bytes += serialize_vpii(m_t_reverse_array,out, nullptr, t);
			written_bytes += serialize_vintv(m_gamma_t,out, nullptr, t);
			written_bytes += serialize_vintv(m_gamma_tq,out, nullptr, t);
			structure_tree::add_size(child, written_bytes);
			return written_bytes;
		}

	private:
		vector<pair<int,int>> m_t_array;	// T-Array, (Faktoranfang,Faktorende) im Referenzstring	
		vector<pair<int,int>> m_t_reverse_array; //T-quer-Array (Faktoranfang,Faktorende)
		csa_wt<> m_sa;				// Suffix-Array	
		csa_wt<wt_hutu<>> m_csa_bwd;	// Suffix-Array von R reverse
		//vector<int_vector<>> m_s;		// Faktorzerlegung der einzelnen S	
		/* I(T) Datenstruktur */
		int_vector<> m_g;			// G-Array
		int_vector<> m_is;			// IS-Array = Faktoranfang im Referenzstring
		rmq_succinct_sct<false> m_ie_rmaxq;	// RMQ auf IE
		rmq_succinct_sct<false> m_ds_rmaxq;	// RMQ auf D'

		/* X(T) Datenstruktur */
		bit_vector m_b_t;			// B-Bitvektor
		bit_vector m_c_t;			// C-Bitvektor
		vector<int_vector<>> m_gamma_t;		// Gamma-Array
		/* X(T~) Datenstruktur */
		bit_vector m_b_tq;			// B-Bitvektor
		bit_vector m_c_tq;			// C-Bitvektor
		vector<int_vector<>> m_gamma_tq;		// Gamma-Array

		csa_wt<wt_int<>, 32, 512, sa_order_sa_sampling<>, int_vector<>, int_alphabet<>> m_f;	//F-Array
		select_support_mcl<1> m_v; //V-Bitvektor
		bit_vector m_v_array;
		wt_int<> m_m;
		bit_vector m_c;	
		rank_support_v<1> m_c_rank;	
		int_vector<> m_l;
		/*
		 * OLI
		 */
		void g_Array();				// G-Array erstellen
		int_vector<> d_Array();			// D-Array erstellen
		void d_Strich(int_vector<> d);		// D'-Array erstellen
		void d_ArrayTest();			// D-Array naiv für ein sicheres Ergebnis
		void p_zu_t(uint64_t st, uint64_t ed, uint64_t& p, uint64_t& q, uint64_t c);			// Funktion für die Datenstruktur X(T)
		void p_zu_tq(uint64_t st, uint64_t ed, uint64_t& p, uint64_t& q, uint64_t c);			// Funktion für die Datenstruktur X(TQ)
		/* für X(T) */
		void bcl_erzeugen();		
		uint64_t binaere_suche(int_vector<> &gamma, uint64_t c);

		/* y(F,T)*/
		void y_array(string &pattern,vector<pair<uint64_t,uint64_t>> &y);
		void q_array(string &pattern,int_vector<> &q_first , int_vector<> &q_second);

		void m_array(string &pattern);
		void phase_1(uint64_t factor,uint64_t st_pos);
		void phase_2(uint64_t factor,uint64_t st_pos);
		/*
		 * SANDRA
		 */
		void searchPattern(uint64_t st,uint64_t ed, uint64_t patternLength);	// Patternsuche
		void getFactors(uint64_t startIndex, uint64_t patternLength, uint64_t ieStartIndex, uint64_t ieEndIndex);
		uint64_t a_array(string &pattern, uint64_t pos, uint64_t &length);

		int_vector<> LZ_factorization(string &R, vector<string> &S, vector<int_vector<>> &m_s); // Faktoren herausfinden
		void f_array(uint64_t max, vector<int_vector<>> &m_s);	//F-Array erzeugen
		void initialize_m(int_vector<> &t_to_t_reverse);	//M-Vektor erzeugen

};

namespace util{
}

#endif
