#include "singleobjectdialog.h"
#include <QGridLayout>
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
      mGroup2(new QGroupBox(this)),
      mTargetBox(new QComboBox(mGroup1)),
      mInputBox(new QComboBox(mGroup1)),
      mInputModel(new QStandardItemModel),
      mInputSelection(new QItemSelectionModel(mInputModel)),
      mWarningLabel(new QLabel(mGroup1)),
      mSizeEdit(new QLineEdit(mGroup2)),
      mTimeEdit(new QLineEdit(mGroup2)),
      mRateEdit(new QLineEdit(mGroup2)),
      mWUpperEdit(new QLineEdit(mGroup2)),
      mWLowerEdit(new QLineEdit(mGroup2)),
      mC1Edit(new QLineEdit(mGroup2)),
      mC2Edit(new QLineEdit(mGroup2)),
      mWarningLabel1(new QLabel(mGroup2))
{
    initialize();
}

SingleObjectDialog::~SingleObjectDialog()
{
    delete mWarningLabel1;
    delete mC2Edit;
    delete mC1Edit;
    delete mWLowerEdit;
    delete mWUpperEdit;
    delete mRateEdit;
    delete mTimeEdit;
    delete mSizeEdit;
    delete mWarningLabel;
    delete mInputSelection;
    delete mInputModel;
    delete mInputBox;
    delete mTargetBox;
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

    QPushButton *buttonoptimize = new QPushButton(this);
    buttonoptimize->setText(tr("To optimize"));
    QHBoxLayout *hlayout1 = new QHBoxLayout;
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::red);
    mWarningLabel1->setPalette(pe);
    hlayout1->addWidget(mWarningLabel1);
    hlayout1->addStretch();
    hlayout1->addWidget(buttonoptimize);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addLayout(hlayout);
    vlayout->addLayout(hlayout1);

    connect(buttonoptimize, SIGNAL(clicked()), this, SLOT(slotOptimize()));
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
    mInputBox->addItems(mInputList);
}

void SingleObjectDialog::setTargetList(QStringList targetlist)
{
    mTargetList = targetlist;
    mTargetBox->addItems(mTargetList);
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

void SingleObjectDialog::slotAddTableItem(QString item, double minimum, double maximum)
{
    qDebug() << "SingleObjectDialog::slotAddTableItem";
    if(!mInputParamName.contains(item)){
        mInputParamName.append(item);
        QList<double> value;
        value << minimum << maximum;
        mInputValue.append(value);
        refreshTable();
    }
}

void SingleObjectDialog::slotChangeData(const QModelIndex &topleft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    disconnect(mInputModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
    qDebug() << "SingleObjectDialog::slotChangeData";
    int row = topleft.row();
    int column = topleft.column();
    bool isDouble;
    double data = mInputModel->data(topleft).toDouble(&isDouble);

    //判断输入是否为数字
    if(isDouble){
        //判断左侧输入值是否小于右侧
        QModelIndex index0 = mInputModel->index(row, 0);
        QModelIndex index1 = mInputModel->index(row, 1);
        if(mInputModel->data(index0) <= mInputModel->data(index1)){
            mWarningLabel->setText(tr(""));
            mInputValue[row][column] = data;
        }else{
            mWarningLabel->setText(tr("Error: The minimum must be less than the maximum!"));
//            mInputModel->setData(topleft, mInputValue[row][column]);
            mInputModel->setItem(row, 0 ,new QStandardItem(QString::number(mInputValue[row][0])));
            mInputModel->setItem(row, 1 ,new QStandardItem(QString::number(mInputValue[row][1])));
        }
    }else{
        mWarningLabel->setText(tr("Error: Properties must be number!"));
        mInputModel->setData(topleft, mInputValue[row][column]);
    }
    connect(mInputModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
}

void SingleObjectDialog::slotOptimize()
{
    if(isParamError()){
        qDebug() << "Input parameter OK!";
        //将input parameter转换成可以传递的形式
        double *lower = new double[static_cast<unsigned long long>(mInputValue.size())];
        double *upper = new double[static_cast<unsigned long long>(mInputValue.size())];
        double *vmax = new double[static_cast<unsigned long long>(mInputValue.size())]; //粒子最大速度
        for(int i = 0; i < mInputValue.size(); ++i){
            lower[i] = mInputValue[i][0];
            upper[i] = mInputValue[i][1];
            vmax[i] = 0.1;
        }
        //读取各个lineedit中的数据，目前变异概率不使用
        int numberOfParticles = mSizeEdit->text().toInt();
        int numberOfVariables = mInputValue.size();
        int maxIteration = mTimeEdit->text().toInt();
        double lowerWeight = mWLowerEdit->text().toDouble();
        double upperWeight = mWUpperEdit->text().toDouble();
        double c1 = mC1Edit->text().toDouble();
        double c2 = mC2Edit->text().toDouble();
        QString stoppingCriteria = "none";
        QString psoType = "Classic";
        double threshold = 0.0001;

        //观察输入参数
        qDebug() << "lower: " << lower[0] << lower[1];
        qDebug() << "upper: " << upper[0] << upper[1];
        qDebug() << "vmax: " << vmax[0] << vmax[1];
        qDebug() << "numberOfParticles: " << numberOfParticles;
        qDebug() << "numberOfVariables: " << numberOfVariables;
        qDebug() << "maxIteration: " << maxIteration;
        qDebug() << "lowerWeight: " << lowerWeight;
        qDebug() << "upperWeight: " << upperWeight;
        qDebug() << "c1: " << c1;
        qDebug() << "c2: " << c2;

        PSO pso(numberOfParticles, numberOfVariables, lower, upper, vmax, SingleObjectDialog::objectiveFunction, lowerWeight, upperWeight, maxIteration, c1, c2, threshold, stoppingCriteria, psoType);

        pso.optimize();

        pso.printBest();

        delete[] vmax;
        delete[] upper;
        delete[] lower;
    }
}

void SingleObjectDialog::initializeGroup1()
{
    mGroup1->setTitle(tr("Target to be optimized && input parameters"));

    //优化目标
    QLabel *targetlabel = new QLabel(tr("Target to be optimized"), mGroup1);
    QLabel *modelabel = new QLabel(tr("Optimize Mode"), mGroup1);
    QComboBox *targetbox = mTargetBox;
    QComboBox *modebox = new QComboBox(mGroup1);
    modebox->addItem(tr("Minimize"));
    modebox->addItem(tr("Maximize"));

    //输入参数
    QLabel *inputparamlabel = new QLabel(tr("Input parameters"), mGroup1);
    QTableView *inputparamtable = new QTableView(mGroup1);
    QLabel *inputparamlabel1 = new QLabel(tr("Input parameters"), mGroup1);
    QComboBox *inputparambox = mInputBox;
    QLabel *mininputparamlabel = new QLabel(tr("Minimum of input parameters: "), mGroup1);
    QLineEdit *mininputparamedit = new QLineEdit(mGroup1);
    QLabel *maxinputparamlabel = new QLabel(tr("Maximum of input parameters: "), mGroup1);
    QLineEdit *maxinputparamedit = new QLineEdit(mGroup1);
    QLabel *warninglabel = mWarningLabel;
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::red);
    mWarningLabel->setPalette(pe);
    QPushButton *addbutton = new QPushButton(tr("Add"), mGroup1);
    QPushButton *deletebutton = new QPushButton(tr("Delete"), mGroup1);

    //表头初始化
    QStringList inputlist;
    inputlist << tr("Minimum of input parameters") << tr("Maximum of input parameters");
    mInputModel->setHorizontalHeaderLabels(inputlist);
    inputparamtable->showGrid();
    inputparamtable->setModel(mInputModel);

    //添加表格元素
    connect(addbutton, &QPushButton::clicked, [this, inputparambox, mininputparamedit, maxinputparamedit]{
        disconnect(mInputModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
        bool isMinDouble = 0;
        bool isMaxDouble = 0;
        double minimum = mininputparamedit->text().toDouble(&isMinDouble);
        double maximum = maxinputparamedit->text().toDouble(&isMaxDouble);
        if(isMinDouble && isMaxDouble){
            if(minimum <= maximum){
                mWarningLabel->setText("");
                slotAddTableItem(inputparambox->currentText(), mininputparamedit->text().toDouble(), maxinputparamedit->text().toDouble());
            }else{
                mWarningLabel->setText(tr("Error: The minimum must be less than the maximum!"));
            }
        }else{
            mWarningLabel->setText(tr("Error: Properties must be number!"));
        }
        connect(mInputModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
    });
    //修改表格数据之后修改对应List
    connect(mInputModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));

    //删除表格的某一行
    //layout
    QGridLayout *gridlayout = new QGridLayout;
    gridlayout->addWidget(targetlabel, 0, 0);
    gridlayout->addWidget(targetbox, 0, 1);
    gridlayout->addWidget(modelabel, 1 ,0);
    gridlayout->addWidget(modebox, 1, 1);

    gridlayout->addWidget(inputparamlabel, 2, 0);
    gridlayout->addWidget(inputparamtable, 3, 0, 1, 2);

    gridlayout->addWidget(inputparamlabel1, 4, 0);
    gridlayout->addWidget(inputparambox, 4, 1);
    gridlayout->addWidget(mininputparamlabel, 5, 0);
    gridlayout->addWidget(mininputparamedit, 5, 1);
    gridlayout->addWidget(maxinputparamlabel, 6, 0);
    gridlayout->addWidget(maxinputparamedit, 6, 1);
//    gridlayout->addWidget(addbutton, 7, 1);
//    gridlayout->addWidget(deletebutton, 7, 0);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addStretch();
    hlayout->addWidget(deletebutton);
    hlayout->addWidget(addbutton);

    QVBoxLayout *vlayout = new QVBoxLayout(mGroup1);
    vlayout->addLayout(gridlayout);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(warninglabel);

    //table setting
    inputparamtable->setShowGrid(true);

}

void SingleObjectDialog::initializeGroup2()
{
    mGroup2->setTitle(tr("Single target PSO parameters"));

    //种群大小
    QLabel *sizelabel = new QLabel(tr("Number of Particles: "), mGroup2);
    QLineEdit *sizeedit = mSizeEdit;

    //粒子群循环次数
    QLabel *timelabel = new QLabel(tr("Max iteration: "), mGroup2);
    QLineEdit *timeedit = mTimeEdit;

    //变异概率
    QLabel *ratelabel = new QLabel(tr("Mutation rate: "), mGroup2);
    QLineEdit *rateedit = mRateEdit;

    //w上界
    QLabel *wupperlabel = new QLabel(tr("Upper weight: "), mGroup2);
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
    flayout->addRow(ratelabel, rateedit);
    flayout->addRow(wupperlabel, wupperedit);
    flayout->addRow(wlowerlabel, wloweredit);
    flayout->addRow(c1label, c1edit);
    flayout->addRow(c2label, c2edit);

//    QVBoxLayout *hlayout = new QVBoxLayout(mGroup2);
//    hlayout->addLayout(flayout);
//    hlayout->addWidget(mWarningLabel1);

}

void SingleObjectDialog::refreshTable()
{
    mInputModel->setRowCount(mInputParamName.size());
    for(int i = 0; i < mInputParamName.size(); ++i){
        qDebug() << mInputParamName[i];
        mInputModel->setHeaderData(i, Qt::Vertical, mInputParamName[i]);
    }
    for(int i = 0; i < mInputValue.size(); ++i){
        qDebug() << mInputValue;
        mInputModel->setItem(i, 0, new QStandardItem(QString::number(mInputValue[i][0])));
        mInputModel->setItem(i, 1, new QStandardItem(QString::number(mInputValue[i][1])));
    }
}

bool SingleObjectDialog::isParamError()
{
    //判断输入参数是否为空
    if(mInputValue.size() == 0){
        mWarningLabel1->setText(tr("Error: Input Parameters is Empty!"));
        return false;
    }

    bool isSizeInt, isTimeInt, isRateDouble, isWUpperDouble, isWLowerDouble, isC1Double, isC2Double;
    mSizeEdit->text().toInt(&isSizeInt);
    mTimeEdit->text().toInt(&isTimeInt);
    mRateEdit->text().toDouble(&isRateDouble);
    mWUpperEdit->text().toDouble(&isWUpperDouble);
    mWLowerEdit->text().toDouble(&isWLowerDouble);
    mC1Edit->text().toDouble(&isC1Double);
    mC2Edit->text().toDouble(&isC2Double);

    if(!isSizeInt){
        mWarningLabel1->setText(tr("Error: Number of particles must be a number!"));
        return false;
    }
    if(!isTimeInt){
        mWarningLabel1->setText(tr("Error: Max iteration must be a integer!"));
        return false;
    }
    if(!isRateDouble){
        mWarningLabel1->setText(tr("Error: Mutation rate must be a number!"));
        return false;
    }
    if(!isWUpperDouble){
        mWarningLabel1->setText(tr("Error: Upper weight must be a number!"));
        return false;
    }
    if(!isWLowerDouble){
        mWarningLabel1->setText(tr("Erorr: Lower weight must be a number!"));
        return false;
    }
    if(!isC1Double){
        mWarningLabel1->setText(tr("Error: C1 must be a number!"));
        return false;
    }
    if(!isC2Double){
        mWarningLabel1->setText(tr("Error: C2 must be a number!"));
        return false;
    }
    return true;
}

void SingleObjectDialog::objectiveFunction(Particle *Particle)
{
    const double *_position = Particle->getPosition ();
    double _constraits;

    if (Particle::getNumberOfVariables () != 2) {
        throw "Number of variables in Particles does not match number of variables in objective function" ;
    }

    Particle->setValue(100 * pow((_position[1] - pow(_position[0], 2)), 2) + pow((1 - _position[0]), 2));
    _constraits = _position[1] + _position[0] > 1 ? 10000000 * (_position[1] + _position[0] - 1) : 0;
    //_constraits = 0;
    Particle->setConstraits(_constraits);
    Particle->setFeasible(_constraits ? false : true);
    return;
}

