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
		FRLZSI();				// Konstruktor
		FRLZSI(string &r, vector<string> &s);	// Konstruktor mit Aufruf Referenzstring, Vektor mit Eingabestrings
		~FRLZSI();				// Destruktor
		void search_pattern(string &pattern);	// Patternsuche
		void test_ausgabe();			// Testausgaben
		void test_search(string &pattern);
		void test_LZ_factorization(string &r, vector<string> &s);
		
		typedef uint64_t size_type;
		      int_vector<64> m_offset;	 	// Number of nodes to skip on each level
        		int_vector<64> m_tree; 			// Tree
			// m_offset und m_tree werden hier benötigt, da sonst ein Fehler auftritt, wenn er es in Zeile 53/54 verwenden will

	//! Load the data structure
        void load(std::istream& in) {		
		m_sa.load(in);
		m_ds.load(in);
		m_g.load(in);
		m_is.load(in);
		m_ie_rmaxq.load(in);
		m_ds_rmaxq.load(in);
            m_offset.load(in);
            m_tree.load(in);
        }

        //! Serialize the data structure
        size_type serialize(std::ostream& out, structure_tree_node* v=nullptr, std::string name="")const {
            structure_tree_node* child = structure_tree::add_child(v, name, util::class_name(*this));
            size_type written_bytes = 0;
            written_bytes += m_sa.serialize(out, child, "sa");
            written_bytes += m_ds.serialize(out, child, "ds");
            written_bytes += m_g.serialize(out, child, "g");
	    written_bytes += m_is.serialize(out, child, "is");
            written_bytes += m_ie_rmaxq.serialize(out, child, "ie RMQ");
            written_bytes += m_ds_rmaxq.serialize(out, child, "ds RMQ");
            written_bytes += m_offset.serialize(out, child, "offset");
            written_bytes += m_tree.serialize(out, child, "tree");
            structure_tree::add_size(child, written_bytes);
            return written_bytes;
        }
		
	private:
		vector<pair<int,int>> m_t_array;	// T-Array, (Faktoranfang,Faktorende) im Referenzstring
		csa_wt<> m_sa;				// Suffix-Array
		vector<int_vector<>> m_s;		// Faktorzerlegung der einzelnen S
		int_vector<> m_ds;			// D'-Array, für Ausgabetests
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

		void LZ_factorization(string &R, vector<string> &S); // Faktoren herausfinden

};

namespace util{
}

#endif
