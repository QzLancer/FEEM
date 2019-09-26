#ifndef INPUTPARAMWIDGET_H
#define INPUTPARAMWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QComboBox>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QString>

class InputParamWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InputParamWidget(QWidget *parent = nullptr);
    QList<QList<double>> getInputValue() const;
    void setWarning(QString string);
    void setInputList(QStringList inputlist);


signals:

private slots:
    void slotAddTableItem();
    void slotChangeData(const QModelIndex &topleft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void slotDeleteTableItem();
private:
    //table中的数据
    QStandardItemModel *mInputModel;
    QItemSelectionModel *mInputSelection;
    QStringList mInputParamName;
    QList<QList<double>> mInputValue;
    QTableView *mInputTable;
    QComboBox *mInputBox;
    QLineEdit *mMaxInputEdit;
    QLineEdit *mMinInputEdit;
    QLabel *mWarningLabel;
    QGridLayout *mGridLayout;
    QPushButton *mDeleteButton;
    QPushButton *mAddButton;
    QStringList mInputList;

    void refreshTable();
};

#endif // INPUTPARAMWIDGET_H
