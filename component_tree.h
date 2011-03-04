//
//=======================================================================
// Copyright 2010 Institute PICB.
// Authors: Hang Xiao, Axel Mosig
// Data : July 11, 2010
//=======================================================================
//

#ifndef COMPONENT_TREE_H_H
#define COMPONENT_TREE_H_H

#include <iostream>
#include <vector>
#include <list>
using namespace std;

class ComponentTree
{
public:	
	struct Element;
	struct Node;
	typedef int Vertex;
	typedef vector<Vertex> Vertices;
	typedef vector<int> Path;
	typedef vector<Path> Paths;
	typedef int ElementId;
	
	enum SEARCH_TYPE{PRE_ORDER, POST_ORDER, BREADTH_FIRST};
	
	struct Element
	{
		ElementId  next;		 // the next element
	};
	typedef vector<Node*> Nodes;
	
	struct Node
	{
		int label;
		int level;        // the lowest level
		double mean;
		double std;
		double centerX;
		double centerY;
		double centerZ;
		//int elementCount;
		int size; // the element in the component exclude the elements in child nodes
		Node* parent; // we will make Node as dynamic memory, for the label is not easy to 
		// set when the set is create this value will be set until the 
		// component tree is built.
		ElementId entryId; //element will set static, the entry shoud set as the one of the lowest element in this component 
		vector<Node*> childs;
	};
	

public:
	ComponentTree();
	ComponentTree(char * imgfile, int _minSize, int _maxSize, int _singleSize);
	ComponentTree(char* treefile);
	~ComponentTree();
	bool create(char * imgfile, int _minSize, int _maxSize, int _singleSize);
	
	bool load(const char* from_tree_file);
	bool load(ifstream& ifs);
	bool save(const char* to_tree_file) const;
	bool save(ofstream& ofs) const;
	void clear();

	int width() const;
	int height() const;
	int depth() const;
	Node* root() const;
	Node* getNode(int) const;  //node of label 
	Nodes getNodes() const; //return all the node which stores in post order, equilivalent to m_root
	Paths getPaths() const;
	Vertices getVertices(int) const; //vertices of label
	int* getMappingMatrix() const; //get the matrix of labels
	int* getMatrix(vector<int> labels , vector<int> values, int ini_value) const; 
	
	int nodeCount() const;
	int leafCount() const;
	int elementCount() const;
	
	void printVertices(int label = -1) const;
	void printTree(int label = -1) const;
	void printMapping() const;
	void printPaths() const;
	void printPath(int path) const;
	
private:
	void printTreeRecursively(int , int) const;
	int postProcess(Node* node, int label); //1. set node labels 2. set mean level  3. store node by post order 4. get leafs
	void setStatistic(unsigned char*);  //set the points mean level and the stand devariance
	void setCenter();
	void setPaths();
	void setOrders();
	Node* MergeNodes(Node* curNode, Node* adjNode);
	
	//8 memervariable, 4 containers, 3 counters, 1 root
public:
	//1 root
	Node* m_root;  //the root Node
private:
	//size
	int m_width ;
	int m_height;
	int m_depth;	
	//3 counters
	int m_numElements;
	int m_numNodes;
	int m_numLeafs;
	
	//4 contaners
	Paths m_paths;
	Nodes m_leafs; //store all the leafs
	vector<Element> m_elements;
	Nodes m_nodes; //store the nodes in post order
	vector<Node*> m_postOrder;
	vector<Node*> m_preOrder;
	vector<Node*> m_breadthFirst;
	
	//3 setting
	int m_minSize;
	int m_maxSize;
	int m_singleSize;
	
	
	//3 iterators
public:
	
	class iterator
	{
	public:
		iterator();
		void operator=(iterator&);
		Node* operator*() const;
		iterator operator++(int);  //  match i++ not ++i
		bool operator!=(iterator&);
	public:
		vector<Node*>::iterator node_itr;
	};
	
	class const_iterator
	{
	public:
		const_iterator();
		void operator=(const_iterator);
		const Node* operator*() const;
		const_iterator operator++(int);   //
		bool operator!=(const_iterator);  // becareful the const_iterator should not be referenced type
	public:
		vector<Node*>::const_iterator node_itr;
	};
	
	
	iterator begin(SEARCH_TYPE = BREADTH_FIRST);
	const_iterator begin(SEARCH_TYPE = BREADTH_FIRST) const;
	iterator end(SEARCH_TYPE = BREADTH_FIRST);
	const_iterator end(SEARCH_TYPE = BREADTH_FIRST) const;
};

class DisjointSets
	{
	public:
		
		// Create an empty DisjointSets data structure
		DisjointSets();
		// Create a DisjointSets data structure with a specified number of elements (with element id's from 0 to count-1)
		DisjointSets(int count);
		// Copy constructor
		DisjointSets(const DisjointSets & s);
		// Destructor
		~DisjointSets();
		
		// Find the set identifier that an element currently belongs to.
		// Note: some internal data is modified for optimization even though this method is consant.
		int FindSet(int element) const;
		// Combine two sets into one. All elements in those two sets will share the same set id that can be gotten using FindSet.
		int Union(int setId1, int setId2);
		// Add a specified number of elements to the DisjointSets data structure. The element id's of the new elements are numbered
		// consequitively starting with the first never-before-used elementId.
		void AddElements(int numToAdd);
		// Returns the number of elements currently in the DisjointSets data structure.
		int NumElements() const;
		// Returns the number of sets currently in the DisjointSets data structure.
		int NumSets() const;
		
	private:
		
		// Internal Node data structure used for representing an element
		struct Node
		{
			int rank; // This roughly represent the max height of the node in its subtree
			int index; // The index of the element the node represents
			Node* parent; // The parent node of the node
		};
		
		int m_numElements; // the number of elements currently in the DisjointSets data structure.
		int m_numSets; // the number of sets currently in the DisjointSets data structure.
		std::vector<Node*> m_nodes; // the list of nodes representing the elements
	};

#endif
