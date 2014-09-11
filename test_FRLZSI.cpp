#include "FRLZSI.hpp"
#include <string>

int main(){

	/*Beispiel aus dem Paper*/
	string r = "ACGTGATAG";
	string pattern = "GA";
	vector<string> s; // hat alle S-Strings
	s.push_back("TGATAGACG");
	s.push_back("GAGTACTA");
	s.push_back("GTACGT");
	s.push_back("AGGA");

	FRLZSI t1(r,s);
	t1.search_pattern(pattern);
		
		/* find als TEstmethode */
					//012345678901234567890123456
	std::string str ("TGATAGACGGAGTACTAGTACGTAGGA");
	
	int occurrences = 0;
	string::size_type start = 0;

	while ((start = str.find(pattern, start)) != string::npos) {
		cout << start << endl;
    ++occurrences;
    start += pattern.length(); // see the note
    
}
	return EXIT_SUCCESS;
}
