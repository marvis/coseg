#include "cellwidget.h"

#include <cassert>

QString getColorStr(unsigned int color);
QString hex2str(int v);

CellWidget::CellWidget(QWidget* parent) : QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout();
	this->setLayout(layout);
}

void CellWidget::clearCells()
{
	QLayout* layout = this->layout();
	if(!m_cells.empty())
	{
		m_cells.clear();
		for(int i = 0; i < this->cellNum(); i++)
		{
			disconnect(m_checkers[i],SIGNAL(stateChanged(int)), this, SLOT(onChecked(int)));
		}
		QLayoutItem* child;
		while((child = layout->takeAt(0)) != NULL)
		{
			delete child;
		}
		m_checkers.clear();
	}
}

int CellWidget::cellNum() const
{
	assert(m_checkers.size() == m_cells.size());
	return m_cells.size();
}

void CellWidget::setCells(vector<CellTrack::Cell*> visable_cells, vector<CellTrack::Cell*> marked_cells)
{
	clearCells();
	m_cells = visable_cells;
	set<CellTrack::Cell*> set_marked_cells(marked_cells.begin(), marked_cells.end());
	int cell_num = m_cells.size();
	m_checkers.resize(cell_num);
	QVBoxLayout* layout = (QVBoxLayout*) this->layout();
	for(int i = 0; i < cell_num; i++)
	{
		CellTrack::Cell* cell = m_cells[i];
		QHBoxLayout* child_layout = new QHBoxLayout();
		QTextEdit* editor = new QTextEdit(tr("<span style=\" color:#%1;\">%2</span>").arg(getColorStr(cell->getColor()))
				.arg(cell->getSize()));
		editor->setReadOnly(true);
		editor->setFixedHeight(20);
		editor->setFixedWidth(120);
		QCheckBox* checker = new QCheckBox(tr("%1 : ").arg(cell->getTrack()->trackId() + 1));
		if(set_marked_cells.find(m_cells[i]) != set_marked_cells.end())
		{
			checker->setChecked(true);
		}
		m_checkers[i] = checker;
		connect(checker,SIGNAL(stateChanged(int)), this, SLOT(onChecked(int)));
		child_layout->addWidget(checker);
		child_layout->addWidget(editor);
		layout->addLayout(child_layout);
	}
	layout->setSpacing(10);
	//layout->setVerticalSpacing(10);
}

void CellWidget::onCellChecked(int state)
{
	int index = 0;
	QCheckBox * checker = (QCheckBox*) this->sender();
	vector<QCheckBox*>::iterator it = m_checkers.begin();
	while(it != m_checkers.end())
	{
		if((*it) == checker) break; 
		index++;
		it++;
	}
	emit cellChecked(m_cells[index]);
}
// color =  a*255^3 + b*255^2 + g*255 + r
QString getColorStr(unsigned int color)
{
    int high,low;
    QString out;
	int r = color % 256;
	int g = (color / 256) % 256;
	int b = (color / 256/ 256) % 256;
    high=r/16;
    low=r%16;
    out+=QObject::tr("%1%2").arg(hex2str(high)).arg(hex2str(low));
    high=g/16;
    low=g%16;
    out+=QObject::tr("%1%2").arg(hex2str(high)).arg(hex2str(low));
    high=b/16;
    low=b%16;
    out+=QObject::tr("%1%2").arg(hex2str(high)).arg(hex2str(low));
    return out;
}

QString hex2str(int v)
{
    if(v<0 || v>15)return QString("");
    else
    {
        switch(v)
        {
            case 10: return QString("a");
            case 11: return QString("b");
            case 12: return QString("c");
            case 13: return QString("d");
            case 14: return QString("e");
            case 15: return QString("f");
            default: return QObject::tr("%1").arg(v);
        }
    }
}

