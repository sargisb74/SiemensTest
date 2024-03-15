//
// Created by Sargis Boyajyan on 26.02.24.
//

#ifndef MATCH_MAKER_USERSDB_H
#define MATCH_MAKER_USERSDB_H

#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QFile>
#include <QDate>
#include <QDebug>

#define DATABASE_HOSTNAME   "UsersDataBase"
#define DATABASE_NAME       "UsersDB.db"

#define USER            "UserTable"
#define USER_NAME       "UserName"
#define FIRST_NAME      "FirstName"
#define LAST_NAME       "LastName"
#define PREFERRED_GAMES "PreferredGames"
#define RATINGS         "Ratings"
#define USER_RATINGS    "User_Ratings"
#define GAME            "Game"
#define RATING          "Rating"

class UsersDB : public QObject
{
 Q_OBJECT
 public:
    explicit UsersDB(QObject* parent = nullptr);
    ~UsersDB() override;

    void ConnectToDataBase();
    static bool InsertIntoUserTable(const QVariantList&, QString&);
    static bool InsertIntoUser_RatingsTable(const QVariantList&);
    static bool RemoveFromUserTable(const QString&);
    static bool RemoveFromUser_RatingsTable(const QString&);
    static bool UpdateRating(const QString&, const QString&, int);
    static QStringList GetUserByRatingOfOpponent(const QString&, const QString&, int);
    static QMap<QString, int> GetGameToRatingMap(const QString&, const QString&);
    static QSqlQuery GetUserRating(const QString&, const QString&);

 private:
    QSqlDatabase m_dbUsers;

 private:
    bool OpenDataBase();
    bool RestoreDataBase();
    void CloseDataBase();
    static bool CreateUserTable();
    static bool CreateUserRatingsTable();
};

#endif //MATCH_MAKER_USERSDB_H
