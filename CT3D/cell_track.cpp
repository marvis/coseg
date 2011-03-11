#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "lp_lib.h"

#include "../component_tree.h"
#include "cell_track.h"
#include "../myalgorithms.h"

using namespace std;
typedef ComponentTree::Node TNode;

/***************************************************************************
 * CellTrack construction function
 **************************************************************************/
CellTrack::CellTrack()
{
}

CellTrack::~CellTrack()
{
	this->releaseFrames();
	this->releaseTracks();
}

bool CellTrack::save(char* track_file)
{
	assert(!m_frames.empty());
	string str_file(track_file);

	bool saveType = 1;
	ofstream ofs;
	if(str_file.find(".bin") != string::npos)
	{
		saveType = 0;
		ofs.open(str_file.c_str(), ios_base::out|ios_base::binary);
	}
	else if(str_file.find(".txt") != string::npos)
	{
		saveType = 1;
		ofs.open(str_file.c_str());
	}
	if(save(ofs, saveType))
	{
		ofs.close();
		return true;
	}
	else
	{
		ofs.close();
		return false;
	}
}

bool CellTrack::save(ofstream & ofs, bool saveType)
{
	if(saveType == 0) // binary
	{
	}
	else
	{
	}
	if(ofs.good()) return true;
	else return false;
}

bool CellTrack::load(char* track_file)
{
	assert(m_frames.empty());
	ifstream ifs;
	bool saveType = true;
	string str_file(track_file);
	if(str_file.find(".bin") != string::npos)
	{
		cout<<"load binary file: "<<track_file<<endl;
		saveType = false;
		ifs.open(track_file, ios_base::in|ios_base::binary);
	}
	else if(str_file.find(".txt") != string::npos)
	{
		cout<<"load txt file: "<<track_file<<endl;
		saveType = true;
		ifs.open(track_file);
	}
	else
	{
		cerr<<"Load Tree File: Cann't analysis file type."<<endl;
		cerr<<"Make sure .bin.tree file as binary and .txt.tree file as txt file"<<endl;
		return false;
	}
	bool rt = load(ifs, saveType);
	ifs.close();
	return rt;
}

bool CellTrack::load(ifstream& ifs, bool saveType)
{
	if(saveType == 0) // binary
	{
	}
	else
	{
	}
	if(ifs.good()) return true;
	else return false;
}

bool CellTrack::reload(char* track_file)
{
	this->releaseAllCells();
	this->releaseFrames();
	this->releaseTracks();
	return load(track_file);
}

bool CellTrack::createFromImages(vector<char*> img_files)
{
	assert(m_frames.empty());
	return false;
}

bool CellTrack::createFromTrees(vector<char*> tree_files)
{
	assert(m_frames.empty());
	return createFramesFromTrees(tree_files, m_frames);
}

void CellTrack::exportImages(char* prefix) const
{
	vector<Frame*>::const_iterator it = m_frames.begin();
	int id = 0;
	while(it != m_frames.end())
	{
		id++;
		ostringstream oss(ostringstream::out);
		oss << prefix;
		oss << id;
		oss << ".tiff";
		//(*it)->exportImage(m_width,m_height,m_depth, (char*) oss.str().c_str());
		(*it)->exportImage(156, 250, 34, (char*) oss.str().c_str());
		it++;
	}
}

CellTrack::Frame* CellTrack::getFrame(int time) const
{
	assert(! m_frames.empty());
	assert(time < this->frameNum());
	return m_frames[time];
}

CellTrack::Track* CellTrack::getTrack(int index) const
{
	assert(! m_tracks.empty());
	assert(index < this->trackNum());
	return m_tracks[index];
}

int CellTrack::frameNum() const
{
	return m_frames.size();
}

int CellTrack::trackNum() const
{
	return m_tracks.size();
}

void CellTrack::releaseFrames()
{
}

void CellTrack::releaseTracks()
{
}

void CellTrack::releaseAllCells()
{
}

bool CellTrack::createFramesFromTrees(ComponentTree* tree1, ComponentTree* tree2,vector<CellTrack::Frame*> &frames)
{
	assert(frames.empty());

	if(tree1->width() != tree2->width() || tree1->height() != tree2->height() || tree1->depth() != tree2->depth())
	{
		cerr<<"The two trees with different size. Unalbe to align."<<endl;
		return false;
	}

	// 1. get weights
	vector<float> weights;
	tree1->setWeightMatrix(tree2, weights);
	int numVars1 = (int)tree1->nodeNum();
	int numVars2 = (int)tree2->nodeNum();
	assert((int)weights.size() == numVars1 * numVars2);

	vector<vector<int> > paths1 = tree1->getPaths();
	vector<vector<int> > paths2 = tree2->getPaths();

	// 2. linear model
	lprec *lp;
	int Ncol, *colno=NULL, k;       
	REAL * row = NULL;
	int i=0,j=0;
	Ncol = numVars1 * numVars2; 
	lp = make_lp(0,Ncol);           
	if(lp == NULL) return false;    

	colno = (int *) malloc(Ncol * sizeof(*colno));
	row = (REAL *) malloc(Ncol * sizeof(*row));
	if((colno == NULL) || (row == NULL)) return false;
	for(i=0;i< Ncol;i++) row[i]=1.0; // assign all the content of row as 1

	set_add_rowmode(lp,TRUE);
	set_binary(lp,Ncol,TRUE);

	// add paths1 constraints
	// one path one constraint

	ComponentTree::Paths::iterator it=paths1.begin();
	while(it != paths1.end())
	{
		k=0;
		ComponentTree::Path::iterator itr = (*it).begin();
		while(itr != (*it).end())
		{
			i = (*itr);
			for(j=0;j<numVars2;j++)colno[k++] = i*numVars2+j+1;
			itr++;
		}
		if(!add_constraintex(lp, k, row, colno, LE, 1))
			return false;
		it++;
	}
	// add paths2 constraints
	it=paths2.begin();
	while (it != paths2.end())
	{
		ComponentTree::Path::iterator itr = (*it).begin();
		k=0;
		while(itr != (*it).end())
		{
			j = (*itr);
			for(i=0;i<numVars1;i++)colno[k++] = i*numVars2+j+1;
			itr++;
		}
		if(!add_constraintex(lp, k, row, colno, LE, 1))
			return false;
		it++;
	}
	set_add_rowmode(lp,FALSE);	

	// add the object
	k=0;
	for(i=0;i< numVars1; i++)
		for(j=0; j< numVars2; j++)
		{
			colno[k] = i*numVars2+j+1;
			row[k++] = weights[i * numVars2 + j];
		}
	if(!set_obj_fnex(lp, k, row, colno))return false;
	set_maxim(lp);
	set_verbose(lp,IMPORTANT);

	// 3. solve the linear problem
	if(::solve(lp) != OPTIMAL)
	{
		cout<<"Not optimized results"<<endl;
		return false;
	}
	// 4. save results to row
	get_variables(lp,row);
	Frame* frame1 = new Frame();
	Frame* frame2 = new Frame();
	frame1->setTree(tree1);
	frame2->setTree(tree2);

	for(int j = 0; j < numVars2; j++)
	{
		for(i = 0; i < numVars1; i++)
		{
			if(fabs(row[i * numVars2 + j] - 1.0) < 0.001)
			{
				Cell* cell1 = new Cell;
				Cell* cell2 = new Cell;
				cell1->setSecNode(tree1->getNode(i));
				cell2->setFirNode(tree2->getNode(j));
				cell1->setNextCell(cell2);
				cell2->setPrevCell(cell1); 
				frame1->addCell(cell1);
				frame2->addCell(cell2);
			}
		}
	}
	frames.push_back(frame1);
	frames.push_back(frame2);

	if(colno != NULL) free(colno);
	if(row != NULL) free(row);
	if(lp != NULL) delete_lp(lp);

	return true;

}

bool CellTrack::createFramesFromTrees(vector<char*> tree_files,vector<CellTrack::Frame*> &frames)
{
	assert(frames.empty());
	assert(tree_files.size() >= 2);
	if(tree_files.size() == 2)
	{
		ComponentTree* tree1 = new ComponentTree(tree_files[0]);
		ComponentTree* tree2 = new ComponentTree(tree_files[1]);
		bool rt = createFramesFromTrees(tree1, tree2, frames);
		assert(frames.size() == 2);
		Frame* frame1 = frames[0];
		Frame* frame2 = frames[1];
		frame1->setTreeFile(tree_files[0]);
		//frame1->releaseTree();
		frame2->setTreeFile(tree_files[1]);
		//frame2->releaseTree();
		return rt;
	}

	frames.resize(tree_files.size());
	int numFrames = frames.size();

	Frame* prev_frame = NULL;
	ComponentTree* first_tree = NULL;
	ComponentTree* second_tree = NULL;
	first_tree = new ComponentTree(tree_files[0]);
	int i = 0;
	for(; i < numFrames-1; i++)
	{
		pair<Frame*, Frame*> frame_pair;
		second_tree = new ComponentTree(tree_files[i+1]);
		vector<Frame*> two_frames;
		if(!createFramesFromTrees(first_tree, second_tree, two_frames))
		{
			cerr<<"Unable to produce frames from two trees"<<endl;
			return false;
		}

		first_tree = second_tree;

		frames[i] = two_frames[0];
		frames[i]->mergePrevFrame(prev_frame);
		frames[i]->releaseTree();          // if space is not enough
		prev_frame = two_frames[1];
	}
	frames[i] = prev_frame;
	frames[i]->releaseTree();
	return true;
}

bool CellTrack::createFramesFromImages(vector<char*> img_files, vector<CellTrack::Frame*> &frames)
{
	return false;
}


bool CellTrack::createTracksFromFrames(CellTrack::Frames& frames, vector<CellTrack::Track*> &tracks)
{
	return false;
}
/**************************************************************************
 * static functions
 *************************************************************************/
CellTrack::Cell::Cell()
{
	m_fir_node_label = -1;
	m_sec_node_label = -1;
	m_cur_node_label = -1;
	m_mod_node_label = -1;

	m_prev_cell = NULL;
	m_next_cell = NULL;

	m_track = NULL;
}

CellTrack::Track* CellTrack::Cell::getTrack() const
{
	return m_track;
}

void CellTrack::Cell::setTrack(CellTrack::Track* track)
{
	assert(track != NULL);
	m_track = track;
}

void CellTrack::Cell::draw() const
{
}

void CellTrack::Cell::drawCenter() const
{
}

vector<int> CellTrack::Cell::getCenterArea() const
{
	vector<int> area;
	return area;
}

void CellTrack::Cell::setCenterArea()
{
}

TNode* CellTrack::Cell::getFirNode(ComponentTree* tree) const
{
	return tree->getNode(m_fir_node_label);
}

void CellTrack::Cell::setFirNode(TNode* node)
{
	m_fir_node_label = node->getLabel();
}

TNode* CellTrack::Cell::getSecNode(ComponentTree* tree) const
{
	return tree->getNode(m_sec_node_label);
}

void CellTrack::Cell::setSecNode(TNode* node)
{
	m_sec_node_label = node->getLabel();
}

TNode* CellTrack::Cell::getCurNode(ComponentTree* tree) const
{
	return tree->getNode(m_cur_node_label);
}

void CellTrack::Cell::setCurNode(TNode* node)
{
	m_cur_node_label = node->getLabel();
}

TNode* CellTrack::Cell::getModNode(ComponentTree* tree) const
{
	return tree->getNode(m_mod_node_label);
}

void CellTrack::Cell::setModNode(TNode* node)
{
	m_mod_node_label = node->getLabel();
}

TNode* CellTrack::Cell::getNode(ComponentTree* tree) const
{
	if(m_mod_node_label != -1) return tree->getNode(m_mod_node_label);
	else return tree->getNode(m_cur_node_label);
}

CellTrack::Cell* CellTrack::Cell::getPrevCell() const
{
	return m_prev_cell;
}

void CellTrack::Cell::setPrevCell(CellTrack::Cell* prev_cell)
{
	m_prev_cell = prev_cell;
}

CellTrack::Cell* CellTrack::Cell::getNextCell() const
{
	return m_next_cell;
}

void CellTrack::Cell::setNextCell(CellTrack::Cell* next_cell)
{
	m_next_cell = next_cell;
}

vector<int> CellTrack::Cell::getVertices() const
{
	return this->m_vertices;
}

/***************************************************************************
 * CellTrack::Frame
 * *************************************************************************/
CellTrack::Frame::Frame()
{
	m_tree = NULL;
	m_tree_file = "";
}

void CellTrack::Frame::exportImage(int width, int height, int depth, char* img_file)
{
	cout<<"output image "<<img_file<<endl;
	this->loadVertices(this->getTree());
	srand(time(NULL));
	int size = width * height * depth;
	unsigned char* img = new unsigned char[size*3];
	for(int i = 0; i < 3*size; i++) img[i] = 0;
	vector<Cell*>::iterator it = m_cells.begin();
	while(it != m_cells.end())
	{
		vector<int> vertices = (*it)->getVertices();
		unsigned char r = rand()%256;
		unsigned char g = rand()%256;
		unsigned char b = rand()%256;
		vector<int>::iterator itr = vertices.begin();
		while(itr != vertices.end())
		{
			img[(*itr)*3] = r;
			img[(*itr)*3 + 1] = g;
			img[(*itr)*3 + 2] = b;
			itr++;
		}
		it++;
	}
	writetiff(img_file, img, 3, width, height, depth);
}

void CellTrack::Frame::addCell(CellTrack::Cell* cell)
{
	m_cells.push_back(cell);
}

bool CellTrack::Frame::createFromImage(char* img_file)
{
	return true;
}

void CellTrack::Frame::mergePrevFrame(CellTrack::Frame* prev_frame)
{
}

int CellTrack::Frame::cellNum() const
{
	return m_cells.size();
}

ComponentTree* CellTrack::Frame::getTree()
{
	if(m_tree != NULL) return m_tree;
	else if(m_tree_file.empty()) return new ComponentTree((char*) m_tree_file.c_str());
	else 
	{
		cerr<<"Frame::getTree() unable to get component tree"<<endl;
		return NULL;
	}
}

void CellTrack::Frame::setTree(ComponentTree* tree)
{
	assert(tree != NULL);
	m_tree = tree;
}

void CellTrack::Frame::setTreeFile(char* tree_file)
{
	m_tree_file = tree_file;
}

void CellTrack::Frame::releaseTree()
{
	if(m_tree != NULL) 
	{
		m_tree->clear();
		m_tree = NULL;
	}
}

void CellTrack::Frame::releaseVertices()
{
	// if load from tree files
	if(m_tree != NULL || !m_tree_file.empty())
	{
		vector<Cell*>::iterator it = m_cells.begin();
		while(it != m_cells.end())
		{
			(*it)->m_vertices.clear();
			it++;
		}
	}
}

void CellTrack::Frame::loadVertices(ComponentTree* tree)
{
	vector<Cell*>::iterator it = m_cells.begin();
	while(it != m_cells.end())
	{
		if((*it)->m_vertices.empty())
		{
			(*it)->m_vertices = (*it)->getNode(tree)->getBetaPoints();
		}
		it++;
	}
}

CellTrack::Track::Track()
{
}

CellTrack::Cell* CellTrack::Track::getStart() const
{
	return m_cells[0]; 
}

CellTrack::Cell* CellTrack::Track::getEnd() const
{
	return *(m_cells.rbegin());
}

void CellTrack::Track::addNext(Cell* cell)
{
	m_cells.push_back(cell);
}

vector<CellTrack::Cell*> CellTrack::Track::getCells() const
{
	return m_cells;
}

int CellTrack::Track::cellNum() const
{
	return m_cells.size();
}


