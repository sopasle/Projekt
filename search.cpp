#include <iostream>
#include <utility>
#include <cstring>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/rmq_support.hpp>

using namespace std;
using namespace sdsl;

void searchPattern(int st,int ed, int_vector<> dStrich, int patternLength, csa_wt<> sa, int g[], int is[], int size_is, int ie[]);
void getFactors(int startIndex, int patternLength, int g[], int is[], int size_is, int ie[]);

int main(){
  int_vector<> dStrich = {4,2,4,3,1,5,2,3,6};
  int g[] = {1,2,3,6,8,5,7,4};
  int is[] = {0,0,0,2,3,4,6,7};
  int size_is = sizeof(is)/sizeof(is[0]);
  int ie[] = {1,2,3,3,8,5,7,8};
  
  csa_wt<> sa;
  construct_im(sa, "acgtgatag", 1);	//Eingabe: Referenzstring

  string pattern = "a";	//Eingabe: Pattern
  
  uint64_t i=0, j=sa.size()-1, startIndex=0, endIndex=0;
  backward_search(sa, i, j, pattern.begin(), pattern.end(), startIndex, endIndex); // Rueckwaertssuche => startIndex, endIndex
  searchPattern(startIndex-1, endIndex-1, dStrich,pattern.size(), sa, g, is, size_is, ie);
  
  
    
  return 0;
}


void searchPattern(int st,int ed, int_vector<> dStrich, int patternLength, csa_wt<> sa, int g[], int is[], int size_is, int ie[]){
  rmq_succinct_sct<false> rmaxq(&dStrich);
  int maxPosition = rmaxq(st,ed);
  if(dStrich[maxPosition] >= patternLength){ //Abbruch, wenn Faktorlänge < Patternlänge
	getFactors(sa[maxPosition+1], patternLength, g, is, size_is, ie);
    if(st < maxPosition) 
      searchPattern(st, maxPosition-1, dStrich, patternLength, sa, g, is, size_is, ie);
    if(ed > maxPosition)
      searchPattern(maxPosition+1, ed, dStrich, patternLength, sa, g, is, size_is, ie);
  }
 }
  
 void getFactors(int startIndex, int patternLength, int g[], int is[], int size_is, int ie[]){	//ToDo: O(1+occ)????
		for(int i = 0; i<size_is; i++){
			if(is[i] <= startIndex){
				if(ie[i] >= startIndex+patternLength-1){
					cout << "Faktor: " << g[i] << endl;
				}
			}
			else{
				break;
			}
		}
}

//ToDo: Faktor -> Startposition im String
