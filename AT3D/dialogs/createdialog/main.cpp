#include <QtGui>
#include "../createdialog.h"

int main(int argc, char* argv[])
{
	QApplication* a = new QApplication(argc, argv);
	CreateDialog* createdlg = new CreateDialog();
	createdlg->setExportButton();
	createdlg->setModal(true);
	createdlg->exec();
	return a->exec();
}
