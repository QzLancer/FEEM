#ifndef BOUNCEWIDGET_H
#define BOUNCEWIDGET_H

#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include "bouncetab.h"
#include <QPushButton>

class BounceWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit BounceWidget(QWidget *parent = nullptr);

signals:

public slots:

private:
    void addMaterialPropertiesTab();
    void addSpringReactionForceTab();
    void addCollisionContactTab();
    void addSolvePropertiesTab();

    void setCentralWidgetProperties();

    QTabWidget *mTabWidget;
    QWidget *mCentralWidget;
    QPushButton *mRunButton;
};

#endif // BOUNCEWIDGET_H
