#include "src/CT3D/cell_track.h"
#include <vector>
#include <string>
int main(int argc, char* argv[])
{
	CellTrack cell_track;
	vector<char*> tree_files;
	tree_files.push_back(argv[1]);
	tree_files.push_back(argv[2]);
	cell_track.createFromTrees(tree_files);
	cell_track.exportImages("cosegtwo_out");
}
