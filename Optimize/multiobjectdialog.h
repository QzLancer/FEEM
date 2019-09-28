#ifndef MULTIOBJECTDIALOG_H
#define MULTIOBJECTDIALOG_H
#include <QDialog>
#include <QGroupBox>
#include <QList>
#include <QString>
#include <QLineEdit>
#include "inputparamwidget.h"
#include "targetwidget.h"
#include "multiobjectcore/pso.h"

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

private slots:
    void slotOptimize();

private:
    void initializeGroup1();
    void initializeGroup2();
    bool isParamError();

    QGroupBox *mGroup1;
    QGroupBox *mGroup2;
    QStringList mInputList;
    QStringList mTargetList;
    InputParamWidget *mInputWidget;
    TargetWidget *mTargetWidget;
    QLabel *mWarningLabel;

    //所有LineEdit
    QLineEdit *mSizeEdit;
    QLineEdit *mTimeEdit;
    QLineEdit *mEliteEdit;
    QLineEdit *mRateEdit;
    QLineEdit *mWLowerEdit;
    QLineEdit *mWUpperEdit;
    QLineEdit *mC1Edit;
    QLineEdit *mC2Edit;

    QStringList mTargetName;
    QStringList mInputName;

    //求解器
    static void objectiveFunction(Particle *Particle);
};

#endif // MULTIOBJECTDIALOG_H
