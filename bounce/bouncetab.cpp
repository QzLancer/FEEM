#include "bouncetab.h"
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include "bouncedialog.h"

BounceTab::BounceTab(BounceDialog *parent) : QWidget(parent),
    mLineEditMap(&parent->mLineEditMap)
{
    setLayout(&mFLayout);
}

void BounceTab::addLine(QString labelname)
{
    QLineEdit *lineedit = new QLineEdit(this);
    mLineEditMap->insert(labelname, lineedit);
    mFLayout.addRow(labelname, lineedit);
    mFLayout.setSpacing(10);
}
