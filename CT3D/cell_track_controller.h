//
//=======================================================================
// Copyright 2011 Institute PICB.
// Authors: Hang Xiao
// Data : March 29, 2011
//=======================================================================
//

#ifndef CELL_TRACK_CONTROL_H_H
#define CELL_TRACK_CONTROL_H_H

#include "cell_track.h"
#include <map>

using namespace std;

class CellTrackController
{
	public:
		CellTrackController();
                virtual ~CellTrackController();
                virtual bool createCellTrack(vector<string> tree_files);
                virtual bool createCellTrack(vector<string> image_files, int _min, int _max, int _single);

                virtual bool loadCellTrack(vector<string> image_result, vector<string> tree_files);
                virtual bool loadCellTrack(string track_file, vector<string> tree_files);

                virtual void setFirst();           // 1. set current time
                virtual void setLast();            // 2. set cells marked which is marked previously
                virtual void setNext();
                virtual void setPrev();
                virtual unsigned char* getTexData(); // include mark information
                virtual int currentTime();
                virtual int width();
                virtual int height();
                virtual int depth();
		
                virtual CellTrack::Cell* getClickedCell(int position);
                virtual vector<CellTrack::Cell*> getMarkedCells(); // get marked cells in current frame
                virtual vector<CellTrack::Track*> getMarkedTracks(); // get marked cells in all frames

		/*virtual*/ void markChoosedCells(/*CellTrack::Frame * frame = NULL*/); // used when change frames
                virtual void markCell(CellTrack::Cell*);
                virtual void unMarkCell(CellTrack::Cell*);
                virtual void markCellsReversely(); // reversly mark cells in current frame

                virtual void initTracksState(vector<CellTrack::Track*> marked_tracks = vector<CellTrack::Track*>());
		
		/*virtual*/ void choose(bool keep_unvisited_tracks = true);     // once choosed, no new cells will produce in next frames
                virtual void pushState(CellTrack*);
                virtual CellTrack* popState();
                virtual void undo();
	private:
		void setCellCenters();
	protected:
		vector<CellTrack*> history;
		CellTrack* celltrack;
		map<CellTrack::Track*, bool> tracks_state; // affect by create choose undo operation
		map<CellTrack::Cell*, int> cell_centers;  // will be cleared when change frames,  choose and undo, will be create when clicked first cell
		int current_time;  // start from 0
};

#endif
