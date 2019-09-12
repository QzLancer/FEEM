#ifndef BOUNCETAB_H
#define BOUNCETAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QString>

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
};

#endif // BOUNCETAB_H
