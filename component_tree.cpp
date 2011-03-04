//
//=======================================================================
// Copyright 2010 Institute PICB.
// Authors: Hang Xiao, Axel Mosig
// Data : July 11, 2010
//=======================================================================
//

#include <iostream>
#include <fstream>
#include <assert.h>
#include <map>
#include <deque>
#include <stack>
#include <cmath>
#include "component_tree.h"
#include "myalgorithms.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;


/******************************************************************
  * ComponentTree : construct component tree
  * 1. create :  create the tree
  * 2. postProcess :  
  *    		a. set a label to each node by post search order
  *    		b. store the node address according to the label
  *    		c. get the leaf nodes
  * 3. setPaths : set pathlist from leaf nodes
  *****************************************************************/
ComponentTree::ComponentTree()
{
	m_root = NULL;
	m_numElements = 0;
	m_numNodes = 0;
	m_numLeafs = 0;
}
ComponentTree::ComponentTree(char * imgfile, int _minSize, int _maxSize, int _singleSize)
{
	create(imgfile,_minSize,_maxSize,_singleSize);
}

/********************************************************************
 * ComponentTree : construct component tree by saved component tree file
 * load : m_numElements, m_numNods, m_numLeafs, m_elements, m_nodes, m_leafs
 *        m_root
 *        m_width, m_height, m_depth
 * setPaths : m_paths
 ********************************************************************/
ComponentTree::ComponentTree(char* treefile)
{
	load(treefile);
}

/*******************************************************************
 *~ComponentTree : destruct component tree.
 * 1. delete all the node
 * 2. clear 4 containers
 * 3. re initial variable
 *******************************************************************/
ComponentTree::~ComponentTree()
{
	clear();
}

/************************************************************************************
 * create : create component tree
 * 
 * create will set these 
 *        member variables : m_root, m_numElements, m_elements
 *        global variables : m_width, m_height, m_depth
 ************************************************************************************/

bool ComponentTree::create(char * imgfile , int _minSize , int _maxSize, int _singleSize)
{
	/*
	 * 1. get _width, _height, _depth, _channel, img
	 */
	int _width = 0;
	int _height = 0;
	int _depth = 0;
	int _channels = 1;
	unsigned char * img = NULL;
	img = readtiff(imgfile, &_width,&_height, &_depth, &_channels);
	
	//check parameters
	assert(_width * _height * _depth <= 2147483647); //don't use m_numElements < 2147483648   (2^31)
	assert(_minSize >= 0);
	assert(_maxSize >= 0);
	if(_maxSize < _minSize) 
	{
		cerr<<"_max("<<_maxSize<<" < _min("<<_minSize<<")"<<endl;
		return false;
	}
	if(_channels > 1)
	{
		cerr<<"_channels > 1 , convert to grayscale ... "<<endl;
		int _size = _width * _height *_depth;
		unsigned char* _img = new unsigned char[_size];
		for(int i = 0; i < _size; i++)
		{
			float average = 0.0;
			int index = i * _channels;
			for(int j = 0; j < _channels; j++)
			{
				average += img[index+j];
			}
			_img[i] = (int)(average/_channels);
		}
		delete img;
		img = _img;
	}
	m_width = _width;
	m_height = _height;
	m_depth = _depth;
	m_minSize = _minSize;
	m_maxSize = _maxSize;
	m_singleSize = _singleSize;
	/*
	 * 2. initial member variable
	 */
	m_numElements = _width * _height * _depth;
	
	m_elements.resize(m_numElements);
	DisjointSets djs(m_numElements);
	vector<Node*> lowestNode;
	lowestNode.resize(m_numElements);
	for(int v = 0; v < m_numElements ; v++)
	{
		//m_elements[v].vertex = v; // set vertex
		m_elements[v].next = v;   // set pointer to itself
		lowestNode[v] = NULL;
	}
	/*
	 * 3. sort img to get the visiting order
	 */
	vector<int> order = bucketSort(img,m_numElements);
	vector<int>::iterator it;
	int   curId;
	Node* curNode;
	int   adjId;
	Node* adjNode;
	for(it = order.begin(); it!=order.end(); it++)
	{
		Vertex v = *it;
		curId = djs.FindSet(v);
		curNode = lowestNode[curId];
		/// Debug cout<<"v = "<<v<<" curId = "<<curId<<" -> ";
		if(curNode == NULL)
		{
			curNode = new Node;
			//curNode->elementCount = 1;
			curNode->size = 1;
			curNode->parent = curNode;
			curNode->entryId = v;
			//m_elements[v].parent = curNode;  // we should keep the entry's node
			curNode->level = img[v]; 
			lowestNode[curId] = curNode;
			/// Debug cout<<"create node : "<<curNode<<endl;
		}
		else
		{
			/// Debug cout<<"find set : "<<curNode<<endl;
		}
		vector<Vertex> neighbors = neighbor(v, _width, _height, _depth);
		vector<Vertex>::iterator itr;
		for(itr = neighbors.begin(); itr != neighbors.end(); itr++)
		{
			Vertex u = *itr;
			if(img[u] >= img[v])
			{
				adjId = djs.FindSet(u);        //todo: link curId and adjId and refresh lowestNode
				adjNode = lowestNode[adjId];
				/// Debug cout<<"\tu = "<<u<<" adjId = "<<adjId<<" -> ";
				//if NULL, add element
				// just put it in the entry's link, other member doesn't change
				if(adjNode == NULL)
				{
					/// Debug cout<<"add vertex "<<u<<" to current set ->";
					Element& entry = m_elements[curNode->entryId];
					m_elements[u].next = entry.next;
					entry.next = u;
					curNode->size++;
					//curNode->elementCount++;
				}
				//if different component
				else if(curNode != adjNode)
				{
					/// Debug cout<<" adjNode = "<<adjNode<<" -> ";
					//If the same level
					//add adjacent point to current component
					//merge their childNode and delete adjNode
					if(curNode->level == adjNode->level)
					{
						/// Debug cout<<" merge same level set ->";
						//swap their next , too produce a new loop
						curNode = MergeNodes(curNode,adjNode);
						delete adjNode;
					}
					else //in different level,  level(curNode) < level(adjNode)
					{
						//Theree filter : MIN FILTER, MAX FILTER, SINGLE FILTER
						//1. merge according to their size, merge adjNode to curNode
						if(_minSize > 0 && adjNode->size < _minSize || _maxSize>=_minSize && adjNode->size > _maxSize)
						{
							/// Debug cout<<"filter by min/max, merge different level->";
							curNode = MergeNodes(curNode,adjNode);
							delete adjNode;
						}
						else// if(adjNode.m_area >= _minSize && adjNode.m_area <= _maxSize)
						{
							//2. filter the single node
							/// Debug cout<<" add child ->";
							curNode->childs.push_back(adjNode);
							adjNode->parent = curNode;
							curNode->size = curNode->size + adjNode->size;
							if(adjNode->childs.size() == 1)
							{
								/// Debug cout<<" merge single child ->";
								Node* childNode = adjNode->childs.front();
								//adjNode->size = adjNode->size - childNode->size;
								/*
								 * if _singleSize = 1 means is_cut_single = false
								 * if _singleSize > maxSize of every single node  than is_cut_single = true
								 */
								if(adjNode->size - childNode->size < _singleSize)
								{
									adjNode->size = adjNode->size - childNode->size;
									adjNode->childs.clear();
									adjNode = MergeNodes(adjNode,childNode);
									delete childNode;
								}
							}
						}
					}
				}
				/// Debug cout<<"union("<<curId<<","<<adjId<<") = ";
				curId = djs.Union(curId,adjId);
				/// Debug cout<<curId<<endl;
				lowestNode[curId] = curNode;
			}
			
		}
	}
	
	/*
	 * 4. get m_root
	 */
	m_root = lowestNode[djs.FindSet(0)];

	/*
	 * 5. build the tree completely by post process
	 */ 
	int label = 0;
	//m_nodes, m_leafs,m_numLeafs,m_numNodes
	postProcess(m_root,label);
	m_numLeafs = m_leafs.size();
	m_numNodes = m_nodes.size();
	//m_paths
	
	/*
	 * 6. set mean and paths
	 */
	
	setStatistic(img);
	setCenter();
	setPaths();
	setOrders();
	/*
	 * 7. free space
	 */
	delete img;
	return true;
}

/**************************************************************************
 * save : save these variables to file
 *        member variable: m_numElements, m_numNodes, m_elements, m_nodes
 *        global variable: m_width, m_height, m_depth
 **************************************************************************/

bool ComponentTree::save(const char* treefile) const
{
	assert(m_numElements > 0);
	assert(m_numNodes > 0);
	cout<<"save to "<<treefile<<endl;
	ofstream ofs(treefile, ios_base::out|ios_base::binary);
	if(ofs.fail()) 
	{
		ofs.close();
		return false;
	}
	bool rt = save(ofs);
	ofs.close();
	return rt;
}

bool ComponentTree::save(ofstream& ofs) const
{
	assert(m_numElements > 0);
	assert(m_numNodes > 0);
	//1. output the element num(m_numElements) and component num(m_numNodes)
	writeValue(ofs,m_numElements);
	writeValue(ofs,m_numNodes);
	//2. save elements by their order
	ofs.write((char*)&(m_elements[0]),m_numElements * sizeof(Element));
	
	//3. save component by there label order
	for(int j = 0; j< m_numNodes && ofs.good(); j++)
	{
		if(ofs.fail()) return false;
		Node* node = m_nodes[j];
		writeValue(ofs,node->label);
		writeValue(ofs,node->level);
		writeValue(ofs,node->mean);
		writeValue(ofs,node->std);
		writeValue(ofs,node->centerX);
		writeValue(ofs,node->centerY);
		writeValue(ofs,node->centerZ);
		writeValue(ofs,node->size);
		writeValue(ofs,node->entryId);
		int child_size = node->childs.size();
		writeValue(ofs,child_size);
		//save childs size
		Nodes::iterator it = node->childs.begin();
		while(it != node->childs.end())
		{
			writeValue(ofs,(*it)->label);
			it++;
		}
	}
	
	//4. save global variable
	writeValue(ofs,m_width);
	writeValue(ofs,m_height);
	writeValue(ofs,m_depth);
	writeValue(ofs,m_minSize);
	writeValue(ofs,m_maxSize);
	writeValue(ofs,m_singleSize);
	//ofs.close();
	return true;
}

/*********************************************************************************
 * load : load the file to set many 
 *        member variable : m_numElement, m_numNodes, m_root, m_elements, m_nodes
 *        global variables : m_width, m_height, m_depth
 *
 * the same role as create(), but load will do one more thing, set 
 *                            m_leafs and m_numLeafs 
 **********************************************************************************/

bool ComponentTree::load(const char* treefile)
{
	ifstream ifs(treefile, ios_base::in|ios_base::binary);
	if(ifs.fail())
	{
		ifs.close();
		return false;
	}
	bool rt = load(ifs);
	ifs.close();
	return rt;
}

bool ComponentTree::load(ifstream& ifs)
{
	//1. get m_numElements, m_numNodes;
	readValue(ifs,m_numElements);
	readValue(ifs,m_numNodes);
	if(false)	cout<<"numElements = "<<m_numElements<<" numNodes= "<<m_numNodes<<endl;
	
	//2. build all the element
	m_elements.resize(m_numElements);
	ifs.read((char*)&(m_elements[0]),m_numElements * sizeof(Element));
	//for(int i = 0; i < m_numElements; i++)
	//{
	//	m_elements[i].next = readValue(ifs);
	//}	
	//3. build all the components
	m_nodes.resize(m_numNodes);
	int childSize = -1;
	int childLabel = -1;
	if(false)	cout<<"label\tlevel\tsize\tentryId"<<endl;
	for(int j = 0; j< m_numNodes && ifs.good(); j++)
	{
		Node* node = new Node;
		readValue(ifs,node->label);
		readValue(ifs,node->level);
		readValue(ifs,node->mean);
		readValue(ifs,node->std);
		readValue(ifs,node->centerX);
		readValue(ifs,node->centerY);
		readValue(ifs,node->centerZ);
		readValue(ifs,node->size);
		readValue(ifs,node->entryId);
		readValue(ifs,childSize);
		if(false)	cout<<node->label<<"\t"<<node->level<<"\t"<<node->size<<"\t"<<node->entryId<<"\t";
		//childSize == 0 set leaf
		if(childSize == 0)
		{
			if(false)	cout<<" leaf ";
			m_leafs.push_back(node);
		}
		//childSize > 0
		else
		{
			if(false)	cout<<childSize<<" childs: ";
			assert(childSize > 0);
			node->childs.resize(childSize);
			for(int i = 0; i < childSize && ifs.good(); i++)
			{
				//ifs.read((char*)&childLabel,sizeof(int));
				readValue(ifs,childLabel);
				node->childs[i] = m_nodes[childLabel];
				if(false)	cout<<"->"<<childLabel;
			}
		}
		if(false)	cout<<endl;
		m_nodes[j]=node;
	}
	m_numLeafs = m_leafs.size();
	//4. get m_root, it is the last element of m_nodes
	m_root = m_nodes[m_numNodes -1];
	
	//5. get global variable
	readValue(ifs,m_width);
	readValue(ifs,m_height);
	readValue(ifs,m_depth);
	if(false)	cout<<"m_width = "<<m_width<<"  m_height = "<<m_height<<"  m_depth = "<<m_depth<<endl;
	readValue(ifs,m_minSize);
	readValue(ifs,m_maxSize);
	readValue(ifs,m_singleSize);
	if(false)	cout<<"m_minSize = "<<m_minSize<<" m_maxSize = "<<m_maxSize<<" m_singleSize = "<<m_singleSize<<endl;
	//ifs.close();
	
	//6. set parent for each node
	m_root->parent = m_root;
	deque<Node*> queue;
	Node* front = NULL;
	queue.push_back(m_root);
	while(!queue.empty())
	{
		front = queue.front();
		queue.pop_front();
		Nodes::iterator it = front->childs.begin();
		while(it != front->childs.end())
		{
			(*it)->parent = front;
			queue.push_back(*it);
			it++;
		}
	}
	//7. set paths
	setPaths();
	//8. set post order, pre order and breadth first searching order
	setOrders();
	return true;
}

void ComponentTree::clear()
{
	for(int i= 0; i< m_numNodes; i++)
	{
		Node* node = m_nodes[i];
		delete node;
	}
	m_elements.clear();
	m_nodes.clear();
	m_leafs.clear();
	m_paths.clear();
	m_preOrder.clear();
	m_postOrder.clear();
	m_breadthFirst.clear();
	m_numLeafs = 0;
	m_numElements = 0;
	m_numNodes = 0;
	m_root = NULL;
}


int ComponentTree::width() const
{
	return m_width;
}

int ComponentTree::height() const
{
	return m_height;
}

int ComponentTree::depth() const
{
	return m_depth;
}

ComponentTree::Node* ComponentTree::root() const
{
	return m_root;
}

ComponentTree::Node* ComponentTree::getNode(int label) const
{
	assert(label >= 0);
	assert(label < (int)m_nodes.size());
	return m_nodes[label];
}

ComponentTree::Nodes ComponentTree::getNodes() const
{
	return m_nodes;
}

ComponentTree::Paths ComponentTree::getPaths() const
{
	return this->m_paths;
}

/*****************************************
 * setStatistic : set the Mean and std of each component
 * 
 * This work should be down after the post store of each node
 *****************************************/

void ComponentTree::setStatistic(unsigned char* img)
{
	vector<Node*>::iterator it = m_nodes.begin();
	
	double _sum_square;
	double _sum_level;
	
	while(it != m_nodes.end())
	{
		_sum_square = 0.0;
		_sum_level = 0.0;
		
		Node* node = *it;
		
		ElementId entryId = node->entryId;
		
		ElementId eid = m_elements[entryId].next;
		while(eid != entryId)
		{			
			_sum_level += img[eid];
			
			_sum_square += img[eid] * img[eid];
			
			eid = m_elements[eid].next;

		}
		_sum_level += img[eid];                         //sum of level
		_sum_square += img[eid] * img[eid];  //sum of square
		
		vector<Node*>::iterator itr = node->childs.begin();
		while(itr!=node->childs.end())
		{
			_sum_level += (*itr)->mean * (*itr)->size;

			_sum_square += (*itr)->std * (*itr)->std * (*itr)->size + (*itr)->size * (*itr)->mean * (*itr)->mean;

			itr++;
		}
		
		double _mean_level = _sum_level / node->size;
		double _mean_square = _sum_square / node->size;
				
		node->mean = _mean_level;
		
		node->std = sqrt(_mean_square - _mean_level * _mean_level);
		
		it++;
	}
}

/********************************************
 * setCenter : set the center of each node
 ********************************************/
void ComponentTree::setCenter()
{
	vector<Node*>::iterator it = m_nodes.begin();
	
	double _sum_square;
	double _sum_level;
	
	double _sum_width;
	double _sum_height;
	double _sum_depth;
	
	while(it != m_nodes.end())
	{
		_sum_square = 0.0;
		_sum_level = 0.0;
		
		_sum_width  =   0.0;
		_sum_height =   0.0;
		_sum_depth   =   0.0;
		
		Node* node = *it;
		
		ElementId entryId = node->entryId;
		
		ElementId eid = m_elements[entryId].next;
		
		while(eid != entryId)
		{	
			//vertex = eid;
			
			_sum_depth = eid/(m_width*m_height);
			_sum_height = (eid % (m_width*m_height))/m_width;
			_sum_width = eid % m_width;
			
			eid = m_elements[eid].next;
		}
		_sum_depth = eid/(m_width*m_height);
		_sum_height = (eid % (m_width*m_height))/m_width;
		_sum_width = eid % m_width;
		
		vector<Node*>::iterator itr = node->childs.begin();
		while(itr!=node->childs.end())
		{
			_sum_depth += (*itr)->centerZ * (*itr)->size;
			_sum_height += (*itr)->centerY * (*itr)->size;
			_sum_width += (*itr)->centerX * (*itr)->size;
			itr++;
		}
		
		node->centerX = _sum_width / node->size;
		node->centerY = _sum_height / node->size;
		node->centerZ = _sum_depth / node->size;
		it++;
	}
}

void ComponentTree::setPaths()
{
	Path path;
	m_paths.clear();
	Nodes::iterator it = m_leafs.begin();
	while(it!= m_leafs.end())
	{
		path.clear();
		Node* p = *it;
		while(p != p->parent)
		{
			path.push_back(p->label);
			p = p->parent;
		}
		// p == p->parent
		path.push_back(p->label);
		m_paths.push_back(path);
		it++;
	}
}

/************************************************************
 * setOrders : set m_preOrder, m_postOrder and m_breadthFirst
 *             which is used to tranverse the tree by preorder
 *             postOrder and breadth first order
 ************************************************************/
void ComponentTree::setOrders()
{
	m_preOrder.resize(m_numNodes);
	m_postOrder.resize(m_numNodes);
	m_breadthFirst.resize(m_numNodes);
	
	int i = 0;
	for(i = 0; i < m_numNodes; i++) m_postOrder[i] = m_nodes[i];
	
	i = 0;
	stack<Node*> pre_stack;
	pre_stack.push(m_root);
	while(! pre_stack.empty())
	{
		Node* top = pre_stack.top();
		pre_stack.pop();
		m_preOrder[i++] = top;
		vector<Node*>::reverse_iterator it = top->childs.rbegin();
		while(it != top->childs.rend())
		{
			pre_stack.push(*it);
			it++;
		}
	}
	
	i = 0;
	deque<Node*> breadth_queue;
	Node* front;
	breadth_queue.push_back(m_root);
	while(!breadth_queue.empty())
	{
		front = breadth_queue.front();
		breadth_queue.pop_front();
		m_breadthFirst[i++] = front;
		vector<Node*>::iterator it = front->childs.begin();
		while(it != front->childs.end())
		{
			breadth_queue.push_back(*it);
			it++;
		}
	}
}

/************************************************************
 * getVertices: get all the vertices of component with label
 * as well as its sub components
 ************************************************************/
ComponentTree::Vertices ComponentTree::getVertices(int label) const
{
	if(label == -1) label = m_root->label;
	assert(label>=0);
	Vertices _vertices;
	Node* node = getNode(label);
	_vertices.resize(node->size);
	int i = 0;
	deque<Node*> queue;
	Node* front;
	queue.push_back(node);
	while(!queue.empty())
	{
		front = queue.front();
		queue.pop_front();
		ElementId entryId = front->entryId;
		ElementId eid = m_elements[entryId].next;
		while(eid != entryId)
		{
			_vertices[i++] = eid;
			eid = m_elements[eid].next;
		}
		_vertices[i++] = entryId;
		
		Nodes::iterator it = front->childs.begin();
		while(it!= front->childs.end())
		{
			queue.push_back(*it);
			it++;
		}
		
	}
	return _vertices;
}

int ComponentTree::nodeCount() const
{
	return m_numNodes;
}

int ComponentTree::elementCount() const
{
	return m_numElements;
}

int ComponentTree::leafCount() const
{
	return m_numLeafs;
}

/*****************************************************************
 * printTree : print component Node recursively
 *****************************************************************/
void ComponentTree::printTreeRecursively(int label, int spaceCount) const
{
	assert(label >=0);
	Node* node = m_nodes[label];
	int _selfSize = node->size;
	cout.precision(3);
	for(Nodes::iterator it = node->childs.begin(); it != node->childs.end(); it++)
	{
		_selfSize -= (*it)->size;
	}
	if(node->childs.empty())
	{
		cout<<label<<" : "<<node->level<<"("<<(node->mean - node->level)<<","<<node->std<<") "<<node->size<<" "<<_selfSize<<"  leaf"<<endl;
		return;
	}
	else
	{
		cout<<label<<" : "<<node->level<<"("<<(node->mean - node->level)<<","<<node->std<<") "<<node->size<<" "<<_selfSize<<endl;
	}
	
	for(Nodes::iterator it = node->childs.begin(); it != node->childs.end(); it++)
	{
		for(int i=0; i < spaceCount; i++)
		{
			cout<<" ";
		}
		cout<<"|"<<endl;
		for(int i=0; i < spaceCount; i++)
		{
			cout<<" ";
		}
		cout<<"|__";
	
		printTreeRecursively((*it)->label, spaceCount+3);
	}
}

void ComponentTree::printTree(int label) const
{
	if(m_root == NULL)
	{
		cout<<"Warning : The tree is empty!"<<endl;
		return;
	}
	if(label == -1) label = m_root->label;
	assert(label>=0);
	cout<<"---------------Component Tree-------------------------"<<endl;
	cout<<"label : level totalsize elementSize"<<endl;
	printTreeRecursively(label,0);
	cout<<endl;
	cout<<"width = "<<m_width<<"\theight = "<<m_height<<"\tdepth = "<<m_depth<<endl;
	cout<<"minSize = "<<m_minSize<<"\tmaxSize = "<<m_maxSize<<"\tsingleSize = "<<m_singleSize<<endl;
	cout<<"total node : "<<m_numNodes<<"\tleaf node : "<<m_numLeafs<<endl;
	cout<<"------------------------------------------------------------"<<endl;
	cout<<"pre order : ";
	const_iterator it = begin(PRE_ORDER);
	while(it != end(PRE_ORDER)) 
	{
		cout<<(*it)->label<<" ";
		it++;
	}
	cout<<endl;
	
	cout<<"post order : ";
	it = begin(POST_ORDER);
	while(it != end(POST_ORDER))
	{
		cout<<(*it)->label<<" ";
		it++;
	}
	cout<<endl;
	cout<<"breadth first : ";
	it = begin(BREADTH_FIRST);
	while(it != end(BREADTH_FIRST))
	{
		cout<<(*it)->label<<" ";
		it++;
	}
	cout<<endl;
	 
}

/****************************************************************************
 * printMapping : print each point's label
 *
 * used function : label(vertex)
 ****************************************************************************/
int* ComponentTree::getMappingMatrix() const
{
	int* matrix = new int[m_numElements];
	memset(matrix,0,m_numElements);
	int label = -1;
	deque<Node*> queue;
	Node* front;
	queue.push_back(m_root);
	while(!queue.empty())
	{
		front = queue.front();
		queue.pop_front();
		label = front->label;
		ElementId entryId = front->entryId;
		ElementId eid = m_elements[entryId].next;
		while(eid != entryId)
		{
			matrix[eid] = label;
			eid = m_elements[eid].next;
		}
		matrix[eid] = label;	
		
		Nodes::iterator it = front->childs.begin();
		while(it!= front->childs.end())
		{
			queue.push_back(*it);
			it++;
		}
		
	}
	return matrix;
}

/********************************************************************
 * getMatrix : get the vertices with label in labels
 * intput : labels ,  values, initial value
 * output : a int* matrix, the vertice will be assign with 
 *          the element of values
 ********************************************************************/
int* ComponentTree::getMatrix(vector<int> labels, vector<int> values ,int ini) const
{
	int* matrix = new int[m_numElements];
	//memset(matrix,0,m_numElements);
	for(int i = 0; i< m_numElements ;i++)
	{
		matrix[i] = ini;
	}
	int label= -1 ;
	int value = -1;
	for(int i = 0; i < (int)labels.size(); i++)
	{
		label = labels[i];             //the label will start from 0
		value = values[i];		
		deque<Node*> queue;
		Node* front;
		queue.push_back(m_nodes[label]);
		while(!queue.empty())
		{
			front = queue.front();
			queue.pop_front();
			//label = front->label;
			ElementId entryId = front->entryId;
			ElementId eid = m_elements[entryId].next;
			while(eid != entryId)
			{
				matrix[eid] = value;
				eid = m_elements[eid].next;
			}
			matrix[eid] = value;	
			
			Nodes::iterator itr = front->childs.begin();
			while(itr!= front->childs.end())
			{
				queue.push_back(*itr);
				itr++;
			}
		}
	}
	return matrix;
}


void ComponentTree::printMapping() const
{
	int* matrix = getMappingMatrix();
	cout<<"------------------------ All Labels -----------------------------------"<<endl;
	int i = 0;
	int w = (int)(log10((float)m_numNodes)) + 1;
	char format[10];
	if(w == 1)
	{
		sprintf(format,"%sd ","%");
	}
	else
	{
		sprintf(format,"%s%d%dd ","%",0,w);
	}

	while(i < m_numElements)
	{
		char out[10];
		sprintf(out,format,matrix[i]);
		cout<<out;
		if(i%(m_width * m_height) == 0 && i/(m_width * m_height) > 0) 
			cout<<"layer "<<i/(m_width * m_height)+1<<": "<<endl;
		if(i%m_width == 0 && i > 0)cout<<endl;
		i++;
	}
	cout<<endl;
	cout<<"width = "<<m_width<<"\t height = "<<m_height<<"\t depth = "<<m_depth<<endl<<endl;
	cout<<"label count : "<<m_numNodes<<endl;
	cout<<"----------------------------------------------------------------------"<<endl;
	delete matrix;
	
}

/*************************************************************
 * printVertices : print the component with label l and its subcomponents
 *************************************************************/

void ComponentTree::printVertices(int label) const
{
	if(m_root == NULL)cout<<"Tree construct failure"<<endl;
	if(label == -1) label = m_root->label;
	assert(label >= 0);
	
	Vertices vertices = getVertices(label);
	Vertices::iterator it;

	for(it = vertices.begin(); it != vertices.end(); it++)
	{
		cout<<*it<<" ";
	}
	cout<<endl;
}

/*************************************************************
 * printPaths : print m_paths
 *************************************************************/
void ComponentTree::printPaths() const
{
	cout<<"---------------------- All Paths -----------------------"<<endl;
	Paths::const_iterator it;
	int i = 1;
	for(it = m_paths.begin(); it != m_paths.end(); it++)
	{
		cout<<"Path "<<i++<<" -> ";
		Path::const_iterator itr;
		for(itr = (*it).begin(); itr != (*it).end(); itr++)
		{
			cout<<(*itr)<<"  ";
		}
		cout<<endl;
	}
	//cout<<"path count :"<<m_numLeafs<<endl;
	cout<<"--------------------------------------------------------"<<endl;
}
/*************************************************************
 * printPath : print the one path from the leaf node 
 * Input :
 *       path from 1 ~ leafCount 
 *************************************************************/
void ComponentTree::printPath(int pathId) const
{
	if(pathId <= 0 || pathId > m_numLeafs) return;
	cout<<"Path "<<pathId<<" : (leaf)";
	Path path = m_paths[pathId - 1];
	Path::iterator itr;
	for(itr = path.begin(); itr != path.end(); itr++)
	{
		if(m_nodes[*itr] != m_nodes[*itr]->parent)
			cout<<(*itr)<<" -> ";
		else cout<<*itr;
	}
	cout<<"(root) "<<endl;

}
/*****************************************************
 * postProcess : setLabels function will do two things
 * 1. set the label of each node
 * 2. store the node index to m_nodes by their label
 * 3. set m_leafs
 *
 * that is to say we can get these member variables : 
 * m_nodes, m_leafs
 *****************************************************/
int ComponentTree::postProcess(Node* node, int label)
{
	/// Debug cout<<"node : "<<node<<" -> ";
	if(node == NULL) return label;
	/// Debug cout<<"(label = "<<label<<" -> ";
	assert(label >= 0);
	if(node->childs.empty())
	{
		/// Debug cout<<"\tleaf node"<<endl;;
		node->label = label++;
		m_leafs.push_back(node);
		m_nodes.push_back(node);
		/// Debug cout<<label<<") -> ";
		return label;
	}
	else
	{
		/// Debug cout<<"child num : "<<node->childs.size()<<" -> ";
		Nodes::iterator it = node->childs.begin();
		for(it = node->childs.begin(); it != node->childs.end(); it++)
		{
			label = postProcess((Node*)(*it),label);
		}
		node->label = label++;
		m_nodes.push_back(node);
	}
	return label;
}


/*****************************************************************************
 * MergesNode : merge two component node
 * 1. merge entry to curNode
 * 2. merge size
 *****************************************************************************/

ComponentTree::Node* ComponentTree::MergeNodes(Node* curNode, Node* adjNode)
{
	Node* root = (curNode->level <= adjNode->level)?curNode:adjNode;
	Node* child = (curNode->level > adjNode->level)?curNode:adjNode;
	
	ElementId temp = m_elements[root->entryId].next;
	m_elements[root->entryId].next = m_elements[child->entryId].next;
	m_elements[child->entryId].next = temp;
	root->size = root->size + child->size;
	//root->elementCount = root->elementCount + child->elementCount;
	Nodes::iterator it = child->childs.begin();
	while(it != child->childs.end())
	{
		root->childs.push_back(*it);
		(*it)->parent = root;
		it++;
	}
	return root;
}

// iterator

ComponentTree::iterator::iterator()
{
}

void ComponentTree::iterator::operator=(ComponentTree::iterator& copy)
{
	this->node_itr = copy.node_itr;
}

ComponentTree::iterator ComponentTree::iterator::operator++(int)
{
	iterator itr = *this;
	node_itr++;
	return itr;
}

ComponentTree::Node* ComponentTree::iterator::operator*() const
{
	return *node_itr;
}

bool ComponentTree::iterator::operator!=(iterator& itr)
{
	return (this->node_itr != itr.node_itr);
}

// const_iterator


ComponentTree::const_iterator::const_iterator()
{
}

void ComponentTree::const_iterator::operator=(ComponentTree::const_iterator copy)
{
	this->node_itr = copy.node_itr;
}

ComponentTree::const_iterator ComponentTree::const_iterator::operator++(int)
{
	const_iterator itr = *this;
	node_itr++;
	return itr;
}

const ComponentTree::Node* ComponentTree::const_iterator::operator*() const
{
	return *node_itr;
}

bool ComponentTree::const_iterator::operator!=(const_iterator itr)
{
	return (this->node_itr != itr.node_itr);
}

ComponentTree::iterator ComponentTree::begin(ComponentTree::SEARCH_TYPE type)
{
		iterator itr;
		switch(type)
		{
			case PRE_ORDER:
				itr.node_itr = m_preOrder.begin();
				break;
			case POST_ORDER:
				itr.node_itr = m_postOrder.begin();
				break;
			case BREADTH_FIRST:
				itr.node_itr = m_breadthFirst.begin();
				break;
			default:
				cerr<<"Unknow type!"<<endl;
				exit(0);
		}
		  
		  return itr;
}

ComponentTree::const_iterator ComponentTree::begin(ComponentTree::SEARCH_TYPE type) const
{
	const_iterator itr;
	switch(type)
	{
		case PRE_ORDER:
			itr.node_itr = m_preOrder.begin();
			break;
		case POST_ORDER:
			itr.node_itr = m_postOrder.begin();
			break;
		case BREADTH_FIRST:
			itr.node_itr = m_breadthFirst.begin();
			break;
		default:
			cerr<<"Unknow type!"<<endl;
			exit(0);
	}
	
	return itr;
}

ComponentTree::iterator ComponentTree::end(ComponentTree::SEARCH_TYPE type)
{
	iterator itr;
	switch(type)
	{
		case PRE_ORDER:
			itr.node_itr = m_preOrder.end();
			break;
		case POST_ORDER:
			itr.node_itr = m_postOrder.end();
			break;
		case BREADTH_FIRST:
			itr.node_itr = m_breadthFirst.end();
			break;
		default:
			cerr<<"Unknow type!"<<endl;
			exit(0);
	}
	
	return itr;
}

ComponentTree::const_iterator ComponentTree::end(ComponentTree::SEARCH_TYPE type) const
{
	const_iterator itr;
	switch(type)
	{
		case PRE_ORDER:
			itr.node_itr = m_preOrder.end();
			break;
		case POST_ORDER:
			itr.node_itr = m_postOrder.end();
			break;
		case BREADTH_FIRST:
			itr.node_itr = m_breadthFirst.end();
			break;
		default:
			cerr<<"Unknow type!"<<endl;
			exit(0);
	}
	
	return itr;
}


DisjointSets::DisjointSets()
{
	m_numElements = 0;
	m_numSets = 0;
}

DisjointSets::DisjointSets(int count)
{
	m_numElements = 0;
	m_numSets = 0;
	AddElements(count);
}

DisjointSets::DisjointSets(const DisjointSets & s)
{
	this->m_numElements = s.m_numElements;
	this->m_numSets = s.m_numSets;
	
	// Copy nodes
	m_nodes.resize(m_numElements);
	for(int i = 0; i < m_numElements; ++i)
		m_nodes[i] = new Node(*s.m_nodes[i]);
	
	// Update parent pointers to point to newly created nodes rather than the old ones
	for(int i = 0; i < m_numElements; ++i)
		if(m_nodes[i]->parent != NULL)
			m_nodes[i]->parent = m_nodes[s.m_nodes[i]->parent->index];
}

DisjointSets::~DisjointSets()
{
	for(int i = 0; i < m_numElements; ++i)
		delete m_nodes[i];
	m_nodes.clear();
	m_numElements = 0;
	m_numSets = 0;
}

// Note: some internal data is modified for optimization even though this method is consant.
int DisjointSets::FindSet(int elementId) const
{
	assert(elementId < m_numElements);
	
	Node* curNode;
	
	// Find the root element that represents the set which `elementId` belongs to
	curNode = m_nodes[elementId];
	while(curNode->parent != NULL)
		curNode = curNode->parent;
	Node* root = curNode;
	
	// Walk to the root, updating the parents of `elementId`. Make those elements the direct
	// children of `root`. This optimizes the tree for future FindSet invokations.
	curNode = m_nodes[elementId];
	while(curNode != root)
	{
		Node* next = curNode->parent;
		curNode->parent = root;
		curNode = next;
	}
	
	return root->index;
}

int DisjointSets::Union(int setId1, int setId2)
{
	assert(setId1 < m_numElements);
	assert(setId2 < m_numElements);
	
	if(setId1 == setId2)
		return FindSet(setId1); // already unioned
	
	Node* set1 = m_nodes[setId1];
	Node* set2 = m_nodes[setId2];
	
	// Determine which node representing a set has a higher rank. The node with the higher rank is
	// likely to have a bigger subtree so in order to better balance the tree representing the
	// union, the node with the higher rank is made the parent of the one with the lower rank and
	// not the other way around.
	if(set1->rank > set2->rank)
	{
		set2->parent = set1;
	}
	else if(set1->rank < set2->rank)
	{
		set1->parent = set2;
	}
	else // set1->rank == set2->rank
	{
		set2->parent = set1;
		++set1->rank; // update rank
	}
	
	// Since two sets have fused into one, there is now one less set so update the set count.
	--m_numSets;
	return FindSet(setId1);
}

void DisjointSets::AddElements(int numToAdd)
{
	assert(numToAdd >= 0);
	
	// insert and initialize the specified number of element nodes to the end of the `m_nodes` array
	m_nodes.insert(m_nodes.end(), numToAdd, (Node*)NULL);
	for(int i = m_numElements; i < m_numElements + numToAdd; ++i)
	{
		m_nodes[i] = new Node();
		m_nodes[i]->parent = NULL;
		m_nodes[i]->index = i;
		m_nodes[i]->rank = 0;
	}
	
	// update element and set counts
	m_numElements += numToAdd;
	m_numSets += numToAdd;
}

int DisjointSets::NumElements() const
{
	return m_numElements;
}

int DisjointSets::NumSets() const
{
	return m_numSets;
}

