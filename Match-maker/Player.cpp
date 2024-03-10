//
// Created by Sargis Boyajyan on 01.03.24.
//

#include <iostream>
#include <QRandomGenerator>
#include "Player.h"

Player::Player(QString user, QString firstName, QString lastName, const QMap<QString, int>&& gameToRating)
    : m_userName(std::move(user)), m_firstName(std::move(firstName)),
      m_lastName(std::move(lastName)), m_ratingByGame(gameToRating)
{
    m_timerRequest = new QTimer(this);
    connect(m_timerRequest, SIGNAL(timeout()), this, SLOT(requestMatch()));
    m_timerRequest->start(2000);
}

Player::~Player()
{
    if (m_timerRequest != nullptr)
    {
        m_timerRequest->stop();
        delete m_timerRequest;
    }
};

QString Player::GetPreferredGames()
{
    QStringList keys = m_ratingByGame.keys();
    return keys.join(',');
}

void Player::requestMatch()
{
    if (m_ratingByGame.isEmpty())
    {
        return;
    }

    if (m_state == State::FREE)
    {
        QList<QString> keys = m_ratingByGame.keys();
        long long game = keys.count() == 1 ? 0 : QRandomGenerator::global()->bounded(0, keys.count());
        m_currentGame = keys.at(game);
        m_state = State::WAITING;
        m_timerRequest->stop();
        m_timerRequest->start(2000);
        return;
    }

    if (m_state == State::WAITING)
    {
        m_state = State::FREE;
        m_timerRequest->stop();
        int timeout = QRandomGenerator::global()->bounded(2000, 3001);
        m_timerRequest->start(timeout);
    }
}
