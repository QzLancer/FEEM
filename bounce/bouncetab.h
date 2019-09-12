#ifndef BOUNCETAB_H
#define BOUNCETAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QString>
#include <QFormLayout>

class BounceTab : public QWidget
{
    Q_OBJECT
public:
    explicit BounceTab(QWidget *parent = nullptr);
    void addLine(QString labelname);

signals:

public slots:

private:
    QVBoxLayout mVLayout;
    QFormLayout mFLayout;
};

#endif // BOUNCETAB_H
