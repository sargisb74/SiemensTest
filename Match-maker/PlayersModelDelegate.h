#ifndef PlayersModelDelegate_H
#define PlayersModelDelegate_H

#include <QItemDelegate>

class PlayersModelDelegate : public QItemDelegate
{
 Q_OBJECT
 public:
    explicit PlayersModelDelegate(QObject* parent = nullptr) :
        QItemDelegate(parent)
    {
    };

    //Disable editing
    QWidget* createEditor(QWidget* parent,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const Q_DECL_OVERRIDE
    {
        return nullptr;
    };
};

#endif // PlayersModelDelegate_H
