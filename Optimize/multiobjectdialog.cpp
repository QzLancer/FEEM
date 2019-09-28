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
      mTargetWidget(new TargetWidget(mGroup1)),
      mWarningLabel(new QLabel(mGroup2)),
      mSizeEdit(new QLineEdit(mGroup2)),
      mTimeEdit(new QLineEdit(mGroup2)),
      mEliteEdit(new QLineEdit(mGroup2)),
      mRateEdit(new QLineEdit(mGroup2)),
      mWUpperEdit(new QLineEdit(mGroup2)),
      mWLowerEdit(new QLineEdit(mGroup2)),
      mC1Edit(new QLineEdit(mGroup2)),
      mC2Edit(new QLineEdit(mGroup2))
{
    initialize();
}

MultiObjectDialog::~MultiObjectDialog()
{
    delete mC2Edit;
    delete mC1Edit;
    delete mWLowerEdit;
    delete mWUpperEdit;
    delete mRateEdit;
    delete mEliteEdit;
    delete mTimeEdit;
    delete mSizeEdit;
    delete mWarningLabel;
    delete mTargetWidget;
    delete mInputWidget;
    delete mGroup2;
    delete mGroup1;
}

void MultiObjectDialog::initialize()
{
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::red);
    mWarningLabel->setPalette(pe);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    initializeGroup1();
    initializeGroup2();

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(mGroup1);
    hlayout->addWidget(mGroup2);

    QPushButton *buttonoptimize = new QPushButton(this);
    buttonoptimize->setText(tr("To optimize"));
    QHBoxLayout *hlayout1 = new QHBoxLayout;
    hlayout1->addWidget(mWarningLabel);
    hlayout1->addStretch();
    hlayout1->addWidget(buttonoptimize);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addLayout(hlayout);
    vlayout->addLayout(hlayout1);

    //connect
    connect(buttonoptimize, &QPushButton::clicked, this, &MultiObjectDialog::slotOptimize);
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

void MultiObjectDialog::slotOptimize()
{
    qDebug() << "MultiObjectDialog::slotOptimize";
    if(isParamError()){

    }
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
    QLabel *sizelabel = new QLabel(tr("Number of Particles: "), mGroup2);
    QLineEdit *sizeedit = mSizeEdit;

    //粒子群循环次数
    QLabel *timelabel = new QLabel(tr("Max iteration: "), mGroup2);
    QLineEdit *timeedit = mTimeEdit;

    //精英粒子群数目
    QLabel *elitelabel = new QLabel(tr("Number of eliet Particles: "), mGroup2);
    QLineEdit *eliteedit = mEliteEdit;

    //变异概率
    QLabel *ratelabel = new QLabel(tr("Mutation rate: "), mGroup2);
    QLineEdit *rateedit = mRateEdit;

    //w上界
    QLabel *wupperlabel = new QLabel(tr("Upper weight"), mGroup2);
    QLineEdit *wupperedit = mWUpperEdit;

    //w下界
    QLabel *wlowerlabel = new QLabel(tr("Lower weight: "), mGroup2);
    QLineEdit *wloweredit = mWLowerEdit;

    //c1
    QLabel *c1label = new QLabel(tr("c1: "), mGroup2);
    QLineEdit *c1edit = mC1Edit;

    //c2
    QLabel *c2label = new QLabel(tr("c2: "), mGroup2);
    QLineEdit *c2edit = mC2Edit;

    //layout
    QFormLayout *flayout = new QFormLayout(mGroup2);
    flayout->addRow(sizelabel, sizeedit);
    flayout->addRow(timelabel, timeedit);
    flayout->addRow(elitelabel, eliteedit);
    flayout->addRow(ratelabel, rateedit);
    flayout->addRow(wlowerlabel, wloweredit);
    flayout->addRow(wupperlabel, wupperedit);
    flayout->addRow(c1label, c1edit);
    flayout->addRow(c2label, c2edit);
}

bool MultiObjectDialog::isParamError()
{
    //判断输入参数是否为空
    if(mTargetWidget->getTargetList().size() == 0){
        mWarningLabel->setText(tr("Error: Target is Empty!"));
        return false;
    }
    mTargetWidget->setWarning("");
    if(mInputWidget->getInputValue().size() == 0){
        mWarningLabel->setText(tr("Error: Input Parameters is Empty!"));
        return false;
    }
    mInputWidget->setWarning("");

    bool isSizeInt, isTimeInt, isEliteInt, isRateDouble, isWUpperDouble, isWLowerDouble, isC1Double, isC2Double;
    double size = mSizeEdit->text().toInt(&isSizeInt);
    mTimeEdit->text().toInt(&isTimeInt);
    double elite = mEliteEdit->text().toInt(&isEliteInt);
    mRateEdit->text().toDouble(&isRateDouble);
    mWUpperEdit->text().toDouble(&isWUpperDouble);
    mWLowerEdit->text().toDouble(&isWLowerDouble);
    mC1Edit->text().toDouble(&isC1Double);
    mC2Edit->text().toDouble(&isC2Double);

    if(!isSizeInt){
        mWarningLabel->setText(tr("Error: Number of particles must be a number!"));
        return false;
    }
    if(!isTimeInt){
        mWarningLabel->setText(tr("Error: Max iteration must be a integer!"));
        return false;
    }
    if(!mEliteEdit){
        mWarningLabel->setText(tr("Error: Number of eliet Particles must be a integer!"));
        return false;
    }
    if(!isRateDouble){
        mWarningLabel->setText(tr("Error: Mutation rate must be a number!"));
        return false;
    }
    if(!isWUpperDouble){
        mWarningLabel->setText(tr("Error: Upper weight must be a number!"));
        return false;
    }
    if(!isWLowerDouble){
        mWarningLabel->setText(tr("Erorr: Lower weight must be a number!"));
        return false;
    }
    if(!isC1Double){
        mWarningLabel->setText(tr("Error: C1 must be a number!"));
        return false;
    }
    if(!isC2Double){
        mWarningLabel->setText(tr("Error: C2 must be a number!"));
        return false;
    }
    if(size <= elite){
        mWarningLabel->setText(tr("Error: The number of elite particle must be not more than Number of particles!"));
    }
    return true;
}
