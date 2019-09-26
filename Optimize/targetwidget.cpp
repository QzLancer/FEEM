#include "targetwidget.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

TargetWidget::TargetWidget(QWidget *parent)
    : QWidget(parent),
      mTargetTable(new QTableView((this))),
      mTargetModel(new QStandardItemModel(this)),
      mTargetSelection(new QItemSelectionModel(mTargetModel)),
      mTargetBox(new QComboBox(this)),
      mModeBox(new QComboBox(this)),
      mTargetDeleteButton(new QPushButton(tr("Delete"), this)),
      mTargetAddButton(new QPushButton(tr("Add"), this)),
      mTargetWarningLabel(new QLabel(this))

{
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::red);
    mTargetWarningLabel->setPalette(pe);

    //优化目标
    QLabel *targetlabel = new QLabel(tr("Target to be optimized"), this);
    QTableView *targettable = mTargetTable;
    QLabel *targetlabel1 = new QLabel(tr("Target to be optimized: "), this);
    QComboBox *targetbox = mTargetBox;
    QLabel *modelabel = new QLabel(tr("Optimize mode: "), this);
    QComboBox *modebox = mModeBox;

    mTargetTable->setModel(mTargetModel);
    QStringList inputlist;
    inputlist << tr("Target to be optimized") << tr("Optimize mode");
    mTargetModel->setHorizontalHeaderLabels(inputlist);
    mTargetTable->resizeColumnsToContents();

    //layout
    QGridLayout *glayout = new QGridLayout;
    glayout->addWidget(targetlabel, 0, 0);
    glayout->addWidget(targettable, 1, 0, 1, 2);
    glayout->addWidget(targetlabel1, 2, 0);
    glayout->addWidget(targetbox, 2, 1);
    glayout->addWidget(modelabel, 3, 0);
    glayout->addWidget(modebox, 3, 1);

    QHBoxLayout *buttonlayout = new QHBoxLayout;
    buttonlayout->addStretch();
    buttonlayout->addWidget(mTargetDeleteButton);
    buttonlayout->addWidget(mTargetAddButton);

    QVBoxLayout *targetlayout = new QVBoxLayout(this);
    targetlayout->addLayout(glayout);
    targetlayout->addLayout(buttonlayout);
    targetlayout->addWidget(mTargetWarningLabel);
}
