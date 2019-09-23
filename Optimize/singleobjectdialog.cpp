#include "singleobjectdialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtDebug>
#include <QPushButton>
#include <QTableView>
#include <QLineEdit>
#include <QFormLayout>

SingleObjectDialog::SingleObjectDialog(QWidget *parent)
    : QDialog(parent),
      mGroup1(new QGroupBox(this)),
      mGroup2(new QGroupBox(this))
{
    initialize();
}

SingleObjectDialog::~SingleObjectDialog()
{
    delete mGroup2;
    delete mGroup1;
}

void SingleObjectDialog::initialize()
{
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    initializeGroup1();
    initializeGroup2();

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(mGroup1);
    hlayout->addWidget(mGroup2);

    QPushButton *buttonadd = new QPushButton(this);
    buttonadd->setText(tr("To optimize"));
    QHBoxLayout *hlayout1 = new QHBoxLayout;
    hlayout1->addStretch();
    hlayout1->addWidget(buttonadd);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addLayout(hlayout);
    vlayout->addLayout(hlayout1);
}

void SingleObjectDialog::initialize(QStringList inputlist, QStringList targetlist)
{
    initialize();
    setInputList(inputlist);
    setTargetList(targetlist);
}

void SingleObjectDialog::setInputList(QStringList inputlist)
{
    mInputList = inputlist;

}

void SingleObjectDialog::setTargetList(QStringList targetlist)
{
    mTargetList = targetlist;
}

QStringList SingleObjectDialog::InputList()
{
    if(mInputList.empty()){
        qDebug() << "InputList is empty!";
    }
    return mInputList;
}

QStringList SingleObjectDialog::TargetList()
{
    if(mTargetList.empty()){
        qDebug() << "TargetList is empty!";
    }
    return mTargetList;
}

void SingleObjectDialog::initializeGroup1()
{
    mGroup1->setTitle(tr("Target to be optimized && input parameters"));

    //优化目标
    QLabel *targetlabel = new QLabel(tr("Target to be optimized"), mGroup1);
    QLabel *modelabel = new QLabel(tr("Optimize Mode"), mGroup1);
    QComboBox *targetbox = new QComboBox(mGroup1);
    QComboBox *modebox = new QComboBox(mGroup1);

    //输入参数
    QLabel *inputparamlabel = new QLabel(tr("Input parameters"), mGroup1);
    QTableView *inputparamtable = new QTableView(mGroup1);
    QLabel *inputparamlabel1 = new QLabel(tr("Input parameters"), mGroup1);
    QComboBox *inputparambox = new QComboBox(mGroup1);
    QLabel *maxinputparamlabel = new QLabel(tr("Maximum of input parameter"), mGroup1);
    QLineEdit *maxinputparamedit = new QLineEdit(mGroup1);
    QLabel *mininputparamlabel = new QLabel(tr("Minimum input parameter"), mGroup1);
    QLineEdit *mininputparamedit = new QLineEdit(mGroup1);

    QPushButton *addbutton = new QPushButton(tr("Add"), mGroup1);

    //layout
    QGridLayout *gridlayout = new QGridLayout(mGroup1);
    gridlayout->addWidget(targetlabel, 0, 0);
    gridlayout->addWidget(targetbox, 0, 1);
    gridlayout->addWidget(modelabel, 1 ,0);
    gridlayout->addWidget(modebox, 1, 1);

    gridlayout->addWidget(inputparamlabel, 2, 0);
    gridlayout->addWidget(inputparamtable, 3, 0, 1, 2);

    gridlayout->addWidget(inputparamlabel1, 4, 0);
    gridlayout->addWidget(inputparambox, 4, 1);
    gridlayout->addWidget(maxinputparamlabel, 5, 0);
    gridlayout->addWidget(maxinputparamedit, 5, 1);
    gridlayout->addWidget(mininputparamlabel, 6, 0);
    gridlayout->addWidget(mininputparamedit, 6, 1);
    gridlayout->addWidget(addbutton, 7, 1);

    //table setting
    inputparamtable->setShowGrid(true);

}

void SingleObjectDialog::initializeGroup2()
{
    mGroup2->setTitle(tr("Single target PSO parameters"));

    //种群大小
    QLabel *sizelabel = new QLabel(tr("Population size: "), mGroup2);
    QLineEdit *sizeedit = new QLineEdit(mGroup2);

    //粒子群循环次数
    QLabel *timelabel = new QLabel(tr("Time of PSO circles: "), mGroup2);
    QLineEdit *timeedit = new QLineEdit(mGroup2);

    //变异概率
    QLabel *ratelabel = new QLabel(tr("Mutation rate: "), mGroup2);
    QLineEdit *rateedit = new QLineEdit(mGroup2);

    //w上界
    QLabel *wupperlabel = new QLabel(tr("W upper bound: "), mGroup2);
    QLineEdit *wupperedit = new QLineEdit(mGroup2);

    //w下界
    QLabel *wlowerlabel = new QLabel(tr("W lower bound: "), mGroup2);
    QLineEdit *wloweredit = new QLineEdit(mGroup2);

    //c1
    QLabel *c1label = new QLabel(tr("c1: "), mGroup2);
    QLineEdit *c1edit = new QLineEdit(mGroup2);

    //c2
    QLabel *c2label = new QLabel(tr("c2: "), mGroup2);
    QLineEdit *c2edit = new QLineEdit(mGroup2);

    //layout
    QFormLayout *flayout = new QFormLayout(mGroup2);
    flayout->addRow(sizelabel, sizeedit);
    flayout->addRow(timelabel, timeedit);
    flayout->addRow(ratelabel, rateedit);
    flayout->addRow(wupperlabel, wupperedit);
    flayout->addRow(wlowerlabel, wloweredit);
    flayout->addRow(c1label, c1edit);
    flayout->addRow(c2label, c2edit);

}
