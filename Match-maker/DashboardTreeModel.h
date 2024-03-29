//
// Created by Sargis Boyajyan on 11.03.24.
//

#ifndef TABLEVIEWEXAMPLE__DASHBOARDTREEMODEL_H_
#define TABLEVIEWEXAMPLE__DASHBOARDTREEMODEL_H_

#include <QStandardItemModel>
#include "Player.h"
#include "CustomTreeItems.h"

enum class DashboardColumns
{
    GAME_COL, USER_COL, RATING_COL, SIZE
};

class DashboardTreeModel : public QStandardItemModel
{
 Q_OBJECT

 public:

    explicit DashboardTreeModel(QObject* parent = nullptr);
    ~DashboardTreeModel() override = default;

    void PopulateDashboard(const QMap<QString, QSharedPointer<Player>>&);
    void AppendUserGames(const QString&, const QStringList&, int rating = 0);
    void RemoveUser(const QString&);
    void UpdateGameRating(const QString&, const QString&, unsigned int);
    void SaveTreeToJson(const QString&);
    template<class T>
    static QJsonObject toJsonRecursive(T*, RatingTreeItem*);
    QJsonArray toJsonArray();

 private:
    [[nodiscard]] QVariant headerData(int, Qt::Orientation, int) const override;
    void AppendRow(const QString&, const QString&, int);

 private:
    QMap<QString, GameTreeItem*> m_gameToItem;
};

#endif //TABLEVIEWEXAMPLE__DASHBOARDTREEMODEL_H_
