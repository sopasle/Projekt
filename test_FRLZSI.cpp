#include "FRLZSI.hpp"


int main(){

	/*Beispiel aus dem Paper*/
	string r = "ACGTGATAG";
	string pattern = "A";
	vector<string> s; // hat alle S-Strings
	s.push_back("TGATAGACG");
	s.push_back("GAGTACTA");
	s.push_back("GTACGT");
	s.push_back("AGGA");

	FRLZSI t1(r,s);
	//t1.test_ausgabe();
	//t1.search_pattern(pattern);
	/*
	sdsl::store_to_file(t1,"ausgabe");
	FRLZSI t2;
	sdsl::load_from_file(t2,"ausgabe");
	t2.test_ausgabe();*/
	/* Diese Variante wirft ebenfalls die Speicherzugriffsfehler wie store to file bzw load from file */
	//const char* a = "out";
	//structure_tree_node* tree;
	//std::ofstream out(a);
	//int i = t1.serialize(out);  // Funktioniert nun mit einem Parameter, da der typ nun size_type ist
	//cout << "Written Bytes: " << i << endl;
	//std::ifstream in(a);
	//t1.load(in);

	//cout << "Testmethode:"  << endl;
	//t1.test_search(pattern);
	//t1.test_LZ_factorization(r,s);
		
	return EXIT_SUCCESS;
}
