#ifndef BOUNCEWIDGET_H
#define BOUNCEWIDGET_H

#include <QDialog>
#include <QWidget>
#include <QTabWidget>
#include "bouncetab.h"
#include <QPushButton>

class BounceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BounceDialog(QWidget *parent = nullptr);
    ~BounceDialog();

signals:

public slots:
    void Run();
private:
    void initialization();
    void addMaterialPropertiesTab();
    void addSpringReactionForceTab();
    void addCollisionContactTab();
    void addSolvePropertiesTab();

    QTabWidget *mTabWidget;
    QPushButton *mRunButton;
    QPushButton *mCancelButton;
};

#endif // BOUNCEWIDGET_H
