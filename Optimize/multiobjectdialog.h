#ifndef MULTIOBJECTDIALOG_H
#define MULTIOBJECTDIALOG_H
#include <QDialog>
#include <QGroupBox>
#include <QList>
#include <QString>
#include <QLineEdit>
#include "inputparamwidget.h"
#include "targetwidget.h"

class MultiObjectDialog : public QDialog
{
public:
    MultiObjectDialog(QWidget *parent = nullptr);
    ~MultiObjectDialog();
    void initialize();
    void initialize(QStringList inputlist, QStringList targetlist);

    //set
    void setInputList(QStringList inputlist);
    void setTargetList(QStringList targetlist);

    //get
    QStringList getInputList();
    QStringList getTargetList();

private:
    void initializeGroup1();
    void initializeGroup2();
    QGroupBox *mGroup1;
    QGroupBox *mGroup2;
    QStringList mInputList;
    QStringList mTargetList;
    InputParamWidget *mInputWidget;
    TargetWidget *mTargetWidget;
};

#endif // MULTIOBJECTDIALOG_H
