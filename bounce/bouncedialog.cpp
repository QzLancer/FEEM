#include "bouncedialog.h"
#include <QVBoxLayout>
#include "bouncecore.h"
#include <QDebug>
#include "plotwidget.h"
#include <QPalette>

BounceDialog::BounceDialog(QWidget *parent) : QDialog(parent),
    mTabWidget(new QTabWidget(this)),
    mRunButton(new QPushButton(tr("Run"), this)),
    mCancelButton(new QPushButton(tr("Cancel"), this)),
    mWarningLabel(new QLabel(this))
{    
    initialization();
}

BounceDialog::~BounceDialog()
{
    delete mWarningLabel;
    delete mCancelButton;
    delete mRunButton;
    delete mTabWidget;
}

void BounceDialog::Run()
{
//    qDebug() << "BounceDialog::Run() OK!";
    bool isError = 0;

    if(mLineEditMap.isEmpty()){
        qDebug() << "mLineEditMap is Empty!!";
    }

    for(auto iter = mLineEditMap.begin(); iter != mLineEditMap.end(); ++iter){
        QString stylesheet = iter.value()->styleSheet();
        if(iter.value()->text().isEmpty()){
            isError = 1;
//            iter.value()->setStyleSheet("border:1px solid red;");
            mWarningLabel->setText(tr("Error: Some properties are empty!"));
        }else{
            iter.value()->setStyleSheet("");
        }
    }

    if(isError == 0){
        mWarningLabel->setText(tr(""));
        bool isdouble;
        double data;
        for(auto iter = mLineEditMap.begin(); iter != mLineEditMap.end(); ++iter){
            data = iter.value()->text().toDouble(&isdouble);
            if(isdouble){
                mData.insert(iter.key(), data);
            }else{
                mWarningLabel->setText(tr("Error: Properties must be number!"));
                isError = 1;
            }
        }
    }

    if(isError == 0){
//        for(auto iter = mData.begin(); iter != mData.end(); ++iter){
//            qDebug() << iter.key() << iter.value();
//        }
        BounceCore core;
        core.Init("D:/FEEM/bounce/cosim3D_force.xlsx");
        core.initMaterialProperties(mData[tr("Open distance: ")], mData[tr("Stroke: ")], mData[tr("Moving contact mass: ")], mData[tr("Armature mass: ")]);
        core.initSpringReactionForce(mData[tr("Stiffness of overtravel spring: ")], mData[tr("Stiffness of return spring: ")], mData[tr("Pre-pressure of overtravel spring: ")], mData[tr("Pre-pressure of return spring: ")]);
        core.initCollisionContact(mData[tr("Stiffness: ")], mData[tr("Depth: ")], mData[tr("Damping: ")], mData[tr("Index: ")]);
        core.initSolveProperties(mData[tr("Initial time: ")], mData[tr("End time: ")], mData[tr("Step size: ")]);
        core.bounceCalculate();

//        QCustomPlot *cp = new QCustomPlot;
//        cp->setInteractions(MYQCP::iRangeZoom | MYQCP::iRangeDrag);
//        cp->show();
//        core.plot(cp);

        PlotWidget *pw = new PlotWidget;
//        pw->show();
//        core.plot(pw->mCustomPlot);
//        pw->mCustomPlot->replot();
        pw->addPlot(core.gett(), core.getxd());
        pw->addPlot(core.gett(), core.getxx());
        pw->show();
        close();
    }
}

void BounceDialog::initialization()
{
    setWindowTitle(tr("Bounce calculate"));

    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::red);
    mWarningLabel->setPalette(pe);

    addMaterialPropertiesTab();
    addSpringReactionForceTab();
    addCollisionContactTab();
    addSolvePropertiesTab();

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget(mRunButton);
    hlayout->addWidget(mCancelButton);

    QVBoxLayout *vlayout = new QVBoxLayout(/*this*/);
    setLayout(vlayout);
    vlayout->addWidget(mTabWidget);
    vlayout->addWidget(mWarningLabel);
    vlayout->addLayout(hlayout);

    connect(mCancelButton, &QPushButton::clicked, [this]{this->reject();});
    connect(mRunButton, &QPushButton::clicked, this, &BounceDialog::Run);
}

void BounceDialog::addMaterialPropertiesTab()
{
    BounceTab *materialpropertiestab = new BounceTab(this);
//    materialpropertiestab->addLine(tr("Open distance: "));
//    materialpropertiestab->addLine(tr("Stroke: "));
//    materialpropertiestab->addLine(tr("Moving contact mass: "));
//    materialpropertiestab->addLine(tr("Armature mass: "));

    materialpropertiestab->addLine("0.0017", tr("Open distance: "));
    materialpropertiestab->addLine("0.00268", tr("Stroke: "));
    materialpropertiestab->addLine("0.007", tr("Moving contact mass: "));
    materialpropertiestab->addLine("0.0129", tr("Armature mass: "));
    mTabWidget->addTab(materialpropertiestab, tr("Material properties"));
}

void BounceDialog::addSpringReactionForceTab()
{
    BounceTab *springreactionforcetab = new BounceTab(this);
//    springreactionforcetab->addLine(tr("Stiffness of overtravel spring: "));
//    springreactionforcetab->addLine(tr("Stiffness of return spring: "));
//    springreactionforcetab->addLine(tr("Pre-pressure of overtravel spring: "));
//    springreactionforcetab->addLine(tr("Pre-pressure of return spring: "));

    springreactionforcetab->addLine("13000", tr("Stiffness of overtravel spring: "));
    springreactionforcetab->addLine("370", tr("Stiffness of return spring: "));
    springreactionforcetab->addLine("7", tr("Pre-pressure of overtravel spring: "));
    springreactionforcetab->addLine("6", tr("Pre-pressure of return spring: "));
    mTabWidget->addTab(springreactionforcetab, tr("Spring reaction force"));
}

void BounceDialog::addCollisionContactTab()
{
    BounceTab *collisioncontacttab = new BounceTab(this);
//    collisioncontacttab->addLine(tr("Stiffness: "));
//    collisioncontacttab->addLine(tr("Depth: "));
//    collisioncontacttab->addLine(tr("Damping: "));
//    collisioncontacttab->addLine(tr("Index: "));

    collisioncontacttab->addLine("5.3e+8", tr("Stiffness: "));
    collisioncontacttab->addLine("0.0001", tr("Depth: "));
    collisioncontacttab->addLine("1000", tr("Damping: "));
    collisioncontacttab->addLine("1.5", tr("Index: "));
    mTabWidget->addTab(collisioncontacttab, tr("Collision contact"));
}

void BounceDialog::addSolvePropertiesTab()
{
    BounceTab *solvepropertiestab = new BounceTab(this);
//    solvepropertiestab->addLine(tr("Initial time: "));
//    solvepropertiestab->addLine(tr("End time: "));
//    solvepropertiestab->addLine(tr("Step size: "));

    solvepropertiestab->addLine("0", tr("Initial time: "));
    solvepropertiestab->addLine("0.08", tr("End time: "));
    solvepropertiestab->addLine("0.00001", tr("Step size: "));
    mTabWidget->addTab(solvepropertiestab, tr("Solve properties"));
}


