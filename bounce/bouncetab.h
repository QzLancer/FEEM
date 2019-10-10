#ifndef BOUNCETAB_H
#define BOUNCETAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QString>
#include <QFormLayout>
#include <QLineEdit>
#include <QMap>

class BounceDialog;

class BounceTab : public QWidget
{
    Q_OBJECT
public:
    explicit BounceTab(BounceDialog *parent = nullptr);
    void addLine(QString labelname);
    void addLine(QString initialstr, QString labelname);

signals:

public slots:

private:
    QVBoxLayout mVLayout;
    QFormLayout mFLayout;
    QMap<QString, QLineEdit*> *mLineEditMap;
};

#endif // BOUNCETAB_H
