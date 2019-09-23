#ifndef MULTIOBJECTDIALOG_H
#define MULTIOBJECTDIALOG_H
#include <QDialog>
#include <QGroupBox>
#include <QList>
#include <QString>

class MultiObjectDialog : public QDialog
{
public:
    MultiObjectDialog(QWidget *parent = nullptr);
    ~MultiObjectDialog();
    void initialize();
    void initialize(QStringList inputlist, QStringList targetlist);

    //set
    void setInputList(QStringList inputlist);
    void setTargetList(QStringList targetlist);

    //get
    QStringList InputList();
    QStringList TargetList();

private:
    void initializeGroup1();
    void initializeGroup2();
    QGroupBox *mGroup1;
    QGroupBox *mGroup2;
    QStringList mInputList;
    QStringList mTargetList;
};

#endif // MULTIOBJECTDIALOG_H
