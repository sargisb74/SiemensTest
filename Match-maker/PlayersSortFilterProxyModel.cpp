//
// Created by Sargis Boyajyan on 07.03.24.
//

#include "PlayersSortFilterProxyModel.h"

bool PlayersSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    if (left.row() >= sourceModel()->rowCount() - 1 || right.row() >= sourceModel()->rowCount() - 1)
    {
        return false;
    }
    return QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0;
}

bool PlayersSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (filterKeyColumn() == 0)
    {
        QModelIndex index = sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent);
        QString value = index.data(Qt::DisplayRole).toString();

        return value.contains(filterRegularExpression());
    }

    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

