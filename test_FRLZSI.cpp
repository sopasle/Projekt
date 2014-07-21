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
	//t1.search_pattern(pattern);
	const char* a = "out";
	structure_tree_node* tree;
	//std::ofstream out(a);
	//int i = t1.serialize(out,tree,"");
	//cout << i << endl;
	std::ifstream in(a);
	t1.load(in);
	t1.test_ausgabe();
	//cout << "Testmethode:"  << endl;
	//t1.test_search(pattern);
	//t1.test_LZ_factorization(r,s);
		
	return EXIT_SUCCESS;
}
