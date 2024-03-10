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

    void SetTreeWidget(QTreeWidget* treeWidget)
    {
        m_treeWidget = treeWidget;
    }

    void Start();

    void Initialise(QMap<QString, QSharedPointer<Player>>* players, UsersDB& userDB, QTreeWidget* treeWidget);

    void InitUsers(QMap<QString, QSharedPointer<Player>>*);

private:

    QTimer* m_timer = nullptr;
    UsersDB* m_userDB{};
    QTreeWidget* m_treeWidget{};
    QVector<QString> m_users;
    QMap<QString, QSharedPointer<Player>>* m_players{};

    void performBackgroundTask();
    void UpdateWinner(Player*);
    void ExecuteGame(Player*, Player*);

public slots:
    void RunMatches();
};


#endif //PLAYER_H
