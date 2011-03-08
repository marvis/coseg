//
//=======================================================================
// Copyright 2010 Institute PICB.
// Authors: Hang Xiao, Axel Mosig
// Data : July 14, 2010
//=======================================================================
//

#ifndef CELL_TRACK_H_H
#define CELL_TRACK_H_H
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <map>
#include "../component_tree.h"

using namespace std;

typedef ComponentTree::Node TNode;
typedef int TIME;


class CellTrack
{
	public:
	class Cell;
	class Track;
	class Frame;

	public:
	CellTrack();
	bool save(char* track_file);
	bool save(ofstream& ofs, bool saveType);
	bool load(char* track_file);              // todo: check beforehand
	bool load(ifstream& ifs, bool saveType); 
	bool reload(char* track_file);
	void clear();
	bool createFromImages(vector<char*> img_files);
	bool createFromTrees(vector<char*> tree_files);
	void exportImages(char* prefix) const;
	//CellTrack* choose(vector<Track*> & tracks);
	//CellTrack* chooseOnly(vector<Track*> & tracks);
	//CellTrack* remove(vector<Track*> & tracks);
	//CellTrack* extractFrames();         // todo: reset time in each track

	vector<Cell*> getFrame(int time) const;
	Track* getTrack(int index) const;
	vector<Track*> getTracks() const;
	vector<Track*> getTracks(vector<vector<Cell*> > &frames);

	int frameNum() const;

	private:
	int m_width;
	int m_height;
	int m_depth;
	//int m_startTime;   // 0 default, used when extractFramesand save
	int m_numFrames;
	int m_numTracks;
	vector<Frame*> m_frames;
	vector<Track*> m_tracks;

	public:
	class Cell
	{
		public:
			Cell();
			void clear();
			int time();
			vector<Track*> getTracks() const;
			vector<int> getColorIds() const;
			double meanHeight() const;
			void draw() const;
			void drawCenter() const;
			vector<float>  getCenters() const;
			void splitTrack(Track * track);
			TNode* firNode();                    // the first alignment result
			TNode* secNode();                    // the second alignment result
			TNode* oriNode();                    // original node
			TNode* modNode();                    // modified node
		private:
			//TIME t;
			int fir_node_label;                    // the first alignment result
			int sec_node_label;                    // the second alignment result
			int ori_node_label;                    // original node
			int mod_node_label;                    // modified node
		public:
			vector<Cell*>  prev_cells;
			vector<Cell*>  next_cell;
			vector<int>    vertices;	
			//vector<int>    colorIds;            // used when load from image
			vector<Track*> tracks;              // used when in choose and remove operation
	}
	class Track
	{
		public:
			Track();
			Cell* getStart() const;
			Cell* getEnd() const;
			vector<Cell*> getCells() const;
			void mergeTrack(Track* next);       // todo: check whether mergeable first
			int cellNum();
		public:
			int start_time;        // 0 for the first time
			vector<Cell*> trajectory;
			int colorId;
	}
	class Frame
	{
		public:
			Frame();
			bool createFromImage(char* img_file);
			bool loadTree(char* tree_file);
			void mergePrevFrame(Frame* prev_frame); // todo: consider NULL
			void linkPrevFrame(Frame* prev_frame);
			int cellNum();
		public:
			static pair<Frame*, Frame*> createFromTrees(ComponentTree* tree1, ComponentTree* tree2);

		public:
			ComponentTree* tree;
			vector<Cell*>  cells;
	}	vector<Track*> m_tracks;
}



#endif
