#include <iostream>
#include <fstream>
#include <sstream> //for stringstream
#include <stdlib.h> //for atof function
#include <vector>
#include "node.cpp"
#include "tree.cpp"

using namespace std;

int main(int argc, char* argv[]){
	int minleaf;

	if(argc <= 2){
		cout << "Please input previous.txt and new.txt" << endl;
		exit(0);
	}

	if(argc == 3){
		minleaf = 20;
	}

	if(argc == 4){
		minleaf = atoi(argv[3]);
	}

	Tree tree(argv[1], argv[2]);
	vector<io_pair*> iopairs = tree.getIOPairs();
	vector< vector<double> > attrData = tree.getAttrData();

	Node* n = tree.DTL(iopairs, minleaf);

	for(int i=0; i<attrData.size(); i++){
		cout << tree.predict(n, attrData[i]) << endl;
	}

	return 0;
}