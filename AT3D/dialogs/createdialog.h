#ifndef CREATEDIALOG_H
#define CREATEDIALOG_H

#include "../../CT3D/cell_track.h"
//#include "../extends/cell_track_ex.h"
#include <QDialog>
#include <QStringList>

namespace Ui {
    class CreateDialog;
}

class CreateDialog : public QDialog {
    Q_OBJECT
public:
    CreateDialog(QWidget *parent = 0);
    ~CreateDialog();
    bool checkValid();
	void setCellTrack(CellTrack* cell_track);
	CellTrack* getCellTrack();
    int getMinThresh();
    int getMaxThresh();
    int getSingleThresh();
    QStringList getFileList();
    bool isFromTrees();
	void setExportButton();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::CreateDialog *ui;
    QStringList m_filelist;
	CellTrack* m_celltrack;
signals:
	void setProgressValue(int);	
public slots:
	void onSetProgressValue(int);
private slots:
    void on_startTrackingButton_clicked();
    void on_exportButton_clicked();
    void on_openFilesButton_clicked();
    void on_fromTreesButton_clicked(bool checked);
    void on_fromImagesButton_clicked(bool checked);
    void accept();
	void reject();
};
#endif // CREATEDIALOG_H
