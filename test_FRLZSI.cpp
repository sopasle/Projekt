#include "FRLZSI.hpp"
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <ctime>

int main(int c, char *v[]){
	
	string r;
	string pattern;
	vector<string> s;
	
    int len;
    struct dirent *pDirent;
    DIR *pDir;

    if (c < 4) {
        printf ("Usage: testprog <filename> <dirname> pattern\n");
        return 1;
    }
    
    //R einlesen
    fstream datei(v[1], ios::in);
	stringstream content;
	string zeile;
	while(!datei.eof()){
		getline(datei,zeile);
		content << zeile;
	}
	r = content.str();
	datei.close();
    
    
    // S einlesen
    pDir = opendir (v[2]);
    if (pDir == NULL) {
        printf ("Cannot open directory '%s'\n", v[2]);
        return 1;
    }

    while ((pDirent = readdir(pDir)) != NULL) {
        if((string)pDirent->d_name != "." && (string)pDirent->d_name != ".."){
			cout << "Datei " << pDirent->d_name << " einlesen" << endl;
			stringstream ss;
			ss << v[2] << "/" << pDirent->d_name;
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
    
	
   
	//FRLZSI erstellen
	clock_t start;
	float elapsed;
	start = clock();
	FRLZSI t1(r,s);
	elapsed = (float)(clock() - start) / CLOCKS_PER_SEC;
	cout << "FRLZSI erstellt, Laufzeit: " << elapsed << endl;
    
	cout << endl;
	cout << endl;
	
	
	
	//Pattern suchen
	//start = clock();
	//t1.search_pattern(pattern);
	//elapsed = (float)(clock() - start) / CLOCKS_PER_SEC;
	//cout << "Laufzeit Suche: " << elapsed << endl;
	
	// treffer vergleichen

	//vector<pair<int,int>> projekt_treffer;
	//t1.return_treffer(projekt_treffer);

	/* Serialize */
	cout << "Groesse " << t1.projekt_serialize() << endl;	
	FRLZSI t2(r);
	t2.projekt_load();

	//Pattern einlesen + suchen
	fstream dat(v[3], ios::in);
	while(!dat.eof()){
		getline(dat,pattern);
		if(!pattern.empty()){
			cout << "Pattern: " << pattern << endl;
			start = clock();
			t2.search_pattern(pattern);
			elapsed = (float)(clock() - start) / CLOCKS_PER_SEC;
			vector<pair<int,int>> projekt_treffer;
			t2.return_treffer(projekt_treffer);
			cout << "Laufzeit Suche: " << elapsed << " Treffer: " << projekt_treffer.size() << endl;

			/*Testmethode*/
			vector<pair<int,int>> find_treffer;
			cout << "Testmethode:" << endl;
			start = clock();
			int occurrences = 0;
			for(int i = 0; i<s.size(); i++){
				string::size_type start = 0;
				while ((start = s[i].find(pattern, start)) != string::npos) {

					find_treffer.push_back(std::make_pair(i+1,start));
					++occurrences;
					start += 1; // see the note
				}
			}
			elapsed = (float)(clock() - start) / CLOCKS_PER_SEC;
			cout << "Laufzeit mit find(): " << elapsed << " Treffer: " << occurrences << endl;

sort(projekt_treffer.begin(), projekt_treffer.end());
				sort(find_treffer.begin(), find_treffer.end());
			/*Vergleich*/
			if(projekt_treffer.size() == find_treffer.size()){
				
				int laufvariable = 0;
				while(projekt_treffer[laufvariable].first == find_treffer[laufvariable].first && projekt_treffer[laufvariable].second == find_treffer[laufvariable].second && laufvariable < projekt_treffer.size()){
					laufvariable++;
				}
				if(laufvariable == projekt_treffer.size()){
					cout << "Erfolg " << laufvariable << endl;
				}else{
					cout << "Fehler " << laufvariable << endl;
					for(int i = 0; i<projekt_treffer.size(); i++){
						cout << i << " Treffer: " << projekt_treffer[i]. first << " " << projekt_treffer[i].second << "\t" << find_treffer[i].first << " " << find_treffer[i].second << " " << projekt_treffer[i].second-find_treffer[i].second << endl;
					}	
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
			cout << "--------------------------------" << endl;
		}
	}
	dat.close();

	//start = clock();
	//t2.search_pattern(pattern);
	//elapsed = (float)(clock() - start) / CLOCKS_PER_SEC;
	//cout << "Laufzeit Suche: " << elapsed << endl;
	//t2.return_treffer(projekt_treffer);	
		/* find als Testmethode */

/*	int occurrences = 0;
	string::size_type start = 0;

	while ((start = str.find(pattern, start)) != string::npos) {
		cout << start << endl;
    ++occurrences;
    start += pattern.length(); // see the note
    
	}	*/	
   // clock_t start;
   // float elapsed;
	//Test mit find()
	/*vector<pair<int,int>> find_treffer;
	cout << endl;
	cout << "Testmethode:" << endl;
	start = clock();
	int occurrences = 0;
	for(int i = 0; i<s.size(); i++){

		string::size_type start = 0;
		while ((start = s[i].find(pattern, start)) != string::npos) {
			//cout << "String: " << i+1 << " Pos: " << start << endl;
			find_treffer.push_back(std::make_pair(i+1,start));
			++occurrences;
			start += 1; // see the note
		}
	}*/
	//elapsed = (float)(clock() - start) / CLOCKS_PER_SEC;
	//cout << "Laufzeit mit find(): " << elapsed << " Treffer: " << occurrences << endl;
	//cout << find_treffer[0].first << " " << find_treffer[0].second << endl;
	//cout << find_treffer[1].first << " " << find_treffer[1].second << endl;

	//sort(projekt_treffer.begin(), projekt_treffer.end());
	//sort(find_treffer.begin(), find_treffer.end());
	
	/*for(int i = 1; i<projekt_treffer.size()-1;i++){
		cout << projekt_treffer[i].first << " " << projekt_treffer[i].second << endl;
	}*/

	/*int laufvariable = 0;
	while(projekt_treffer[laufvariable].first == find_treffer[laufvariable].first && projekt_treffer[laufvariable].second == find_treffer[laufvariable].second && laufvariable < projekt_treffer.size()){
		laufvariable++;
	}
	if(laufvariable == projekt_treffer.size()){
		cout << "Erfolg " << laufvariable << endl;
	}else{
		cout << "Fehler " << laufvariable << endl;	
	}*/	
	//cout << projekt_treffer[laufvariable].first << " " << projekt_treffer[laufvariable].second << endl;
	//cout << find_treffer[laufvariable].first << " " << find_treffer[laufvariable].second << endl;
	//cout << projekt_treffer[laufvariable+1].first << " " << projekt_treffer[laufvariable+1].second << endl;
	//cout << find_treffer[laufvariable+1].first << " " << find_treffer[laufvariable+1].second << endl;

	return EXIT_SUCCESS;
}
