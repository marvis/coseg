//
//=======================================================================
// Copyright 2011 Institute PICB.
// Authors: Hang Xiao
// Data : March 20, 2011
//=======================================================================
//


#include "../../CT3D/cell_track.h"
#include <vector>
#include <string>
int main(int argc, char* argv[])
{
	CellTrack cell_track;
	vector<char*> tree_files;
	for(int i = 1; i < argc; i++)
	{
		tree_files.push_back(argv[i]);
	}
	cell_track.createFromTrees(tree_files);
	cell_track.exportImages((char*)"cosegmore_out");
}
