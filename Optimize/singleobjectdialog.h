#ifndef DIALOG_H
#define DIALOG_H
#include <QDialog>
#include <QGroupBox>
#include <QString>
#include <QComboBox>
#include <QStandardItemModel>
#include <QList>
#include <QItemSelectionModel>
#include <QLabel>
#include "singleobjectcore/spso.h"
#include "inputparamwidget.h"

class SingleObjectDialog : public QDialog
{
    Q_OBJECT

public:
    SingleObjectDialog(QWidget *parent = nullptr);
    ~SingleObjectDialog();
    void initialize();
    void initialize(QStringList inputlist, QStringList targetlist);

    //set
    void setInputList(QStringList inputlist);
    void setTargetList(QStringList targetlist);

    //get
    QStringList InputList();
    QStringList TargetList();

public slots:
    void slotOptimize();

private:
    QGroupBox *mGroup1;
    QGroupBox *mGroup2;
    void initializeGroup1();
    void initializeGroup2();
    void refreshTable();
    bool isParamError();

    QStringList mTargetList;
    QStringList mInputList;

    //控件
    QComboBox *mTargetBox;
    QComboBox *mModeBox;

    //所有的参数输入LineEdit
    QLineEdit *mSizeEdit;
    QLineEdit *mTimeEdit;
    QLineEdit *mRateEdit;
    QLineEdit *mWLowerEdit;
    QLineEdit *mWUpperEdit;
    QLineEdit *mC1Edit;
    QLineEdit *mC2Edit;
    QLabel *mWarningLabel1;

    InputParamWidget *mInputWidget;
    QStringList mInputName;
    //求解器
    static void objectiveFunction (SParticle *Particle);
};

#endif // DIALOG_H
