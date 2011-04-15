#include "regionadjustdialog.h"
#include "ui_regionadjustdialog.h"

RegionAdjustDialog::RegionAdjustDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegionAdjustDialog)
{
    ui->setupUi(this);
}

RegionAdjustDialog::~RegionAdjustDialog()
{
    delete ui;
}

void RegionAdjustDialog::changeEvent(QEvent *e)
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
