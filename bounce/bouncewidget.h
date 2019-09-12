#ifndef BOUNCEWIDGET_H
#define BOUNCEWIDGET_H

#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include "bouncetab.h"

class BounceWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit BounceWidget(QWidget *parent = nullptr);

signals:

public slots:

private:
    void addMaterialPropertiesTab();
    void addSpringForceTab();
    QTabWidget *mTabWidget;
    QWidget *mInputDynamicCurve;
    QWidget *mMaterialPropoties;

};

#endif // BOUNCEWIDGET_H
