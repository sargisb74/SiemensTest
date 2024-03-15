//
// Created by Sargis Boyajyan on 23.02.24.
//

#include <QDir>
#include <QProcessEnvironment>
#include "PlayersSortFilterProxyModel.h"
#include "main_wnd.h"
#include "ui_main_wnd.h"

MainWnd::MainWnd(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWnd)
{
    ui->setupUi(this);

    InitializeUIComponents();

    m_maker.Initialise(&m_players, *m_userDB, m_treeModel);
}

void MainWnd::InitializeUIComponents()
{
    m_userDB->ConnectToDataBase();

    InitializeViews();

    ui->menuEdit->setTitle(ui->menuEdit->title().prepend(QString::fromUtf8("\u200C")));
    ui->menuView->setTitle(ui->menuView->title().prepend(QString::fromUtf8("\u200C")));

    ui->tableView->verticalHeader()->setFixedWidth(ui->tableView->
        columnWidth(static_cast<int>(UserTableColumns::USERNAME_COL)));
    ui->status_line_edit->hide();

    ConnectSignalsToSlots();

    PopulateUsersTables();
}

void MainWnd::InitializeViews()
{
    PopulateUsers();
    auto* proxyModel = new PlayersSortFilterProxyModel(this);
    m_tableModel = new PlayersModel(1, static_cast<int>(UserTableColumns::USER_COL_SIZE), this);
    m_treeModel = new DashboardTreeModel();

    proxyModel->setSourceModel(m_tableModel);

    ui->tableView->verticalHeader()->setVisible(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setShowGrid(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setFixedWidth(ui->tableView->columnWidth(0));
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableView->horizontalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "    border: 1px solid #d0d0d0;"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f6f7fa, stop:1 #d0d0d0);"
        "    padding: 4px;"
        "    margin: 0;"
        "}"
        "QHeaderView::section:pressed {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d0d0d0, stop:1 #f6f7fa);"
        "}"
    );
    ui->treeView->header()->setStyleSheet(
        "QHeaderView::section {"
        "    border: 1px solid #d0d0d0;"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f6f7fa, stop:1 #d0d0d0);"
        "    padding: 4px;"
        "    margin: 0;"
        "}"
        "QHeaderView::section:pressed {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d0d0d0, stop:1 #f6f7fa);"
        "}"
    );

    m_myDelegate = new PlayersModelDelegate(this);

    m_tableModel->PopulateData(m_players.values());

    ui->tableView->setItemDelegate(m_myDelegate);
    ui->tableView->setModel(proxyModel);
    ui->tableView->verticalHeader()->show();
    ui->tableView->setSpan(m_tableModel->rowCount() - 1,
        static_cast<int>(UserTableColumns::USERNAME_COL), 1, m_tableModel->columnCount());

    ui->treeView->setModel(m_treeModel);
    ui->treeView->setItemDelegate(m_myDelegate);
    ui->treeView->expandAll();
}

void MainWnd::ConnectSignalsToSlots()
{
    auto* addUserMenuItem = new QAction("Add User", this);
    auto* removeUserMenuItem = new QAction("Remove User", this);
    m_contextMenu.addAction(addUserMenuItem);
    m_contextMenu.addAction(removeUserMenuItem);

    m_timerShowError = new QTimer(this);

    connect(ui->tableView->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
        this, SLOT(sortIndicatorChangedSlot(int, Qt::SortOrder)));
    connect(ui->tableView, &QTableView::customContextMenuRequested, this,
        &MainWnd::slotCustomMenuRequested);
    connect(addUserMenuItem, SIGNAL(triggered()), this,
        SLOT(on_action_Add_user_triggered()));
    connect(removeUserMenuItem, SIGNAL(triggered()), this,
        SLOT(on_action_Remove_user_triggered()));
    connect(m_timerShowError, SIGNAL(timeout()), this, SLOT(showError()));
}

void MainWnd::AddUser()
{
    auto* pModel = dynamic_cast<PlayersSortFilterProxyModel*>(ui->tableView->model());
    if (m_addUserDlg.exec() == QDialog::Accepted)
    {
        ui->tableView->setSortingEnabled(false);
        ui->tableView->setSpan(m_tableModel->rowCount() - 1,
            static_cast<int>(UserTableColumns::USERNAME_COL), 1, 1);

        QString user = m_addUserDlg.getUsername();
        QStringList games = m_addUserDlg.getPreferredGames();
        QString error;
        if (!m_userDB->InsertIntoUserTable(QVariantList() << user
                                                          << m_addUserDlg.getFirstName()
                                                          << m_addUserDlg.getLastName()
                                                          << games.join(", ")
                                                          << QString::number(0), error))
        {
            ui->status_line_edit->show();
            ui->status_line_edit->setText(error);
            m_timerShowError->start(SHOW_ERROR_TIMEOUT);

            ui->tableView->setSpan(m_tableModel->rowCount() - 1,
                static_cast<int>(UserTableColumns::USERNAME_COL), 1,
                m_tableModel->columnCount());

            return;
        }
        for (const auto& str : std::as_const(games))
        {
            m_userDB->InsertIntoUser_RatingsTable(QVariantList() << m_addUserDlg.getUsername() <<
                                                                 str << QString::number(0));
        }

        m_treeModel->AppendUserGames(user, games);

        ui->treeView->expandAll();

        QMap<QString, int> gameToRating;
        for (const QString& str : games)
        {
            gameToRating[str] = 0;
        }

        m_tableModel->insertRow(m_tableModel->rowCount(QModelIndex()));
        int newRow = pModel->rowCount() + 1;
        QString userName = m_addUserDlg.getUsername();
        QSharedPointer<Player> player = QSharedPointer<Player>::create(
            m_addUserDlg.getUsername(),
            m_addUserDlg.getFirstName(),
            m_addUserDlg.getLastName(),
            std::move(gameToRating));

        m_tableModel->AppendData(player, newRow);
        pModel->invalidate();
        m_players[userName] = player;
        m_maker.InitUsers(&m_players);

        int count = m_tableModel->GetPlayersCount();
        m_tableModel->SetLastItemSection(GetLastItemSection());
        ui->tableView->verticalHeader()
            ->moveSection(m_tableModel->GetLastItemRowToSection(), count - 1);
        ui->tableView->setSpan(m_tableModel->rowCount() - 1,
            static_cast<int>(UserTableColumns::USERNAME_COL), 1, m_tableModel->columnCount());
    }

    m_tableModel->SetVerticalHeaderSize(pModel->rowCount());
}

void MainWnd::PopulateUsers()
{
    QSqlQuery query("SELECT "
                    USER_NAME ", "
                    FIRST_NAME ", "
                    LAST_NAME ", "
                    PREFERRED_GAMES
                    " FROM " USER);

    QMap<QString, QMap<QString, int>> userToGame;
    for (int index = 0; query.next(); index++)
    {
        QString games = query.value(3).toString();
        QString user = query.value(0).toString();
        userToGame[user] = m_userDB->GetGameToRatingMap(user, games);

        //query.value(1) - First Name, query.value(2) - Last Name
        m_players[user] = QSharedPointer<Player>::create(user, query.value(1).toString(),
            query.value(2).toString(), std::move(userToGame[user]));
    }
};

void MainWnd::PopulateUsersTables()
{
    QList<QSharedPointer<Player>> players = m_players.values();
    for (const QSharedPointer<Player>& player : players)
    {
        QString user = player->GetUseName();
        for (auto [game, rating] : player->GetRatingByGame().asKeyValueRange())
        {
            QSqlQuery query = m_userDB->GetUserRating(user, game);
            for (; query.next();)
            {
                player->SetRating(game, query.value(1).toInt());
            }
        }
    }

    m_treeModel->PopulateDashboard(m_players);
    ui->treeView->expandAll();
}

void MainWnd::RemoveFromPlayers(const QString& user)
{
    m_players.remove(user);
    m_maker.InitUsers(&m_players);
}

void MainWnd::on_action_Add_user_triggered()
{
    AddUser();
}

void MainWnd::on_action_Remove_user_triggered()
{
    auto* pModel = dynamic_cast<PlayersSortFilterProxyModel*>(ui->tableView->model());
    ui->tableView->setSortingEnabled(false);
    QList<QModelIndex> selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
    QSet<int> selectedRows;

    for (const auto& index : selectedIndexes)
    {
        selectedRows.insert(index.row());
    }

    for (int row : selectedRows)
    {
        if (row == m_tableModel->rowCount() - 1)
        {
            continue;
        }

        QModelIndex proxyIndex = pModel->index(row, static_cast<int>(UserTableColumns::USERNAME_COL));

        QModelIndex sourceIndex = pModel->mapToSource(proxyIndex);

        QString userName = sourceIndex.data(Qt::DisplayRole).toString();
        m_userDB->RemoveFromUserTable(userName);
        m_userDB->RemoveFromUser_RatingsTable(userName);

        m_tableModel->RemoveData(userName);
        pModel->removeRow(row);

        m_tableModel->SetLastItemSection(GetLastItemSection());
        m_treeModel->RemoveUser(userName);
        RemoveFromPlayers(userName);
    }
    m_tableModel->SetVerticalHeaderSize(pModel->rowCount());
}

[[maybe_unused]] void MainWnd::on_actionShow_Hide_Dashboard_triggered()
{
    if (ui->treeView->isHidden())
    {
        ui->treeView->show();
    }
    else
    {
        ui->treeView->hide();
    }
}

void MainWnd::on_actionShow_Hide_User_List_triggered()
{
    if (ui->tableView->isHidden())
    {
        ui->tableView->show();
        ui->label->show();
        ui->filterPushButton->show();
        ui->filterLineEdit->show();
    }
    else
    {
        ui->tableView->hide();
        ui->label->hide();
        ui->filterPushButton->hide();
        ui->filterLineEdit->hide();
    }
}

void MainWnd::slotCustomMenuRequested(const QPoint& pos)
{
    QModelIndex index = ui->tableView->indexAt(pos);

    if (!index.isValid() ||
        (index.column() == static_cast<int>(UserTableColumns::USERNAME_COL) &&
            index.row() == ui->tableView->model()->rowCount() - 1))
    {
        m_contextMenu.actions().at(1)->setDisabled(true);
    }
    else
    {
        m_contextMenu.actions().at(1)->setDisabled(false);
    }

    m_contextMenu.popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void MainWnd::sortIndicatorChangedSlot(int col, Qt::SortOrder sortType)
{
    if (col == static_cast<int>(UserTableColumns::GAMES_COL))
    {
        ui->tableView->setSortingEnabled(false);
    }
    else
    {
        ui->tableView->setSortingEnabled(true);
    }
}

void MainWnd::on_actionSave_the_Dashboard_to_File_triggered()
{
    QProcess process;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString separator = QDir::separator();
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString fileName = "Dashboard_" + currentDateTime.toString(Qt::ISODate) + ".json";
    QString filePath = env.value("DASHBOARD_FILE_PATH", QDir::homePath()) + separator + fileName;

    m_treeModel->SaveTreeToJson(filePath);
}

void MainWnd::on_tableView_clicked(const QModelIndex& index)
{
    if (index.column() == static_cast<int>(UserTableColumns::USERNAME_COL) and
        index.row() == m_tableModel->GetLastItemRowToSection())
    {
        AddUser();
    }
}

void MainWnd::on_filterPushButton_clicked()
{
    QString filter = ui->filterLineEdit->text();
    auto* pModel = dynamic_cast<PlayersSortFilterProxyModel*>(ui->tableView->model());

    if (filter.trimmed().isEmpty())
    {
        ui->tableView->setSpan(pModel->rowCount() - 1,
            static_cast<int>(UserTableColumns::USERNAME_COL), 1, 1);
        pModel->setFilterRegularExpression(filter);
        pModel->invalidate();
        int lastSection = GetLastItemSection();
        ui->tableView->verticalHeader()->moveSection(lastSection, pModel->rowCount() - 1);
        ui->tableView->setSpan(m_tableModel->rowCount() - 1,
            static_cast<int>(UserTableColumns::USERNAME_COL), 1, m_tableModel->columnCount());
        m_tableModel->SetLastItemSection(lastSection);
        m_tableModel->SetVerticalHeaderSize(pModel->rowCount());
    }
    else
    {
        pModel->setFilterRegularExpression("(" + filter + "|\\" +
            m_tableModel->GetLastItemText() + ")");

        int lastSection = GetLastItemSection();
        ui->tableView->verticalHeader()->moveSection(lastSection, pModel->rowCount() - 1);
        ui->tableView->setSpan(pModel->rowCount() - 1,
            static_cast<int>(UserTableColumns::USERNAME_COL), 1, m_tableModel->columnCount());
        m_tableModel->SetLastItemSection(pModel->rowCount() - 1);
        m_tableModel->SetVerticalHeaderSize(pModel->rowCount());
    }
}

void MainWnd::showError()
{
    ui->status_line_edit->hide();
}

int MainWnd::GetLastItemSection()
{
    QAbstractItemModel* headerModel = ui->tableView->verticalHeader()->model();

    if (headerModel)
    {
        for (int i = 0; i < headerModel->rowCount(); i++)
        {
            QVariant headerData = headerModel->headerData(i, Qt::Vertical, Qt::DisplayRole);
            if (headerData.toString().isEmpty())
            {
                return i;
            }
        }
    }

    return m_tableModel->GetLastItemRowToSection();
}


