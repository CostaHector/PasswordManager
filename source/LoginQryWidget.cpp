#include "LoginQryWidget.h"
#include <QMessageBox>
#include "PublicVariable.h"

LoginQryWidget::LoginQryWidget(QWidget *parent)
  : QDialog{parent} {
  updateTitle(false); // 初始标题
  setFont(ViewStyleSheet::TEXT_EDIT_FONT);

  mSourceCombo = new QComboBox(this);
  mSourceCombo->insertItem(DataSourceType::ENCRYPT, "From Encrypt CSV File");
  mSourceCombo->insertItem(DataSourceType::PLAIN, "From Plain CSV File");

  mKeyEdit = new QLineEdit(this);
  mKeyEdit->setPlaceholderText("Input AES key here");
  mKeyEdit->setEchoMode(QLineEdit::Password);
  mSourceCombo->setToolTip("Input limited to 16 characters (printable ASCII only)");

  // 定义正则表达式：仅允许可打印 ASCII 字符（32-126），且长度 ≤16
  static const QRegularExpressionValidator validator{QRegularExpression("[\\x20-\\x7E]{0,16}")};
  mKeyEdit->setValidator(&validator);

  mConfirmKeyEdit = new QLineEdit(this);
  mConfirmKeyEdit->setPlaceholderText("Input AES key again here");
  mConfirmKeyEdit->setEchoMode(QLineEdit::Password);
  mConfirmKeyEdit->hide();

  mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,this);

  mMainLayout = new QVBoxLayout;
  mMainLayout->addWidget(mSourceCombo);
  mMainLayout->addWidget(mKeyEdit);
  mMainLayout->addWidget(mConfirmKeyEdit);
  mMainLayout->addWidget(mButtonBox);
  setLayout(mMainLayout);
  setMinimumWidth(600);

  Subscribe();
}

void LoginQryWidget::onSourceChanged(int index) {
  const bool isEncrypted = (index == 0);

  mKeyEdit->setPlaceholderText(isEncrypted ? "Enter AES decryption key here" : "Set AES decryption key here");

  mConfirmKeyEdit->setVisible(!isEncrypted);
  updateTitle(false); // 重置标题
  adjustSize();
}

void LoginQryWidget::validateInput() {
  if (!isEncryptedSource()) { // 明文
    if (mKeyEdit->text() != mConfirmKeyEdit->text()) {
      updateTitle(true); // 显示不匹配提示
      return;
    } 
  }
  accept(); // 验证通过
}

void LoginQryWidget::updateTitle(bool isMismatch) {
  if (isMismatch) {
    setWindowIcon(QIcon(":/WARNING"));
    setWindowTitle("Passwords Do Not Match - Try Again");
  } else {
    setWindowIcon(QIcon(":/AES_KEY"));
    setWindowTitle("Pre-Login Security Check");
  }
}

void LoginQryWidget::Subscribe() {
  connect(mSourceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LoginQryWidget::onSourceChanged);
  connect(mButtonBox, &QDialogButtonBox::accepted, this, &LoginQryWidget::validateInput);
  connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::close);
}

std::pair<bool, QString> LoginQryWidget::GetEncryptAndKey() const {
  return {isEncryptedSource(), getAESKey()};
}
