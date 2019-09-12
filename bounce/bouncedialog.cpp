#include "bouncedialog.h"
#include <QVBoxLayout>
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

    connect(mCancelButton, &QPushButton::clicked, [this]{this->close();});
    connect(mRunButton, &QPushButton::clicked, this, &BounceDialog::Run);
}

void BounceDialog::addMaterialPropertiesTab()
{
    BounceTab *materialpropertiestab = new BounceTab(this);
    materialpropertiestab->addLine(tr("Open distance: "));
    materialpropertiestab->addLine(tr("Stroke: "));
    materialpropertiestab->addLine(tr("Moving contact mass: "));
    materialpropertiestab->addLine(tr("Quality mass: "));
    mTabWidget->addTab(materialpropertiestab, tr("Material properties"));
}

void BounceDialog::addSpringReactionForceTab()
{
    BounceTab *springreactionforcetab = new BounceTab(this);
    springreactionforcetab->addLine(tr("Stiffness of out-of-range spring: "));
    springreactionforcetab->addLine(tr("Stiffness of return spring: "));
    springreactionforcetab->addLine(tr("Pre-pressure of out-of-range spring: "));
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


