#ifndef PlayersModelDelegate_H
#define PlayersModelDelegate_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QSpinBox>


class PlayersModelDelegate : public QItemDelegate
{
Q_OBJECT
public:
    explicit PlayersModelDelegate(QObject* parent = 0);

    // Create Editor when we construct PlayersModelDelegate
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    // Then, we set the Editor
    void setEditorData(QWidget* editor, const QModelIndex& index) const;

    // When we modify data, this m_tableModel reflect the change
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

    // Give the SpinBox the info on size and location
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;

signals:

public slots:

};

#endif // PlayersModelDelegate_H
