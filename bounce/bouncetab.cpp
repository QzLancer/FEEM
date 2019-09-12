#include "bouncetab.h"
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>

BounceTab::BounceTab(QWidget *parent) : QWidget(parent)
{
    setLayout(&mFLayout);
}

void BounceTab::addLine(QString labelname)
{
//    QHBoxLayout *hlayout = new QHBoxLayout;
//    QLabel *label = new QLabel(labelname, this);
//    QLineEdit *lineedit = new QLineEdit(this);
//    hlayout->addWidget(label);
//    hlayout->addWidget(lineedit);
//    mVLayout.addLayout(hlayout);


    QLineEdit *lineedit = new QLineEdit(this);
    mFLayout.addRow(labelname, lineedit);
    mFLayout.setSpacing(10);


}
