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

class CellTrackController
{
	public:
		CellTrackController();
		bool createCellTrack(vector<char*> tree_files);
		bool createCellTrack(vector<char*> image_files, int _min, int _max, int _single);

		bool loadCellTrack(vector<char*> image_result, vector<char*> tree_files);
		bool loadCellTrack(string track_file, vector<string> tree_files);
		void setTracksInitState();   // tracks_state will be set false for all tracks

		void setFirst();           // 1. set current time
		void setLast();            // 2. set cells marked which is marked previously
		void setNext();            
		void setPrev();
		unsigned char* getTexData(); // include mark information
		int currentTime();
		int width();
		int height();
		int depth();
		
		CellTrack::Cell* getClickedCell(int position);
		vector<CellTrack::Cell*> getMarkedCells(); // get marked cells in current frame
		void setMarkedCells(); // used when change frames

		void markCell(CellTrack::Cell*);	
		void unMarkCell(CellTrack::Cell*);
		void markCellsReversely(); // reversly mark cells in current frame
		
		// choose and remove will produce new CellTrack object, old CellTrack object will be saved.
		void chooseMarkedCellsLocally();      // do not affect the choose of cells in other frames
		void chooseMarkedCellsGlobally();     // once choosed, no new cells will produce in next frames
		void removeMarkedCells();
		void pushState(CellTrack*);
		CellTrack* popState();
		void undo();
	private:
		void setCellCenters();
	private:
		vector<CellTrack*> history;
		CellTrack* celltrack;
		map<CellTrack::Track*, bool> tracks_state; // affect by choose or remove operation
		map<CellTrack::Cell*, int> cell_centers;  // will be cleared when change frames, affect by choose of remove operation
		int current_time;  // start from 0
};

#endif
