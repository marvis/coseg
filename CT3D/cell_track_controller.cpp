#include "../component_tree.h"
#include "cell_track.h"
#include "cell_track_controller.h"

#include <iostream>
#include <map>
using namespace std;

#define INT_MAX       2147483647

CellTrackController::CellTrackController()
{
	celltrack = NULL;
	current_time = 0;
}

bool CellTrackController::createCellTrack(vector<char*> tree_files)
{
	celltrack = new CellTrack();	
	return celltrack->createFramesFromTrees(tree_files);
}

bool CellTrackController::createCellTrack(vector<char*> image_files, int _min, int _max, int _single)
{
	if(_min > _max || _min < 0 || _single < 0 ) 
	{
		return false;
	}
	else
	{
		ComponentTree *tree = new ComponentTree();
		vector<char*> tree_files;
		for(int i = 0; i < image_files.sizel(); i++)
		{
			tree->clear;
			tree->create(image_files[i], _min, _max, _single);
			//===============================================
			char* tree_file = new char[200];
			string str_file(image_files[i]);
			str_file = str_file.substr(0, str_file.rfind("."));
			str_file.append(".bin.tree");
			strcpy(tree_file, str_file.c_str());
			//===============================================
			tree.save((const char*)tree_file);
			tree_files.push_back(tree_file);
		}
		celltrack = new CellTrack();
		bool rt = celltrack->createFramesFromTrees(tree_files);
		if(rt)
		{
			this->initTracksState();
		}
		vector<char*>::iterator = tree_files.begin();
		while(it != tree_files.end())
		{
			delete (*it);
			it++;
		}

		return rt;
	}
}

bool CellTrackController::loadCellTrack(vector<char*> image_results, vector<char*> tree_files)
{
	celltrack = new CellTrack();
	bool rt = cell_track->createFromImages(img_results);
	if(rt)
	{
		this->initTracksState();
		if(! tree_files.empty() && tree_files.size() == image_results.size())
		{
			if(!cell_track->correspondToTrees(tree_files))
			{
				cerr<<"loadCellTrack error : unable to match trees with cells of frames"<<endl;
			}
		}
	}
	return rt;
}

bool CellTrackController::loadCellTrack(string track_file, vector<string> tree_files)
{
	celltrack = new CellTrack();
	return celltrack->load((char*)track_file.c_str());
}

void CellTrackController::setFirst()
{
	current_time = 0;
	markChoosedCells();
	cell_centers.clear();
}

void CellTrackController::setLast()
{
	current_time = celltrack->frameNum() - 1;
	markChoosedCells();
	cell_centers.clear();
}

void CellTrackController::setNext()
{
	if(current_time == celltrack->frameNum() - 1)
	{
	}
	else
	{
		current_time++;
	}
	markChoosedCells();
	cell_centers.clear();
}

void CellTrackController::setPrev()
{
	if(current_time == 0)
	{
	}
	else
	{
		current_time--;
	}
	markChoosedCells();
	cell_centers.clear();
}
/************************************************************
 * Extract 3d texture of current frame
 * **********************************************************/
unsigned char* CellTrackController::getTexData()
{
	int w = width();
	int h = height();
   	int d = depth();
	unsigned char* image = new  unsigned char[3*w*h*d];
	vector<CellTrack::Cell*> visable_cells = celltrack->getFrame(current_time)->getCells();
	vector<CellTrack::Cell*>::iterator it = visable_cells.begin();
	while(it != visable_cells.end())
	{
		(*it)->draw(image, w, h, d);
		it++;
	}
	vector<Cell*> marked_cells = getMarkedCells();
	it = marked_cells.begin();
	while(it != marked_cells)
	{
		(*it)->drawMarker(image, w, h, d);
		it++;
	}
	return image;
}

int CellTrackController::currentTime()
{
	return current_time;
}

int CellTrackController::width()
{
	return celltrack->getFrame(current_time)->width();
}

int CellTrackController::height()
{
	return celltrack->getFrame(current_time)->height();
}

int CellTrackController::depth()
{
	return celltrack->getFrame(current_time)->depth();
}

CellTrack::Cell* CellTrackController::getClickedCell(int position)
{
	if(cell_centers.empty())
	{
		setCellCenters();
	}
	int w = this->width();
	int h = this->height();
	int d = this->height();
	int mouse_w = position % w;
	int mouse_h = (position /w ) % h;
	int mouse_d = (position /w /h) % d;
	int min_dist = INT_MAX;
	CellTrack::Cell* obj_cell = NULL;
	vector<CellTrack::Cell*> cells = celltrack->getFrame(current_time)->getCells();
	vector<CellTrack::Cell*>::iterator it = cells.begin();
	while(it != cells.end())
	{
		int center = cell_centers[*it];
		int center_w = center % w;
		int center_h = (center / w) % h;
		int center_d = (center / w / h) % d;
		int dist = (mouse_w - center_w)*(mouse_w - center_w) +
		   	(mouse_h - center_h)*(mouse_h - center_h) + 
			(mouse_d - center_d)*(mouse_d - center_d);
		if (dist < min_dist)
		{
			min_dist = dist;
			obj_cell = *it;
		}
		it++;
	}
	return obj_cell;
}
// todo : doxygen comment
vector<CellTrack::Cell*> CellTrackController::getMarkedCells()
{
	vector<CellTrack::Cell*> current_cells = celltrack->getFrame(current_time)->getCells();
	vector<CellTrack::Cell*> marked_cells;
	vector<CellTrack::Cell*>::iterator it = current_cells.begin();
	while(it != current_cells.end())
	{
		CellTrack::Cell* cell = *it;
		if(tracks_state[cell->getTrack()])
		{
			marked_cells.push_back(cell);
		}
		it++;
	}
	return marked_cells;
}

vector<CellTrack::Track*> CellTrackController::getMarkedTracks()
{
	vector<CellTrack::Track*> tracks;
	map<CellTrack::Cell*,  bool>::iterator it = tracks_state.begin();
	while(it != tracks_state.end())
	{
		if((*it).second) tracks.push_back((*it).first);
		it++;
	}
	return tracks;
}

void CellTrackController::markCell(CellTrack::Cell* cell)
{
	tracks_state[cell->getTrack()] = true;
	this->markChoosedCells();
}

void CellTrackController::unMarkCell(CellTrack::Cell* cell)
{
	tracks_state[cell->getTrack()] = false;
	this->markChoosedCells();
}

void CellTrackController::markCellsReversely()
{
	vector<CellTrack::Cell*> cells = celltrack->getFrame(current_time)->getCells();
	vector<CellTrack::Cell*>::iterator it = cells.begin();
	while(it != cells.end())
	{
		CellTrack::Cell* cell = *it;
		if(tracks_state[cell->getTrack()])
		{
			tracks_state[cell->getTrack()] = false;
		}
		else tracks_state[cell->getTrack()] = true;
		it++;
	}
}

void CellTrackController::initTracksState(vector<CellTrack::Track*> marked_tracks)
{
	assert(celltrack != NULL);
	tracks_state.clear();
	int track_num = celltrack->trackNum();
	for(int i = 0; i < track_num; i++)
	{
		CellTrack::Track* track = celltrack->getTrack(i);
		tracks_state[track] = false;
	}
	
	if(!marked_tracks.empty())
	{
		vector<CellTrack::Track*>::iterator it = marked_tracks.begin();
		while(it != marked_tracks.end())
		{
			assert(tracks_state.find(*it) != tracks_state.end());
			tracks_state[*it] = true;
			it++;
		}
	}
}

/************************************************************
 * tracks start from later frame are called unvisited tracks
 * **********************************************************/
void CellTrackController::choose(bool keep_unvisited_tracks)
{
	CellTrack* old_celltrack = celltrack;
	vector<CellTrack::Track*> tracks = this->getMarkedTracks();
	if(keep_unvisited_tracks)
	{
		vector<CellTrack::Track*> all_tracks = tracks;
        for(int i = current_time  + 1 ; i < frameNum(); i++)
        {
            vector<CellTrack::Cell*> cells = celltrack->getFrame(i)->getCells();
            vector<CellTrack::Cell*>::iterator it = cells.begin();
            while(it != cells.end())
            {
				CellTrack::Cell* cell = *it;
				CellTrack::Track* track = cell->getTrack();
				//if appeare in later frame and not manually track
                if(cell->getPrevCell() == NULL && !tracks_state[track])
                {
                   	all_tracks.push_back(cell->getTrack());
                }
                it++;
            }
        }
		celltrack = celltrack->choose(all_tracks);
	}
	else
	{
		celltrack = celltrack->choose(tracks);
	}
	this->initTracksState(tracks);
	this->markChoosedCells();
	pushState(old_celltrack);
	cell_centers.clear();
}

void CellTrackController::pushState(CellTrack* old)
{
	history.push_back(old);
}

CellTrack* CellTrackController::popState()
{
	if(history.empty())
	{
		return NULL;
	}
	else
		return history.pop_back();
}

void CellTrackController::undo()
{
	if(!history.empty())
	{

		vector<CellTrack::Track*> tracks = this->getMarkedTracks();
		celltrack = this->popState();
		this->initTracksState(tracks);
		this->markChoosedCells();
	}
}

void CellTrackController::setCellCenters()
{
	assert(cell_centers.empty());
	int w = this->width();
	int h = this->height();
	int d = this->depth();
	vector<CellTrack*> cells = celltrack->getFrame(current_time)->getCells();
	vector<CellTrack*>::iterator it = cells.begin();
	while(it != cells.end())
	{
		int center = (*it)->getCenter(w, h, d);
		cell_centers[*it] = center;
		it++;
	}
}
