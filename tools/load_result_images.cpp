#include "../CT3D/cell_track.h"
#include <vector>
#include <string>
int main(int argc, char* argv[])
{
	CellTrack cell_track;
	vector<char*> img_files;
	for(int i = 1; i < argc; i++)
	{
		img_files.push_back(argv[i]);
	}
	cell_track.createFromImages(img_files);
	cell_track.exportImages((char*)"reout");
}
