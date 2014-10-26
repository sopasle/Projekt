#include "FRLZSI.hpp"
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <ctime>

int main(int c, char *v[]){
	
	string pattern;
	vector<string> s;
	
    int len;
    struct dirent *pDirent;
    DIR *pDir;

    if (c < 3) {
        printf ("Usage: testprog <dirname> pattern\n");
        return 1;
    }
    
    
    // S einlesen
    pDir = opendir (v[1]);
    if (pDir == NULL) {
        printf ("Cannot open directory '%s'\n", v[2]);
        return 1;
    }

    while ((pDirent = readdir(pDir)) != NULL) {
        if((string)pDirent->d_name != "." && (string)pDirent->d_name != ".."){
			cout << "Datei " << pDirent->d_name << " einlesen" << endl;
			stringstream ss;
			ss << v[1] << "/" << pDirent->d_name;
			fstream datei(ss.str(), ios::in); 
			stringstream content;
			string zeile;
			while(!datei.eof()){
				getline(datei,zeile);
				content << zeile;
			}
			s.push_back(content.str());
			datei.close();
		}
    }
    closedir (pDir);

	//Pattern einlesen + suchen
	fstream dat(v[2], ios::in);
	while(!dat.eof()){
		getline(dat,pattern);
		if(!pattern.empty()){
			cout << "Pattern: " << pattern.size() << endl;

			/*Testmethode*/
			vector<pair<int,int>> find_treffer;
			for(int i = 0; i<s.size(); i++){
				string::size_type start = 0;
				while ((start = s[i].find(pattern, start)) != string::npos) {

					find_treffer.push_back(std::make_pair(i+1,start));
					start += 1; // see the note
				}
			}
			cout << "Treffer: " << find_treffer.size() << endl;
		}
	}

	dat.close();
	return EXIT_SUCCESS;
}
