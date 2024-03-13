#include "PlayersModelDelegate.h"

PlayersModelDelegate::PlayersModelDelegate(QObject* parent)
    :
    QItemDelegate(parent)
{
}

// TableView need to create an Editor
// Create Editor when we construct PlayersModelDelegate
// and return the Editor
QWidget*
PlayersModelDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return nullptr;
}

// Then, we set the Editor
// Gets the data from Model and feeds the data to delegate Editor
void PlayersModelDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{

}

// When we modify data, this m_tableModel reflect the change
// Data from the delegate to the m_tableModel
void PlayersModelDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    model->setData(index, model->data(index).view<QVariant>(), Qt::EditRole);
}

// Give the SpinBox the info on size and location
void PlayersModelDelegate::updateEditorGeometry(QWidget* editor,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    editor->setGeometry(option.rect);
}
