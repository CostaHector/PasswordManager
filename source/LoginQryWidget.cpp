#include "LoginQryWidget.h"
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include "AccountStorage.h"
#include "PublicVariable.h"
#include "WinCredUtil.h"

LoginQryWidget::LoginQryWidget(QWidget *parent)
  : QDialog{parent} {
  setFont(ViewStyleSheet::TEXT_EDIT_FONT);

  mLoginRegisterTab = new QTabBar;
  mLoginRegisterTab->addTab(QIcon(":/FROM_ENCRYPT_CSV"), "Login");
  mLoginRegisterTab->addTab(QIcon(":/FROM_PLAIN_CSV"), "Register");
  mLoginRegisterTab->setShape(QTabBar::RoundedNorth);

  mLoginRegisterStkLo = new QStackedLayout;
  mLoginWid = CreateLoginPage();
  mLoginRegisterStkLo->insertWidget(ENCRYPT, mLoginWid);
  mRegisterWid = CreateRegisterPage();
  mLoginRegisterStkLo->insertWidget(PLAIN, mRegisterWid);

  mMainLayout = new QVBoxLayout;
  mMainLayout->addWidget(mLoginRegisterTab);
  mMainLayout->addLayout(mLoginRegisterStkLo);
  setLayout(mMainLayout);

  Subscribe();
  setMinimumWidth(720);
  setWindowIcon(QIcon(":/AES_KEY"));
  setWindowTitle("Pre-Login Security Check");
}

QLineEdit *LoginQryWidget::CreateKeyLineEdit() const {
  QLineEdit *keyLe = new QLineEdit;
  keyLe->setEchoMode(QLineEdit::Password);
  keyLe->setClearButtonEnabled(false);
  keyLe->setToolTip("Maximum 16 printable ASCII characters(AES-256 key 16 bytes)");
  QRegularExpressionValidator *validator = new QRegularExpressionValidator{QRegularExpression("[\\x20-\\x7E]{0,16}")};
  keyLe->setValidator(validator);
  // printable ASCII char, count <=16
  return keyLe;
}

QLabel *LoginQryWidget::CreateMessageLabel() const {
  QLabel *messageLabel = new QLabel;
  messageLabel->setStyleSheet("color: red;");
  messageLabel->setFont(ViewStyleSheet::TEXT_EDIT_FONT);
  return messageLabel;
}

QWidget *LoginQryWidget::CreateLoginPage() {
  QLineEdit *inputKeyLe = CreateKeyLineEdit();
  inputKeyLe->setPlaceholderText("Enter AES decryption key");

  QCheckBox *remeberKey = new QCheckBox{"Remember key"};
  remeberKey->setTristate(false);
  const int rememberState = PreferenceSettings().value("REMEMBER_KEY", Qt::CheckState::Unchecked).toInt();
  remeberKey->setCheckState(rememberState == Qt::CheckState::Checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

  QCheckBox *autoLogin = new QCheckBox{"Log in automatically"};
  autoLogin->setTristate(false);
  const int autoLoginState = PreferenceSettings().value("LOG_IN_AUTOMATICALLY", Qt::CheckState::Unchecked).toInt();
  autoLogin->setCheckState(autoLoginState == Qt::CheckState::Checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
  QLabel *messageLabel = CreateMessageLabel();

  QDialogButtonBox *loginButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  loginButtonBox->button(QDialogButtonBox::Ok)->setText("Login");

  QFormLayout *loginLo = new QFormLayout;
  loginLo->addRow("Decryption Key:", inputKeyLe);
  loginLo->addRow(remeberKey, autoLogin);
  loginLo->addWidget(messageLabel);
  loginLo->addWidget(loginButtonBox);

  QWidget *loginWid = new QWidget;
  loginWid->setLayout(loginLo);

  connect(loginButtonBox, &QDialogButtonBox::accepted, this, [this, inputKeyLe]() {
    mKey = inputKeyLe->text();
    accept();
  });
  connect(remeberKey, &QCheckBox::stateChanged, this, [inputKeyLe](int state) {
    PreferenceSettings().setValue("REMEMBER_KEY", state);
#ifdef _WIN32
    if (state == Qt::Checked && !inputKeyLe->text().isEmpty()) {
      if (!WinCredUtil::savePassword("PASSWORD_MANAGER_AES_KEY", inputKeyLe->text())) {
        qWarning("Failed to save password to Windows Credential Manager");
      }
    } else {
      WinCredUtil::deletePassword("PASSWORD_MANAGER_AES_KEY");
    }
#endif
  });
  connect(autoLogin, &QCheckBox::stateChanged, this, [](int state) { PreferenceSettings().setValue("LOG_IN_AUTOMATICALLY", state); });

  connect(loginButtonBox, &QDialogButtonBox::rejected, this, &QDialog::close);
  if (AccountStorage::IsAccountCSVFileInExistOrEmpty()) {
    // login disable, register at first
    loginWid->setEnabled(false);
    const QString msg{"File [" + AccountStorage::ENC_CSV_FILE + "] not exists."};
    messageLabel->setText("Register first! " + msg);
    qWarning("Register first! %s", qPrintable(msg));
  } else {
#ifdef _WIN32
    // allowed login
    if (rememberState == Qt::CheckState::Checked) {
      const QString aesKey = WinCredUtil::readPassword("PASSWORD_MANAGER_AES_KEY");
      if (!aesKey.isEmpty()) {
        inputKeyLe->setText(aesKey);
      }
    }
    if (autoLoginState == Qt::CheckState::Checked) {
      QTimer *autoLoginTimer = new QTimer(this);
      autoLoginTimer->setInterval(2000);
      autoLoginTimer->setSingleShot(true);
      messageLabel->setText("Auto login in " + QString::number(autoLoginTimer->interval() / 1000) + " seconds");
      connect(autoLoginTimer, &QTimer::timeout, this, [loginButtonBox]() { emit loginButtonBox->accepted(); });
      autoLoginTimer->start();
    }
#endif
  }
  return loginWid;
}

QWidget *LoginQryWidget::CreateRegisterPage() {
  QLineEdit *inputKeyLe = CreateKeyLineEdit();
  inputKeyLe->setPlaceholderText("Enter AES encryption key");

  QLineEdit *inputKeyAgainLe = CreateKeyLineEdit();
  inputKeyAgainLe->setPlaceholderText("Confirm AES encryption key");
  QLabel *messageLabel = CreateMessageLabel();

  QDialogButtonBox *registerButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  registerButtonBox->button(QDialogButtonBox::Ok)->setText("Register");

  QFormLayout *registerLo = new QFormLayout;
  registerLo->addRow("Encryption Key:", inputKeyLe);
  registerLo->addRow("Confirm Key:", inputKeyAgainLe);
  registerLo->addWidget(messageLabel);
  registerLo->addWidget(registerButtonBox);
  QWidget *registerWid = new QWidget;
  registerWid->setLayout(registerLo);

  connect(registerButtonBox, &QDialogButtonBox::accepted, this, [this, inputKeyLe, inputKeyAgainLe, messageLabel]() {
    if (inputKeyLe->text() != inputKeyAgainLe->text()) {
      qDebug("Key mismatch - first length: %d, second length: %d", inputKeyLe->text().size(), inputKeyAgainLe->text().size());
      messageLabel->setText("The encryption keys do not match");
      return;
    }
    mKey = inputKeyLe->text();
    accept();
  });
  connect(registerButtonBox, &QDialogButtonBox::rejected, this, &QDialog::close);

  if (!AccountStorage::IsAccountCSVFileInExistOrEmpty()) {
    // register disabled, login allowed
    registerWid->setEnabled(false);
    const QString msg{"File [" + AccountStorage::ENC_CSV_FILE + "] already exists."};
    messageLabel->setText("Override danger! " + msg);
  }
  return registerWid;
}

void LoginQryWidget::Subscribe() {
  connect(mLoginRegisterTab, &QTabBar::currentChanged, mLoginRegisterStkLo, &QStackedLayout::setCurrentIndex);
}

std::pair<bool, QString> LoginQryWidget::GetEncryptAndKey() const {
  return {isEncryptedSource(), getAESKey()};
}
