#include "FRLZSI.hpp"
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <sstream>

int main(int c, char *v[]){
	
	string r = "ACGTCGATTAG";
	vector<string> s;
	
    int len;
    struct dirent *pDirent;
    DIR *pDir;

    if (c < 3) {
        printf ("Usage: testprog <filename> <dirname>\n");
        return 1;
    }
    
    
    fstream datei(v[1], ios::in);
	getline(datei,r);
    datei.close();
    
    
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
			string zeile;
			getline(datei,zeile);
			s.push_back(zeile);
			datei.close();
		}
    }
    closedir (pDir);
     


	/*Beispiel aus dem Paper*/
	//string r = "CGATGCATTACGGTAACTGTCTGAAT";
	string pattern = "AGGTTGACTGACTGACGGT";
	//vector<string> s; // hat alle S-Strings
	//s.push_back("TGATAGACG");
	//s.push_back("GAGTACTA");
	//s.push_back("GTACGT");
	//s.push_back("AGGA");
	
	cout << "FRLZSI t1(r,s)" << endl;
	FRLZSI t1(r,s);
	cout << "FRLZSI erstellt" << endl;
	t1.search_pattern(pattern);
		
		/* find als TEstmethode */
					//012345678901234567890123456
	std::string str ("TGATAGACGGAGTACTAGTACGTAGGA");
	
/*	int occurrences = 0;
	string::size_type start = 0;

	while ((start = str.find(pattern, start)) != string::npos) {
		cout << start << endl;
    ++occurrences;
    start += pattern.length(); // see the note
    
	}	*/	

	//Test mit find()
			cout << endl;
	cout << "Testmethode:" << endl;
			cout << endl;
	for(int i = 0; i<s.size(); i++){
		int occurrences = 0;
		string::size_type start = 0;
		while ((start = s[i].find(pattern, start)) != string::npos) {
			cout << "String: " << i+1 << " Pos: " << start << endl;
			++occurrences;
			start += pattern.length(); // see the note
		}
	}
	
	
	return EXIT_SUCCESS;
}
