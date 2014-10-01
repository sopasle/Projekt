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
	getline(datei,r);
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
			string zeile;
			getline(datei,zeile);
			s.push_back(zeile);
			datei.close();
		}
    }
    closedir (pDir);
    
    //Pattern einlesen
    fstream dat(v[3], ios::in);
	getline(dat,pattern);
    datei.close();
   
   
    //FRLZSI erstellen
    clock_t start;
    float elapsed;
    start = clock();
    FRLZSI t1(r,s);
    elapsed = (float)(clock() - start) / CLOCKS_PER_SEC;
	cout << "FRLZSI erstellt, Laufzeit: " << elapsed << endl;
    
    

	
	
	//Pattern suchen
	start = clock();
	t1.search_pattern(pattern);
	elapsed = (float)(clock() - start) / CLOCKS_PER_SEC;
	cout << "Laufzeit Suche: " << elapsed << endl;
	
	/* Serialize */
	//cout << "Groesse " << t1.projekt_serialize() << endl;
	/*Beispiel aus dem Paper*/
	//string r = "CGATGCATTACGGTAACTGTCTGAAT";
	
	//vector<string> s; // hat alle S-Strings
	//s.push_back("TGATAGACG");
	//s.push_back("GAGTACTA");
	//s.push_back("GTACGT");
	//s.push_back("AGGA");
	

	
		
		/* find als Testmethode */

	//pattern = "CCTTGAGTGGCTGGTGTGGGGCTCATGCCTGTCATCCCAGCAGTTTGGGAGGCCGAAGCAGATGGATCACTTGAGCTCAGGAGTTCGAGACCAGCCTGGCCAACATGGCAACAAAAACTACAAAAATGGCCGGGTGTGGTGGCTCACGCCTATAATCCCAGCACTTTGGGAGGCTGAGACGGGTGGATCACTTGAGCCCAGGATTTCAAGACCGGCCTGGCCAACATGGAAACACCCATTTCTACAAAAAATACAAAAATAGCCGGGCATGGTGGCGCATGCCTGTGGTCCCAGCTACTCGGGAGGCTGAGGTGGGAGGACCGCTTGAGCCCAGGAAGTTGAGGCTACAGTGAGTGTGATTGTGCCACTGCATCCAACCTAGGCGACAGAGCGAGATCCTGTCTCAAAAAAATAAATAAATACTAGGTCTTGAGTGAAGCCTGAGGCACGTTCTCCTGAGCAGGGTATGTGTGCAGCAGCAATAGCTGTAGCTGGCCTTCAGCTGCCTGCATGCTAGCTGCCATCTCCACACCAGGCAGGTGAGGCTGCTGGAAGGGAGGTGTATCCCACTGCTCCTGCAGCAATGAACACAGCCACAGCCCGAGATGCACCAATCTGTACTTTTGTAACAATATTCTCTGAAGCTGTTGGCTTTACATGATAGTAAATGGATCTGCTGTTCAGGTCAGGCCTACCTGGGGTTTGTTCCCCCAACGAATTAGCCCCCTGAGCACAGGGACCATGCCTTGGCCACACCTGTGAGACCAACAAACAGCAGATGCAGACACACATGGCTGGTCCATTCAAACCAACTTGCCCTCCAGGTGCTCCCAGGAGCTGGGATCTGGTGTGACACCCAAGTGTAAAAATGCACATTCTGATTTCTGCCTGTTTCCCAACCCCAGAGAAGGACAGCCAGAAGACAGAGCGGCTGCCTGCTCCCCCTGGGACACCCAGCTCCTGGAGGGGAGAAGCCCCTGCACTGGCTCTACAGAAACCCCTGTCCAAGGAGGGCAGCGATCTTCTCCAACTGCCTGGGGGGAAAACTGACACCCGTCACCACCTCCTGGGAAGGAACAGCACACCCAGTGTGTCACCAGGAAGGGACTGATGAATTTCTTTGAACTCCACTGGAAGGTCTGTAAGAAATGATAGTTTATAAATAGAAGCTCGGTCACAGTTTTGAGAGGCTGGTGAAGCTCTCTTACGGAGCACATGGCCCAAACTCTTCATTTCCCTTTAGAGAAATGAGGCTCAGAGAGGGGAGGGGAGCTGCTCAGCGTGGCCCAGCAGACCCAGGCACAGAATCTAGGCCTCTTCACTTCCATCCTGATAGTTTCTCCCTTAAAGCCTACTGGCTCCCAAGAAGTTTTTTTTTTTTGAGACAGGGTCTTGTTCTGTTACCCAGGCTGGAGTACAGTGGTGCGATCACAGTTCAGCCTCCCAGGCTCAAACGATCCTCTCACCTCAGCCTCCCAAGTAGCTGGGACTACAGGCGCCCACCACCATGCCCGGCTAACTTAAAAAATATATTTACAAAAAAAGAGACAGTGGTGTTGGTGCGGGGGGTGCGGGGGGGTAGGTCTCACTACGTTGCTCAGGCTGGTCTTGAACTCCCGGGCTCAAGCAATCCTCCCGCCTTCACCTCCCAAAGTGCTGGGATTACAGGCACACACGGGCCACTCTGCCCTGGTCCAAAAAGCTTTTTTTTGGCCACTTTGTTTAGCTGAAATCTTAGGAGGATGCAACTAAGAAACAAAAAGAGAGAAAAGCAGAGTTGCTCTAGGGGAAGGAGAGTGGGGGCTCAGCCGCTGCCCTGGAGAGCCCTCCGAGTTGGAAAAGCACCTTGGGCTTGTGCTGGGCGTGAGGAGGGCCCTAAGGGCAGGAGAAGGTGCTTCTCTGTGACCCACAGTGCCCTGCCCTTGGAGCGCTCCAATCACATGACCCCACTGAAGTTTACATCTTCGGAAAAATAATGAGGGTTGGTCACTAATGGGCCTTTGAGAGTTGTCACCCGCTTGTCGATAATTACAGAGCTTTAATGGGGCTAATTCAGAGAAAATCAAGCTCAAGCCCCTGTTGGCTCCTCTAAGGCCGAGCACCCCCTGCTTGGCTGATGTAGACCTGACAGCATCATCATAACCTTGGTTTGCACCTGGACTTTCCTGACACCCTTGGGAATAGGAAATCTGCCCTGATTTTTTTTTTTTTTTTTTTTAAGAAAAATGTCAGGCTCTCAAATACTTTAAAAAAAACAAAACAATTGTGGCAAGGACCATAGAAAATCATCAAGCTTGTGTTGAAAGCATTTTATAGTTGGGGAAACAGCCATTCAGCCAGGCCGGCACTGCAGGGGGGACAGAGAGAAATGAGCCATCATCCCTGCTTCCCAGGAGCTTAGAGACGAGTGTTAAGATTTTTATAAACATCTTCATTCATCTGCTTTAAATTGTTGAAAGGTTCTCCATCAACTCCTGGACTCAGTTCAACCTCCTTGGTGTGGCATTCAAGACCTTGTGTGATATGACCACTTTGGCCTCCTCATAGGTCTCTTCCCCTTCTTTGAGCCACTTGGCTCTACTGTTTTGCCTACACTGGGCTGAGTTTCTGACACTTCAGGACCTGTGTACACAGCTGTCCCTCTGCGAAGAGCACCTCCCTTTCCTCTTTTCCCTCTGTCAGCCCACCTCCTCCCTCCTCCCAGTCTTAGGGCTGAGCCCTCTCTGTGAGTGACCTCCGCCTGCCAGAGCTTTCCATCTGGGTTCTTGCCTCCCTGTCCCTCGCTGTGGGGTCACCTGTTTGTCTTGGTTTTGATGAACACCCTCAGGTCAGAGGTTGCCTCTCAGCATTCTTGTATCTCTGTGCCTAACACAGAGCCTGCCACATAGTTGGTGCCGTAACATCGAGTGCATTACAAATATCACAGGTCCAGGTAGAACATGACTGGGGCCAAAGAGGTTCTGGGCTCAAAGGAAGGGGCACCACAGCCCATGGGGGCTGGAATATGCCCCCTAACACGGAGAACCTTTGTGGTGGACTCAACCCCTTGTCCTTGCTCCAGACCTGCACAGGCATCGGTCCTGACCCGGTCCTGGGTGAACCACACAGGGCAGGTTTAACACGGGCATGTGATCCAATTCTGGCCAATGAGACAAAAGGACAGGTCTCCTGGGGACTTCTGGGAGAGGTTTCCTCGCTCTTAAACTGAGACATGAGAAAAGGAATCGGTCCTTACCAATTCCTTACCCAGTCAAGGACCTTATTGTTACATGTGATGCCTGGACCTGCAGCAGCCACCTTGGACCTGAGGGTCCTGCTTGGCTGATGGAGACTGACAGCGTCATTCTGACTTTGGTCTGCACCTGAACCTTCCTCACCCAGACACCCTTAGGAATAGGAAACGGGCCTTGATTTTGCTCTACAAGGAAAAACGCACAGCCACGAGCAAAGATGGACAGAACCATCACCCCAACAGTGCTGAGGGGCTGCTGCATTGGTGCACCCTGGTGCAGCAAGACCTCAGCTGTCCTCATTGTTTAAGCCACTCTGAGTTGGTTATCTGTCACTTGCAACCCATGGCACTCTAGTGATTACAGCTTTGAAGAATGGACTGGGCTTCCACAGGTGGGTGCCGGGCAGGAGAGCCTCTCAGGTAGGGGCAGCGTGTGGGGTGTGTGGCCCACGGCCCACCTGGAGGGCAGCACGCCACAGAGGCCACCCTAGGAGAACTGGCAGGGCCGACACTGGGTCCTGCTCCTCATTGGGACATTCACTGCCTTTCCTCCCTGTCATCGCCCCTCTTCGAGATCTGCTGGGAACATGCGAGTTATGTGCTTGTGCTTTGACATGACTTAATAAAGAAAGGGCTTTAAAAAGCCAGGAGAAAAGGCTGAGTAAATAAACAGCACTTCACTCTCTGCCTTGGACAACAGAGCAACCGCCCTCATTTCTTTTTGTCAGAACAGAGATAATCCAATAATTAGGGCAGCAAATAGCATAACCCTGGCTAAAGCCGTAATGAACCATCTGGCTTGAATCATTGAGGCTTATTCTAAGGATCTGGTTGTCACAGCCCAGAGGGGGGCACCGCCTGGGGTACTGGGTGGAA";
	
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
	cout << endl;
	cout << "Testmethode:" << endl;
	start = clock();
	int occurrences = 0;
	for(int i = 0; i<s.size(); i++){

		string::size_type start = 0;
		while ((start = s[i].find(pattern, start)) != string::npos) {
			cout << "String: " << i+1 << " Pos: " << start << endl;
			++occurrences;
			start += pattern.length(); // see the note
		}
	}
	elapsed = (float)(clock() - start) / CLOCKS_PER_SEC;
	cout << "Laufzeit mit find(): " << elapsed << " Treffer: " << occurrences << endl;
	
	
	return EXIT_SUCCESS;
}
