#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QLineEdit>
#include <QMainWindow>
#include <QStatusBar>
#include <QTextEdit>
#include <QToolBar>
#include "AccountDetailView.h"
#include "AccountListView.h"
#include "CSVInputDialog.h"

class PasswordManager : public QMainWindow {
 public:
  explicit PasswordManager(QWidget *parent = nullptr);
  ~PasswordManager();
  void closeEvent(QCloseEvent* event) override;
  void ReadSettings();
  void Subscribe();
  void SetPWBookName();

 private:
  void onSave();
  void onGetRecordsFromInput();
  void onLoadRecordsFromCSVInput();
  void ShowPlainCSVContents();

  AccountListView* mAccountListView {nullptr};
  AccountDetailView* mAccountDetailView{nullptr};
  QLineEdit* mSearchText{nullptr};
  QToolBar* mToolBar {nullptr};
  QStatusBar* mStatusBar {nullptr};
  CSVInputDialog* mTextInputDialog{nullptr};
  QTextEdit *mPlainCSVContentWid{nullptr};
};
#endif // PASSWORDMANAGER_H
