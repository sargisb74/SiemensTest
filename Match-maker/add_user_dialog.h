//
// Created by Sargis Boyajyan on 26.02.24.
//

#ifndef MATCH_MAKER_ADD_USER_DIALOG_H
#define MATCH_MAKER_ADD_USER_DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class AddUserDialog;
}
QT_END_NAMESPACE

class AddUserDialog : public QDialog
{
 Q_OBJECT

 public:
    explicit AddUserDialog(QWidget* parent = nullptr);

    ~AddUserDialog() override;

 public:
    QString getUsername();

    QString getFirstName();

    QString getLastName();

    QStringList getPreferredGames();

 private slots:
    void validateAndAccept();

 private:
    Ui::AddUserDialog* ui;
};

#endif //MATCH_MAKER_ADD_USER_DIALOG_H
