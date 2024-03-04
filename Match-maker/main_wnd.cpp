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
#include "main_wnd.h"
#include "ui_main_wnd.h"
#include "add_user_dialog.h"


MainWnd::MainWnd(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWnd)
{
	ui->setupUi(this);

	InitializeUIComponents();

	//timer = new QTimer(this);

	m_maker.Initialise(&m_players, *m_userDB, ui->treeWidget);
}

MainWnd::~MainWnd()
{

}

void MainWnd::InitializeUIComponents()
{
	m_userDB->connectToDataBase();

	InitializeUsersTable();

	ui->menuEdit->setTitle(ui->menuEdit->title().prepend(QString::fromUtf8("\u200C")));
	ui->menuView->setTitle(ui->menuView->title().prepend(QString::fromUtf8("\u200C")));

	ui->tableWidget->setVerticalHeaderItem(ui->tableWidget->rowCount() - 1, &m_lastItem);
	ui->status_line_edit->hide();

	ConnectSignalsToSlots();

	PopulateUsersTables();
}

void MainWnd::InitializeUsersTable()
{
	ui->tableWidget->setRowCount(1);
	ui->tableWidget->setColumnCount(4);
	ui->tableWidget->setHorizontalHeaderLabels(m_TableHeader);
	ui->tableWidget->verticalHeader()->setVisible(true);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setShowGrid(true);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setFixedWidth(ui->tableWidget->columnWidth(1));
	ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	// Apply stylesheets to customize the header appearance
	ui->tableWidget->horizontalHeader()->setStyleSheet(
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
	ui->tableWidget->verticalHeader()->setStyleSheet(
		"QHeaderView::section {"
		"    border: 1px solid #d0d0d0;"
		"    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #f6f7fa, stop:1 #d0d0d0);"
		"    padding: 4px;"
		"    margin: 0;"
		"}"
		"QHeaderView::section:pressed {"
		"    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #d0d0d0, stop:1 #f6f7fa);"
		"}"
	);
}

void MainWnd::ConnectSignalsToSlots()
{
	auto *addUserMenuItem = new QAction("Add User", this);
	auto *removeUserMenuItem = new QAction("Remove User", this);
	/* Set the actions to the menu */
	m_contextMenu.addAction(addUserMenuItem);
	m_contextMenu.addAction(removeUserMenuItem);

	m_timerShowError = new QTimer(this);

	connect(ui->tableWidget->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
			this, SLOT(sortIndicatorChangedSlot(int, Qt::SortOrder)));
	// Connect SLOT to context menu
	connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this,
			&MainWnd::slotCustomMenuRequested);
	connect(addUserMenuItem, SIGNAL(triggered()), this,
			SLOT(on_action_Add_user_triggered()));
	connect(removeUserMenuItem, SIGNAL(triggered()), this,
			SLOT(on_action_Remove_user_triggered()));
	connect(m_timerShowError, SIGNAL(timeout()), this, SLOT(ShowError()));
}

void MainWnd::AddUser()
{
	ui->tableWidget->setSortingEnabled(false);
	int index = ui->tableWidget->rowCount() - 1;

	if (m_addUserDlg.exec() == QDialog::Accepted) {
		QStringList list = m_addUserDlg.getPreferredGames();
		QString error;
		if (!m_userDB->insertIntoUserTable(QVariantList() << m_addUserDlg.getUsername()
														  << m_addUserDlg.getFirstName()
														  << m_addUserDlg.getLastName()
														  << list.join(", ")
														  << QString::number(0), error)) {
			ui->status_line_edit->show();
			ui->status_line_edit->setText(error);
			m_timerShowError->start(5000);

			return;
		}
		for (const auto &str: std::as_const(list)) {
			m_userDB->insertIntoUser_RatingsTable(QVariantList() << m_addUserDlg.getUsername() <<
																 str << QString::number(0));
			QList<QTreeWidgetItem *> items = ui->treeWidget->findItems(str, Qt::MatchExactly);
			if (items.count() == 0) {
				addTreeRoot(str, "");
			}
			else
				for (QTreeWidgetItem *item: items) {
					addTreeChild(item, m_addUserDlg.getUsername(), QString::number(0));
				}
		}

		ui->tableWidget->insertRow(index);
		ui->tableWidget->setItem(index, 0,
								 new QCustomTableWidgetItem(m_addUserDlg.getUsername()));
		ui->tableWidget->setItem(index, 1, new QTableWidgetItem(m_addUserDlg.getFirstName()));
		ui->tableWidget->setItem(index, 2, new QTableWidgetItem(m_addUserDlg.getLastName()));
		ui->tableWidget->setItem(index, 3, new QTableWidgetItem(list.join(", ")));

		QMap<QString, int> gameToRating;
		for (const QString &str: list) {
			gameToRating[str] = 0;
		}

		m_players[m_addUserDlg.getUsername()] = make_unique<Player>(m_addUserDlg.getUsername(),
																	std::move(gameToRating));
		m_maker.InitUsers();
	}

	ui->treeWidget->expandAll();
}

void MainWnd::PopulateUsersTables()
{
	QSqlQuery query("SELECT "
					USER_NAME ", "
					FIRST_NAME ", "
					LAST_NAME ", "
					PREFERRED_GAMES
					" FROM " USER);

	QMap<QString, QMap<QString, int>> userToGame;
	for (int index = 0; query.next(); index++) {
		// TODO: reused in AddUser, need to make a method.
		ui->tableWidget->insertRow(index);
		ui->tableWidget->setItem(index, 0,
								 new QCustomTableWidgetItem(query.value(0).toString()));
		ui->tableWidget->setItem(index, 1, new QTableWidgetItem(query.value(1).toString()));
		ui->tableWidget->setItem(index, 2, new QTableWidgetItem(query.value(2).toString()));
		ui->tableWidget->setItem(index, 3,
								 new QTableWidgetItem(query.value(3).toString()));

		QString games = query.value(3).toString();
		userToGame[query.value(0).toString()] = m_userDB->GetGameToRatingMap(query.value(0).toString(), games);
	}

	for (auto [key, value]: userToGame.asKeyValueRange()) {
		m_players[key] = std::make_unique<Player>(key, std::move(value));
	}

	m_AddUserItem.setText("+Add User");
	m_AddUserItem.setData(Qt::UserRole, true);
	ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, &m_AddUserItem);
	ui->tableWidget->setSpan(ui->tableWidget->rowCount() - 1, 1, 1, 2);
	m_lastItem.setText("");
	ui->tableWidget->setVerticalHeaderItem(ui->tableWidget->rowCount() - 1, &m_lastItem);

	QSqlQuery query1("SELECT DISTINCT " GAME
					 " FROM " USER_RATINGS " ORDER BY " GAME);

	for (int index = 0; query1.next(); index++) {
		addTreeRoot(query1.value(0).toString(), "");
	}

	ui->treeWidget->expandAll();
}

void MainWnd::addTreeRoot(const QString &game, const QString &description)
{
	auto *treeItem = new QCustomTreeWidgetItem(ui->treeWidget);

	QSqlQuery query = m_userDB->GetUserRating(game);
	for (; query.next();) {
		addTreeChild(treeItem, query.value(0).toString(), query.value(1).toString());
	}

	treeItem->setText(0, game);
	treeItem->setText(1, "");
	treeItem->setText(2, description);
}

void MainWnd::addTreeChild(QTreeWidgetItem *parent,
						   const QString &name, const QString &description)
{
	auto *treeItem = new QCustomTreeWidgetItem();

	treeItem->setText(1, name);
	treeItem->setText(2, description);

	parent->addChild(treeItem);
	parent->sortChildren(2, Qt::DescendingOrder);
}

void MainWnd::RemoveFromPlayers(const QString &user)
{
	m_players.erase(user);
	m_maker.InitUsers();
}

void MainWnd::SaveTreeToJson(const QString &fileName)
{
	QJsonDocument jsonDoc(toJsonArray(ui->treeWidget));

	// Save to file
	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&file);
		out << jsonDoc.toJson();
		file.close();
	}
	else {
		qDebug() << "Failed to open file for writing:" << file.errorString();
	}
}

QJsonObject MainWnd::toJsonRecursive(QTreeWidgetItem *item, bool child)
{
	QJsonObject obj;
	if (!child) {
		obj["Game"] = item->text(0);
	}
	else {
		obj[item->text(1)] = item->text(2);
	}

	int childCount = item->childCount();
	if (childCount > 0) {
		QJsonArray childrenArray;
		for (int i = 0; i < childCount; ++i) {
			QTreeWidgetItem *childItem = item->child(i);
			childrenArray.append(toJsonRecursive(childItem, true));
		}
		obj["Users"] = childrenArray;
	}

	return obj;
}

QJsonArray MainWnd::toJsonArray(QTreeWidget *treeWidget)
{
	QJsonArray jsonArray;
	int topLevelItemCount = treeWidget->topLevelItemCount();
	for (int i = 0; i < topLevelItemCount; ++i) {
		QTreeWidgetItem *topLevelItem = treeWidget->topLevelItem(i);
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
	QList<QTableWidgetItem *> selectedItems = ui->tableWidget->selectedItems();
	QSet<int> selectedRows;

	for (const auto *item: selectedItems) {
		selectedRows.insert(item->row());
	}

	for (int row: selectedRows) {
		if (row == ui->tableWidget->rowCount() - 1) {
			continue;
		}
		QString userName = ui->tableWidget->item(row, 0)->text();
		m_userDB->removeFromUserTable(userName);
		m_userDB->removeFromUser_RatingsTable(userName);
		ui->tableWidget->removeRow(row);

		QList<QTreeWidgetItem *> items = ui->treeWidget->
			findItems(userName, Qt::MatchExactly | Qt::MatchRecursive, 1);
			foreach(QTreeWidgetItem *item, items) {
				QTreeWidgetItem *parent = item->parent();
				parent->removeChild(item);
				delete item;
				if (parent->childCount() == 0) {
					delete parent;
				}
			}
		RemoveFromPlayers(userName);
	}
}

[[maybe_unused]] void MainWnd::on_actionShow_Hide_Dashboard_triggered()
{
	if (ui->treeWidget->isHidden()) {
		ui->treeWidget->show();
	}
	else {
		ui->treeWidget->hide();
	}
}

void MainWnd::on_actionShow_Hide_User_List_triggered()
{
	if (ui->tableWidget->isHidden()) {
		ui->tableWidget->show();
		ui->label->show();
		ui->filterPushButton->show();
		ui->filterLineEdit->show();
	}
	else {
		ui->tableWidget->hide();
		ui->label->hide();
		ui->filterPushButton->hide();
		ui->filterLineEdit->hide();
	}
}

void MainWnd::slotCustomMenuRequested(const QPoint &pos)
{
	QTableWidgetItem *item = ui->tableWidget->itemAt(pos);
	if (item == nullptr ||
		(item->column() == 0 && item->row() == ui->tableWidget->rowCount() - 1)) {
		m_contextMenu.actions().at(1)->setDisabled(true);
	}
	else {
		m_contextMenu.actions().at(1)->setDisabled(false);
	}

	m_contextMenu.popup(ui->tableWidget->viewport()->mapToGlobal(pos));
}

void MainWnd::sortIndicatorChangedSlot(int col, Qt::SortOrder sortType)
{
	if (col == 3) {
		ui->tableWidget->setSortingEnabled(false);
	}
	else {
		ui->tableWidget->setSortingEnabled(true);
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

void MainWnd::on_tableWidget_cellClicked(int row, int column)
{
	if (column == 0 and row == ui->tableWidget->rowCount() - 1) {
		AddUser();
	}
}

void MainWnd::on_filterPushButton_clicked()
{
	QString filter = ui->filterLineEdit->text();
	for (int i = 0; i < ui->tableWidget->rowCount() - 1; i++) {
		if (filter.trimmed().isEmpty()) {
			ui->tableWidget->showRow(i);
		}
		else {
			ui->tableWidget->hideRow(i);
		}
	}

	if (filter.trimmed().isEmpty()) {
		return;
	}

	QList<QTableWidgetItem *> items = ui->tableWidget->findItems(filter, Qt::MatchRegularExpression);
	for (int i = 0; i < items.count(); i++) {
		if (items.at(i)->column() == 0) {
			ui->tableWidget->showRow(items.at(i)->row());
		}
	}
}

void MainWnd::ShowError()
{
	ui->status_line_edit->hide();
}