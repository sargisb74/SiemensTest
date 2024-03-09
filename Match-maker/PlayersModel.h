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

#pragma once

#include <QStandardItemModel>
#include <QVector>
#include <QPair>
#include "Player.h"
class PlayersModel : public QStandardItemModel
{
Q_OBJECT

public:
	[[maybe_unused]] explicit PlayersModel(QObject *parent);
	PlayersModel(int rows, int columns, QObject *parent);

	void populateData(const QVector<QSharedPointer<Player>> &);
	void appendData(const QSharedPointer<Player>& , int );
	void removeData(const QString& );
	void updateVerticalHeader( );
	QString GetLastItemText() {
		return m_players.at(m_players.size() - 1)->GetUseName();
	}
	void SetLastItemSection (int section) {
		m_lastItemRowToSection.second = section;
	}
	QPair<int, int> GetLastItemRowToSection () {
		return m_lastItemRowToSection;
	}

	int GetPlayersCount() {
		return m_players.size();
	}

public slots:
	void requestVHeaderUpdate();


	int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	[[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;

private:
	QVector<QSharedPointer<Player>> m_players;
	mutable int m_verticalHeaderCounter;
	QTimer *m_timer = nullptr;
	QPair<int, int> m_lastItemRowToSection;
};