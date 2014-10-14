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

    if (c < 3) {
        printf ("Usage: testprog <filename> <dirname>\n");
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
	FRLZSI t1(r,s);
	

	/* Serialize */
	store_to_file(t1,"test");
	fstream f("speicher.html", ios::out);
	write_structure<HTML_FORMAT>(t1,f);
return EXIT_SUCCESS;
}
