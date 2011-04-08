#include "createform.h"
#include "ui_createform.h"

CreateForm::CreateForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateForm)
{
    ui->setupUi(this);
}

CreateForm::~CreateForm()
{
    delete ui;
}

void CreateForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
