#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include "source/include/PasswordTable.h"
#include <QMainWindow>
#include <QToolBar>
#include <QLineEdit>
#include <QLabel>
#include <QStatusBar>

class PasswordManager : public QMainWindow
{
 public:
  explicit PasswordManager(QWidget *parent = nullptr);
  ~PasswordManager();
  void closeEvent(QCloseEvent* event) override;
  void updateWindowsSize();
  void Subscribe();
  void onLoginKeyChanged();
  void onAESKeyEdited();
  void onSetNewAESEncrytKey();
  void SetPWBookName(const QString& pwdBookName = "");
 private:
  bool IsLoginKeyCorrect() const {
    return mLoginKey != nullptr && mLoginKey->text() == "999";
  }
  void ChangeLineEditStyleByValidContent(QLineEdit* pLE, bool isValid);

  PasswordTable* mPwdTbl {nullptr};
  QToolBar* mToolBar {nullptr};
  // -- aes label
  QLabel* mAESDecryptKeyLabel {nullptr};
  QLineEdit* mAESDecryptKey {nullptr};
  QLabel* mAESNewEncryptKeyLabel {nullptr};
  QLineEdit* mAESNewEncryptKey {nullptr};
  // -- login label
  QLabel* mLoginLabel {nullptr};
  QLineEdit* mLoginKey {nullptr};
  // --
  QStatusBar* mStatusBar {nullptr};
};
#endif // PASSWORDMANAGER_H
