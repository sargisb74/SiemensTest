//
// Created by Sargis Boyajyan on 07.03.24.
//

#ifndef PLAYERSSORTFILTERPROXYMODEL_H
#define PLAYERSSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class PlayersSortFilterProxyModel Q_DECL_FINAL : public QSortFilterProxyModel
{
 Q_OBJECT
 public:
    explicit PlayersSortFilterProxyModel(QObject* parent)
        : QSortFilterProxyModel(parent)
    {
    }

 protected:
    [[nodiscard]] bool lessThan(const QModelIndex& left, const QModelIndex& right) const Q_DECL_OVERRIDE;

    [[nodiscard]] bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const Q_DECL_OVERRIDE;
};

#endif //PLAYERSSORTFILTERPROXYMODEL_H
