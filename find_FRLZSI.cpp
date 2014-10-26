#include "FRLZSI.hpp"
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <ctime>


int main(int c, char *v[]){
	
	string pattern;

	if (c < 2) {
		printf ("Usage: testprog <pattern>\n");
		return 1;
	}
    
	FRLZSI t2(pattern);
	load_from_file(t2,"test");
	//t2.projekt_load();
	//Pattern einlesen + suchen
	fstream dat(v[1], ios::in);
	while(!dat.eof()){
		getline(dat,pattern);
		if(!pattern.empty()){
			cout << "Pattern: " << pattern.size() << endl;
			t2.search_pattern(pattern);
			vector<pair<int,int>> projekt_treffer;
			t2.return_treffer(projekt_treffer);
			cout << "Treffer: " << projekt_treffer.size() << endl;
		}
	}
	dat.close();
	return EXIT_SUCCESS;
}
