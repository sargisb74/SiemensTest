//
// Created by Sargis Boyajyan on 02.03.24.
//

#include <iostream>
#include <QRandomGenerator>
#include <QTreeWidgetItem>
#include <QProcess>
#include <QDir>
#include <QThread>
#include "MatchMaker.h"

void MatchMaker::RunMatches()
{
    performBackgroundTask();
}

void MatchMaker::performBackgroundTask()
{
    try
    {
        if (m_players->size() > 1)
        {
            int count = m_users.size();
            int index = QRandomGenerator::global()->bounded(0, count);
            Player* firstPlayer = ((*m_players)[m_users[index]]).get();
            if (firstPlayer != nullptr && (firstPlayer->GetState() == State::WAITING))
            {
                QStringList list = m_userDB->GetUserByRatingOfOpponent(firstPlayer->GetUseName(),
                    firstPlayer->GetCurrentGame(),
                    firstPlayer
                        ->GetRating(firstPlayer->GetCurrentGame()));

                for (const QString& user: list)
                {
                    Player* secondPlayer = ((*m_players)[user]).get();
                    if (secondPlayer != nullptr && (secondPlayer->GetState() == State::WAITING))
                    {
                        firstPlayer->SetState(State::BUSY);
                        firstPlayer->SetState(State::BUSY);
                        std::cout << firstPlayer->GetUseName().toStdString() << " and " <<
                                  secondPlayer->GetUseName().toStdString()
                                  << " are playing " << firstPlayer->GetCurrentGame().toStdString() <<
                                  std::endl << std::endl;
                        ExecuteGame(firstPlayer, secondPlayer);
                    }
                    else
                    {
                    }
                }
            }
        }
    }
    catch (exception& ex)
    {
        std::cout << m_users[0].toStdString() << "Exception occurred: " << ex.what() << std::endl;
    }
}

void MatchMaker::UpdateWinner(Player* winner)
{
    QString game = winner->GetCurrentGame();
    QString user = winner->GetUseName();
    int rating = winner->GetRating(game) + 1;

    if (UsersDB::UpdateRating(user, game, rating))
    {
        QList<QTreeWidgetItem*> items = m_treeWidget->findItems(game, Qt::MatchRegularExpression);
        for (QTreeWidgetItem* parentItem: items)
        {
            for (int i = 0; i < parentItem->childCount(); ++i)
            {
                QTreeWidgetItem* childItem = parentItem->child(i);

                if (childItem->text(1) == user)
                {
                    childItem->setText(2, QString::number(rating));
                    parentItem->sortChildren(2, Qt::DescendingOrder);
                    qDebug() << winner->GetUseName() << "wins " << winner->GetCurrentGame() << " game : old rating:  "
                             << winner->GetRating(winner->GetCurrentGame());
                    winner->SetRating(game, rating);
                    qDebug() << "New rating: " << winner->GetRating(winner->GetCurrentGame());

                    break;
                }
            }
        }
    }
}

void MatchMaker::ExecuteGame(Player* firstPlayer, Player* secondPlayer)
{
    QProcess* process = new QProcess();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString separator = QDir::separator();
    QString fileName = "XOGame";
    QString filePath = env.value("GAME_EXECUTABLE_PATH",
        QDir::currentPath()) + separator + fileName;

    QObject::connect(process, &QProcess::finished,
        [this, firstPlayer, secondPlayer, process](int exitCode, QProcess::ExitStatus exitStatus) mutable
        {
            // Process the result here
            if (exitStatus == QProcess::NormalExit)
            {
                QTextStream outStream(process->readAllStandardOutput());
                QString stdOutput = outStream.readAll();

                if (!stdOutput.isEmpty())
                {
                    qDebug() << "stdOutput.toInt() " << stdOutput.toInt();
                    if (GameResult(stdOutput.toInt()) == GameResult::FIRST)
                    {
                        UpdateWinner(firstPlayer);
                    }
                    if (GameResult(stdOutput.toInt()) == GameResult::SECOND)
                    {
                        UpdateWinner(secondPlayer);
                    }

                    firstPlayer->SetState(State::FREE);
                    secondPlayer->SetState(State::FREE);
                }

                auto* process1 = qobject_cast<QProcess*>(sender());
                if (process1)
                {
                    process1->deleteLater();
                }
            }
        });

    process->start(filePath, QStringList() << secondPlayer->GetCurrentGame());

    QTextStream outStream(process->readAllStandardOutput());
    QString stdOutput = outStream.readAll();
    QTextStream errStream(process->readAllStandardError());
    QString stdError = errStream.readAll();

    if (!stdError.isEmpty())
    {
        cerr << "Error occurred during game execution: " << stdError.toStdString() << endl;
    }
}

void MatchMaker::Start()
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(RunMatches()));
    m_timer->start(100);
}

void MatchMaker::Initialise(QMap<QString, QSharedPointer<Player>>* players, UsersDB& userDB, QTreeWidget* treeWidget)
{
    m_players = players;
    m_userDB = &userDB;
    m_treeWidget = treeWidget;

    std::transform(
        m_players->begin(),
        m_players->end(),
        std::back_inserter(m_users),
        [](const QSharedPointer<Player>& player) -> QString
        {
            QString userName = player.get() != NULL ? player->GetUseName() : "";
            return (player.get() != NULL && !player->GetPreferredGames().isEmpty()) ? userName
                                                                                    : QString();  // Ignore empty strings
        });

    Start();
}

void MatchMaker::InitUsers(QMap<QString, QSharedPointer<Player>>* players)
{
    m_players = players;

    std::transform(
        m_players->begin(),
        m_players->end(),
        std::back_inserter(m_users),
        [](const QSharedPointer<Player>& player) -> QString
        {
            QString userName = player.get() != NULL ? player->GetUseName() : "";
            return (player.get() != NULL && !player->GetPreferredGames().isEmpty()) ? userName
                                                                                    : QString();  // Ignore empty strings
        });
}
