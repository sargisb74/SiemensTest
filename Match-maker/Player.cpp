//
// Created by Sargis Boyajyan on 01.03.24.
//

#include <iostream>
#include <QRandomGenerator>
#include "Player.h"

void Player::requestMatch()
{
	if (m_state == State::FREE) {
		QList<QString> keys = m_ratingByGame.keys();
		long long game = keys.count() == 1 ? 0 : QRandomGenerator::global()->bounded(0, keys.count());
		m_currentGame = keys.at(game);
		m_state = State::WAITING;
		m_timerRequest->stop();
		m_timerRequest->start(2000);
		return;
	}

	if (m_state == State::WAITING) {
		m_state = State::FREE;
		m_timerRequest->stop();
		int timeout = QRandomGenerator::global()->bounded(2000, 3001);
		m_timerRequest->start(timeout);
	}
}

