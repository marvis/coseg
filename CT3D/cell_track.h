//
//=======================================================================
// Copyright 2011 Institute PICB.
// Authors: Hang Xiao
// Data : March 20, 2011
//=======================================================================
//

#ifndef CELL_TRACK_H_H
#define CELL_TRACK_H_H

#include "../component_tree.h"
#include "palette.h"

#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

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
			friend class Track;
			friend class CellTrack;
		public:
			Cell();
			Track* getTrack() const;
			void setTrack(Track*);
			//float meanHeight() const;
			void draw(unsigned char* image,/* int w, int h, int d, int c=3,*/ ComponentTree* tree = NULL);
			void drawMarker(unsigned char* image, int w, int h, int d, ComponentTree* tree);

			vector<int>&  getCenterArea(int width, int height, int depth, ComponentTree* tree);
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

			vector<int>& getVertices(ComponentTree* tree = NULL);
			int getVolume() const;
			int getSize() const;
			void getCenter(float & mean_w, float& mean_h, float & mean_d, int w, int h, int d);

			unsigned int getColor() const;
			void setColor(unsigned int color);
		private:
			int m_fir_node_label;                    // the first alignment result
			int m_sec_node_label;                    // the second alignment result
			int m_cur_node_label;                    // original node
			int m_mod_node_label;                    // modified node

			Cell*  m_prev_cell;
			Cell*  m_next_cell;
			vector<int>    m_vertices;	          // seldom set this value
			vector<int> m_center_area;

			Track* m_track;              // used when in choose and remove operation
			unsigned int m_color; // used only in createFromImages
	};

	class Frame
	{
		public:
			friend class CellTrack;
			friend class Cell;
			friend class Track;
		public:
			Frame();
			void exportImage(char* img_file/*, Palette& palette*/);
			void addCell(Cell* cell);
			map<unsigned int, Cell*> createFromImage(char* img_file, map<unsigned int, Cell*> &prev_map_cell);

			void mergePrevFrame(Frame* prev_frame); // todo: consider NULL, free prev_frame
			//vector<int> getReverseAlphaMapping(); // used in mergePrevFrame
			//void linkPrevFrame(Frame* frame);  //  used when loading image files

			ComponentTree* getTree();
			void setTree(ComponentTree*);
			bool correspondToTree(ComponentTree*);
			//void setTreeFile(char* tree_file);

			void releaseTree(char*);
			void releaseVertices();
			//void setVertices();
			int cellNum() const;

			vector<Cell*> getCells() const;

			int width() const;
			int height() const;
			int depth() const;

		private:
			ComponentTree* m_tree; // replaced by getTree
			string m_tree_file;
			vector<Cell*>  m_cells;
			int m_width;
			int m_height;
			int m_depth;
	};	

	
	class Track
	{
		public:
			friend class CellTrack;
		public:
			Track();
			Cell* getStartCell() const;
			void setStartCell(Cell*);
			//int getColorId() const;
			unsigned int getColor() const;
			void setColor(unsigned int color);
			void addNext(Cell* cell);
			vector<Cell*> getCells() const;
			int cellNum() const;
			int trackId() const;
		private:
			//int m_start_time;        // 0 for the first time
			//vector<Cell*> m_cells;
			Cell* m_entry_cell;
			//int m_color_id;
			unsigned int m_color; // a*255^3 + b*255^2 + g*255 + r
			int m_track_id;    //start from 0
	};

	public:
	CellTrack();
	virtual	~CellTrack();
	bool save(char* track_file);
	bool save(ofstream& ofs, bool saveType);
	bool load(char* track_file);              // todo: check beforehand
	bool load(ifstream& ifs, bool saveType); 
	bool reload(char* track_file);

	virtual bool createFromTrees(vector<char*> tree_files);
	virtual bool createFromTrees(vector<string> tree_files);
	bool createFromImages(vector<char*> img_results);
	bool createFromImages(vector<string> img_results);
	bool correspondToTrees(vector<char*> tree_files);         // used when cells are filtered
	bool correspondToTrees(vector<string> tree_files);         // used when cells are filtered
	void setTracksColor();
	void exportImages(char* prefix = NULL, char* dir = NULL) const;

	//CellTrack* chooseLocally(vector<Track*>  tracks, int frame_id);
	//CellTrack* chooseGlobally(vector<Track*>  tracks);
	CellTrack* choose(vector<Track*>  tracks);
	CellTrack* remove(vector<Track*>  tracks);
	CellTrack* clip(int frame_start_id, int frame_end_id);   

	Frame* getFrame(int time) const;
	Track* getTrack(int index) const;
	//vector<Track*> getTracks() const;
	//vector<Track*> getTracks(vector<vector<Cell*> > &frames);

	unsigned int frameNum() const;
	unsigned int trackNum() const;

	public:
	void releaseAllCells();
	void releaseFrames();
	void releaseTracks();

	virtual /*static*/ bool createFramesFromTrees(ComponentTree* tree1, ComponentTree* tree2,vector<Frame*> &frames );
	virtual /*static*/ bool createFramesFromTrees(vector<char*> tree_files,vector<Frame*> &frames);
	virtual /*static*/ bool createFramesFromImages(vector<char*> img_files,vector<Frame*> &frames );
	virtual /*static*/ bool createTracksFromFrames(Frames & frames, vector<Track*> &tracks,int start_time = 0 );

	protected:
	//int m_width;       // in Frame::m_width
	//int m_height;
	//int m_depth;
	//int m_startTime;   // 0 default, used when extractFramesand save
	//int m_numFrames;
	//int m_numTracks;
	Frames m_frames; // Frames = vector<Frame*>
	Tracks m_tracks; // Tracks = vector<Track*>
	//vector<char*> m_tree_files;
	//vector<char*> m_img_files;
};


#endif
