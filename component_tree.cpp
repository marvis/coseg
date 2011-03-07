//
//=======================================================================
// Copyright 2010 Institute PICB.
// Authors: Hang Xiao, Axel Mosig
// Data : July 11, 2010
//=======================================================================
//

#include <iostream>
#include <fstream>
#include <string>
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

bool component_tree_verbose = false;

/******************************************************************
 * ComponentTree::Pixel
 *****************************************************************/
ComponentTree::Pixel::Pixel()
{
}

bool ComponentTree::Pixel::save(ofstream& ofs, bool saveType) const
{
	if(saveType == 0) // binary format
	{
		writeValue(ofs, pos);
		writeValue(ofs, next->pos);
		writeValue(ofs, level);
		writeValue(ofs, node->label);
	}
	else // txt format
	{
		ofs<<pos<<" ";
		ofs<<next->pos<<" ";
		ofs<<level<<" ";
		ofs<<node->label<<" ";
		ofs<<endl;
	}
	if(ofs.good()) return true;
	else return false;
}

bool ComponentTree::Pixel::load(ifstream& ifs, vector<Pixel>& pixels,vector<Node*>& nodes, bool saveType)
{
	if(saveType == 0) // binary format
	{
		readValue(ifs, pos);
		int next_pos;
		readValue(ifs, next_pos); 
		next=&pixels[next_pos];
		readValue(ifs, level);
		int node_label;
		readValue(ifs, node_label);
		node = nodes[node_label];

	}
	else // txt format
	{
		ifs>>pos;
		int next_pos;
		ifs>>next_pos;
		next=&pixels[next_pos];
		ifs>>level;
		int node_label;
		ifs>>node_label;
		node = nodes[node_label];
	}
	if(ifs.good()) return true;
	else return false;
}

void ComponentTree::Pixel::merge_entry(ComponentTree::Pixel *entry)
{
    Pixel* temp = this->next;
    this->next = entry->next;
    entry->next = temp;
}

/******************************************************************
 * ComponentTree::Node
 ****************************************************************/

bool ComponentTree::Node::save(ofstream& ofs, bool saveType) const
{
	if(saveType == 0) // binary format
	{
		writeValue(ofs, label);
		writeValue(ofs, lowest_level);
		writeValue(ofs, highest_alpha_level);
		writeValue(ofs, alpha_size);
		writeValue(ofs, beta_size);
		writeValue(ofs, parent->label);
		writeValue(ofs, entry_pixel->pos);
		if(childs.empty())
		{
			int child_size = 0;
			writeValue(ofs,child_size);
		}
		else
		{
			int child_size = childs.size();
			writeValue(ofs, child_size);
			vector<Node*>::const_iterator it = childs.begin();
			while(it != childs.end())
			{
				writeValue(ofs, (*it)->label);
				it++;
			}
		}
	}
	else       // txt format
	{
		ofs<<label<<" ";
		ofs<<lowest_level<<" ";
		ofs<<highest_alpha_level<<" ";
		ofs<<alpha_size<<" ";
		ofs<<beta_size<<" ";
		ofs<<parent->label<<" "; // not necessary when load
		ofs<<entry_pixel->pos<<" ";
		if(childs.empty())
		{
			ofs<<(int)0<<" ";
		}
		else
		{
			ofs<<childs.size()<<" ";
			vector<Node*>::const_iterator it = childs.begin();
			while(it != childs.end())
			{
				ofs<<(*it)->label<<" ";
				it++;
			}
		}
		ofs<<endl;
	}
	if(ofs.good()) return true;
	else return false;
}

bool ComponentTree::Node::load(ifstream& ifs, vector<ComponentTree::Pixel>& pixels, vector<ComponentTree::Node*>& nodes, bool saveType)
{
	if(saveType == 0) // binary format
	{
		readValue(ifs, label);
		readValue(ifs, lowest_level);
		readValue(ifs, highest_alpha_level);
		readValue(ifs, alpha_size);
		readValue(ifs, beta_size);
		int par_label;
		readValue(ifs, par_label);
		assert(par_label >= label);
		parent = nodes[par_label];
		int pixel_pos;
		readValue(ifs, pixel_pos);
		entry_pixel = &pixels[pixel_pos];
		int child_size;
		readValue(ifs, child_size);
		for(int i = 0; i < child_size; i++)
		{
			int child_label;
			readValue(ifs, child_label);
			assert(child_label < label);
			childs.push_back(nodes[child_label]);

		}
	}
	else // read from txt files
	{
		ifs>>label;
		ifs>>lowest_level;
		ifs>>highest_alpha_level;
		ifs>>alpha_size;
		ifs>>beta_size;
		int par_label;
		ifs>>par_label;
		assert(par_label >= label);
		parent = nodes[par_label];
		int pixel_pos;
		ifs>>pixel_pos;
		entry_pixel = &pixels[pixel_pos];
		int child_size;
		ifs>>child_size;
		for(int i=0; i < child_size; i++)
		{
			int child_label;
			ifs>>child_label;
			assert(child_label < label);
			childs.push_back(nodes[child_label]);
		}
	}
	if(ifs.good()) return true;
	else return false;
}

vector<ComponentTree::Pixel*> ComponentTree::Node::alpha_pixels()
{
    vector<Pixel*> pixels;
    Pixel* start = this->entry_pixel;
    pixels.push_back(start);
    Pixel* p = this->entry_pixel->next;
    while(p!=start)
    {
        pixels.push_back(p);
        p = p->next;
    }
    return pixels;
}

vector<ComponentTree::Pixel*> ComponentTree::Node::beta_pixels()
{
    vector<Pixel*> pixels = alpha_pixels();

    vector<Node*> post_order = this->getPostOrderNodes();

    vector<Node*>::iterator itr = post_order.begin();
    while(itr != post_order.end())
    {
        vector<Pixel*> temp_pixels = (*itr)->alpha_pixels();
        pixels.insert(pixels.end(),temp_pixels.begin(), temp_pixels.end());
        itr++;
    }
    return pixels;
}

vector<int> ComponentTree::Node::alpha_points()
{
    vector<int> points;
    Pixel* start = this->entry_pixel;
    points.push_back(start->pos);
    Pixel* p = this->entry_pixel->next;
    while(p!=start)
    {
        points.push_back(p->pos);
        p = p->next;
    }
    return points;
}

vector<int> ComponentTree::Node::beta_points()
{
    vector<Pixel*> pixels = beta_pixels();
    vector<int> points;
    vector<Pixel*>::iterator itr = pixels.begin();
    while(itr != pixels.end())
    {
        points.push_back((*itr)->pos);
        itr++;
    }
    return points;
}

void ComponentTree::Node::merge_node(ComponentTree::Node *node)
{
    this->entry_pixel->merge_entry(node->entry_pixel);
    this->alpha_size += node->alpha_size;
    this->highest_alpha_level = node->highest_alpha_level > this->highest_alpha_level ? node->highest_alpha_level: this->highest_alpha_level;
    if(node->parent != this)  // adjacent node || small node || large node
    {
        this->beta_size += node->beta_size;
    }
    else if(node->childs.size() == 1)
    {
        vector<Node*>::iterator it=this->childs.begin();
	while(it != this->childs.end())
	{
		if(*it == node) 
		{
			this->childs.erase(it);
			break;
		}
	}
    }
    else cerr<<"merge_node error!"<<endl; 

    vector<Node*>::iterator itr = node->childs.begin();
    while(itr != node->childs.end())
    {
        this->childs.push_back(*itr);
        (*itr)->parent = this;
        itr++;
    }
    delete node;
}

ComponentTree::Nodes ComponentTree::Node::getPostOrderNodes()
{
    vector<Node*> postOrder;

    stack<Node*> post_stack;
    stack<bool> status_stack;
    post_stack.push(this);
    status_stack.push(false);
    while(! post_stack.empty())
    {
            Node* top = post_stack.top();
            bool&  top_status = status_stack.top();
            if(top_status || top->childs.empty())
            {
                post_stack.pop();
		status_stack.pop();
                postOrder.push_back(top);
            }
            else
            {
		top_status = true;
                vector<Node*>::reverse_iterator it = top->childs.rbegin();
                while(it != top->childs.rend())
                {
                        post_stack.push(*it);
                        status_stack.push(false);
                        it++;
                }
            }
    }
    return postOrder;
}

ComponentTree::Nodes ComponentTree::Node::getPreOrderNodes()
{
    vector<Node*> preOrder;

    stack<Node*> pre_stack;
    pre_stack.push(this);
    while(! pre_stack.empty())
    {
            Node* top = pre_stack.top();
            pre_stack.pop();
            preOrder.push_back(top);
            vector<Node*>::reverse_iterator it = top->childs.rbegin();
            while(it != top->childs.rend())
            {
                    pre_stack.push(*it);
                    it++;
            }
    }
    return preOrder;
}

ComponentTree::Nodes ComponentTree::Node::getBreadthFirstNodes()
{
    vector<Node*> breadthFirst;
    deque<Node*> breadth_queue;
    Node* front;
    breadth_queue.push_back(this);
    while(!breadth_queue.empty())
    {
            front = breadth_queue.front();
            breadth_queue.pop_front();
            breadthFirst.push_back(front);
            vector<Node*>::iterator it = front->childs.begin();
            while(it != front->childs.end())
            {
                    breadth_queue.push_back(*it);
                    it++;
            }
    }
    return breadthFirst;
}

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
        m_numPixels = 0;
	m_numNodes = 0;
	m_numLeafs = 0;
}
ComponentTree::ComponentTree(char * imgfile, int _minSize, int _maxSize, int _singleSize)
{
	create(imgfile,_minSize,_maxSize,_singleSize);
}

/********************************************************************
 * ComponentTree : construct component tree by saved component tree file
 * load : m_numPixels, m_numNods, m_numLeafs, m_pixels, m_nodes, m_leafs
 *        m_root
 *        m_width, m_height, m_depth
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
 *        member variables : m_root, m_numPixels, m_pixels
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
        assert(_width * _height * _depth <= 2147483647); //don't use m_numPixels < 2147483648   (2^31)
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
        m_numPixels = _width * _height * _depth;
	
        m_pixels.resize(m_numPixels);
	for(int i = 0; i < m_numPixels; i++) 
	{
		m_pixels[i].pos = i;
		m_pixels[i].level = img[i];
		m_pixels[i].next = &m_pixels[i];
		m_pixels[i].node = NULL;
	}
        DisjointSets djs(m_numPixels);
	vector<Node*> lowestNode;
        lowestNode.resize(m_numPixels);
        for(int v = 0; v < m_numPixels ; v++)	lowestNode[v] = NULL;
	/*
	 * 3. sort img to get the visiting order
	 */
        vector<int> order = bucketSort(img,m_numPixels);
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
                // The following if/else will make sure curNode is not NULL
                if(curNode == NULL) // curPoint is unvisited, create a new node
		{
			curNode = new Node;
			//curNode->pixelNum = 1;
			curNode->alpha_size = 1;
                        curNode->beta_size = 1;
			curNode->parent = curNode;
			curNode->entry_pixel = &m_pixels[v];
			//m_pixels[v].parent = curNode;  // we should keep the entry's node
			curNode->lowest_level = img[v]; 
                        curNode->highest_alpha_level = img[v];
			lowestNode[curId] = curNode;
			/// Debug cout<<"create node : "<<curNode<<endl;
		}
                else  // curPoint is visited, we can get the node which contains it
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
				//if NULL, add pixel
				// just put it in the entry's link, other member doesn't change
                                if(adjNode == NULL) // "adjId unsivisited" => "img[u] <= img[v]" => "img[u] == img[v]"
				{
                                        //Pixel& entry = m_pixels[curNode->entry_pixel];
                                        //m_pixels[u].next = entry.next;
                                        //entry.next = u;
                                        curNode->entry_pixel->merge_entry(&m_pixels[u]);
					curNode->alpha_size++;
                                        curNode->beta_size++;
					//curNode->pixelNum++;
				}
                                else if(curNode == adjNode)
                                {
                                    // nothing happened here, maybe the adj point is visited earlier
                                }
                                //if different component, v is a connector between currentNode and adjNode
                                else if(curNode != adjNode)
				{
					/// Debug cout<<" adjNode = "<<adjNode<<" -> ";
					//If the same level
					//add adjacent point to current component
					//merge their childNode and delete adjNode
					if(curNode->lowest_level == adjNode->lowest_level)
					{
						curNode->merge_node(adjNode);
					}
                                        else //in different level, adj intensity "img[u]" > "img[v]"  cur intensity
					{
                                                //Three filter : MIN FILTER, MAX FILTER, SINGLE FILTER
                                                //1. Because the beta_size of adjNode is already fixed, we can filter it
                                                // if the size of adjNode is too small or too large, merge it to current Node
                                                if((_minSize > 0 && adjNode->beta_size < _minSize) || (_maxSize>=_minSize && adjNode->beta_size > _maxSize))
						{
							/// Debug cout<<"filter by min/max, merge different level->";
							curNode->merge_node(adjNode);
						}
                                                else// if(adjNode->beta_size >= _minSize && adjNode->beta_size <= _maxSize)
						{
							//2. filter the single node
							/// Debug cout<<" add child ->";
							curNode->childs.push_back(adjNode);
							adjNode->parent = curNode;
                                                        curNode->beta_size = curNode->beta_size + adjNode->beta_size;
							if(adjNode->childs.size() == 1)
							{
								/// Debug cout<<" merge single child ->";
                                                                //Node* childNode = adjNode->childs.front();
								/*
								 * if _singleSize = 1 means is_cut_single = false
								 * if _singleSize > maxSize of every single node  than is_cut_single = true
								 */
                                                                // The alpha_size if fixed, so we can check it here
                                                                if(adjNode->alpha_size  < _singleSize)
								{
                                                                    // add the (alpha) points of childNode to (alpha) points of curNode, instead of childNod of adjNode
								    curNode->merge_node(adjNode);
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
	//m_nodes, m_leafs,m_numLeafs,m_numNodes
	m_nodes = m_root->getPostOrderNodes();
	m_numNodes = m_nodes.size();

	for(int i = 0; i < m_numNodes; i++)
	{
		Node* node = m_nodes[i];
		node->label = i;
		if(node->childs.empty()) m_leafs.push_back(node);
		vector<Pixel*> pixels = node->alpha_pixels();
		vector<Pixel*>::iterator it = pixels.begin();
		while(it != pixels.end())
		{
			(*it)->node = node;
			it++;
		}
	}
	m_numLeafs = m_leafs.size();
	
	/*
	 * 6. set mean and paths
	 */
	
	//setStatistic(img);
	//setCenter();
	//setPaths();
	/*
	 * 7. free space
	 */
	delete img;
	return true;
}

/**************************************************************************
 * save : save these variables to file
 *        member variable: m_numPixels, m_numNodes, m_pixels, m_nodes
 *        global variable: m_width, m_height, m_depth
 **************************************************************************/

bool ComponentTree::save(const char* treefile) const
{
        assert(m_numPixels > 0);
	assert(m_numNodes > 0);
	ofstream ofs;
	bool saveType = true;
	string str_file(treefile);
	if(str_file.find("bin") != string::npos)
	{
		cout<<"save to binary file : "<<treefile<<endl;
		saveType = false;
		ofs.open(treefile, ios_base::out|ios_base::binary);
	}
	else if(str_file.find("txt") != string::npos)
	{
		cout<<"save to txt file : "<<treefile<<endl;
		saveType = true;
		ofs.open(treefile);
	}
	else 
	{
		cerr<<"Load Tree File: Cann't analysis file type."<<endl;
		cerr<<"Make sure .bin.tree file as binary and .txt.tree file as txt file"<<endl;
		return false;
	}
	if(ofs.fail()) 
	{
		ofs.close();
		return false;
	}
	bool rt = save(ofs, saveType);
	ofs.close();
	
	return rt;
}

bool ComponentTree::save(ofstream& ofs, bool saveType) const
{
	cout<<"save type = "<<saveType<<endl;
	assert(m_numPixels > 0);
	assert(m_numNodes > 0);
	if(saveType == 0) // binary format save
	{
		writeValue(ofs, m_width);
		writeValue(ofs, m_height);
		writeValue(ofs, m_depth);

		writeValue(ofs, m_minSize);
		writeValue(ofs, m_maxSize);
		writeValue(ofs, m_singleSize);

		writeValue(ofs, m_numPixels);
		writeValue(ofs, m_numNodes);
		writeValue(ofs, m_numLeafs);

		vector<Pixel>::const_iterator it = m_pixels.begin();
		while(it != m_pixels.end())
		{
			(*it).save(ofs, saveType);
			it++;
		}
		vector<Node*>::const_iterator itr = m_nodes.begin();
		while(itr != m_nodes.end())
		{
			(*itr)->save(ofs,saveType);
			itr++;
		}
		itr = m_leafs.begin();
		while(itr != m_leafs.end())
		{
			writeValue(ofs, (*itr)->label);
			itr++;
		}
		writeValue(ofs, m_root->label);

	}
	else  //txt format
	{
		ofs<<m_width<<" ";
		ofs<<m_height<<" ";
		ofs<<m_depth<<" ";
		ofs<<endl;
		ofs<<m_minSize<<" ";
		ofs<<m_maxSize<<" ";
		ofs<<m_singleSize<<" ";
		ofs<<endl;
		ofs<<m_numPixels<<" ";
		ofs<<m_numNodes<<" ";
		ofs<<m_numLeafs<<" ";
		ofs<<endl;
		vector<Pixel>::const_iterator it = m_pixels.begin();
		while(it != m_pixels.end())
		{
			(*it).save(ofs, saveType);
			it++;
		}
		vector<Node*>::const_iterator itr = m_nodes.begin();
		while(itr != m_nodes.end())
		{
			(*itr)->save(ofs,saveType);
			itr++;
		}
		itr = m_leafs.begin();
		while(itr != m_leafs.end())
		{
			ofs<<(*itr)->label<<" ";
			itr++;
		}
		ofs<<endl;
		ofs<<m_root->label<<endl;
	}
	if(ofs.good()) return true;
	else return false;
}

/*********************************************************************************
 * load : load the file to set many 
 *        member variable : m_numPixel, m_numNodes, m_root, m_pixels, m_nodes
 *        global variables : m_width, m_height, m_depth
 *
 * the same role as create(), but load will do one more thing, set 
 *                            m_leafs and m_numLeafs 
 **********************************************************************************/

bool ComponentTree::load(const char* treefile)
{
	ifstream ifs;
	bool saveType = true;
	string str_file(treefile);
	if(str_file.find("bin") != string::npos)
	{
		cout<<"load binary file: "<<treefile<<endl;
		saveType = false;
		ifs.open(treefile, ios_base::in|ios_base::binary);
	}
	else if(str_file.find("txt") != string::npos)
	{
		cout<<"load txt file: "<<treefile<<endl;
		saveType = true;
		ifs.open(treefile);
	}
	else 
	{
		cerr<<"Load Tree File: Cann't analysis file type."<<endl;
		cerr<<"Make sure .bin.tree file as binary and .txt.tree file as txt file"<<endl;
		return false;
	}
	if(ifs.fail())
	{
		ifs.close();
		return false;
	}
	bool rt = load(ifs, saveType);
	ifs.close();
	return rt;
}

bool ComponentTree::load(ifstream& ifs, bool saveType)
{
	if(saveType == 0)
	{
		readValue(ifs, m_width);
		readValue(ifs, m_height);
		readValue(ifs, m_depth);
		readValue(ifs, m_minSize);
		readValue(ifs, m_maxSize);
		readValue(ifs, m_singleSize);
		readValue(ifs, m_numPixels);
		readValue(ifs, m_numNodes);
		readValue(ifs, m_numLeafs);

		m_pixels.resize(m_numPixels);
		m_nodes.resize(m_numNodes);

		for(int i = 0; i < m_numNodes; i++) m_nodes[i] = new Node;

		m_leafs.resize(m_numLeafs);


		for(int i = 0; i < m_numPixels; i++)
		{
			m_pixels[i].load(ifs, m_pixels,m_nodes,saveType);
		}

		for(int i = 0; i < m_numNodes; i++)
		{
			m_nodes[i]->load(ifs, m_pixels,m_nodes,saveType);
		}
		for(int i = 0; i < m_numLeafs; i++)
		{
			int leaf_label;
			readValue(ifs, leaf_label);
			m_leafs[i] = m_nodes[leaf_label];
		}
		int root_label;
		readValue(ifs, root_label);
		m_root = m_nodes[root_label];

	}
	else
	{
		ifs>>m_width;
		ifs>>m_height;
		ifs>>m_depth;
		ifs>>m_minSize;
		ifs>>m_maxSize;
		ifs>>m_singleSize;
		ifs>>m_numPixels;
		ifs>>m_numNodes;
		ifs>>m_numLeafs;

		m_pixels.resize(m_numPixels);
		m_nodes.resize(m_numNodes);

		for(int i = 0; i < m_numNodes; i++) m_nodes[i] = new Node;

		m_leafs.resize(m_numLeafs);


		for(int i = 0; i < m_numPixels; i++)
		{
			m_pixels[i].load(ifs, m_pixels,m_nodes,saveType);
		}

		for(int i = 0; i < m_numNodes; i++)
		{
			m_nodes[i]->load(ifs, m_pixels,m_nodes,saveType);
		}
		for(int i = 0; i < m_numLeafs; i++)
		{
			int leaf_label;
			ifs>>leaf_label;
			m_leafs[i] = m_nodes[leaf_label];
		}
		int root_label;
		ifs>>root_label;
		m_root = m_nodes[root_label];
	}
	if(ifs.good()) return true;
	else return false;
}

void ComponentTree::clear()
{
	for(int i= 0; i< m_numNodes; i++)
	{
		Node* node = m_nodes[i];
		delete node;
	}
	m_pixels.clear();
	m_nodes.clear();
	m_leafs.clear();
	m_numLeafs = 0;
        m_numPixels = 0;
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

ComponentTree::Paths ComponentTree::getPaths() const
{
	vector<vector<int> > paths;
	Nodes::const_iterator it = m_leafs.begin();
	while(it!= m_leafs.end())
	{
		vector<int> path;
		Node* p = *it;
		while(p != p->parent)
		{
			path.push_back(p->label);
			p = p->parent;
		}
		// p == p->parent
		path.push_back(p->label);
		paths.push_back(path);
		it++;
	}
	return paths;
}

/*****************************************
 * setStatistic : set the Mean and std of each component
 * 
 * This work should be down after the post store of each node
 *****************************************/

//void ComponentTree::setStatistic(unsigned char* img)
//{
//}

/********************************************
 * setCenter : set the center of each node
 ********************************************/
//void ComponentTree::setCenter()
//{
//}

int ComponentTree::nodeNum() const
{
	return m_numNodes;
}

int ComponentTree::pixelNum() const
{
        return m_numPixels;
}

int ComponentTree::leafNum() const
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
	int _selfSize = node->alpha_size;
	cout.precision(3);
	for(Nodes::iterator it = node->childs.begin(); it != node->childs.end(); it++)
	{
		_selfSize -= (*it)->alpha_size;
	}
	if(node->childs.empty())
	{
		cout<<label<<" : "<<node->lowest_level<<" "<<node->alpha_size<<" "<<_selfSize<<"  leaf"<<endl;
		return;
	}
	else
	{
		cout<<label<<" : "<<node->lowest_level<<" "<<node->alpha_size<<" "<<_selfSize<<endl;
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
	cout<<"label : lowest_level totalsize pixelSize"<<endl;
	printTreeRecursively(label,0);
	cout<<endl;
	cout<<"width = "<<m_width<<"\theight = "<<m_height<<"\tdepth = "<<m_depth<<endl;
	cout<<"minSize = "<<m_minSize<<"\tmaxSize = "<<m_maxSize<<"\tsingleSize = "<<m_singleSize<<endl;
	cout<<"total node : "<<m_numNodes<<"\tleaf node : "<<m_numLeafs<<endl;
	cout<<"------------------------------------------------------------"<<endl;
	cout<<"pre order : ";
	vector<Node*> pre_nodes = this->root()->getPreOrderNodes();
	vector<Node*>::iterator it = pre_nodes.begin();
	while(it != pre_nodes.end()) 
	{
		cout<<(*it)->label<<" ";
		it++;
	}
	cout<<endl;
	
	cout<<"post order : ";
	vector<Node*> post_nodes = this->root()->getPostOrderNodes();
	it = post_nodes.begin();
	while(it != post_nodes.end())
	{
		cout<<(*it)->label<<" ";
		it++;
	}
	cout<<endl;
	cout<<"breadth first : ";
	vector<Node*> breadth_nodes = this->root()->getBreadthFirstNodes();
	it = breadth_nodes.begin();
	while(it != breadth_nodes.end())
	{
		cout<<(*it)->label<<" ";
		it++;
	}
	cout<<endl;
	 
}

/****************************************************************************
 * printReverseAlphaMapping : print each point's label
 *
 * used function : label(vertex)
 ****************************************************************************/
int* ComponentTree::getReverseAlphaMapping() const
{
        int* matrix = new int[m_numPixels];
	for(int i = 0; i < m_numPixels; i++)
	{
		matrix[i] = m_pixels[i].node->label;
	}
	return matrix;
}

/********************************************************************
 * getMatrix : get the vertices with label in labels
 * intput : labels ,  values, initial value
 * output : a int* matrix, the vertice will be assign with 
 *          the pixel of values
 ********************************************************************/
int* ComponentTree::getMatrix(vector<int> labels, vector<int> values ,int ini) const
{
        int* matrix = new int[m_numPixels];
        //memset(matrix,0,m_numPixels);
        for(int i = 0; i< m_numPixels ;i++)
	{
		matrix[i] = ini;
	}
	int label= -1 ;
	int value = -1;
	for(int i = 0; i < (int)labels.size(); i++)
	{
		label = labels[i];             //the label will start from 0
		value = values[i];		
		Node* node = m_nodes[label];
		vector<int> points = node->beta_points();
		vector<int>::iterator it = points.begin();
		while(it != points.end())
		{
			matrix[*it] = value;
			it++;
		}
	}
	return matrix;
}


void ComponentTree::printReverseAlphaMapping() const
{
	int* matrix = getReverseAlphaMapping();
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

        while(i < m_numPixels)
	{
		char out[10];
		sprintf(out,format,matrix[i]);
		cout<<out;
		if(i%(m_width * m_height) == 0 && i/(m_width * m_height) > 0) 
			cout<<"layer "<<i/(m_width * m_height)+1<<": "<<endl;
		if((i+1)%m_width == 0)cout<<endl;
		i++;
	}
	cout<<endl;
	cout<<"width = "<<m_width<<"\t height = "<<m_height<<"\t depth = "<<m_depth<<endl<<endl;
	cout<<"label count : "<<m_numNodes<<endl;
	cout<<"----------------------------------------------------------------------"<<endl;
	delete matrix;
	
}


/*************************************************************
 * printPaths : print paths
 *************************************************************/
void ComponentTree::printPaths() const
{
	cout<<"---------------------- All Paths -----------------------"<<endl;
	Paths paths = this->getPaths();
	Paths::const_iterator it;
	int i = 1;
	for(it = paths.begin(); it != paths.end(); it++)
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


DisjointSets::DisjointSets()
{
        m_numPixels = 0;
	m_numSets = 0;
}

DisjointSets::DisjointSets(int count)
{
        m_numPixels = 0;
	m_numSets = 0;
	AddPixels(count);
}

DisjointSets::DisjointSets(const DisjointSets & s)
{
        this->m_numPixels = s.m_numPixels;
	this->m_numSets = s.m_numSets;
	
	// Copy nodes
        m_nodes.resize(m_numPixels);
        for(int i = 0; i < m_numPixels; ++i)
		m_nodes[i] = new Node(*s.m_nodes[i]);
	
	// Update parent pointers to point to newly created nodes rather than the old ones
        for(int i = 0; i < m_numPixels; ++i)
		if(m_nodes[i]->parent != NULL)
			m_nodes[i]->parent = m_nodes[s.m_nodes[i]->parent->index];
}

DisjointSets::~DisjointSets()
{
        for(int i = 0; i < m_numPixels; ++i)
		delete m_nodes[i];
	m_nodes.clear();
        m_numPixels = 0;
	m_numSets = 0;
}

// Note: some internal data is modified for optimization even though this method is consant.
int DisjointSets::FindSet(int pixelId) const
{
        assert(pixelId < m_numPixels);
	
	Node* curNode;
	
	// Find the root pixel that represents the set which `pixelId` belongs to
	curNode = m_nodes[pixelId];
	while(curNode->parent != NULL)
		curNode = curNode->parent;
	Node* root = curNode;
	
	// Walk to the root, updating the parents of `pixelId`. Make those pixels the direct
	// children of `root`. This optimizes the tree for future FindSet invokations.
	curNode = m_nodes[pixelId];
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
        assert(setId1 < m_numPixels);
        assert(setId2 < m_numPixels);
	
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

void DisjointSets::AddPixels(int numToAdd)
{
	assert(numToAdd >= 0);
	
	// insert and initialize the specified number of pixel nodes to the end of the `m_nodes` array
	m_nodes.insert(m_nodes.end(), numToAdd, (Node*)NULL);
        for(int i = m_numPixels; i < m_numPixels + numToAdd; ++i)
	{
		m_nodes[i] = new Node();
		m_nodes[i]->parent = NULL;
		m_nodes[i]->index = i;
		m_nodes[i]->rank = 0;
	}
	
	// update pixel and set counts
        m_numPixels += numToAdd;
	m_numSets += numToAdd;
}

int DisjointSets::NumPixels() const
{
        return m_numPixels;
}

int DisjointSets::NumSets() const
{
	return m_numSets;
}

