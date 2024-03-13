//
// Created by Sargis Boyajyan on 12.03.24.
//

#ifndef TABLEVIEWEXAMPLE__CUSTOMTREEITEMS_H_
#define TABLEVIEWEXAMPLE__CUSTOMTREEITEMS_H_

#include <QStandardItem>

class RatingTreeItem: public QStandardItem
{
 public:
    RatingTreeItem() = default;
    explicit RatingTreeItem(const QString &str);

    bool operator<(const QStandardItem &other) const override;
};


class GameTreeItem: public QStandardItem
{
 public:
    GameTreeItem() = default;
    explicit GameTreeItem(const QString &str): QStandardItem(str){};

    void SetUserAndRatingItems(const QString& userName, QStandardItem* userItem, RatingTreeItem* ratingItem)
    {
        m_userNameToItems[userName] = qMakePair(userItem, ratingItem);
    }

    QStandardItem* GetUserItem(const QString& userName)
    {
        return m_userNameToItems.find(userName) == m_userNameToItems.end() ?
        nullptr : m_userNameToItems[userName].first;
    }

    QStandardItem* GetRatingItem(const QString& userName)
    {
        return m_userNameToItems.find(userName) == m_userNameToItems.end()
        ? nullptr : m_userNameToItems[userName].second;
    }

    const QMap<QString, QPair<QStandardItem*, RatingTreeItem*>>& GetGameChildItems()
    {
        return m_userNameToItems;
    }

    int GetUserItemsCount()
    {
        return m_userNameToItems.size();
    }

    void RemoveUserItem(const QString& userName)
    {
        m_userNameToItems.remove(userName);
    }

 private:
    QMap<QString, QPair<QStandardItem*, RatingTreeItem*>> m_userNameToItems;
};

#endif //TABLEVIEWEXAMPLE__CUSTOMTREEITEMS_H_
