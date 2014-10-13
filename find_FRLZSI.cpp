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
	t2.projekt_load();
    //Pattern einlesen + suchen
	fstream dat(v[1], ios::in);
	while(!dat.eof()){
		getline(dat,pattern);
		if(!pattern.empty()){
			cout << "Pattern: " << pattern << endl;
			t2.search_pattern(pattern);
			vector<pair<int,int>> projekt_treffer;
			t2.return_treffer(projekt_treffer);
			sort(projekt_treffer.begin(), projekt_treffer.end());
			for(int i = 0; i < projekt_treffer.size();i++){
			cout << "T: " << projekt_treffer[i].first << " " << projekt_treffer[i].second<< endl;
			}
			/*Testmethode*/
			/*vector<pair<int,int>> find_treffer;
			cout << "Testmethode:" << endl;
			for(int i = 0; i<s.size(); i++){
				string::size_type start = 0;
				while ((start = s[i].find(pattern, start)) != string::npos) {
					find_treffer.push_back(std::make_pair(i+1,start));
					start += 1; // see the note
				}
			}
			sort(projekt_treffer.begin(), projekt_treffer.end());
			sort(find_treffer.begin(), find_treffer.end());*/
			/*Vergleich*/
			/*if(projekt_treffer.size() == find_treffer.size()){
				int laufvariable = 0;
				while(projekt_treffer[laufvariable].first == find_treffer[laufvariable].first && projekt_treffer[laufvariable].second == find_treffer[laufvariable].second && laufvariable < projekt_treffer.size()){
					laufvariable++;
				}
				if(laufvariable == projekt_treffer.size()){
					cout << "Erfolg " << laufvariable << endl;
				}else{
					cout << "Fehler " << laufvariable << endl;
					for(int i = 0; i<projekt_treffer.size(); i++){
						//if(projekt_treffer[i].second-find_treffer[i].second == 0){
						//}else{
						cout << i << " Treffer: " << projekt_treffer[i]. first << " " << projekt_treffer[i].second << "\t" << find_treffer[i].first << " " << find_treffer[i].second << " " << projekt_treffer[i].second-find_treffer[i].second << endl;
					}	//}
				}	
			
			}else{
				cout << "Fehler " << endl;
				for(int i = 0; i<projekt_treffer.size(); i++){
						cout << i << " Treffer: " << projekt_treffer[i]. first << " " << projekt_treffer[i].second << "\t" << find_treffer[i].first << " " << find_treffer[i].second << " " << projekt_treffer[i].second-find_treffer[i].second << endl;
				}
				cout << endl;
				for(int i = 0; i<find_treffer.size(); i++){
					//	cout << find_treffer[i].first << " " << find_treffer[i].second << endl;
				}		
			}
			cout << "--------------------------------" << endl;*/
		}
	}
	dat.close();
	return EXIT_SUCCESS;
}
