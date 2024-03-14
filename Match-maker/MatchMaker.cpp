//
// Created by Sargis Boyajyan on 02.03.24.
//

#include <QRandomGenerator>
#include <QProcess>
#include <QDir>
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
                QString currentGame = firstPlayer->GetCurrentGame();
                QStringList list = UsersDB::GetUserByRatingOfOpponent(firstPlayer->GetUseName(), currentGame,
                    firstPlayer->GetRating(currentGame));

                for (const QString& user : list)
                {
                    Player* secondPlayer = ((*m_players)[user]).get();
                    if (secondPlayer != nullptr && (secondPlayer->GetState() == State::WAITING))
                    {
                        firstPlayer->SetState(State::BUSY);
                        firstPlayer->SetState(State::BUSY);
                        std::cout << firstPlayer->GetUseName().toStdString() << " and " <<
                                  secondPlayer->GetUseName().toStdString()
                                  << " are playing " << currentGame.toStdString() <<
                                  std::endl << std::endl;
                        ExecuteGame(firstPlayer, secondPlayer, currentGame);
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
        std::cout << "Exception occurred: " << ex.what() << std::endl;
    }
}

void MatchMaker::UpdateWinner(Player* winner, const QString& game)
{
    QString user = winner->GetUseName();
    int rating = winner->GetRating(game) + 1;

    if (UsersDB::UpdateRating(user, game, rating))
    {
        m_players->value(user)->SetRating(game, rating);
        m_treeModel->UpdateGameRating(game, user, rating);
    }
}

void MatchMaker::ExecuteGame(Player* firstPlayer, Player* secondPlayer, const QString& game)
{
    auto* process = new QProcess();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString separator = QDir::separator();
    QString fileName = "XOGame";
    QString filePath = env.value("GAME_EXECUTABLE_PATH",
        QDir::currentPath()) + separator + fileName;

    QObject::connect(process, &QProcess::finished,
        [this, firstPlayer, secondPlayer, game, process](int exitCode, QProcess::ExitStatus exitStatus) mutable
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
                      UpdateWinner(firstPlayer, game);
                  }
                  if (GameResult(stdOutput.toInt()) == GameResult::SECOND)
                  {
                      UpdateWinner(secondPlayer, game);
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
    m_timer->start(RUN_MATCHES_FREQUENCY);
}

void MatchMaker::Initialise(QMap<QString, QSharedPointer<Player>>* players,
    UsersDB& userDB,
    DashboardTreeModel* treeModel)
{
    m_players = players;
    m_userDB = &userDB;
    m_treeModel = treeModel;

    std::transform(
        m_players->begin(),
        m_players->end(),
        std::back_inserter(m_users),
        [](const QSharedPointer<Player>& player) -> QString
        {
          QString userName = player.get() != nullptr ? player->GetUseName() : "";
          return (player.get() != nullptr && !player->GetPreferredGames().isEmpty()) ? userName : QString();
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
          QString userName = player.get() != nullptr ? player->GetUseName() : "";
          return (player.get() != nullptr && !player->GetPreferredGames().isEmpty()) ? userName : QString();
        });
}
