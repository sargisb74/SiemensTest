//
// Created by Sargis Boyajyan on 01.03.24.
//

#ifndef PLAYER_H
#define PLAYER_H

enum class State
{
	FREE, WAITING, Busy
};

#include <QString>
#include <QTimer>
#include <QMap>

class Player: public QObject
{
Q_OBJECT
public:
	Player() = default;
	Player(QString user, const QMap<QString, int> &&gameToRating)
		: m_userName(std::move(user)), m_ratingByGame(gameToRating)
	{
		m_timerRequest = new QTimer(this);
		connect(m_timerRequest, SIGNAL(timeout()), this, SLOT(requestMatch()));
		m_timerRequest->start(2000);
	}

	QString GetUseName()
	{
		return m_userName;
	}

	State GetState()
	{
		return m_state;
	}

	void SetState(State state)
	{
		m_state = state;
	}

	QString GetCurrentGame()
	{
		return m_currentGame;
	}

	int GetRating(const QString &game)
	{
		return m_ratingByGame[game];
	}

	void SetRating(const QString &game, int rating)
	{
		m_ratingByGame[game] = rating;
	}

	~Player() override
	{
		m_timerRequest->stop();
		delete m_timerRequest;
	};

private:

private:
	QString m_userName;
	QString m_currentGame;
	QMap<QString, int> m_ratingByGame;

	State m_state = State::FREE;
	QTimer *m_timerRequest = nullptr;
public slots:
	void requestMatch();
};


#endif //PLAYER_H
