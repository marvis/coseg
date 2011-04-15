#ifndef REGIONADJUSTDIALOG_H
#define REGIONADJUSTDIALOG_H

#include <QDialog>

namespace Ui {
    class RegionAdjustDialog;
}

class RegionAdjustDialog : public QDialog {
    Q_OBJECT
public:
    RegionAdjustDialog(QWidget *parent = 0);
    ~RegionAdjustDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::RegionAdjustDialog *ui;
};

#endif // REGIONADJUSTDIALOG_H
