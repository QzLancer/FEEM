#include "multiobjectdialog.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QGridLayout>
#include <QTableView>
#include <QComboBox>
#include <QDebug>
MultiObjectDialog::MultiObjectDialog(QWidget *parent)
    : QDialog (parent) ,
      mGroup1(new QGroupBox(this)) ,
      mGroup2(new QGroupBox(this))
{
    initialize();
}

MultiObjectDialog::~MultiObjectDialog()
{
    delete mGroup2;
    delete mGroup1;
}

void MultiObjectDialog::initialize()
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

void MultiObjectDialog::initialize(QStringList inputlist, QStringList targetlist)
{
    initialize();
    setInputList(inputlist);
    setTargetList(targetlist);
}

void MultiObjectDialog::setInputList(QStringList inputlist)
{
    mInputList = inputlist;
}

void MultiObjectDialog::setTargetList(QStringList targetlist)
{
    mTargetList = targetlist;
}

QStringList MultiObjectDialog::InputList()
{
    if(mInputList.empty()){
        qDebug() << "InputList is empty!";
    }
    return mInputList;
}

void MultiObjectDialog::initializeGroup1()
{
    mGroup1->setTitle(tr("Target to be optimized && input parameters"));

    //优化目标
    QLabel *targetlabel = new QLabel(tr("Target to be optimized"), mGroup1);
    QTableView *targettable = new QTableView(mGroup1);
    QLabel *targetlabel1 = new QLabel(tr("Target to be optimized: "), mGroup1);
    QComboBox *targetbox = new QComboBox(mGroup1);
    QLabel *modelabel = new QLabel(tr("Optimize mode: "), mGroup1);
    QComboBox *modebox = new QComboBox(mGroup1);
    QPushButton *addtargetbutton = new QPushButton(tr("Add"), mGroup1);

    //输入参数
    QLabel *inputparamlabel = new QLabel(tr("Input parameters: "), mGroup1);
    QTableView *inputparamtable = new QTableView(mGroup1);
    QLabel *inputparamlabel1 = new QLabel(tr("Input parameters: "), mGroup1);
    QComboBox *inputparambox = new QComboBox(mGroup1);
    QLabel *maxinputlabel = new QLabel(tr("Maximum of input parameter: "), mGroup1);
    QLineEdit *maxinputedit = new QLineEdit(mGroup1);
    QLabel *mininputlabel = new QLabel(tr("Minimum of input parameter: "), mGroup1);
    QLineEdit *mininputedit = new QLineEdit(mGroup1);
    QPushButton *addinputbutton = new QPushButton(tr("Add"), mGroup1);

    //layout
    QGridLayout *glayout = new QGridLayout(mGroup1);
    glayout->addWidget(targetlabel, 0, 0);
    glayout->addWidget(targettable, 1, 0, 1, 2);
    glayout->addWidget(targetlabel1, 2, 0);
    glayout->addWidget(targetbox, 2, 1);
    glayout->addWidget(modelabel, 3, 0);
    glayout->addWidget(modebox, 3, 1);
    glayout->addWidget(addtargetbutton, 4, 1);

    glayout->addWidget(inputparamlabel, 0, 2);
    glayout->addWidget(inputparamtable, 1, 2, 1, 2);
    glayout->addWidget(inputparamlabel1, 2, 2);
    glayout->addWidget(inputparambox, 2, 3);
    glayout->addWidget(maxinputlabel, 3, 2);
    glayout->addWidget(maxinputedit, 3, 3);
    glayout->addWidget(mininputlabel, 4, 2);
    glayout->addWidget(mininputedit, 4, 3);
    glayout->addWidget(addinputbutton, 5, 3);
}

void MultiObjectDialog::initializeGroup2()
{
    mGroup2->setTitle(tr("Single target PSO parameters"));

    //种群大小
    QLabel *sizelabel = new QLabel(tr("Population size: "), mGroup2);
    QLineEdit *sizeedit = new QLineEdit(mGroup2);

    //粒子群循环次数
    QLabel *timelabel = new QLabel(tr("Time of PSO circles: "), mGroup2);
    QLineEdit *timeedit = new QLineEdit(mGroup2);

    //精英粒子群数目
    QLabel *elitelabel = new QLabel(tr("Number of eliet PSO: "), mGroup2);
    QLineEdit *eliteedit = new QLineEdit(mGroup2);

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
    flayout->addRow(elitelabel, eliteedit);
    flayout->addRow(ratelabel, rateedit);
    flayout->addRow(wupperlabel, wupperedit);
    flayout->addRow(wlowerlabel, wloweredit);
    flayout->addRow(c1label, c1edit);
    flayout->addRow(c2label, c2edit);
}
