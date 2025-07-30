#include "PasswordManager.h"
#include "source/include/PublicVariable.h"
#include "source/include/TableEditActions.h"
#include <QMessageBox>

typedef void (*SET_WINDOW_TILE)(QWidget* pWid, const QString& pwdBookName);

PasswordManager::PasswordManager(QWidget* parent) : QMainWindow(parent) {
  mPwdTbl = new PasswordTable{this};
  setCentralWidget(mPwdTbl);

  mAESDecryptKeyLabel = new QLabel{"AES Key:", this};
  mAESDecryptKey = new QLineEdit{"", this};
  mAESDecryptKey->setEchoMode(QLineEdit::Password);

  mAESNewEncryptKeyLabel = new QLabel{"New Encrypt AES Key:", this};
  mAESNewEncryptKey = new QLineEdit{"", this};
  mAESNewEncryptKey->setPlaceholderText("use new aes key? input here");
  mAESNewEncryptKey->setEchoMode(QLineEdit::Password);
  mAESNewEncryptKey->setToolTip("If you want to use new aes key, input (0,16] char ascii here");

  mLoginLabel = new QLabel{"Login Key:", this};
  mLoginKey = new QLineEdit{"999", this};
  mLoginKey->setEchoMode(QLineEdit::PasswordEchoOnEdit);

  mToolBar = new QToolBar{"Edit Toolbar", this};
  mToolBar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
  const auto& tblEditInst = GetTableEditActionsInst();
  mToolBar->addActions(tblEditInst.ROW_EDIT_AG->actions());
  mToolBar->addSeparator();
  mToolBar->addWidget(mAESDecryptKeyLabel);
  mToolBar->addWidget(mAESDecryptKey);
  mToolBar->addSeparator();
  mToolBar->addWidget(mAESNewEncryptKeyLabel);
  mToolBar->addWidget(mAESNewEncryptKey);
  mToolBar->addSeparator();
  mToolBar->addWidget(mLoginLabel);
  mToolBar->addWidget(mLoginKey);
  mToolBar->addSeparator();
  mToolBar->addAction(tblEditInst.SAVE_CHANGES);
  addToolBar(Qt::ToolBarArea::TopToolBarArea, mToolBar);

  mStatusBar = new QStatusBar{this};
  setStatusBar(mStatusBar);

  Subscribe();

  updateWindowsSize();
  setWindowIcon(QIcon(":/bankcard/PASSWORD_MANAGER"));
  SetPWBookName(JsonReader::PDBOOK_TABLE_NAME);
}

PasswordManager::~PasswordManager() {}

void PasswordManager::closeEvent(QCloseEvent* event) {
  PreferenceSettings().setValue("PASSWORD_TABLEVIEW_GEOMETRY", saveGeometry());
  QMainWindow::closeEvent(event);
}

void PasswordManager::updateWindowsSize() {
  if (PreferenceSettings().contains("PASSWORD_TABLEVIEW_GEOMETRY")) {
    restoreGeometry(PreferenceSettings().value("PASSWORD_TABLEVIEW_GEOMETRY").toByteArray());
  } else {
    setGeometry(QRect(0, 0, 1024, 768));
  }
}

void PasswordManager::Subscribe() {
  connect(mLoginKey, &QLineEdit::returnPressed, this, &PasswordManager::onLoginKeyChanged);
  connect(mAESDecryptKey, &QLineEdit::returnPressed, this, &PasswordManager::onAESKeyEdited);
  connect(mAESNewEncryptKey, &QLineEdit::returnPressed, this, &PasswordManager::onSetNewAESEncrytKey);
}

void PasswordManager::onLoginKeyChanged() {
  const bool isLoginKeyCorrect = IsLoginKeyCorrect();
  const bool isAESKeyCorrect = mPwdTbl->IsDecryptKeyCorrect();
  ChangeLineEditStyleByValidContent(mLoginKey, isLoginKeyCorrect);
  const auto& tblEditInst = GetTableEditActionsInst();
  tblEditInst.SAVE_CHANGES->setEnabled(isLoginKeyCorrect && isAESKeyCorrect);

  mStatusBar->showMessage(QString("Login Key bCorrect[%1] | AES Key bCorrect[%2]").arg(isLoginKeyCorrect).arg(isAESKeyCorrect));
}

void PasswordManager::onAESKeyEdited() {
  QString key16 = mAESDecryptKey->text();
  if (mPwdTbl->IsDirty()) {
    auto btn = QMessageBox::warning(this, "Reload json and use new key16 to decrypt",
                                    "<b>Changes unsaved</b>.<br/>"
                                    "Reload now will <b>lose all you changes</b>.<br/>"
                                    "Are you sure apply new key to json decryption and reload now?",
                                    QMessageBox::StandardButton::Apply | QMessageBox::StandardButton::Cancel,  // candidate
                                    QMessageBox::StandardButton::Cancel);
    if (btn != QMessageBox::StandardButton::Apply) {
      qWarning("Used cancel apply new key to json decryption.");
      return;
    }
  }
  mPwdTbl->SetKey16(key16);
  const bool isAESKeyCorrect = mPwdTbl->IsDecryptKeyCorrect();
  const bool isLoginKeyCorrect = IsLoginKeyCorrect();
  ChangeLineEditStyleByValidContent(mAESDecryptKey, isAESKeyCorrect);

  const auto& tblEditInst = GetTableEditActionsInst();
  tblEditInst.SAVE_CHANGES->setEnabled(isLoginKeyCorrect && isAESKeyCorrect);

  mStatusBar->showMessage(QString("Login Key bCorrect[%1] | AES Key bCorrect[%2]").arg(isLoginKeyCorrect).arg(isAESKeyCorrect));
}

#include <QInputDialog>
void PasswordManager::onSetNewAESEncrytKey() {
  const QString newKeyUsedToSaveJson = mAESNewEncryptKey->text();
  if (!JsonReader::IsKeyLengthValid(newKeyUsedToSaveJson)) {
    QMessageBox::warning(this, "New AES encryption key length invalid", newKeyUsedToSaveJson);
    return;
  }
  const QString reenteredKey{QInputDialog::getText(this, "Confirm change to the AES encryption key?", " Please re-enter the new AES key in below line edit")};
  if (newKeyUsedToSaveJson != reenteredKey) {
    QMessageBox::warning(this, "AES encryption key change aborted",  //
                         QString{"The entered AES keys do not match the former one:\n%1\n%2"}.arg(newKeyUsedToSaveJson).arg(reenteredKey));
    return;
  }
  mPwdTbl->SetEncryptKey16(newKeyUsedToSaveJson);
  const bool isNewEncryptKeyLengthValid = mPwdTbl->IsEncryptKeyCorrect();
  ChangeLineEditStyleByValidContent(mAESNewEncryptKey, isNewEncryptKeyLengthValid);
  mStatusBar->showMessage("AES encryption key has been reset. ‌Warning:‌ Saving will immediately activate the new key and irreversibly re-encrypt all data using the updated encryption.");
}

void PasswordManager::SetPWBookName(const QString& pwdBookName) {
  QString title = "Password Manager";
  title += " | ";
  if (pwdBookName.isEmpty()) {
    title += "No password book selected";
  } else {
    title += pwdBookName;
  }
  setWindowTitle(title);
}

void PasswordManager::ChangeLineEditStyleByValidContent(QLineEdit* pLE, bool isValid) {
  if (pLE == nullptr) {
    qWarning("pLE is nullptr");
    return;
  }

  pLE->setStyleSheet(isValid ? "border : 4px solid green; background-color:#4EEE94" : "border : 4px solid red; background-color:#FF6000");
}
