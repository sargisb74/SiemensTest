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
    // Include the rows containing "+Add User"
    //QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    // *pModel = dynamic_cast<PlayersSortFilterProxyModel*>(ui->tableView->model());


    // Check if the filter is set for the first column
    /*if(filterKeyColumn() == 0 && index.row() == sourceModel()->rowCount() - 1) {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        QString value = index.data(Qt::DisplayRole).toString();
        qDebug() << value;
        return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
    }*/

    if (filterKeyColumn() == 0)
    {
        // Apply the filter only to the first column
        QModelIndex index = sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent);
        QString value = index.data(Qt::DisplayRole).toString();

        // Your filtering logic for the first column
        return value.contains(filterRegularExpression());
    }

    // If filterColumn() is not 0, apply the base class implementation
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

