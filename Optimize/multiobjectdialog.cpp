#include "multiobjectdialog.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QGridLayout>
#include <QTableView>
#include <QDebug>
MultiObjectDialog::MultiObjectDialog(QWidget *parent)
    : QDialog (parent),
      mGroup1(new QGroupBox(this)),
      mGroup2(new QGroupBox(this)),
      mInputWidget(new InputParamWidget(mGroup1)),
      mTargetWidget(new TargetWidget(mGroup1))
{
    initialize();
}

MultiObjectDialog::~MultiObjectDialog()
{
    delete mTargetWidget;
    delete mInputWidget;
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
    mInputWidget->setInputList(mInputList);
}

void MultiObjectDialog::setTargetList(QStringList targetlist)
{
    mTargetList = targetlist;
    mTargetWidget->setTargetList(mTargetList);
}

QStringList MultiObjectDialog::getInputList()
{
    if(mInputList.empty()){
        qDebug() << "InputList is empty!";
    }
    return mInputList;
}

void MultiObjectDialog::initializeGroup1()
{
    mGroup1->setTitle(tr("Target to be optimized && input parameters"));

    QHBoxLayout *hlayout = new QHBoxLayout(mGroup1);
    hlayout->addWidget(mTargetWidget);
    hlayout->addWidget(mInputWidget);
    hlayout->setStretchFactor(mTargetWidget, 1);
    hlayout->setStretchFactor(mInputWidget, 1);

}

void MultiObjectDialog::initializeGroup2()
{
    mGroup2->setTitle(tr("Multiple target PSO parameters"));

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
