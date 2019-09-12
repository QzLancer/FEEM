#include "bouncewidget.h"
#include <QVBoxLayout>
BounceWidget::BounceWidget(QWidget *parent) : QMainWindow(parent),
    mTabWidget(new QTabWidget(this)),
    mCentralWidget(new QWidget(this)),
    mRunButton(new QPushButton(tr("Run"), mCentralWidget))
{
    //    resize(800,600);
    setWindowTitle(tr("Bounce calculate"));

    addMaterialPropertiesTab();
    addSpringReactionForceTab();
    addCollisionContactTab();
    addSolvePropertiesTab();

    setCentralWidgetProperties();
    setCentralWidget(mCentralWidget);

}

void BounceWidget::addMaterialPropertiesTab()
{
    BounceTab *materialpropertiestab = new BounceTab(this);
    materialpropertiestab->addLine(tr("Open distance"));
    materialpropertiestab->addLine(tr("Stroke"));
    materialpropertiestab->addLine(tr("Moving contact mass"));
    materialpropertiestab->addLine(tr("Quality mass"));
    mTabWidget->addTab(materialpropertiestab, tr("Material properties"));
}

void BounceWidget::addSpringReactionForceTab()
{
    BounceTab *springreactionforcetab = new BounceTab(this);
    springreactionforcetab->addLine(tr("Stiffness of out-of-range spring"));
    springreactionforcetab->addLine(tr("Stiffness of return spring"));
    springreactionforcetab->addLine(tr("Pre-pressure of out-of-range spring"));
    springreactionforcetab->addLine(tr("Pre-pressure of return spring"));
    mTabWidget->addTab(springreactionforcetab, tr("Spring reaction force"));
}

void BounceWidget::addCollisionContactTab()
{
    BounceTab *collisioncontacttab = new BounceTab(this);
    collisioncontacttab->addLine(tr("Stiffness"));
    collisioncontacttab->addLine(tr("Depth"));
    collisioncontacttab->addLine(tr("Damping"));
    collisioncontacttab->addLine(tr("Index"));
    mTabWidget->addTab(collisioncontacttab, tr("Collision contact tab"));
}

void BounceWidget::addSolvePropertiesTab()
{
    BounceTab *solvepropertiestab = new BounceTab(this);
    solvepropertiestab->addLine(tr("Initial time"));
    solvepropertiestab->addLine(tr("End time"));
    solvepropertiestab->addLine(tr("Step size"));
    mTabWidget->addTab(solvepropertiestab, tr("Solve properties"));
}

void BounceWidget::setCentralWidgetProperties()
{
    QVBoxLayout *vlayout = new QVBoxLayout(mCentralWidget);
    vlayout->addWidget(mTabWidget);
    vlayout->addWidget(mRunButton);
}


