#include "createdialog.h"
#include "ui_createdialog.h"
#include "../../CT3D/cell_track.h"
#include <QtGui>
#include <QFileDialog>

bool isInt(QString str)
{
    if(str.length() == 0) return false;
    for(int i = 0; i < str.length(); i++)
    {
        if(str.at(i) > QChar('9') || str.at(i) < QChar('0')) return false;
    }
    return true;
}

CreateDialog::CreateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateDialog)
{
    ui->setupUi(this);
	ui->progressBar->setHidden(true);
	ui->progressLabel->setHidden(true);
	ui->exportButton->setEnabled(false);
	ui->exportButton->setHidden(true);
	connect(this,SIGNAL(setProgressValue(int)), this, SLOT(onSetProgressValue(int)));
	m_celltrack = new CellTrack();
}

CreateDialog::~CreateDialog()
{
    delete ui;
}
CellTrack* CreateDialog::getCellTrack()
{
	return m_celltrack;
}
int CreateDialog::getMinThresh()
{
    return atoi((char*) ui->minEditor->text().toStdString().c_str());
}

int CreateDialog::getMaxThresh()
{
    return atoi((char*) ui->maxEditor->text().toStdString().c_str());

}

int CreateDialog::getSingleThresh()
{
    return atoi((char*) ui->singleEditor->text().toStdString().c_str());

}

QStringList CreateDialog::getFileList()
{
    return m_filelist;
}

bool CreateDialog::checkValid()
{
   if(ui->fromImagesButton->isChecked())
    {
        QString minStr = ui->minEditor->text();
        QString maxStr = ui->maxEditor->text();
        QString singleStr = ui->singleEditor->text();
        return isInt(minStr) && isInt(maxStr) && isInt(singleStr) && (!m_filelist.empty());
    }
    return !m_filelist.empty();
}

bool CreateDialog::isFromTrees()
{
    return ui->fromTreesButton->isChecked();
}

void CreateDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CreateDialog::on_fromImagesButton_clicked(bool checked)
{
        ui->parasGroupBox->setVisible(checked);
        ui->openFilesButton->setText(tr("Open Images"));
}

void CreateDialog::on_fromTreesButton_clicked(bool checked)
{
    ui->parasGroupBox->setHidden(checked);
    ui->openFilesButton->setText(tr("Open Trees"));
}

void CreateDialog::setExportButton()
{
	ui->exportButton->setVisible(true);
}
void CreateDialog::onSetProgressValue(int value)
{
	if(value >= m_filelist.size()) return;
	ui->progressBar->setValue(value+1);
	//QString output = ui->filesListEditor->toPlainText();
	//output += tr("\n create component tree %1").arg(value);
	//ui->filesListEditor->setText(output);
	QString str = m_filelist.at(value);
	str = str.left(str.lastIndexOf("."));
	str = str + tr(".bin.tree");
	ui->progressLabel->setText(tr("create component tree %1").arg(str));

}


void CreateDialog::on_openFilesButton_clicked()
{

    //QStringList filelist ;
    if(ui->fromImagesButton->isChecked())
    {
         m_filelist = QFileDialog::getOpenFileNames(
                                this,
                                "Select one or more files to open",
                                "",
                                "Images (*.tif *.tiff)");
        /*
           filelist =  QFileDialog::getOpenFileNames(
            this,
            "Choose image files",
            "",
            "Images (*.tif *.tiff)");
            */
    }
    else
    {
         m_filelist = QFileDialog::getOpenFileNames(
                                this,
                                "Select one or more files to open",
                                "",
                                "Images (*.tree)");
        /*
        filelist = QFileDialog::getOpenFileNames(
                this,
                "Choose tree files",
                "",
                "Images (*.tree)");
                */
    }
    QString allfiles = "";
    QStringList::iterator it = m_filelist.begin();
    while(it != m_filelist.end())
    {
        allfiles += tr("%1\n").arg(*it);
        it++;
    }
    ui->filesListEditor->setText(allfiles);
}


void CreateDialog::accept()
{
    if(checkValid())
	{
        vector<string> filelist;
        QStringList::iterator it = m_filelist.begin();
        while(it != m_filelist.end())
        {
            QString file = *it;
            filelist.push_back(file.toStdString().c_str());
            it++;
        }
        if(this->isFromTrees())
        {
			vector<string> & tree_files = filelist;
			m_celltrack->releaseFrames();
			m_celltrack->releaseAllCells();
			m_celltrack->releaseTracks();
            if(!m_celltrack->createFromTrees(tree_files))
            {
                QMessageBox::information(this,tr("Create Error"),tr("Unable to create from tree files!"));
            }
			else ui->exportButton->setEnabled(true);
        }
        else
        {
			//ui->progressBar->setVisible(true);
			ui->progressBar->setMinimum(0);
			ui->progressBar->setMaximum(filelist.size());
			ui->progressBar->show();
			ui->progressLabel->show();
            int min = this->getMinThresh();
            int max = this->getMaxThresh();
            int single = this->getSingleThresh();

			vector<string> & image_files = filelist;
			ComponentTree * tree = new ComponentTree();
			vector<string> tree_files;
			for(int i = 0; i < image_files.size(); i++)
			{
				emit setProgressValue(i);
				tree->clear();
				tree->create((char*) image_files[i].c_str(), min, max, single);
				//===============================================
				string tree_file = image_files[i];
				tree_file = tree_file.substr(0, tree_file.rfind("."));
				tree_file.append(".bin.tree");
				//===============================================
				tree->save((const char*)tree_file.c_str());
				tree_files.push_back(tree_file);
			}
			m_celltrack->releaseFrames();
			m_celltrack->releaseAllCells();
			m_celltrack->releaseTracks();
        	if(! m_celltrack->createFromTrees(tree_files))
            {
                QMessageBox::information(this,tr("Create Error"),tr("Unable to create from image files!"));
            }
			else ui->exportButton->setEnabled(true);
        }
		return QDialog::accept();
	}
    else
    {
        QMessageBox::information(this, tr("Invalid Information"),tr("Please Check Your Input!"));
        if(ui->fromImagesButton->isChecked())
         {
             QString minStr = ui->minEditor->text();
             QString maxStr = ui->maxEditor->text();
             QString singleStr = ui->singleEditor->text();
             if(!isInt(minStr)) ui->minEditor->setText(tr(""));
             if(!isInt(maxStr)) ui->maxEditor->setText(tr(""));
             if(!isInt(singleStr)) ui->singleEditor->setText(tr(""));
         }
    }
}

void CreateDialog::on_exportButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     "",
                                                     QFileDialog::ShowDirsOnly);
    m_celltrack->exportImages((char*)"",(char*)dir.toStdString().c_str());
}
