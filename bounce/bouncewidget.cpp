#include "bouncewidget.h"

BounceWidget::BounceWidget(QWidget *parent) : QMainWindow(parent),
    mTabWidget(new QTabWidget(this)),
    mInputDynamicCurve(new QWidget(this)),
    mMaterialPropoties(new QWidget(this))
{
    resize(800,600);
    setWindowTitle(tr("Bounce calculate"));
    setCentralWidget(mTabWidget);
    mTabWidget->addTab(mInputDynamicCurve, tr("Input dynamic curve"));
    mTabWidget->addTab(mMaterialPropoties, tr("mMaterialPropoties"));
}
