#ifndef TARGETWIDGET_H
#define TARGETWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "itemdelegate.h"

class TargetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TargetWidget(QWidget *parent = nullptr);

    //getter
    QStringList getTargetList() const;
    QStringList getModeList() const;
    QMap<QString, QString> getTargetModeMap() const;
    //setter
    void setTargetList(QStringList targetlist);
    void setModeList(QStringList modelist);
    void setWarning(QString string);
    enum OptimizeMode{
        MINIMIZE,
        MAXIMIZE,
    };

signals:

private slots:
    void slotAddTableItem();
    void slotChangeData(const QModelIndex &topleft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void slotDeleteTableItem();

private:
    void refreshTable();
    //target控件
    QTableView *mTargetTable;
    QStandardItemModel *mTargetModel;
    QItemSelectionModel *mTargetSelection;
    QComboBox *mTargetBox;
    QComboBox *mModeBox;
    QPushButton *mTargetDeleteButton;
    QPushButton *mTargetAddButton;
    QLabel *mTargetWarningLabel;

    //ComboBox中的列表
    QStringList mTargetList;
    QStringList mModeList;
    //Table中被添加的选项
    QMap<QString, QString> mTargetModeMap;
};

#endif // TARGETWIDGET_H
