#include <utility> //for pair
#include <map>
#include <algorithm> //sort
#include <cmath> //for log2

using namespace std;

struct io_pair{
	vector<double> x;
	string y;
};

class Tree{
private:
	vector<io_pair*> iopairs; //input output pairs from previous.txt
	vector< vector<double> > attrData; //input from new.txt to be given rating

public:
	Tree(string prevData, string newData);
	void storeInput(string filename, int prevOrNew); //stores input from files into vectors
	Node* DTL(vector<io_pair*> data, int minleaf);
	string mode(vector<io_pair*> data); //finds mode of ratings
	pair<int, double> chooseSplit(vector<io_pair*> data);
	string predict(Node* n, vector<double> data);
	double informationGain(vector<io_pair*> data, int attr, double splitVal);
	vector<io_pair*> getIOPairs();
	vector< vector<double> > getAttrData();
};

Tree::Tree(string prevData, string newData){
	storeInput(prevData, 1);
	storeInput(newData, 2);
}

void Tree::storeInput(string filename, int prevOrNew){
	string nothing; //to store first line that will be discarded
	string wholeLine; //used by while loop to store each line, line by line
	ifstream file; //ifstream to read from file
	file.open(filename.c_str());
	getline(file, nothing); //remove first line

	if(file.is_open()){
		string sep;
		stringstream line;
		string WC_TA = "";
		string RE_TA = "";
		string EBIT_TA = "";
		string MVE_BVTD = "";
		string S_TA = "";
		string rating = "";

		while(getline(file, wholeLine)){
			line << wholeLine;
			//storing all attributes into containers
			line >> ws >> WC_TA >> ws >> RE_TA >> ws >> EBIT_TA >> ws >> MVE_BVTD >> ws >> S_TA;
			
			if(prevOrNew == 1){ //if previous.txt
				line >> ws >> rating;

				io_pair *n = new io_pair;

				n->x.push_back(atof(WC_TA.c_str()));
				n->x.push_back(atof(RE_TA.c_str()));
				n->x.push_back(atof(EBIT_TA.c_str()));
				n->x.push_back(atof(MVE_BVTD.c_str()));
				n->x.push_back(atof(S_TA.c_str()));
				n->y = rating;

				iopairs.push_back(n);
			}

			else{ //if new.txt
				vector<double> attr;

				attr.push_back(atof(WC_TA.c_str()));
				attr.push_back(atof(RE_TA.c_str()));
				attr.push_back(atof(EBIT_TA.c_str()));
				attr.push_back(atof(MVE_BVTD.c_str()));
				attr.push_back(atof(S_TA.c_str()));

				attrData.push_back(attr);
			}

			line.clear();
		}
	}
}

Node* Tree::DTL(vector<io_pair*> data, int minleaf){

	bool equalX = true, equalY = true;
	pair<int, double> splitPair;

	//checks if any y's are equal
	for(int i=0; i<data.size(); i++){
		for(int j=0; j<data.size(); j++){
			if(j!=i && data[i]->y != data[j]->y){
				equalY = false;
			}
		}
	}

	//checks if any x's are equal
	for(int i=1; i<data.size(); i++){
		for(int j=0; j<data.size(); j++){
			for(int k=0; k<5; k++){
				if(data[i]->x[k] != data[j]->x[k]){
					equalX = false;
				}
			}
		}
	}


	//recursion base case
	if(data.size() <= minleaf || equalY == true || equalX == true){
		Node *n = new Node; //create leaf node
		n->isLeaf = true;
		string ratingMode = mode(data); //set label as mode
		if(ratingMode == "-1"){
			n->label = "unknown";
		}
		else{
			n->label = ratingMode;
		}

		return n;
	}

	Node *n = new Node;
	splitPair = chooseSplit(data);
	n->attr = splitPair.first;
	n->splitVal = splitPair.second;
	n->isLeaf = false;

	switch(splitPair.first){
		case 0:
			n->label = "WC_TA";
			break;
		case 1:
			n->label = "RE_TA";
			break;
		case 2:
			n->label = "EBIT_TA";
			break;
		case 3:
			n->label = "MVE_BVTD";
			break;
		case 4:
			n->label = "S_TA";
			break;
	}

	vector<io_pair*> leftSplit, rightSplit;

	//split data using splitval
	for(int i=0; i<data.size(); i++){
		if(data[i]->x[splitPair.first] > splitPair.second){
			rightSplit.push_back(data[i]);
		}
		else{
			leftSplit.push_back(data[i]);
		}
	}
	
	//recursively call dtl from left and right nodes
	n->left = DTL(leftSplit, minleaf);
	n->right = DTL(rightSplit, minleaf);

	return n;
}

string Tree::mode(vector<io_pair*> data){
	vector< pair<string, int> > ratingCount;
	string ratings[] = {"AAA", "AA", "A", "BBB", "BB", "B", "CCC"};

	for(int i=0; i<7; i++){
		ratingCount.push_back(make_pair(ratings[i], 0));
	}

	for(int i=0; i<data.size(); i++){
		if(data[i]->y == "AAA"){
			ratingCount[0].second++;
		}
		if(data[i]->y == "AA"){
			ratingCount[1].second++;
		}
		if(data[i]->y == "A"){
			ratingCount[2].second++;
		}
		if(data[i]->y == "BBB"){
			ratingCount[3].second++;
		}
		if(data[i]->y == "BB"){
			ratingCount[4].second++;
		}
		if(data[i]->y == "B"){
			ratingCount[5].second++;
		}
		if(data[i]->y == "CCC"){
			ratingCount[6].second++;
		}
	}

	int mode = 0;
	bool singleMode = true;

	for(int i=1; i<7; i++){
		if(ratingCount[i].second > ratingCount[mode].second){
			mode = i;
			singleMode = true;
		}
		else if(ratingCount[i].second == ratingCount[mode].second){
			singleMode = false;
		}
	}

	if(!singleMode){
		return "-1";
	}

	return ratingCount[mode].first;
}

pair<int, double> Tree::chooseSplit(vector<io_pair*> data){
	pair<int, double> split;
	double bestGain = 0, bestSplitVal = 0;
	int bestAttr = 0;

	//for all 5 attributes
	for(int i=0; i<5; i++){
		vector<double> attrs;
		for(int j=0; j<data.size(); j++){
			attrs.push_back(data[j]->x[i]); //store every attribute in prev
		}

		sort(attrs.begin(), attrs.end());

		for(int j=0; j<attrs.size()-1; j++){
			double splitVal = 0.5*(attrs[j] + attrs[j+1]);
			double gain = informationGain(data, i, splitVal);

			if(gain > bestGain){
				bestAttr = i;
				bestSplitVal = splitVal;
				bestGain = gain;
			}
		}
	}

	split.first = bestAttr;
	split.second = bestSplitVal;
	return split;
}

string Tree::predict(Node* n, vector<double> data){
	while(n->isLeaf == false){
		if(data[n->attr] <= n->splitVal){
			n = n->left;
		}
		else{
			n = n->right;
		}
	}

	return n->label;
}

double Tree::informationGain(vector<io_pair*> data, int attr, double splitVal){
	//creating variables needed
	map<string, int> aboveSplit, belowSplit, total;
	double aboveTotal = 0, belowTotal = 0, allTotal = 0;
	map<string, int>::iterator it;
	vector<double> aboveProb, belowProb, totalProb;
	double aboveInfo = 0, belowInfo = 0, totalInfo = 0;

	//count of how many ratings are above/below splitVal
	for(int i=0; i<data.size(); i++){
		if(data[i]->x[attr] > splitVal){
			aboveTotal++;
			aboveSplit[data[i]->y]++;
			total[data[i]->y]++;
		}
		if(data[i]->x[attr] <= splitVal){
			belowTotal++;
			belowSplit[data[i]->y]++;
			total[data[i]->y]++;
		}
	}

	allTotal = aboveTotal + belowTotal;

	//calculating probabilities
	for(it=aboveSplit.begin(); it!=aboveSplit.end(); it++){
		aboveProb.push_back(it->second/aboveTotal);
	}

	for(it=belowSplit.begin(); it!=belowSplit.end(); it++){
		belowProb.push_back(it->second/belowTotal);
	}

	for(it=total.begin(); it!=total.end(); it++){
		totalProb.push_back(it->second/allTotal);
	}

	//calculating infos
	for(int i=0; i<aboveProb.size(); i++){
		if(aboveProb[i] != 0){
			aboveInfo -= aboveProb[i]*log2(aboveProb[i]);
		}
	}

	for(int i=0; i<belowProb.size(); i++){
		if(belowProb[i] != 0){
			belowInfo -= belowProb[i]*log2(belowProb[i]);
		}
	}

	for(int i=0; i<totalProb.size(); i++){
		if(totalProb[i] != 0){
			totalInfo -= totalProb[i]*log2(totalProb[i]);
		}
	}

	double rem = (belowTotal/allTotal)*belowInfo + (aboveTotal/allTotal)*aboveInfo;
	double infoGain = totalInfo - rem;
	
	return infoGain;
}


vector<io_pair*> Tree::getIOPairs(){
	return iopairs;
}
	
vector< vector<double> > Tree::getAttrData(){
	return attrData;
}


