//
// Created by Sargis Boyajyan on 02.03.24.
//

#ifndef MATCHMAKER_H
#define MATCHMAKER_H

#include <iostream>
#include <QString>
#include <QTimer>
#include <QMap>
#include <QTreeWidget>
#include <utility>
#include <unordered_set>
#include <QMutex>
#include "Player.h"
#include "DashboardTreeModel.h"
#include "UsersDB.h"

using namespace std;

enum class GameResult
{
    NO_WINNER, FIRST, SECOND
};

class MatchMaker : public QObject
{
Q_OBJECT

public:
    ~MatchMaker() override
    {
        m_timer->stop();
    };

    void SetPlayers(QMap<QString, QSharedPointer<Player>>* players)
    {
        m_players = players;
    }

    void SetUserDb(UsersDB& userDB)
    {
        m_userDB = &userDB;
    }

    /*void SetTreeModel(DashboardTreeModel* treeModel)
    {
        m_treeModel = treeModel;
    }*/

    void Start();

    void Initialise(QMap<QString, QSharedPointer<Player>>* , UsersDB& , DashboardTreeModel* );

    void InitUsers(QMap<QString, QSharedPointer<Player>>*);

private:

    QTimer* m_timer = nullptr;
    UsersDB* m_userDB{};
    DashboardTreeModel* m_treeModel{};
    QVector<QString> m_users;
    QMap<QString, QSharedPointer<Player>>* m_players{};

    void performBackgroundTask();
    void UpdateWinner(Player*);
    void ExecuteGame(Player*, Player*);

public slots:
    void RunMatches();
};


#endif //PLAYER_H
