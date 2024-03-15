#pragma once

#include <QStandardItemModel>
#include "Player.h"

enum class UserTableColumns
{
    USERNAME_COL, FIRSTNAME_COL, LASTNAME_COL, GAMES_COL, USER_COL_SIZE
};

#define VHEADER_UPDATE_TIMEOUT 20

class PlayersModel : public QStandardItemModel
{
 Q_OBJECT

 public:
    [[maybe_unused]] explicit PlayersModel(QObject* parent);
    PlayersModel(int rows, int columns, QObject* parent);

    void PopulateData(const QVector<QSharedPointer<Player>>&);
    void AppendData(const QSharedPointer<Player>&, int);
    void RemoveData(const QString&);
    void UpdateVerticalHeader();
    QString GetLastItemText()
    {
        return m_players.at(m_players.size() - 1)->GetUseName();
    }

    void SetLastItemSection(int section)
    {
        m_lastItemRowToSection = section;
    }

    int GetLastItemRowToSection()
    {
        return m_lastItemRowToSection;
    }

    int GetPlayersCount()
    {
        return m_players.size();
    }

    void SetVerticalHeaderSize(int newSize)
    {
        m_verticalHeaderSize = newSize;
    }

 public slots:
    void requestVHeaderUpdate();

    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;

 private:

    QTimer* m_timer = nullptr;
    QVector<QSharedPointer<Player>> m_players;
    mutable int m_verticalHeaderCounter = 0;
    mutable int m_verticalHeaderSize = 0;
    int m_lastItemRowToSection = 0;
};