#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>
using namespace std;
//int g[];

int d();
int main(){
	
	d();
	
}


int d(){
	int d[9];	// temporär
	int d_hilfe[9];
//	int d1[];	// temporär
	int is[8];	// temporär
	int ie[8];	// temporär
	is[0] = 0;	ie[0] = 1;
	is[1] = 0;	ie[1] = 2;
	is[2] = 0;	ie[2] = 3;
	is[3] = 2;	ie[3] = 3;
	is[4] = 3;	ie[4] = 8;
	is[5] = 4;	ie[5] = 5;
	is[6] = 6;	ie[6] = 7;
	is[7] = 7;	ie[7] = 8;
	int n = 8;
	int p = 0;
	int j = 0;
	int j1 = 0;
	int k = 0;
/*	while(p < n){
		k = j;
		while(is[j] <= p){
			printf("erhöhen\n");
			d_hilfe[j] = ie[j]-p+1;
			// RMQ von k bis j -> zahl für d
			j++;
		}
		while(p<j){
		d[p] = ie[RMQ]-p+1;
		printf("D: %d, J: %d, P: %d\n",d[p],j,p);
		p++;
	}
	printf("-\n");
	}*/
	

	while(p<n){ 			// solange p kleiner n, p = aktuelle Stelle in d, n = Länge von G, bzw IS und IE
		d[p] = ie[j]-p+1; 	// Berechnung des Faktors an der ersten neuen Stellen, zwingend für den Vergleich
		j++;				// j = Zählvariable in IS, erhöhen, da erste neue Stelle bereits berechnet
		while(is[j]<=p){ 	// solange Startposition des Strings kkleiner p, p = aktuelle Stelle in d
			k = ie[j]-p+1; 	// Berechnung des Faktors an der neuen Stelle, zwingend für den Vergleich
			if(d[p] < k){  	// Abfrage ob der aktuelle Wert keliner ist als der neu Berechnete	
			d[p] = k;		// Falls ja, neuen Wert speichern
			j1 = j;			// aktuelles j Abspeichern, damit nicht jedesmal von Beginn an getestet wird in der while Schleife
			}
			j++;
		}
		j = j1;				
		cout << p << "|" << d[p] << endl; // Ausgabe von d
		p++;
	}
	d[p] = 1;				// An letzter Stelle 1 schreiben, da D nur bis zum vorletzten Element berechnet wird und das letzte immer 1 sein muss, da der letzte String zu Ende ist
	cout << p << "|" << d[p] << endl;	// Ausgabe letztes Element
}

