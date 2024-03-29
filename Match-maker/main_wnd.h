//
// Created by Sargis Boyajyan on 23.02.24.
//

#ifndef MATCH_MAKER_MAIN_WND_H
#define MATCH_MAKER_MAIN_WND_H

#include <QMainWindow>
#include <QMenu>
#include <QApplication>
#include <unordered_set>
#include <iostream>
#include <QMap>
#include "add_user_dialog.h"
#include "UsersDB.h"
#include "Player.h"
#include "DashboardTreeModel.h"
#include "MatchMaker.h"
#include "PlayersModel.h"
#include "PlayersModelDelegate.h"

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWnd;
}
QT_END_NAMESPACE

#define SHOW_ERROR_TIMEOUT 5000

class MainWnd : public QMainWindow
{
 Q_OBJECT

 public:
    explicit MainWnd(QWidget*);

    ~MainWnd() override = default;

 private:
    Ui::MainWnd* ui;

    QStringList m_TableHeader;
    QMenu m_contextMenu;

    AddUserDialog m_addUserDlg;
    unique_ptr<UsersDB> m_userDB = make_unique<UsersDB>(new UsersDB);
    QMap<QString, QSharedPointer<Player>> m_players;
    MatchMaker m_maker;

    PlayersModel* m_tableModel{};
    DashboardTreeModel* m_treeModel{};
    PlayersModelDelegate* m_myDelegate{};

 private slots:
    void on_actionSave_the_Dashboard_to_File_triggered();
    void on_actionShow_Hide_Dashboard_triggered();
    void on_actionShow_Hide_User_List_triggered();
    [[maybe_unused]] void on_filterPushButton_clicked();

    void on_action_Add_user_triggered();
    void on_action_Remove_user_triggered();
    void slotCustomMenuRequested(const QPoint&);
    void sortIndicatorChangedSlot(int, Qt::SortOrder);
    void showError();

    void on_tableView_clicked(const QModelIndex&);

 private:
    QTimer* m_timerShowError{};

 private:
    void InitializeUIComponents();
    void InitializeViews();
    void ConnectSignalsToSlots();
    void PopulateUsersTables();
    void PopulateUsers();
    void AddUser();
    void RemoveFromPlayers(const QString&);
    int GetLastItemSection();
};

#endif //MATCH_MAKER_MAIN_WND_H
