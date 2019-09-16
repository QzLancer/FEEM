#ifndef BOUNCEWIDGET_H
#define BOUNCEWIDGET_H

#include <QDialog>
#include <QWidget>
#include <QTabWidget>
#include "bouncetab.h"
#include <QPushButton>
#include <QString>
#include <QLineEdit>
#include <QMap>
#include <QLabel>

class BounceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BounceDialog(QWidget *parent = nullptr);
    ~BounceDialog();
    QMap<QString, QLineEdit*> mLineEditMap;

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
    QMap<QString, double> mData;
    QLabel *mWarningLabel;

//    QObjectList *mLineEditList;

//    QLineEdit *mOpenDistanceEdit;
//    QLineEdit *mStrokeEdit;
//    QLineEdit *mMovingContactMassEdit;
//    QLineEdit *mQualityMass;

//    QLineEdit *mStiffofOutofRangeSpring;
//    QLineEdit *mStiffofReturnSpring;
//    QLineEdit *mPrePressofOutofRangeSpring;
//    QLineEdit *mPrePressofReturnSpring;

};

#endif // BOUNCEWIDGET_H
