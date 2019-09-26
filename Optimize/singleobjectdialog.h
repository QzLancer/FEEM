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
#include "singleobjectcore/pso.h"
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
    void slotAddTableItem(QString item, double minimum, double maximum);
    void slotChangeData(const QModelIndex &topleft, const QModelIndex &bottomRight, const QVector<int> &roles);
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
    QComboBox *mInputBox;

    //table中的数据
    QStandardItemModel *mInputModel;
    QItemSelectionModel *mInputSelection;
    QStringList mInputParamName;
    QList<QList<double>> mInputValue;

    QLabel *mWarningLabel;

    //所有的参数输入LineEdit
    QLineEdit *mSizeEdit;
    QLineEdit *mTimeEdit;
    QLineEdit *mRateEdit;
    QLineEdit *mWUpperEdit;
    QLineEdit *mWLowerEdit;
    QLineEdit *mC1Edit;
    QLineEdit *mC2Edit;
    QLabel *mWarningLabel1;

    //求解器
    static void objectiveFunction (Particle *Particle);
};

#endif // DIALOG_H
