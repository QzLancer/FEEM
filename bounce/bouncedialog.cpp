#include "bouncedialog.h"
#include <QVBoxLayout>
#include "bouncecore.h"
#include <QDebug>
#include "qcustomplot.h"

BounceDialog::BounceDialog(QWidget *parent) : QDialog(parent),
    mTabWidget(new QTabWidget(this)),
    mRunButton(new QPushButton(tr("Run"), this)),
    mCancelButton(new QPushButton(tr("Cancel"), this))
{    
    initialization();
}

BounceDialog::~BounceDialog()
{
    delete mCancelButton;
    delete mRunButton;
    delete mTabWidget;
}

void BounceDialog::Run()
{
//    qDebug() << "BounceDialog::Run() OK!";
    bool hasEmptyLine = 0;
    if(mLineEditMap.isEmpty()){
        qDebug() << "mLineEditMap is Empty!!";
    }

    for(auto iter = mLineEditMap.begin(); iter != mLineEditMap.end(); ++iter){
        QString stylesheet = iter.value()->styleSheet();
        if(iter.value()->text().isEmpty()){
            hasEmptyLine = 1;
            iter.value()->setStyleSheet("border:1px solid red;");
        }else{
            iter.value()->setStyleSheet("");
        }
    }

    if(hasEmptyLine == 0){
        for(auto iter = mLineEditMap.begin(); iter != mLineEditMap.end(); ++iter){
            mData.insert(iter.key(), iter.value()->text().toDouble());
        }

        BounceCore core;
        core.Init("D:/FEEM/bounce/cosim3D_force.xlsx");
//        core.initMaterialProperties(mData[tr("Open distance: ")], mData[tr("Stroke: ")], mData[tr("Moving contact mass: ")], mData[tr("Armature mass: ")]);
//        core.initSpringReactionForce(mData[tr("Stiffness of overtravel spring: ")], mData[tr("Stiffness of return spring: ")], mData[tr("Pre-pressure of overtravel spring: ")], mData[tr("Pre-pressure of return spring: ")]);
//        core.initCollisionContact(mData[tr("Stiffness: ")], mData[tr("Depth: ")], mData[tr("Damping: ")], mData[tr("Index: ")]);
//        core.initSolveProperties(mData[tr("Initial time: ")], mData[tr("End time: ")], mData[tr("Step size: ")]);
        core.bounceCalculate();

        QCustomPlot *cp = new QCustomPlot;
        cp->show();
        core.plot(cp);

        close();
    }

}

void BounceDialog::initialization()
{
    setWindowTitle(tr("Bounce calculate"));

    addMaterialPropertiesTab();
    addSpringReactionForceTab();
    addCollisionContactTab();
    addSolvePropertiesTab();

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget(mRunButton);
    hlayout->addWidget(mCancelButton);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addWidget(mTabWidget);
    vlayout->addLayout(hlayout);

    connect(mCancelButton, &QPushButton::clicked, [this]{this->reject();});
    connect(mRunButton, &QPushButton::clicked, this, &BounceDialog::Run);
}

void BounceDialog::addMaterialPropertiesTab()
{
    BounceTab *materialpropertiestab = new BounceTab(this);
    materialpropertiestab->addLine(tr("Open distance: "));
    materialpropertiestab->addLine(tr("Stroke: "));
    materialpropertiestab->addLine(tr("Moving contact mass: "));
    materialpropertiestab->addLine(tr("Armature mass: "));
    mTabWidget->addTab(materialpropertiestab, tr("Material properties"));
}

void BounceDialog::addSpringReactionForceTab()
{
    BounceTab *springreactionforcetab = new BounceTab(this);
    springreactionforcetab->addLine(tr("Stiffness of overtravel spring: "));
    springreactionforcetab->addLine(tr("Stiffness of return spring: "));
    springreactionforcetab->addLine(tr("Pre-pressure of overtralve spring: "));
    springreactionforcetab->addLine(tr("Pre-pressure of return spring: "));
    mTabWidget->addTab(springreactionforcetab, tr("Spring reaction force"));
}

void BounceDialog::addCollisionContactTab()
{
    BounceTab *collisioncontacttab = new BounceTab(this);
    collisioncontacttab->addLine(tr("Stiffness: "));
    collisioncontacttab->addLine(tr("Depth: "));
    collisioncontacttab->addLine(tr("Damping: "));
    collisioncontacttab->addLine(tr("Index: "));
    mTabWidget->addTab(collisioncontacttab, tr("Collision contact"));
}

void BounceDialog::addSolvePropertiesTab()
{
    BounceTab *solvepropertiestab = new BounceTab(this);
    solvepropertiestab->addLine(tr("Initial time: "));
    solvepropertiestab->addLine(tr("End time: "));
    solvepropertiestab->addLine(tr("Step size: "));
    mTabWidget->addTab(solvepropertiestab, tr("Solve properties"));
}


