#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QGroupBox>
#include <QString>

class SingleObjectDialog : public QDialog
{
    Q_OBJECT

public:
    SingleObjectDialog(QWidget *parent = nullptr);
    ~SingleObjectDialog();
    void initialize();
    void initialize(QStringList inputlist, QStringList targetlist);

    //set
    void setInputList(QStringList inputlist);
    void setTargetList(QStringList targetlist);

    //get
    QStringList InputList();
    QStringList TargetList();

private:
    QGroupBox *mGroup1;
    QGroupBox *mGroup2;
    virtual void initializeGroup1();
    virtual void initializeGroup2();
    QStringList mInputList;
    QStringList mTargetList;
};

#endif // DIALOG_H
