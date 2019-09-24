#include <QApplication>
#include "singleobjectdialog.h"
#include "multiobjectdialog.h"
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SingleObjectDialog w;
    QStringList InputList;
    QStringList TargetList;
    InputList << "Input1" << "Input2" << "Input3";
    TargetList << "Target1" << "Target2" << "Target3" << "Target4";
    w.setInputList(InputList);
    w.setTargetList(TargetList);
//    MultiObjectDialog m;
//    m.show();

    w.show();
    return a.exec();
}
