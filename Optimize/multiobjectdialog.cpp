#include "multiobjectdialog.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QGridLayout>
#include <QTableView>
#include <QDebug>
#include <QStringList>

MultiObjectDialog::MultiObjectDialog(QWidget *parent)
    : QDialog (parent),
      mGroup1(new QGroupBox(this)),
      mGroup2(new QGroupBox(this)),
      mInputWidget(new InputParamWidget(mGroup1)),
      mTargetWidget(new TargetWidget(mGroup1)),
      mWarningLabel(new QLabel(mGroup2)),
      mSizeEdit(new QLineEdit("50", mGroup2)),
      mTimeEdit(new QLineEdit("100", mGroup2)),
      mEliteEdit(new QLineEdit("50", mGroup2)),
      mRateEdit(new QLineEdit("0.1", mGroup2)),
      mWLowerEdit(new QLineEdit("0.3", mGroup2)),
      mWUpperEdit(new QLineEdit("0.4", mGroup2)),
      mC1Edit(new QLineEdit("1.5", mGroup2)),
      mC2Edit(new QLineEdit("1.5", mGroup2))
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
        qDebug() << "Input parameter OK!";
        //optimize mode
        QStringList TargetMode;
        QMap<QString, QString> TargetModeMap = mTargetWidget->getTargetModeMap();
        for(auto iter = TargetModeMap.begin(); iter != TargetModeMap.end(); ++iter){
            qDebug() << iter.key() << iter.value();
            mTargetName.append(iter.key());
            TargetMode.append(iter.value());
        }
        mInputName = mInputWidget->getInputName();
        //将input parameter转换成可以传递的形式
        QList<QList<double>> InputValue = mInputWidget->getInputValue();
        double *lower = new double[static_cast<unsigned long long>(InputValue.size())];
        double *upper = new double[static_cast<unsigned long long>(InputValue.size())];
        double *vmax = new double[static_cast<unsigned long long>(InputValue.size())]; //粒子最大速度
        for(int i = 0; i < InputValue.size(); ++i){
            lower[i] = InputValue[i][0];
            upper[i] = InputValue[i][1];
            vmax[i] = 0.5;
        }
        //读取各个lineedit中的数据，目前变异概率不使用
        int numberOfParticles = mSizeEdit->text().toInt();
        int numberOfExtraParticles = mEliteEdit->text().toInt();
        int numberOfVariables = InputValue.size();
        int numberOfObjectives = TargetMode.size();
        int maxIteration = mTimeEdit->text().toInt();
        double lowerWeight = mWLowerEdit->text().toDouble();
        double upperWeight = mWUpperEdit->text().toDouble();
        double c1 = mC1Edit->text().toDouble();
        double c2 = mC2Edit->text().toDouble();
        QString stoppingCriteria = "none";
        QString psoType = "Classic";
        double vari = 0.05;

        //观察输入参数
        qDebug() << "lower: " << lower[0] << lower[1];
        qDebug() << "upper: " << upper[0] << upper[1];
        qDebug() << "vmax: " << vmax[0] << vmax[1];
        qDebug() << "numberOfParticles: " << numberOfParticles;
        qDebug() << "numberOfExtraParticles: " << numberOfExtraParticles;
        qDebug() << "numberOfVariables: " << numberOfVariables;
        qDebug() << "numberOfobjectives: " << numberOfObjectives;
        qDebug() << "maxIteration: " << maxIteration;
        qDebug() << "lowerWeight: " << lowerWeight;
        qDebug() << "upperWeight: " << upperWeight;
        qDebug() << "c1: " << c1;
        qDebug() << "c2: " << c2;

        PSO pso(numberOfParticles, numberOfVariables, numberOfObjectives, numberOfExtraParticles, lower, upper, vmax, MultiObjectDialog::objectiveFunction, lowerWeight, upperWeight, maxIteration, c1, c2, vari, TargetMode, stoppingCriteria, psoType);
        pso.optimize();
        pso.printBest();
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

    //w下界
    QLabel *wlowerlabel = new QLabel(tr("Lower weight: "), mGroup2);
    QLineEdit *wloweredit = mWLowerEdit;

    //w上界
    QLabel *wupperlabel = new QLabel(tr("Upper weight"), mGroup2);
    QLineEdit *wupperedit = mWUpperEdit;

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
    if(!isWLowerDouble){
        mWarningLabel->setText(tr("Erorr: Lower weight must be a number!"));
        return false;
    }
    if(!isWUpperDouble){
        mWarningLabel->setText(tr("Error: Upper weight must be a number!"));
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
    return true;
}

void MultiObjectDialog::objectiveFunction(Particle *Particle)
{
    const double *_position = Particle->getPosition();
    int numberOfObjectives = Particle->getNumberOfObjective();
    double *Objective;
    Objective = new double [numberOfObjectives];
    int _constraits;

    Objective[0] = _position[0] * _position[0] + _position[1] * _position[1];
    Objective[1] = (1 - _position[0]) * (1 - _position[0]) + (1 - _position[1]) * (1 - _position[1]);

    Particle->setValue(Objective);
    _constraits = 0;
    Particle->setConstraits(_constraits);
    Particle->setFeasible(_constraits ? false : true);
    delete[] Objective;
    return;
}
