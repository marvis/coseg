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
	CellTrackEX* cell_track_ex = new CellTrackEX();
	createdlg->setCellTrack(cell_track_ex);
	createdlg->setModal(true);
	if(createdlg->exec() == QDialog::Accepted && createdlg->getCellTrack() != NULL)
	{
		celltrack = createdlg->getCellTrack();
		this->initTracksState(); // important
		current_time = 0;
		unsigned char* img = getTexData();
		m_glWidget->loadTexture(img, this->getWidth(), this->getHeight(), this->getDepth(),3);
		m_glWidget->updateGL();
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
}

void AT3DVIEW::onLast()
{
	setLast();
	ui.glGroupBox->setTitle(tr("Time : %1").arg(current_time + 1));
	m_glWidget->loadTexture(this->getTexData(), this->getWidth(), this->getHeight(), this->getDepth(),3);
	m_glWidget->updateGL();
}

void AT3DVIEW::onPrevious()
{
	setPrev();
	ui.glGroupBox->setTitle(tr("Time : %1").arg(current_time + 1));
	m_glWidget->loadTexture(this->getTexData(), this->getWidth(), this->getHeight(), this->getDepth(),3);
	m_glWidget->updateGL();
}

void AT3DVIEW::onNext()
{
	setNext();
	ui.glGroupBox->setTitle(tr("Time : %1").arg(current_time + 1));
	m_glWidget->loadTexture(this->getTexData(), this->getWidth(), this->getHeight(), this->getDepth(),3);
	m_glWidget->updateGL();
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


