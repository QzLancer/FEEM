#include "inputparamwidget.h"
#include <QPalette>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

InputParamWidget::InputParamWidget(QWidget *parent) :
    QWidget(parent),
    mInputModel(new QStandardItemModel(this)),
    mInputSelection(new QItemSelectionModel(mInputModel)),
    mInputTable(new QTableView(this)),
    mInputBox(new QComboBox(this)),
    mMaxInputEdit(new QLineEdit(this)),
    mMinInputEdit(new QLineEdit(this)),
    mWarningLabel(new QLabel(this)),
    mGridLayout(new QGridLayout),
    mDeleteButton(new QPushButton(tr("Delete"), this)),
    mAddButton(new QPushButton(tr("Add"), this))
{
    //label设置
    QLabel *InputParamLabel0 = new QLabel(tr("Input parameters"), this);
    QLabel *InputParamLabel1 = new QLabel(tr("Input parameters"), this);
    QLabel *InputParamMin = new QLabel(tr("Minimum of input parameters: "), this);
    QLabel *InputParamMax = new QLabel(tr("Maximum of input Parameters: "), this);
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::red);
    mWarningLabel->setPalette(pe);

    //设置table
    QStringList inputlist;
    inputlist << tr("Minimum of input parameters") << tr("Maximum of input parameters");
    mInputModel->setHorizontalHeaderLabels(inputlist);
    mInputTable->showGrid();
    mInputTable->setModel(mInputModel);

    //设置layout
    mGridLayout->addWidget(InputParamLabel0, 0, 0);
    mGridLayout->addWidget(mInputTable, 1, 0, 1, 2);
    mGridLayout->addWidget(InputParamLabel1, 2, 0);
    mGridLayout->addWidget(mInputBox, 2, 1);
    mGridLayout->addWidget(InputParamMin, 3, 0);
    mGridLayout->addWidget(mMinInputEdit, 3, 1);
    mGridLayout->addWidget(InputParamMax, 4, 0);
    mGridLayout->addWidget(mMaxInputEdit, 4, 1);
    QHBoxLayout *HLayout = new QHBoxLayout;
    HLayout->addStretch();
    HLayout->addWidget(mDeleteButton);
    HLayout->addWidget(mAddButton);
    QVBoxLayout *VLayout = new QVBoxLayout(this);
    VLayout->addLayout(mGridLayout);
    VLayout->addLayout(HLayout);
    VLayout->addWidget(mWarningLabel);

    //配置signal和slot
    connect(mAddButton, &QPushButton::clicked, this, &InputParamWidget::slotAddTableItem);
    connect(mInputModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
    connect(mDeleteButton, &QPushButton::clicked, this, &InputParamWidget::slotDeleteTableItem);
}

void InputParamWidget::setWarning(QString string)
{
    mWarningLabel->setText(string);
}

void InputParamWidget::setInputList(QStringList inputlist)
{
    mInputList = inputlist;
    mInputBox->addItems(mInputList);
}

void InputParamWidget::slotAddTableItem()
{
    qDebug() << "InputParamWidget::slotAddTableItem";
    disconnect(mInputModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
    //输入错误检查
    bool isMinDouble = 0;
    bool isMaxDouble = 0;
    double minimum = mMinInputEdit->text().toDouble(&isMinDouble);
    double maximum = mMaxInputEdit->text().toDouble(&isMaxDouble);
    if(isMinDouble && isMaxDouble){
        if(minimum <= maximum){
            setWarning("");
            //将数据存储并在表格中显示
            QString item = mInputBox->currentText();
            if(!mInputParamName.contains(item)){
                mInputParamName.append(item);
                QList<double> value;
                value << minimum << maximum;
                mInputValue.append(value);
                refreshTable();
            }
        }else{
            setWarning(tr("Error: The minimum must be less than the maximum!"));
        }
    }else{
        setWarning(tr("Error: Properties must be number!"));
    }
    connect(mInputModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotChangeData(QModelIndex,QModelIndex,QVector<int>)));
}

void InputParamWidget::slotChangeData(const QModelIndex &topleft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    qDebug() << "InputParamWidget::slotChangeData";

}

void InputParamWidget::slotDeleteTableItem()
{
    qDebug() << "InputParamWidget::slotDeleteTableItem";
}

void InputParamWidget::refreshTable()
{
    mInputModel->setRowCount(mInputParamName.size());
    for(int i = 0; i < mInputParamName.size(); ++i){
        qDebug() << mInputParamName[i];
        mInputModel->setHeaderData(i, Qt::Vertical, mInputParamName[i]);
    }
    for(int i = 0; i < mInputValue.size(); ++i){
        qDebug() << mInputValue;
        mInputModel->setItem(i, 0, new QStandardItem(QString::number(mInputValue[i][0])));
        mInputModel->setItem(i, 1, new QStandardItem(QString::number(mInputValue[i][1])));
    }
}