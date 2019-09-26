#include "itemdelegate.h"
#include <QComboBox>

ItemDelegate::ItemDelegate(QObject *parent)
    : QItemDelegate (parent)
{

}

QWidget * ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->addItem(tr("Maximize"));
    editor->addItem(tr("Minimize"));
    return editor;
}

void ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString text = index.model()->data(index, Qt::EditRole).toString();
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    int tindex = comboBox->findText(text);
    comboBox->setCurrentIndex(tindex);
}

void ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    QString text = comboBox->currentText();
    model->setData(index, text, Qt::EditRole);
}
