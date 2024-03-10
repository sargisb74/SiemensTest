//
// Created by Sargis Boyajyan on 23.02.24.
//

// You may need to build the project (run Qt uic code generator) to get "ui_main_wnd.h" resolved

#include <QMessageBox>
#include <QDir>
#include <iostream>
#include <QRandomGenerator>
#include <QThread>
#include <QAbstractEventDispatcher>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QProcessEnvironment>
#include "PlayersSortFilterProxyModel.h"
#include "PlayersModelDelegate.h"
#include "main_wnd.h"
#include "ui_main_wnd.h"
#include "add_user_dialog.h"


MainWnd::MainWnd(QWidget* parent, PlayersModel* mModel, PlayersModelDelegate* mMyDelegate)
    : QMainWindow(parent), ui(new Ui::MainWnd), m_model(mModel), m_myDelegate(mMyDelegate)
{
    ui->setupUi(this);

    InitializeUIComponents();

    m_maker.Initialise(&m_players, *m_userDB, ui->treeWidget);
}

void MainWnd::InitializeUIComponents()
{
    m_userDB->connectToDataBase();

    InitializeUsersTable();

    ui->menuEdit->setTitle(ui->menuEdit->title().prepend(QString::fromUtf8("\u200C")));
    ui->menuView->setTitle(ui->menuView->title().prepend(QString::fromUtf8("\u200C")));

    ui->tableView->verticalHeader()->setFixedWidth(ui->tableView->columnWidth(0));
    ui->status_line_edit->hide();

    ConnectSignalsToSlots();

    PopulateUsersTables();
}

void MainWnd::InitializeUsersTable()
{
    PopulateUsers();
    auto* proxyModel = new PlayersSortFilterProxyModel(this);
    m_model = new PlayersModel(1, 4, this, proxyModel);

    proxyModel->setSourceModel(m_model);

    ui->tableView->verticalHeader()->setVisible(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setShowGrid(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setFixedWidth(ui->tableView->columnWidth(0));
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    // Apply stylesheets to customize the header appearance
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
    ui->treeWidget->header()->setStyleSheet(
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

    m_model->populateData(m_players.values().toVector());

    m_myDelegate = new PlayersModelDelegate(this);
    ui->tableView->setItemDelegate(m_myDelegate);
    ui->tableView->setModel(proxyModel);
    ui->tableView->verticalHeader()->show();
    ui->tableView->setSpan(m_model->rowCount() - 1, 1, 1, m_model->columnCount());
}

void MainWnd::ConnectSignalsToSlots()
{
    auto* addUserMenuItem = new QAction("Add User", this);
    auto* removeUserMenuItem = new QAction("Remove User", this);
    /* Set the actions to the menu */
    m_contextMenu.addAction(addUserMenuItem);
    m_contextMenu.addAction(removeUserMenuItem);

    m_timerShowError = new QTimer(this);

    connect(ui->tableView->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
        this, SLOT(sortIndicatorChangedSlot(int, Qt::SortOrder)));
    // Connect SLOT to context menu
    connect(ui->tableView, &QTableWidget::customContextMenuRequested, this,
        &MainWnd::slotCustomMenuRequested);
    connect(addUserMenuItem, SIGNAL(triggered()), this,
        SLOT(on_action_Add_user_triggered()));
    connect(removeUserMenuItem, SIGNAL(triggered()), this,
        SLOT(on_action_Remove_user_triggered()));
    connect(m_timerShowError, SIGNAL(timeout()), this, SLOT(ShowError()));
}

void MainWnd::AddUser()
{
    PlayersSortFilterProxyModel* pModel = dynamic_cast<PlayersSortFilterProxyModel*>(ui->tableView->model());
    if (m_addUserDlg.exec() == QDialog::Accepted)
    {
        ui->tableView->setSortingEnabled(false);
        ui->tableView->setSpan(m_model->rowCount() - 1, 1, 1, 1);

        QStringList list = m_addUserDlg.getPreferredGames();
        QString error;
        if (!m_userDB->insertIntoUserTable(QVariantList() << m_addUserDlg.getUsername()
                                                          << m_addUserDlg.getFirstName()
                                                          << m_addUserDlg.getLastName()
                                                          << list.join(", ")
                                                          << QString::number(0), error))
        {
            ui->status_line_edit->show();
            ui->status_line_edit->setText(error);
            m_timerShowError->start(5000);

            ui->tableView->setSpan(m_model->rowCount() - 1, 1, 1, m_model->columnCount());

            return;
        }
        for (const auto& str: std::as_const(list))
        {
            m_userDB->insertIntoUser_RatingsTable(QVariantList() << m_addUserDlg.getUsername() <<
                                                                 str << QString::number(0));
            QList<QTreeWidgetItem*> items = ui->treeWidget->findItems(str, Qt::MatchExactly);
            if (items.count() == 0)
            {
                addTreeRoot(str, "");
            }
            else
                for (QTreeWidgetItem* item: items)
                {
                    addTreeChild(item, m_addUserDlg.getUsername(), QString::number(0));
                }
        }

        QMap<QString, int> gameToRating;
        for (const QString& str: list)
        {
            gameToRating[str] = 0;
        }

        m_model->insertRow(m_model->rowCount(QModelIndex()));
        int newRow = pModel->rowCount() + 1;//m_model->rowCount(QModelIndex()) - 1;
        QString userName = m_addUserDlg.getUsername();
        QSharedPointer<Player> player = QSharedPointer<Player>::create(
            m_addUserDlg.getUsername(),
            m_addUserDlg.getFirstName(),
            m_addUserDlg.getLastName(),
            std::move(gameToRating));
        m_model->appendData(player, newRow);
        pModel->invalidate();
        for (int i = 0; i < pModel->rowCount(); i++)
        {
            QModelIndex proxyIndex = pModel->index(i, 0);

            // Get the item text from the proxy model
            QString username = pModel->data(proxyIndex, Qt::DisplayRole).toString();
        }

        m_players[userName] = player;
        m_maker.InitUsers(&m_players);

        int plcount = m_model->GetPlayersCount();
        m_model->SetLastItemSection(GetLastItemSection());
        ui->tableView->verticalHeader()
            ->moveSection(m_model->GetLastItemRowToSection(), /*m_model->rowCount()*/plcount - 1);
        ui->tableView->setSpan(m_model->rowCount() - 1, 1, 1, m_model->columnCount());
    }

    ui->treeWidget->expandAll();

    m_model->SetVerticalHeaderSize(pModel->rowCount());
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

        m_players[user] = QSharedPointer<Player>::create(user, query.value(1).toString(),
            query.value(2).toString(), std::move(userToGame[user]));
    }

    m_players["last"] = QSharedPointer<Player>::create("+Add User", "",
        "", std::move(QMap<QString, int>()));
};


void MainWnd::PopulateUsersTables()
{
    QSqlQuery query("SELECT "
                    USER_NAME ", "
                    FIRST_NAME ", "
                    LAST_NAME ", "
                    PREFERRED_GAMES
                    " FROM " USER);

    QSqlQuery query1("SELECT DISTINCT " GAME
                     " FROM " USER_RATINGS " ORDER BY " GAME);

    for (int index = 0; query1.next(); index++)
    {
        addTreeRoot(query1.value(0).toString(), "");
    }

    ui->treeWidget->expandAll();
}

void MainWnd::addTreeRoot(const QString& game, const QString& description)
{
    auto* treeItem = new QCustomTreeWidgetItem(ui->treeWidget);

    QSqlQuery query = m_userDB->GetUserRating(game);
    for (; query.next();)
    {
        addTreeChild(treeItem, query.value(0).toString(), query.value(1).toString());
    }

    treeItem->setText(0, game);
    treeItem->setText(1, "");
    treeItem->setText(2, description);
}

void MainWnd::addTreeChild(QTreeWidgetItem* parent,
    const QString& name, const QString& description)
{
    auto* treeItem = new QCustomTreeWidgetItem();

    treeItem->setText(1, name);
    treeItem->setText(2, description);

    parent->addChild(treeItem);
    parent->sortChildren(2, Qt::DescendingOrder);
}

void MainWnd::RemoveFromPlayers(const QString& user)
{
    m_players.remove(user);
    m_maker.InitUsers(&m_players);
}

void MainWnd::SaveTreeToJson(const QString& fileName)
{
    QJsonDocument jsonDoc(toJsonArray(ui->treeWidget));

    // Save to file
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << jsonDoc.toJson();
        file.close();
    }
    else
    {
        qDebug() << "Failed to open file for writing:" << file.errorString();
    }
}

QJsonObject MainWnd::toJsonRecursive(QTreeWidgetItem* item, bool child)
{
    QJsonObject obj;
    if (!child)
    {
        obj["Game"] = item->text(0);
    }
    else
    {
        obj[item->text(1)] = item->text(2);
    }

    int childCount = item->childCount();
    if (childCount > 0)
    {
        QJsonArray childrenArray;
        for (int i = 0; i < childCount; ++i)
        {
            QTreeWidgetItem* childItem = item->child(i);
            childrenArray.append(toJsonRecursive(childItem, true));
        }
        obj["Users"] = childrenArray;
    }

    return obj;
}

QJsonArray MainWnd::toJsonArray(QTreeWidget* treeWidget)
{
    QJsonArray jsonArray;
    int topLevelItemCount = treeWidget->topLevelItemCount();
    for (int i = 0; i < topLevelItemCount; ++i)
    {
        QTreeWidgetItem* topLevelItem = treeWidget->topLevelItem(i);
        jsonArray.append(toJsonRecursive(topLevelItem));
    }

    return jsonArray;
}

void MainWnd::on_action_Add_user_triggered()
{
    AddUser();
}

void MainWnd::on_action_Remove_user_triggered()
{
    PlayersSortFilterProxyModel* pModel = dynamic_cast<PlayersSortFilterProxyModel*>(ui->tableView->model());
    ui->tableView->setSortingEnabled(false);
    QList<QModelIndex> selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
    QSet<int> selectedRows;

    for (const auto& index: selectedIndexes)
    {
        selectedRows.insert(index.row());
    }

    for (int row: selectedRows)
    {
        if (row == m_model->rowCount() - 1)
        {
            continue;
        }

        PlayersSortFilterProxyModel* pModel = dynamic_cast<PlayersSortFilterProxyModel*>(ui->tableView->model());
        QModelIndex proxyIndex = pModel->index(row, 0);

        // Map to the source model to get the original item
        QModelIndex sourceIndex = pModel->mapToSource(proxyIndex);

        // Get the item text from the source model
        QString userName = sourceIndex.data(Qt::DisplayRole).toString();
        //QString userName = m_model->item(row, 0)->text();
        m_userDB->removeFromUserTable(userName);
        m_userDB->removeFromUser_RatingsTable(userName);

        m_model->removeData(userName);
        pModel->removeRow(row);

        m_model->SetLastItemSection(GetLastItemSection());

        QList<QTreeWidgetItem*> items = ui->treeWidget->
            findItems(userName, Qt::MatchExactly | Qt::MatchRecursive, 1);
            foreach(QTreeWidgetItem* item, items)
            {
                QTreeWidgetItem* parent = item->parent();
                parent->removeChild(item);
                delete item;
                if (parent->childCount() == 0)
                {
                    delete parent;
                }
            }
        RemoveFromPlayers(userName);
    }
    m_model->SetVerticalHeaderSize(pModel->rowCount());
}

[[maybe_unused]] void MainWnd::on_actionShow_Hide_Dashboard_triggered()
{
    if (ui->treeWidget->isHidden())
    {
        ui->treeWidget->show();
    }
    else
    {
        ui->treeWidget->hide();
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
        (index.column() == 0 && index.row() == ui->tableView->model()->rowCount() - 1))
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
    if (col == 3)
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

    SaveTreeToJson(filePath);
}

void MainWnd::on_tableView_clicked(const QModelIndex& index)
{
    if (index.column() == 0 and index.row() == m_model->GetLastItemRowToSection())
    {
        AddUser();
    }
}

void MainWnd::on_filterPushButton_clicked()
{
    QString filter = ui->filterLineEdit->text();
    PlayersSortFilterProxyModel* pModel = dynamic_cast<PlayersSortFilterProxyModel*>(ui->tableView->model());
    pModel->setFilterRegularExpression(filter);

    if (filter.trimmed().isEmpty())
    {
        pModel->setFilterRegularExpression(filter);
        m_model->SetVerticalHeaderSize(pModel->rowCount());
        int lastSection = GetLastItemSection();
        ui->tableView->verticalHeader()->moveSection(lastSection, pModel->rowCount() - 1);
        m_model->SetLastItemSection(lastSection);
    }
    else
    {
        pModel->setFilterRegularExpression("(" + filter + "|\\" +
            m_model->GetLastItemText() + ")");

        int lastSection = GetLastItemSection();
        ui->tableView->verticalHeader()->moveSection(lastSection, pModel->rowCount() - 1);
        m_model->SetLastItemSection(pModel->rowCount() - 1);
        m_model->SetVerticalHeaderSize(pModel->rowCount());
    }
}

void MainWnd::ShowError()
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
        // Use headerData as needed
    }

    return m_model->GetLastItemRowToSection();
}


