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
#include <string>
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
		class Frame;
		class Track;
		typedef vector<Frame*> Frames;
		typedef vector<Track*> Tracks;
	public:
	class Cell
	{
		public:
			friend class Frame;
		public:
			Cell();
			Track* getTrack() const;
			void setTrack(Track*);
			//double meanHeight() const;
			void draw() const;
			void drawCenter() const;

			vector<int>  getCenterArea() const;
			void setCenterArea();

			TNode* getFirNode(ComponentTree*) const;                    // the first alignment result
			void setFirNode(TNode*);                    // the first alignment result

			TNode* getSecNode(ComponentTree*) const;                    // the second alignment result
			void setSecNode(TNode*);                    // the second alignment result

			TNode* getCurNode(ComponentTree*) const;                    // original node
			void setCurNode(TNode* );                    // original node

			TNode* getModNode(ComponentTree*) const;                    // modified node
			void setModNode(TNode*);                    // modified node

			TNode* getNode(ComponentTree*) const;

			Cell* getPrevCell() const;
			void  setPrevCell(Cell*);

			Cell* getNextCell() const;
			void  setNextCell(Cell*);

			vector<int> getVertices() const;
		private:
			int m_fir_node_label;                    // the first alignment result
			int m_sec_node_label;                    // the second alignment result
			int m_cur_node_label;                    // original node
			int m_mod_node_label;                    // modified node

			Cell*  m_prev_cell;
			Cell*  m_next_cell;
			vector<int>    m_vertices;	          // seldom set this value

			Track* m_track;              // used when in choose and remove operation
	};

	class Frame
	{
		public:
			Frame();
			void exportImage(int width, int height, int depth, char* img_file);
			void addCell(Cell* cell);
			bool createFromImage(char* img_file);
			void mergePrevFrame(Frame* prev_frame); // todo: consider NULL, free prev_frame
			void linkPrevFrame(Frame* frame);  //  used when loading image files
			int cellNum() const;
			ComponentTree* getTree();
			void setTree(ComponentTree*);
			void setTreeFile(char* tree_file);

			void releaseTree();
			void releaseVertices();
			void loadVertices(ComponentTree*);

		private:
			ComponentTree* m_tree; // replaced by getTree
			string m_tree_file;
			vector<Cell*>  m_cells;
	};	

	
	class Track
	{
		public:
			Track();
			Cell* getStart() const;
			Cell* getEnd() const;
			void addNext(Cell* cell);
			vector<Cell*> getCells() const;
			int cellNum() const;
		private:
			int m_start_time;        // 0 for the first time
			vector<Cell*> m_cells;
			int m_colorId;
	};

	public:
	CellTrack();
	~CellTrack();
	bool save(char* track_file);
	bool save(ofstream& ofs, bool saveType);
	bool load(char* track_file);              // todo: check beforehand
	bool load(ifstream& ifs, bool saveType); 
	bool reload(char* track_file);

	bool createFromTrees(vector<char*> tree_files);
	bool createFromImages(vector<char*> img_files);
	void exportImages(char* prefix) const;
	//CellTrack* choose(vector<Track*> & tracks);
	//CellTrack* chooseOnly(vector<Track*> & tracks);
	//CellTrack* remove(vector<Track*> & tracks);
	//CellTrack* extractFrames();         // todo: reset time in each track

	Frame* getFrame(int time) const;
	Track* getTrack(int index) const;
	//vector<Track*> getTracks() const;
	//vector<Track*> getTracks(vector<vector<Cell*> > &frames);

	int frameNum() const;
	int trackNum() const;

	public:
	void releaseFrames();
	void releaseTracks();
	void releaseAllCells();

	static bool createFramesFromTrees(ComponentTree* tree1, ComponentTree* tree2,vector<Frame*> &frames );
	static bool createFramesFromTrees(vector<char*> tree_files,vector<Frame*> &frames);
	static bool createFramesFromImages(vector<char*> img_files,vector<Frame*> &frames );
	static bool createTracksFromFrames(Frames & frames, vector<Track*> &tracks );

	private:
	int m_width;
	int m_height;
	int m_depth;
	//int m_startTime;   // 0 default, used when extractFramesand save
	int m_numFrames;
	int m_numTracks;
	Frames m_frames;
	Tracks m_tracks;
};

#endif
