#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>
using namespace std;


int d_Array(int d[], int size_d,int is[], int ie[]);
int d_Strich(int d_1[],int size_d,int SAr[], int d[]);

int main(){

	int d[9];
	int d_1[9];
	int is[8];	
	int ie[8];	
	int SAr[9];
	is[0] = 0;	ie[0] = 1;
	is[1] = 0;	ie[1] = 2;
	is[2] = 0;	ie[2] = 3;
	is[3] = 2;	ie[3] = 3;
	is[4] = 3;	ie[4] = 8;
	is[5] = 4;	ie[5] = 5;
	is[6] = 6;	ie[6] = 7;
	is[7] = 7;	ie[7] = 8;
	
	SAr[0] = 0;
	SAr[1] = 7;
	SAr[2] = 5;
	SAr[3] = 1;
	SAr[4] = 8;
	SAr[5] = 4;
	SAr[6] = 2;
	SAr[7] = 6;
	SAr[8] = 3;
	
	int size_d = sizeof(d) / sizeof(int);
	d_Array(d,size_d,is,ie);
	d_Strich(d_1,size_d,SAr,d);
	for(int i = 0; i<sizeof(d) / sizeof(int); i++){
	cout << "d: " << d[i]<< " SAr: " << SAr[i]+1 << " d_1: " << d_1[i] << endl;
}
	return 0;
	
}

int d_Array(int d[],int size_d,int is[], int ie[]){
	int p = 0; 							// Aktuelle Stelle in d
	int j = 0; 							// Laufvariable für die maximale Distanz
	int j1 = 0;							// Varible für das Rücksetzen von j
	int neuerFaktor = 0;				// Speicher für Faktor an neuer Stelle

	while(p<size_d-1){ 					// solange p kleiner n, p = aktuelle Stelle in d, n = Länge von G, bzw IS und IE
		d[p] = ie[j]-p+1; 				// Berechnung des Faktors an der ersten neuen Stellen, zwingend für den Vergleich
		j++;							// j = Zählvariable in IS, erhöhen, da erste neue Stelle bereits berechnet
		while(is[j]<=p){ 				// solange Startposition des Strings kkleiner p, p = aktuelle Stelle in d
			neuerFaktor = ie[j]-p+1; 	// Berechnung des Faktors an der neuen Stelle, zwingend für den Vergleich
			if(d[p] < neuerFaktor){  	// Abfrage ob der aktuelle Wert keliner ist als der neu Berechnete	
			d[p] = neuerFaktor;			// Falls ja, neuen Wert speichern
			j1 = j;						// aktuelles j Abspeichern, damit nicht jedesmal von Beginn an getestet wird in der while Schleife
			}			
			j++;			
		}
		j = j1;				

		p++;
	}
	d[p] = 1;							// An letzter Stelle 1 schreiben, da D nur bis zum vorletzten Element berechnet
										// wird und das letzte immer 1 sein muss, da der letzte String zu Ende ist

}


int d_Strich(int d_1[] ,int size_d,int SAr[], int d[]){
	for(int i = 0; i < size_d;i++){
		d_1[i] = d[SAr[i]];				// Berechnung von d', Länge des längsten Intervalls an der Position SAr[i]
	}
	
	
}
