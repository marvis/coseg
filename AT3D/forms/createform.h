#ifndef CREATEFORM_H
#define CREATEFORM_H

#include <QWidget>

namespace Ui {
    class CreateForm;
}

class CreateForm : public QWidget {
    Q_OBJECT
public:
    CreateForm(QWidget *parent = 0);
    ~CreateForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CreateForm *ui;
};

#endif // CREATEFORM_H
