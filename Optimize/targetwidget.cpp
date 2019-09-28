#include "targetwidget.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include "readonlydelegate.h"

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

    //委托构造
    ItemDelegate *itemdelegate = new ItemDelegate(this);
    mTargetTable->setItemDelegateForColumn(1, itemdelegate);
    ReadOnlyDelegate *readonlydelegate = new ReadOnlyDelegate(this);
    mTargetTable->setItemDelegateForColumn(0, readonlydelegate);


    //表头设置
    mTargetTable->resizeColumnsToContents();
    QStringList modeist;
    modeist << tr("Maximize") << tr("Minimize");
    mModeBox->addItems(modeist);

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


    //connect
    connect(mTargetAddButton, &QPushButton::clicked, this, &TargetWidget::slotAddTableItem);
    connect(mTargetDeleteButton, &QPushButton::clicked, this, &TargetWidget::slotDeleteTableItem);
    connect(mTargetModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
}

QStringList TargetWidget::getTargetList() const
{
    return mTargetList;
}

QStringList TargetWidget::getModeList() const
{
    return mModeList;
}

QMap<QString, QString> TargetWidget::getTargetModeMap() const
{
    return mTargetModeMap;
}

void TargetWidget::setTargetList(QStringList targetlist)
{
    mTargetList = targetlist;
    mTargetBox->addItems(mTargetList);
}

void TargetWidget::setModeList(QStringList modelist)
{
    mModeList = modelist;
    mModeBox->addItems(mModeList);
}

void TargetWidget::slotAddTableItem()
{
    qDebug() << "TargetWidget::slotAddTableItem";
    disconnect(mTargetModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
    QString target = mTargetBox->currentText();
    QString mode = mModeBox->currentText();
    if(!mTargetModeMap.contains(mTargetBox->currentText())){
        setWarning(tr(""));
        mTargetModeMap.insert(target, mode);
        refreshTable();
    }else{
        setWarning(tr("Error: Current Target Exist!"));
    }
    connect(mTargetModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
}

void TargetWidget::slotChangeData(const QModelIndex &topleft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    qDebug() << "TargetWidget::slotChangeData";
    disconnect(mTargetModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
    QString key = mTargetModel->item(topleft.row())->text();
    mTargetModeMap[key] = mTargetModel->itemFromIndex(topleft)->text();
//    qDebug() << mTargetModeMap;
    connect(mTargetModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
}

void TargetWidget::slotDeleteTableItem()
{
    qDebug() << "TargetWidget::slotDeleteTableItem";
    disconnect(mTargetModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
    QModelIndex index = mTargetTable->currentIndex();
//    qDebug() << index;
    if(index.row() != -1){
        int row = index.row();
        QString key = mTargetModel->item(row, 0)->text();
//        mTargetModel->removeRow(row);
        mTargetModeMap.remove(key);
        refreshTable();
    }
    connect(mTargetModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
}

void TargetWidget::setWarning(QString string)
{
    mTargetWarningLabel->setText(string);
}

void TargetWidget::refreshTable()
{
    mTargetModel->setRowCount(mTargetModeMap.size());
    int count = 0;
    for(auto i = mTargetModeMap.begin(); i != mTargetModeMap.end(); ++i){
        mTargetModel->setItem(count, 0, new QStandardItem(i.key()));
        mTargetModel->setItem(count, 1, new QStandardItem(i.value()));
        ++count;
    }
    mTargetTable->resizeColumnsToContents();
}
