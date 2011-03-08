#include <iostream>
#include <string>
#include <fstream>
#include "../component_tree.h"
using namespace std;
typedef ComponentTree::Node TNode;
typedef int TIME;

/***************************************************************************
 * CellTrack construction function
 **************************************************************************/
CellTrack::CellTrack()
{
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
	if(save(ofs))
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
        string str_file(treefile);
        if(str_file.find(".bin") != string::npos)
        {
                cout<<"load binary file: "<<treefile<<endl;
                saveType = false;
                ifs.open(treefile, ios_base::in|ios_base::binary);
        }
        else if(str_file.find(".txt") != string::npos)
        {
                cout<<"load txt file: "<<treefile<<endl;
                saveType = true;
                ifs.open(treefile);
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
	this->clear();
	load(track_file);
}

void CellTrack::clear()
{
}

bool CellTrack::createFromImages(vector<char*> img_files)
{
	assert(m_frames.empty());
}

bool CellTrack::createFromTrees(vector<char*> tree_files);
{
	assert(m_frames.empty());
	m_numFrames = tree_files.size();
	m_frames.resize(m_numFrames);

	vector<ComponentTree*> trees;
	trees.resize(m_numFrames);

	Frame* prev_frame = NULL;
	ComponentTree* first_tree = NULL;
	ComponentTree* second_tree = NULL;
	first_tree = new ComponentTree(tree_files[0]);
	for(int i = 0; i < m_numFrames-1; i++)
	{
		pair<Frame*, Frame*> frame_pair;
		second_tree = new ComponentTree(tree_files[i+1]);
		frame_pair = Frame::createFromTrees(first_tree, second_tree);

		first_tree.clear();
		first_tree = second_tree;

		m_frames[i] = frame_pair.first;
		m_frames[i]->mergePrevFrame(prev_frame);
		prev_frame = frame_pair.second;
	}
	m_frames[i] = prev_frame;
}


