#ifndef CELL_WIDGET_H_H
#define CELL_WIDGET_H_H

#include "../CT3D/cell_track.h"

#include <QtGui>
#include <vector>

using namespace std;

class CellWidget : public QWidget
{
	Q_OBJECT

public:
	CellWidget(QWidget* parent = NULL);
	void setCells(vector<CellTrack::Cell*> visable_cells, vector<CellTrack::Cell*> marked_cells);
	void clearCells();
	int cellNum() const;
public slots:
	void onCellChecked(int state);
signals:
	void cellChecked(CellTrack::Cell* cell);

private:
	vector<QCheckBox*> m_checkers;
	vector<CellTrack::Cell*> m_cells;
};

#endif
