using namespace std;

struct Node{
	int attr;
	string label;
	double splitVal;
	Node* left;
	Node* right;
	bool isLeaf;
};