#ifndef TARGETWIDGET_H
#define TARGETWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class TargetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TargetWidget(QWidget *parent = nullptr);

    enum OptimizeMode{
        MINIMIZE,
        MAXIMIZE,
    };

signals:

public slots:

private:
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
    QMap<QString, OptimizeMode> mTargetModeMap;
};

#endif // TARGETWIDGET_H
