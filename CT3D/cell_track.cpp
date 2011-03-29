//
//=======================================================================
// Copyright 2011 Institute PICB.
// Authors: Hang Xiao
// Data : March 20, 2011
//=======================================================================
//


#include "cell_track.h"
#include "palette.h"
#include "../component_tree.h"
#include "../myalgorithms.h"

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "lp_lib.h"

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
	assert(!img_files.empty());
	if(! createFramesFromImages(img_files, m_frames))
	{
		cerr<<"unable to create frames from images "<<endl;
		return false;
	}
	if(! createTracksFromFrames(m_frames, m_tracks))
	{
		cerr<<"unable to create tracks from frames"<<endl;
		return false;
	}
	//setTracksColor(); // the color of Tracks is alread set
	this->m_img_files = img_files;
	return true;
}

bool CellTrack::createFromTrees(vector<char*> tree_files)
{
	assert(m_frames.empty());
	if(!createFramesFromTrees(tree_files, m_frames))
	{
		cerr<<"unable to create frames from trees "<<endl;
		return false;
	}
	if(!createTracksFromFrames(m_frames, m_tracks))
	{
		cerr<<"unable to create tracks from frames"<<endl;
		return false;
	}
	setTracksColor();
	m_tree_files = tree_files;
	return true;
}

void CellTrack::setTracksColor()
{
	assert(this->m_tracks.size() > 0);
	Palette palette(m_tracks.size());
	vector<Track*>::iterator it = this->m_tracks.begin();
	int i = 0;
	while(it != m_tracks.end())
	{
		unsigned int r = palette(i).r;
		unsigned int g = palette(i).g;
		unsigned int b = palette(i).b;
		int color = r + g * 256 + b * 256 * 256; 
		(*it)->setColor(color);
		i++;
		it++;
	}
}

void CellTrack::exportImages(char* prefix) const
{
	vector<Frame*>::const_iterator it = m_frames.begin();
	int id = 0;
	//int color_num = this->trackNum();
	//Palette palette(color_num);
	//setTrackColor();
	while(it != m_frames.end())
	{
		id++;
		ostringstream oss(ostringstream::out);
		if(prefix != NULL) oss << prefix;
		oss << id;
		oss << ".tiff";
		(*it)->exportImage((char*) oss.str().c_str()/*, palette*/);
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
	/*
	for(int i = 0; i < numVars1; i++)
	{
		for(int j = 0; j < numVars2; j++)
		{
			cout<<weights[i*numVars2 + j]<<" ";
		}
		cout<<endl;
	}
*/
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
			colno[k] = i*numVars2+j+1; //todo: why i*numVar2 + j + 1
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

	for(i = 0; i < numVars1; i++)
	{
		for(int j = 0; j < numVars2; j++)
		{
			if(fabs(row[i * numVars2 + j] - 1.0) < 0.1)
			{
				Cell* cell1 = new Cell;
				Cell* cell2 = new Cell;
				cell1->setCurNode(tree1->getNode(i));
				cell2->setCurNode(tree2->getNode(j));
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
		frame1->releaseTree(tree_files[0]);
		frame2->releaseTree(tree_files[1]);
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
		frames[i]->releaseTree(tree_files[i]);          // if space is not enough
		prev_frame = two_frames[1];
	}
	frames[i] = prev_frame;
	frames[i]->releaseTree(tree_files[i]);
	return true;
}

bool CellTrack::createFramesFromImages(vector<char*> img_files, vector<CellTrack::Frame*> &frames)
{
	assert(!img_files.empty());
	assert(frames.empty());
	int frame_num = img_files.size();
	frames.resize(frame_num);
	map<unsigned int, Cell*> map_cell;
	for(int i = 0;i < frame_num; i++)
	{
		frames[i] = new Frame;
		map_cell = frames[i]->createFromImage(img_files[i], map_cell);
	}
	return true;
}


bool CellTrack::createTracksFromFrames(CellTrack::Frames& frames, vector<CellTrack::Track*> &tracks, int start_time)
{
	assert(!frames.empty());
	assert(tracks.empty());
	vector<Frame*>::iterator it = frames.begin();
	int t = start_time;
	//int color_id = 0;
	while(it != frames.end())
	{
		vector<Cell*> cells = (*it)->getCells();
		vector<Cell*>::iterator itr = cells.begin();
		while(itr != cells.end())
		{
			if((*itr)->getPrevCell() == NULL)
			{
				Track* track = new Track;
				//track->m_start_time = t;
				track->m_entry_cell = (*itr);
				//track->m_color_id = color_id++;
				track->m_color = (*itr)->m_color;
				(*itr)->m_track = track;
				tracks.push_back(track);
			}
			else
			{
				(*itr)->m_track = (*itr)->getPrevCell()->getTrack();
			}
			itr++;
		}
		it++;
		t++;
	}
	return true;
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
	m_vertices.clear();

	m_color = 0;
}

CellTrack::Track* CellTrack::Cell::getTrack() const
{
	if(m_track == NULL)
	{
		cerr<<"Cell::getTrack null track"<<endl;
	}
	return m_track;
}

void CellTrack::Cell::setTrack(CellTrack::Track* track)
{
	assert(track != NULL);
	m_track = track;
}

void CellTrack::Cell::draw(unsigned char* image, int w, int h, int d) const
{
}

void CellTrack::Cell::drawMarker(unsigned char* image, int w, int h, int d) const
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
	else if(m_cur_node_label != -1) return tree->getNode(m_cur_node_label);
	else if(m_sec_node_label != -1 || m_fir_node_label != -1)
	{
		if(m_fir_node_label != -1) return tree->getNode(m_fir_node_label);
		if(m_sec_node_label != -1) return tree->getNode(m_sec_node_label);
	}
	cerr<<"Cell::getNode error, no effective node label"<<endl;
	return NULL;
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

vector<int> CellTrack::Cell::getVertices(ComponentTree* tree) const
{
	if(!m_vertices.empty()) return m_vertices;
	if(tree != NULL) return this->getNode(tree)->getBetaPoints();
	else
	{
		//cerr<<"Cell::getVertices: Unable to get vertices"<<endl;
		return m_vertices;
	}
}

int CellTrack::Cell::getCenter(int w, int h, int d) const
{
	vector<int> vertices = getVertices();
	vector<int>::iterator it = vertices.begin();
	int mean_w = vertices.size()/2;
	int mean_h = vertices.size()/2;
	int mean_d = vertices.size()/2;
	while(it != vertices.end())
	{
		mean_w += (*it) % w;
		mean_h += (*it/w) % h;
		mean_d += (*it/w/h) % d;
		it++;
	}
	mean_w = mean_w / vertices.size();
	mean_h = mean_h / vertices.size();
	mean_d = mean_d / vertices.size();
	
	return mean_d * w * h + mean_h * w + mean_w;
}

/***************************************************************************
 * CellTrack::Frame
 * *************************************************************************/
CellTrack::Frame::Frame()
{
	m_tree = NULL;
	m_tree_file = "";
	m_width = -1;
	m_height = -1;
	m_depth = -1;
}

void CellTrack::Frame::exportImage(char* img_file/*, Palette& palette*/)
{
	int width = this->width();
	int height = this->height();
	int depth = this->depth();
	cout<<"output image "<<img_file<<" with ";
	cout<<this->cellNum()<<" cells"<<endl;
	//this->setVertices();
	//srand(time(NULL));
	int size = width * height * depth;
	unsigned char* img = new unsigned char[size*3];
	for(int i = 0; i < 3*size; i++) img[i] = 0;
	vector<Cell*>::iterator it = m_cells.begin();
	while(it != m_cells.end())
	{
		vector<int> vertices = (*it)->getVertices(this->getTree());
		assert(!vertices.empty());
		//int color_id = (*it)->getTrack()->getColorId();
		//unsigned char r = palette(color_id).r; //rand() % 256;
		//unsigned char g = palette(color_id).g; //rand() % 256;
		//unsigned char b = palette(color_id).b; //rand() % 256;
		unsigned int color = (*it)->getTrack()->getColor();
		unsigned char r = color % 256;
		unsigned char g = (color/256)%256;
		unsigned char b = (color/256/256)%256;
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
	delete img;
}

void CellTrack::Frame::addCell(CellTrack::Cell* cell)
{
	m_cells.push_back(cell);
}

map<unsigned int, CellTrack::Cell*> CellTrack::Frame::createFromImage(char* img_file, map<unsigned int, CellTrack::Cell*> &prev_map_cell)
{
	int width = 0;
	int height = 0;
	int depth = 0;
	int channels = 0;
	unsigned char* img = readtiff(img_file, &width, &height, &depth, &channels);
	assert(width >= 1);
	assert(height >= 1);
	assert(depth >= 1);
	assert(channels == 3);
	m_width = width;
	m_height = height;
	m_depth = depth;
	int img_size = width * height * depth;
	
	map<unsigned int, Cell*> map_cell;
	for(int i = 0; i < img_size; i++)
	{
		unsigned int color = img[3*i] + img[3*i + 1] * 256 + img[3*i + 2] * 256 * 256 ;
		if(color == 0) continue;
		Cell* cell = NULL;
		if(map_cell.find(color) != map_cell.end())
		{
			cell = map_cell[color];
			cell->m_vertices.push_back(i);
		}
		else
		{
			cell = new Cell;
			cell->m_vertices.push_back(i);
			cell->m_color = color;

			map_cell[color] = cell;
			this->m_cells.push_back(cell);

			if(prev_map_cell.find(color) != prev_map_cell.end())
			{
				Cell* prev_cell = prev_map_cell[color];
				cell->m_prev_cell = prev_cell;
				prev_cell->m_next_cell = cell;
			}
		}
	}
	delete img;
	return map_cell;
}

void CellTrack::Frame::mergePrevFrame(CellTrack::Frame* prev_frame)
{
	if(prev_frame == NULL) return;
	assert(prev_frame->getTree() == this->getTree());
	ComponentTree* tree = getTree();
	int size = this->width() * this->height() * this->depth();
	vector<int> matrix1(size, -1);
	vector<int> matrix2(size, -1);
	vector<Cell*> cells1 = prev_frame->m_cells;
;
	vector<Cell*> cells2 = this->m_cells;
	vector<Cell*>::iterator it = cells1.begin();
	int id = 0;
	while(it != cells1.end())
	{
		vector<int> vertices = (*it)->getVertices(tree);
        vector<int>::iterator itr = vertices.begin();
        while(itr != vertices.end())
        {
            matrix1[*itr] = id;
            itr++;
        }
		id++;
		it++;
	}
	it = cells2.begin();
	id = 0;
	while(it != cells2.end())
	{
		vector<int> vertices = (*it)->getVertices(tree);
        vector<int>::iterator itr = vertices.begin();
        while(itr != vertices.end())
        {
            matrix2[*itr] = id;
            itr++;
        }
		id++;
		it++;
	}
	int cells1Num = cells1.size();
	int cells2Num = cells2.size();
	vector<bool> weight(cells1Num*cells2Num, 0);
	for(int i = 0; i < size; i++)
	{
		int id1 = matrix1[i];
		int id2 = matrix2[i];
		if(id1 != -1 && id2 != -1) weight[id1*cells2Num + id2] = true;
	}
	for(int i = 0; i < cells1Num; i++)
	{
		int sum = 0;
		int the_j = -1;
		for(int j = 0; j < cells2Num; j++) 
		{
			sum += weight[i*cells2Num + j];
			if(weight[i*cells2Num + j] && the_j == -1) the_j = j;
		}
		assert(sum <= 1);
		if(sum)
		{
			assert(tree != NULL);
			cells2[the_j]->setFirNode(cells1[i]->getCurNode(tree));
			cells2[the_j]->setSecNode(cells2[the_j]->getCurNode(tree));	
			int cur_label = cells1[i]->m_cur_node_label > cells2[the_j]->m_cur_node_label ? cells1[i]->m_cur_node_label:  cells2[the_j]->m_cur_node_label;
			cells2[the_j]->setCurNode(tree->getNode(cur_label));
			Cell* prev_cell = cells1[i]->getPrevCell();
			cells2[the_j]->setPrevCell(prev_cell);
			prev_cell->setNextCell(cells2[the_j]);
			delete cells1[i];
		}
		else
		{
			m_cells.push_back(cells1[i]);
		}
	}
	delete prev_frame;
}

/***********************************************************************
 * CellTrack::Frame::getReverseAlphaMappint(ComponentTree* tree)
 * us m_cur_node_label to fill matrix
 ***********************************************************************/
/*vector<int> CellTrack::Frame::getReverseAlphaMapping()
{	
	ComponentTree* tree = this->getTree();
	int size = tree->pixelNum();
	vector<int> matrix(size, -1);
	vector<Cell*>::iterator it = m_cells.begin();
	while(it != m_cells.end())
	{
		//int label = (*it)->getCurNode(tree)->getLabel();
		int label = (*it)->m_cur_node_label;
		vector<int> vertices;
		if((*it)->m_vertices.empty()) vertices =  (*it)->getNode(tree)->getBetaPoints();
		else vertices = (*it)->getVertices();
		vector<int>::iterator itr = vertices.begin();
		while(itr != vertices.end())
		{
			matrix[*itr] = label;
			itr++;
		}
		it++;
	}
	return matrix;
}
*/
ComponentTree* CellTrack::Frame::getTree()
{
	if(m_tree != NULL) return m_tree;
	else if(!m_tree_file.empty())
	{
		m_tree = new ComponentTree((char*) m_tree_file.c_str());
		return m_tree;
	}
	else 
	{
		//cerr<<"Frame::getTree() unable to get component tree "<<m_tree_file.c_str()<<endl;
		return NULL;
	}
}

void CellTrack::Frame::setTree(ComponentTree* tree)
{
	assert(tree != NULL);
	m_tree = tree;
	m_width = tree->width();
	m_height = tree->height();
	m_depth = tree->depth();
}

void CellTrack::Frame::releaseTree(char* tree_file)
{
	m_tree_file = string(tree_file);
	cout<<"release tree "<< tree_file<<endl;
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
/*
void CellTrack::Frame::setVertices()
{

	ComponentTree* tree = this->getTree();
	if(tree == NULL)
	{
		//cerr<<"Unable to setVertices, make sure all cells' vertices are alread set"<<endl;
		return;
	}
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
*/
int CellTrack::Frame::cellNum() const
{
	return m_cells.size();
}

vector<CellTrack::Cell*> CellTrack::Frame::getCells() const
{
	return m_cells;
}

int CellTrack::Frame::width() const
{
	assert(m_width > 0);
	return m_width;
}

int CellTrack::Frame::height() const
{
	assert(m_height > 0);
	return m_height;
}

int CellTrack::Frame::depth() const
{
	assert(m_height > 0);
	return m_depth;
}

CellTrack::Track::Track()
{
	//m_start_time = -1;
	m_entry_cell = NULL;
	//m_color_id = -1;
	m_color = 0;
}

CellTrack::Cell* CellTrack::Track::getStartCell() const
{
	return m_entry_cell;
}

//int CellTrack::Track::getColorId() const
//{
//	assert(m_color_id != -1);
//	return m_color_id;
//}

unsigned int CellTrack::Track::getColor() const
{
	return m_color;
}

void CellTrack::Track::setColor(unsigned int color)
{
	m_color = color;
}

void CellTrack::Track::addNext(Cell* cell)
{
	//m_cells.push_back(cell);
}

vector<CellTrack::Cell*> CellTrack::Track::getCells() const
{
	vector<Cell*> cells;
	Cell* p = m_entry_cell;
	while(p != NULL)
	{
		cells.push_back(p);
		p = p->getNextCell();
	}
	return cells;
}

int CellTrack::Track::cellNum() const
{
	int cell_num = 1;

	Cell* p = m_entry_cell;
	while(p->getNextCell() != NULL) 
	{
		cell_num++;
		p = p->getNextCell();
	}
	return cell_num;
}
