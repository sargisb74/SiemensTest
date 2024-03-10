//
// Created by Sargis Boyajyan on 26.02.24.
//

#include <QDir>
#include "UsersDB.h"

UsersDB::UsersDB(QObject* parent)
    : QObject(parent)
{

}

UsersDB::~UsersDB()
{
    closeDataBase();
}

void UsersDB::connectToDataBase()
{
    if (!QFile(QDir::homePath() + QDir::separator() + DATABASE_NAME).exists())
    {
        this->restoreDataBase();
    }
    else
    {
        this->openDataBase();
    }
}

bool UsersDB::restoreDataBase()
{
    if (this->openDataBase())
    {
        if (!this->createUserTable() || !createUserRatingsTable())
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        qDebug() << "Failed to restore the database";
        return false;
    }
}

bool UsersDB::openDataBase()
{
    dbUsers = QSqlDatabase::addDatabase("QSQLITE");
    dbUsers.setHostName(DATABASE_HOSTNAME);
    dbUsers.setDatabaseName(QDir::homePath() + QDir::separator() + DATABASE_NAME);
    if (dbUsers.open())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void UsersDB::closeDataBase()
{
    dbUsers.close();
}

bool UsersDB::createUserTable()
{
    QSqlQuery query;
    if (!query.exec("CREATE TABLE " USER " ("
                    USER_NAME       " VARCHAR(16)   NOT NULL UNIQUE,"
                    FIRST_NAME      " VARCHAR(225)  NOT NULL,"
                    LAST_NAME       " VARCHAR(225)  NOT NULL,"
                    PREFERRED_GAMES " VARCHAR(225)  NOT NULL,"
                    RATINGS         " INTEGER       NOT NULL"
                    " )"
    ))
    {
        qDebug() << "DataBase: error of create " << USER;
        qDebug() << query.lastError().text();
        return false;
    }
    else
    {
        return true;
    }
}

bool UsersDB::createUserRatingsTable()
{
    QSqlQuery query;
    if (!query.exec("CREATE TABLE " USER_RATINGS " ("
                    USER_NAME " VARCHAR(16)   NOT NULL,"
                    GAME      " VARCHAR(16)   NOT NULL,"
                    RATING    " INTEGER       NOT NULL"
                    " )"
    ))
    {
        qDebug() << "DataBase: error of create " << USER_RATINGS;
        qDebug() << query.lastError().text();
        return false;
    }
    else
    {
        return true;
    }
}

bool UsersDB::insertIntoUserTable(const QVariantList& data, QString& error)
{
    QSqlQuery query;
    query.clear();
    if (!query.prepare("INSERT INTO " USER " ( " USER_NAME ", "
                       FIRST_NAME ", "
                       LAST_NAME ", "
                       PREFERRED_GAMES ", "
                       RATINGS " ) "
                       "VALUES (:UserName, :FirstName, :LastName, :PreferredGames, :Ratings)"))
    {
        error = "Error preparing query:" + query.lastError().text();
        qDebug() << error;
        return false;
    }

    query.bindValue(":UserName", data[0].toString());
    query.bindValue(":FirstName", data[1].toString());
    query.bindValue(":LastName", data[2].toString());
    query.bindValue(":PreferredGames", data[3].toString());
    query.bindValue(":Ratings", data[4].toInt());

    if (!query.exec())
    {
        error = "Error in creating user query for " + QString(USER) + ": " + query.lastError().text();
        qDebug() << error;

        return false;
    }
    else
    {
        return true;
    }
}

bool UsersDB::UserExists(const QString& userName)
{
    QSqlQuery query;
    if (!query.prepare("SELECT " USER_NAME " FROM " USER
                       " WHERE " USER_NAME " = :UserName"))
    {
        qDebug() << "Error preparing query:" << query.lastError().text();
        return false;
    }

    query.bindValue(":UserName", userName);

    if (!query.exec())
    {
        qDebug() << "error getting user from " << USER;
        qDebug() << query.lastError().text();
        return false;
    }

    for (int index = 0; query.next(); index++)
    {
        if (query.value(0).toString() == userName)
        {
            return true;
        }
    }

    return false;
}

bool UsersDB::insertIntoUser_RatingsTable(const QVariantList& data)
{
    QSqlQuery query;
    if (!query.prepare("INSERT INTO " USER_RATINGS " ( " USER_NAME ", "
                       GAME ", " RATING " ) VALUES (:UserName, :Game, :Rating)"))
    {
        qDebug() << "Error preparing query:" << query.lastError().text();
        return false;
    }

    query.bindValue(":UserName", data[0].toString());
    query.bindValue(":Game", data[1].toString());
    query.bindValue(":Rating", data[2].toInt());

    if (!query.exec())
    {
        qDebug() << "error insert into " << USER_RATINGS;
        qDebug() << query.lastError().text();
        return false;
    }
    else
    {
        return true;
    }
}

bool UsersDB::removeFromUserTable(const QString& userName)
{
    QSqlQuery query;
    if (!query.prepare("DELETE FROM " USER " WHERE " USER_NAME " = (:UserName)"))
    {
        qDebug() << "Error preparing query:" << query.lastError().text();
        return false;
    }

    query.bindValue(":UserName", userName);

    if (!query.exec())
    {
        qDebug() << "error deleting from " << USER;
        qDebug() << query.lastError().text();
        return false;
    }
    else
    {
        return true;
    }
}

bool UsersDB::removeFromUser_RatingsTable(const QString& userName)
{
    QSqlQuery query;
    if (!query.prepare("DELETE FROM " USER_RATINGS " WHERE " USER_NAME " = (:UserName)"))
    {
        qDebug() << "Error preparing query:" << query.lastError().text();
        return false;
    }

    query.bindValue(":UserName", userName);

    if (!query.exec())
    {
        qDebug() << "error deleting from " << USER_RATINGS;
        qDebug() << query.lastError().text();
        return false;
    }
    else
    {
        return true;
    }
}

bool UsersDB::UpdateRating(const QString& user, const QString& game, int rating)
{
    QSqlQuery query;
    if (!query.prepare("UPDATE " USER_RATINGS " SET " RATING
                       " = (:Rating) WHERE " GAME " = (:Game) AND " USER_NAME " = (:UserName)"))
    {
        qDebug() << "Error preparing query:" << query.lastError().text();
        return false;
    }

    query.bindValue(":Rating", rating);
    query.bindValue(":Game", game);
    query.bindValue(":UserName", user);

    if (!query.exec())
    {
        qDebug() << "error deleting from " << USER_RATINGS;
        qDebug() << query.lastError().text();
        return false;
    }
    else
    {
        return true;
    }
}

QStringList UsersDB::GetUserByRatingOfOpponent(const QString& user, const QString& game, int rating)
{
    QStringList list;
    QSqlQuery query;

    if (!query.prepare("SELECT " USER_NAME
                       " FROM " USER_RATINGS " WHERE " USER_NAME " != (:UserName) AND "
                       GAME " == (:Game) AND " RATING " BETWEEN (:Rating1) AND (:Rating2)"))
    {
        qDebug() << "Error preparing query:" << query.lastError().text();
        return {};
    }

    query.bindValue(":UserName", user);
    query.bindValue(":Game", game);
    query.bindValue(":Rating1", rating - 1);
    query.bindValue(":Rating2", rating + 3);

    //qDebug() << "+++     rating    ++++" << rating;

    if (!query.exec())
    {
        qDebug() << "error deleting from " << USER;
        qDebug() << query.lastError().text();
        return {};
    }

    for (; query.next();)
    {
        list.push_back(query.value(0).toString());
    }

    return list;
}

QMap<QString, int> UsersDB::GetGameToRatingMap(const QString& user, const QString& games)
{
    QMap<QString, int> gameToRating;
    QList list = games.split(", ");

    for (const QString& game: list)
    {
        QSqlQuery query;
        if (!query.prepare("SELECT " GAME ", " RATING
                           " FROM " USER_RATINGS " WHERE " USER_NAME " = (:UserName)"))
        {
            qDebug() << "Error preparing query:" << query.lastError().text();
            return {};
        }

        query.bindValue(":UserName", user);

        if (!query.exec())
        {
            qDebug() << "error deleting from " << USER;
            qDebug() << query.lastError().text();
            return {};
        }

        query.next();

        gameToRating[game] = query.value(1).toInt();
    }

    return gameToRating;
}

QSqlQuery UsersDB::GetUserRating(const QString& game)
{
    QSqlQuery query;
    if (!query.prepare("SELECT "
                       USER_NAME ", "
                       RATING
                       " FROM " USER_RATINGS
                       " WHERE " GAME " = :Game ORDER BY " RATING " DESC"))
    {
        qDebug() << "Error preparing query:" << query.lastError().text();
        return query;
    }

    query.bindValue(":Game", game);

    if (!query.exec())
    {
        qDebug() << "error deleting from " << USER;
        qDebug() << query.lastError().text();
        return query;
    }

    return query;
}