#include <QTableView>
#include "PlayersModel.h"
#include "PlayersSortFilterProxyModel.h"

[[maybe_unused]] PlayersModel::PlayersModel(QObject* parent)
    : QStandardItemModel((QObject*)parent)
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(requestVHeaderUpdate()));
    m_timer->start(VHEADER_UPDATE_TIMEOUT);
}

PlayersModel::PlayersModel(int rows, int columns, QObject* parent)
    : QStandardItemModel(rows, columns, (QObject*)parent)
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(requestVHeaderUpdate()));
    m_timer->start(VHEADER_UPDATE_TIMEOUT);
};

void PlayersModel::PopulateData(const QVector<QSharedPointer<Player>>& players)
{
    m_players.clear();
    m_players = players;
    m_players.append(QSharedPointer<Player>::create("+Add User", "",
        "", std::move(QMap<QString, int>())));

    for (int i = 0; i < m_players.size(); ++i)
    {
        setItem(i, static_cast<int>(UserTableColumns::USERNAME_COL),
            new QStandardItem(m_players[i]->GetUseName()));
        setItem(i, static_cast<int>(UserTableColumns::FIRSTNAME_COL),
            new QStandardItem(m_players[i]->GetFirstName()));
        setItem(i, static_cast<int>(UserTableColumns::LASTNAME_COL),
            new QStandardItem(m_players[i]->GetLastName()));
        setItem(i, static_cast<int>(UserTableColumns::GAMES_COL),
            new QStandardItem(m_players[i]->GetPreferredGames()));
    }

    m_lastItemRowToSection = m_players.size() - 1;

    m_verticalHeaderCounter = 0;
    m_verticalHeaderSize = m_players.size();;
}

void PlayersModel::UpdateVerticalHeader()
{
    m_verticalHeaderCounter = 0;

    emit headerDataChanged(Qt::Vertical, 0, rowCount() - 1);
}

void PlayersModel::AppendData(const QSharedPointer<Player>& player, int row)
{
    QList<QSharedPointer<Player>>::iterator it = m_players.begin() + rowCount() - 1;
    m_players.insert(it, player);
    m_verticalHeaderCounter = 0;
}

void PlayersModel::RemoveData(const QString& username)
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

QVariant PlayersModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() | (role != Qt::DisplayRole))
    {
        return {};
    }

    if (index.row() >= m_players.size())
    {
        return {};
    }

    const QSharedPointer<Player>& player = m_players[index.row()];
    if (!player.isNull())
    {
        switch (static_cast<UserTableColumns>(index.column()))
        {
        case UserTableColumns::USERNAME_COL:
            return { player->GetUseName() };
        case UserTableColumns::FIRSTNAME_COL:
            return { player->GetFirstName() };
        case UserTableColumns::LASTNAME_COL:
            return { player->GetLastName() };
        case UserTableColumns::GAMES_COL:
            return { player->GetPreferredGames() };
        }
    }

    return {};
}

QVariant PlayersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if (section == static_cast<int>(UserTableColumns::USERNAME_COL))
            {
                return QString("Username");
            }
            else if (section == static_cast<int>(UserTableColumns::FIRSTNAME_COL))
            {
                return QString("First Name");
            }
            else if (section == static_cast<int>(UserTableColumns::LASTNAME_COL))
            {
                return QString("Last Name");
            }
            else if (section == static_cast<int>(UserTableColumns::GAMES_COL))
            {
                return QString("Preferred Gams");
            }
        }

        if (orientation == Qt::Vertical && section != this->rowCount() - 1)
        {
            if (m_verticalHeaderCounter == m_verticalHeaderSize - 1)
            {
                m_verticalHeaderCounter = 0;
            }
            return QString::number(m_verticalHeaderCounter++);
        }
    }
    return QVariant();
}

void PlayersModel::requestVHeaderUpdate()
{
    UpdateVerticalHeader();
}

