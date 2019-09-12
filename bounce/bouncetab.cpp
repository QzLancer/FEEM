#include "bouncetab.h"
#include <QLabel>
#include <QlineEdit>
#include <QHBoxLayout>

BounceTab::BounceTab(QWidget *parent) : QWidget(parent)
{
    setLayout(&mVLayout);
}

void BounceTab::addLine(QString labelname)
{
    QHBoxLayout *hlayout = new QHBoxLayout;
    QLabel *label = new QLabel(labelname, this);
    QLineEdit *lineedit = new QLineEdit(this);
    hlayout->addWidget(label);
    hlayout->addWidget(lineedit);
    mVLayout.addLayout(hlayout);
}
