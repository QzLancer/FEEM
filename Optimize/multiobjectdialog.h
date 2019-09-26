#ifndef MULTIOBJECTDIALOG_H
#define MULTIOBJECTDIALOG_H
#include <QDialog>
#include <QGroupBox>
#include <QList>
#include <QString>
#include "inputparamwidget.h"
#include <QLineEdit>
#include <QComboBox>

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

    //target控件
    QTableView *mTargetTable;
    QStandardItemModel *mTargetModel;
    QItemSelectionModel *mTargetSelection;
    QComboBox *mTargetBox;
    QComboBox *mModeBox;
    InputParamWidget *mInputWidget;
    QPushButton *mTargetDeleteButton;
    QPushButton *mTargetAddButton;
    QLabel *mTargetWarningLabel;
};

#endif // MULTIOBJECTDIALOG_H
