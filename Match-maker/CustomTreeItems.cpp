#include <QException>
#include "CustomTreeItems.h"
#include "DashboardTreeModel.h"

RatingTreeItem::RatingTreeItem(const QString &str)
    : QStandardItem(str)
{

}

bool RatingTreeItem::operator<(const QStandardItem &other) const
{
    try
    {
        if (column() == static_cast<int>(DashboardColumns::RATING_COL))
        {
            return data(Qt::DisplayRole).toInt() < other.data(Qt::DisplayRole).toInt();
        }
    }
    catch(const QException &exception)
    {
        qDebug() << "Exception occurred during sorting process: " << exception.what();
    }

    return QStandardItem::operator<(other);
}