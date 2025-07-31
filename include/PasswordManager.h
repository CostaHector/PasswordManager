#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QStatusBar>
#include <QToolBar>
#include "AccountDetailView.h"
#include "AccountListView.h"

class PasswordManager : public QMainWindow {
 public:
  explicit PasswordManager(QWidget *parent = nullptr);
  ~PasswordManager();
  void closeEvent(QCloseEvent* event) override;
  void ReadSettings();
  void Subscribe();
  void SetPWBookName(const QString& pwdBookName = "");

 private:
  AccountListView* mAccountListView {nullptr};
  AccountDetailView* mAccountDetailView{nullptr};
  QToolBar* mToolBar {nullptr};
  QStatusBar* mStatusBar {nullptr};
};
#endif // PASSWORDMANAGER_H
