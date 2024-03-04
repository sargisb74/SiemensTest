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

class MatchMaker: public QObject
{
Q_OBJECT

public:
	map<QString, unique_ptr<Player>> *m_players{};

	~MatchMaker() override
	{
		m_timer->stop();
	};

	void SetPlayers(map<QString, unique_ptr<Player>> *players)
	{
		m_players = players;
	}

	void SetUserDb(UsersDB &userDB)
	{
		m_userDB = &userDB;
	}

	void SetTreeWidget(QTreeWidget *treeWidget)
	{
		m_treeWidget = treeWidget;
	}

	void Start()
	{
		m_timer = new QTimer(this);
		connect(m_timer, SIGNAL(timeout()), this, SLOT(RunMatches()));
		m_timer->start(10);
	}

	void Initialise(map<QString, unique_ptr<Player>> *players, UsersDB &userDB, QTreeWidget *treeWidget)
	{
		m_players = players;
		m_userDB = &userDB;
		m_treeWidget = treeWidget;

		std::transform(
			m_players->begin(),
			m_players->end(),
			std::back_inserter(m_users),
			[](const std::map<QString, unique_ptr<Player>>::value_type &pair)
			{ return pair.first; });

		Start();
	}

	void InitUsers()
	{
		m_users.clear();
		std::transform(
			m_players->begin(),
			m_players->end(),
			std::back_inserter(m_users),
			[](const std::map<QString, unique_ptr<Player>>::value_type &pair)
			{ return pair.first; });
	}

private:

	QTimer *m_timer = nullptr;
	UsersDB *m_userDB{};
	QTreeWidget *m_treeWidget{};
	std::vector<QString> m_users;

	void performBackgroundTask();
	void UpdateWinner(Player &);
	static GameResult ExecuteGame(const QString &);
public slots:
	void RunMatches();
};


#endif //PLAYER_H
