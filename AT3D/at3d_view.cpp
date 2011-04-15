#include <QtGui>
#include <cmath>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <cassert>

#include "at3d_view.h"
#include "../CT3D/cell_track_controller.h"
#include "../myalgorithms.h"
#include "../component_tree.h"
#include "ui/adjustregiondialog.h"
#include "dialogs/createdialog.h"
using namespace std;

/************************************
 * variables
 ************************************/

/***********************************
 * Realize Member Functions According
 * to Their Declaration
 ***********************************/
AT3DVIEW::AT3DVIEW(QWidget* parent) : QWidget(parent), CellTrackController()
{
	//step 1 : setUi
	ui.setupUi(this);
	//ui.cellWidget->setAttribute(Qt::WA_DeleteOnClose, false);
	
	//step 2 : initialization
	
	m_glWidget = new GLWidget();
	m_cellWidget = new CellWidget();
	
	QLayout * layout = new QVBoxLayout();
	layout->addWidget(m_glWidget);
	ui.glGroupBox->setLayout(layout);

	ui.scrollArea->setWidget(m_cellWidget);	
}

/***************************************************
 * onOpen : open data
 ***************************************************/

void AT3DVIEW::onOpen()
{
	CreateDialog* createdlg = new CreateDialog();
	createdlg->setModal(true);
	createdlg->setCellTrackController(this);
	connect(this, SIGNAL(setProgressValue(int)), createdlg, SLOT(onSetProgressValue(int)));
	createdlg->exec();
	//celltrack = createdlg->getCellTrack();
}

/************************************************
 * SLot Functions Begin
 ************************************************/

//File Group
void AT3DVIEW::onLoadResult()
{

}

void AT3DVIEW::onSaveFrames()
{

}
//Edit Group
void AT3DVIEW::onApplyFilter()
{
}

void AT3DVIEW::onFilter()
{
}

void AT3DVIEW::onSetColor()
{
}

void AT3DVIEW::onSummary()
{
}

void AT3DVIEW::onSpeed()
{

}

void AT3DVIEW::onVolume()
{
}

void AT3DVIEW::onDeformation()
{
	
}

//View Group
void AT3DVIEW::onNew3D()
{
}

void AT3DVIEW::onResetView()
{
}


void AT3DVIEW::onViewTree()
{
}

//Control Group
void AT3DVIEW::onFirst()
{
}

void AT3DVIEW::onLast()
{
}

void AT3DVIEW::onPrevious()
{
}

void AT3DVIEW::onNext()
{
}

//Cell Widget
void AT3DVIEW::onReverse()
{
}

void AT3DVIEW::onUndo()
{
}
/************************************************
 * When checkbox are checked and clicked the choose button
 ************************************************/
void AT3DVIEW::onChoose()
{
}

void AT3DVIEW::onDelete()
{
}

void AT3DVIEW::onNearestCellChoosed(int id)
{
}

void AT3DVIEW::onCheckBoxChanged()
{
}


/************************************************
 * SLot Functions End
 ************************************************/


/*******************************************
 * functions begins
 *******************************************/


void AT3DVIEW::clear()
{
	while(!history.empty())
	{
		CellTrack* ct = history.back();
		history.pop_back();
		ct->releaseFrames();
	}
	if(celltrack != NULL)
	{
		celltrack->releaseFrames();
		celltrack->releaseTracks();
		celltrack->releaseAllCells();
	}
}

bool AT3DVIEW::createCellTrack(vector<string> tree_files)
{
    return CellTrackController::createCellTrack(tree_files);
}

bool AT3DVIEW::createCellTrack(vector<string> image_files, int _min, int _max, int _single)
{
	if(_min > _max || _min < 0 || _single < 0 ) 
	{
		return false;
	}
	else
	{
		ComponentTree *tree = new ComponentTree();
		vector<string> tree_files;
		for(int i = 0; i < image_files.size(); i++)
		{
			emit setProgressValue(i);
			tree->clear();
			tree->create((char*) image_files[i].c_str(), _min, _max, _single);
			//===============================================
			string tree_file = image_files[i];
			tree_file = tree_file.substr(0, tree_file.rfind("."));
			tree_file.append(".bin.tree");
			//===============================================
			tree->save((const char*)tree_file.c_str());
			tree_files.push_back(tree_file);
		}
		celltrack = new CellTrack();
		bool rt = celltrack->createFromTrees(tree_files);
		if(rt)
		{
			this->initTracksState();
		}
		/*
		vector<char*>::iterator it = tree_files.begin();
		while(it != tree_files.end())
		{
			delete (*it);
			it++;
		}
		*/

		return rt;
	}
}
