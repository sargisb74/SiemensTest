//
// Created by Sargis Boyajyan on 11.03.24.
//

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "DashboardTreeModel.h"
#include "CustomTreeItems.h"

DashboardTreeModel::DashboardTreeModel(QObject* parent)
    : QStandardItemModel(parent)
{
}

QVariant DashboardTreeModel::headerData(int section, Qt::Orientation orientation,
    int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case static_cast<int>(DashboardColumns::GAME_COL):
            return "Game";
        case static_cast<int>(DashboardColumns::USER_COL):
            return "User";
        case static_cast<int>(DashboardColumns::RATING_COL):
            return "Rating";
        }
    }
    return QVariant{};
}

void DashboardTreeModel::PopulateDashboard(const QMap<QString, QSharedPointer<Player>>& m_players)
{
    for (auto [user, player] : m_players.asKeyValueRange())
    {
        for (auto [game, rating] : player->GetRatingByGame().asKeyValueRange())
        {
            AppendRow(game, user, rating);
        }
    }

    setColumnCount(static_cast<int>(DashboardColumns::SIZE));

    sort(static_cast<int>(DashboardColumns::GAME_COL));
    sort(static_cast<int>(DashboardColumns::RATING_COL), Qt::DescendingOrder);
}

void DashboardTreeModel::AppendRow(const QString& game, const QString& userName, int rating)
{
    GameTreeItem* parentItem;
    if (m_gameToItem.find(game) == m_gameToItem.end())
    {
        parentItem = new GameTreeItem(game);
        appendRow(parentItem);
        m_gameToItem[game] = parentItem;
    }

    auto* ratingItem = new RatingTreeItem(QString::number(rating));
    auto* usernameItem = new QStandardItem(userName);

    parentItem = m_gameToItem[game];
    parentItem->SetUserAndRatingItems(userName, usernameItem, ratingItem);
    parentItem->appendRow({{}, usernameItem, ratingItem });
}

void DashboardTreeModel::RemoveUser(const QString& user)
{
    QStringList removedGames;
    for (auto [game, gameItem] : m_gameToItem.asKeyValueRange())
    {
        auto* userItem = gameItem->GetUserItem(user);
        if (userItem)
        {
            gameItem->removeRow(userItem->row());
            gameItem->RemoveUserItem(user);
            if (gameItem->GetUserItemsCount() == 0)
            {
                removeRow(gameItem->row());
                removedGames.insert(0, game);
            }
        }
    }

    for (const QString& game : removedGames)
    {
        m_gameToItem.remove(game);
    }
}

void DashboardTreeModel::UpdateGameRating(const QString& game, const QString& user, unsigned int rating)
{
    GameTreeItem* gameItem = m_gameToItem[game];
    if (gameItem)
    {
        auto* ratingItem = gameItem->GetRatingItem(user);
        if (ratingItem)
        {
            ratingItem->setData(rating, Qt::DisplayRole);
            ratingItem->parent()->sortChildren(static_cast<int>(DashboardColumns::RATING_COL),
                Qt::DescendingOrder);
        }
    }
}

void DashboardTreeModel::AppendUserGames(const QString& userName, const QStringList& games, int rating)
{
    for (const QString& game : games)
    {
        AppendRow(game, userName, rating);
    }

    sort(static_cast<int>(DashboardColumns::GAME_COL));
    sort(static_cast<int>(DashboardColumns::RATING_COL), Qt::DescendingOrder);
}

void DashboardTreeModel::SaveTreeToJson(const QString& fileName)
{
    QJsonDocument jsonDoc(toJsonArray());

    // Save to file
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << jsonDoc.toJson();
        file.close();
    }
    else
    {
        qDebug() << "Failed to open file for writing:" << file.errorString();
    }
}

QJsonArray DashboardTreeModel::toJsonArray()
{
    QJsonArray jsonArray;
    int topLevelItemCount = invisibleRootItem()->rowCount();//rowCount(rootIndex);

    for (int i = 0; i < topLevelItemCount; ++i)
    {
        auto* topLevelItem = dynamic_cast<GameTreeItem*>(invisibleRootItem()->
            child(i, static_cast<int>(DashboardColumns::GAME_COL)));

        jsonArray.append(toJsonRecursive(topLevelItem, nullptr));
    }

    return jsonArray;
}

template<class T>
QJsonObject DashboardTreeModel::toJsonRecursive(T* gameOrUserItem, RatingTreeItem* ratingItem)
{
    QJsonObject obj;
    auto* gameItem = dynamic_cast<GameTreeItem*>(gameOrUserItem);

    if (!ratingItem)
    {
        obj["Game"] = gameItem->text();
    }
    else
    {
        obj[gameOrUserItem->text()] = ratingItem->text();
    }

    int childCount = gameOrUserItem->rowCount();
    if (childCount > 0)
    {
        QJsonArray childrenArray;

        for (auto [game, items] : gameItem->GetGameChildItems().asKeyValueRange())
        {
            childrenArray.append(toJsonRecursive(items.first, items.second));
        }

        obj["Users"] = childrenArray;
    }

    return obj;
}

