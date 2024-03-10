//
// Created by Sargis Boyajyan on 01.03.24.
//

#ifndef PLAYER_H
#define PLAYER_H

enum class State
{
    FREE, WAITING, BUSY
};

#include <QString>
#include <QTimer>
#include <QMap>

class Player : public QObject
{
Q_OBJECT
public:
    Player() = default;
    Player(QString user, QString firstName, QString lastName, const QMap<QString, int>&& gameToRating);

    QString GetUseName()
    {
        return m_userName;
    }

    QString GetFirstName()
    {
        return m_firstName;
    }

    QString GetLastName()
    {
        return m_lastName;
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

    int GetRating(const QString& game)
    {
        return m_ratingByGame[game];
    }

    void SetRating(const QString& game, int rating)
    {
        m_ratingByGame[game] = rating;
    }

    QString GetPreferredGames();

    ~Player() override;

private:

private:
    QString m_userName;
    QString m_firstName;
    QString m_lastName;
    QString m_currentGame;
    QMap<QString, int> m_ratingByGame;

    State m_state = State::FREE;
    QTimer* m_timerRequest = nullptr;
public slots:
    void requestMatch();
};


#endif //PLAYER_H
