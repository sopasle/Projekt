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
	t1.test_ausgabe();
	//t1.search_pattern(pattern); 
		
	return EXIT_SUCCESS;
}
