/* MIT License

Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <QTableView>
#include "PlayersModel.h"
#include "PlayersSortFilterProxyModel.h"

[[maybe_unused]] PlayersModel::PlayersModel(QObject* parent, PlayersSortFilterProxyModel* proxyModel)
	: QStandardItemModel((QObject*)parent), m_proxyModel(proxyModel)
{
	// Just some random test data
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(requestVHeaderUpdate()));
	m_timer->start(20);
}

PlayersModel::PlayersModel(int rows, int columns, QObject* parent, PlayersSortFilterProxyModel* proxyModel)
	: QStandardItemModel(rows, columns, (QObject*)parent), m_proxyModel(proxyModel)
{
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(requestVHeaderUpdate()));
	m_timer->start(20);
};

// Create a method to populate the m_model with data:
void PlayersModel::populateData(const QVector<QSharedPointer<Player>>& players)
{
	m_players.clear();
	m_players = players;

	for (int i = 0; i <= m_players.size(); ++i)
	{
		if (i < m_players.size())
		{
			//m_userToRow[players[i]->GetUseName()] = i;
			setItem(i, 0, new QStandardItem(players[i]->GetUseName()));
			setItem(i, 1, new QStandardItem(players[i]->GetFirstName()));
			setItem(i, 2, new QStandardItem(players[i]->GetLastName()));
			setItem(i, 3, new QStandardItem(players[i]->GetPreferredGames()));
		}
	}

	m_lastItemRowToSection = m_players.size() - 1;

	m_verticalHeaderCounter = 0;
	m_verticalHeaderSize = m_players.size();;
}

void PlayersModel::updateVerticalHeader()
{
	m_verticalHeaderCounter = 0;

	//qDebug() << "PlayersModel::updateVerticalHeader 1" << m_verticalHeaderSize;

	emit headerDataChanged(Qt::Vertical, 0, rowCount() - 1);
}

void PlayersModel::appendData(const QSharedPointer<Player>& player, int row)
{
	// Insert player at the specified row
	QList<QSharedPointer<Player>>::iterator it = m_players.begin() + rowCount() - 1;
	m_players.insert(it, player);
	m_verticalHeaderCounter = 0;

	// Set data in the newly added row
	/*setData(index(row, 0), player->GetUseName());
	setData(index(row, 1), player->GetFirstName());
	setData(index(row, 2), player->GetLastName());
	setData(index(row, 3), player->GetUseName());*/

	/*m_proxyModel->setData(index(row, 0), "");
	m_proxyModel->setData(index(row, 1), "");
	m_proxyModel->setData(index(row, 2), "");
	m_proxyModel->setData(index(row, 3), "");*/
}

void PlayersModel::removeData(const QString& username)
{
	auto it = std::remove_if(m_players.begin(), m_players.end(),
		[username](const QSharedPointer<Player>& player)
		{
			return player->GetUseName() == username;
		});

	m_players.erase(it, m_players.end());
}

int PlayersModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return m_players.length();
}

int PlayersModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 4;
}

QVariant PlayersModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() | (role != Qt::DisplayRole))
	{
		return QVariant();
	}

	if (index.row() >= m_players.size())
	{
		return QVariant();
	}
	const QSharedPointer<Player>& player = m_players[index.row()];
	if (!player.isNull())
	{
		switch (index.column())
		{
		case 0:
			return QVariant(player->GetUseName());
		case 1:
			return QVariant(player->GetFirstName());
		case 2:
			return QVariant(player->GetLastName());
		case 3:
			return QVariant(player->GetPreferredGames());
		}
	}

	return QVariant();
}

QVariant PlayersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			if (section == 0)
			{
				return QString("Username");
			}
			else if (section == 1)
			{
				return QString("First Name");
			}
			else if (section == 2)
			{
				return QString("Last Name");
			}
			else if (section == 3)
			{
				return QString("Preferred Gams");
			}
		}

		if (orientation == Qt::Vertical && section != this->rowCount() - 1)
		{
			if (m_verticalHeaderCounter == m_verticalHeaderSize - 1)
			{
				//if(m_verticalHeaderCounter == m_players.size() - 1) {
				m_verticalHeaderCounter = 0;
			}
			return QString::number(m_verticalHeaderCounter++);
		}
	}
	return QVariant();
}

void PlayersModel::requestVHeaderUpdate()
{
	updateVerticalHeader();
}

