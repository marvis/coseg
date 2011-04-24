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
#include "dialogs/createdialog.h"
#include "extends/cell_track_ex.h"
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
	connect(m_glWidget, SIGNAL(mouseClicked(float,float)), this, SLOT(onCellMarked(float, float)));
	m_cellWidget = new CellWidget();
	connect(m_cellWidget, SIGNAL(cellChecked(CellTrack::Cell* )), this, SLOT(onCellMarked(CellTrack::Cell*)));
	
	QLayout * layout = new QVBoxLayout();
	layout->addWidget(m_glWidget);
	ui.glGroupBox->setLayout(layout);

	ui.scrollArea->setWidget(m_cellWidget);	
}

CellTrack::Cell* AT3DVIEW::getClickedCell(float posX, float posY, float posZ)
{
	if(cell_centers.empty())
	{
		setCellCenters();
	}
	int w = this->getWidth();
	int h = this->getHeight();
	int d = this->getDepth();
	float min_dist = (float)INT_MAX;
	CellTrack::Cell* obj_cell = NULL;
	vector<CellTrack::Cell*> cells = celltrack->getFrame(current_time)->getCells();
	vector<CellTrack::Cell*>::iterator it = cells.begin();
	while(it != cells.end())
	{
		vector<float>& center = cell_centers[*it];
		double center_x = center[0];
		double center_y = center[1];
		double center_z = center[2];
		double winX, winY, winZ;
		this->m_glWidget->getProjection(winX, winY, winZ, center_x, center_y, center_z);
		winZ = 0.0;
		float dist = (posX - winX)*(posX - winX) + 
			(posY - winY)*(posY - winY) + 
			(posZ - winZ)*(posZ - winZ);
		if(dist < min_dist)
		{
			min_dist = dist;
			obj_cell = *it;
		}
		it++;
	}
	return obj_cell;
}

/***************************************************
 * onOpen : open data
 ***************************************************/

void AT3DVIEW::onOpen()
{
	CreateDialog* createdlg = new CreateDialog();
	CellTrackEX* cell_track_ex = new CellTrackEX();
	createdlg->setCellTrack(cell_track_ex);
	createdlg->setModal(true);
	if(createdlg->exec() == QDialog::Accepted && createdlg->getCellTrack() != NULL)
	{
		celltrack = createdlg->getCellTrack();
		this->initTracksState(); // important
		current_time = 0;
		// 1. set m_glWidget
		unsigned char* img = getTexData();
		m_glWidget->loadTexture(img, this->getWidth(), this->getHeight(), this->getDepth(),3);
		m_glWidget->updateGL();

		// 2. set m_cellWidget
		m_cellWidget->setCells(celltrack->getFrame(current_time)->getCells(), this->getMarkedCells());
	}
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
	setFirst();
	ui.glGroupBox->setTitle(tr("Time : %1").arg(current_time + 1));
	m_glWidget->loadTexture(this->getTexData(), this->getWidth(), this->getHeight(), this->getDepth(),3);
	m_glWidget->updateGL();
	m_cellWidget->setCells(celltrack->getFrame(current_time)->getCells(), this->getMarkedCells());
}

void AT3DVIEW::onLast()
{
	setLast();
	ui.glGroupBox->setTitle(tr("Time : %1").arg(current_time + 1));
	m_glWidget->loadTexture(this->getTexData(), this->getWidth(), this->getHeight(), this->getDepth(),3);
	m_glWidget->updateGL();
	m_cellWidget->setCells(celltrack->getFrame(current_time)->getCells(), this->getMarkedCells());
}

void AT3DVIEW::onPrevious()
{
	setPrev();
	ui.glGroupBox->setTitle(tr("Time : %1").arg(current_time + 1));
	m_glWidget->loadTexture(this->getTexData(), this->getWidth(), this->getHeight(), this->getDepth(),3);
	m_glWidget->updateGL();
	m_cellWidget->setCells(celltrack->getFrame(current_time)->getCells(), this->getMarkedCells());
}

void AT3DVIEW::onNext()
{
	setNext();
	ui.glGroupBox->setTitle(tr("Time : %1").arg(current_time + 1));
	m_glWidget->loadTexture(this->getTexData(), this->getWidth(), this->getHeight(), this->getDepth(),3);
	m_glWidget->updateGL();
	m_cellWidget->setCells(celltrack->getFrame(current_time)->getCells(), this->getMarkedCells());
}

//Cell Widget
void AT3DVIEW::onReverse()
{
}

void AT3DVIEW::onUndo()
{
	if(history.empty()) return;
	undo();
	m_glWidget->loadTexture(this->getTexData(), this->getWidth(), this->getHeight(), this->getDepth(),3);
	m_glWidget->updateGL();
	m_cellWidget->setCells(celltrack->getFrame(current_time)->getCells(), this->getMarkedCells());
}
/************************************************
 * When checkbox are checked and clicked the choose button
 ************************************************/
void AT3DVIEW::onChoose()
{
	choose();
	m_glWidget->loadTexture(this->getTexData(), this->getWidth(), this->getHeight(), this->getDepth(),3);
	m_glWidget->updateGL();
	m_cellWidget->setCells(celltrack->getFrame(current_time)->getCells(), this->getMarkedCells());
}

void AT3DVIEW::onDelete()
{
	remove();
	m_glWidget->loadTexture(this->getTexData(), this->getWidth(), this->getHeight(), this->getDepth(),3);
	m_glWidget->updateGL();
	m_cellWidget->setCells(celltrack->getFrame(current_time)->getCells(), this->getMarkedCells());
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

// signal flow : glwidget -> cellwidget -> AT3DVIEW
void AT3DVIEW::onCellMarked(float x, float y)
{
	CellTrack::Cell* cell = this->getClickedCell(x,y,0.0);
	if(!tracks_state[cell->getTrack()])
	{
		m_cellWidget->setCellChecked(cell, true);
	}
	else 
	{
		m_cellWidget->setCellChecked(cell, false);
	}
	m_glWidget->loadTexture(this->getTexData(), this->getWidth(), this->getHeight(), this->getDepth(),3);
	m_glWidget->updateGL();

}

// signal from cellwidget
void AT3DVIEW::onCellMarked(CellTrack::Cell* cell)
{
//	if(cell_centers.empty())
//	{
//		setCellCenters();
//	}
	if(!tracks_state[cell->getTrack()])
	{
		this->markCell(cell);
	}
	else 
	{
		unMarkCell(cell);
	}

	m_glWidget->loadTexture(this->getTexData(), this->getWidth(), this->getHeight(), this->getDepth(),3);
	m_glWidget->updateGL();
	//m_cellWidget->setCells(celltrack->getFrame(current_time)->getCells(), this->getMarkedCells());
}

void AT3DVIEW::on_fineTuningButton_clicked()
{
    vector<CellTrack::Cell*> cells = this->getMarkedCells();
    if(cells.size() != 1)
    {
            QMessageBox::information(this,"","Please choose only one cell!");
            return;
    }
    CellTrack::Cell* cell = cells[0];
	ComponentTree* tree = cell->getTree();
    int node_label = cell->getNodeLabel();
	cout<<"node_label = "<<node_label<<" size = "<<tree->getNode(node_label)->getBetaSize()<<endl;

    FineTuningDialog* dlg = new FineTuningDialog();
    dlg->setModal(true);
    dlg->setParameters(tree, node_label);
    dlg->exec();
}
