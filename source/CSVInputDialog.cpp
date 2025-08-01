#include "CSVInputDialog.h"
#include <QIcon>
#include <QMessageBox>
#include <QPushButton>
#include "AccountStorage.h"
#include "PublicVariable.h"

CSVInputDialog::CSVInputDialog(QWidget* parent) //
  : QDialog{parent}                             //
{
  textEdit = new QTextEdit(this);
  textEdit->setPlaceholderText("Enter your text here...");

  buttonBox = new QDialogButtonBox(this);
  buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel
                                | QDialogButtonBox::Help);
  auto* okBtn = buttonBox->button(QDialogButtonBox::Ok);
  QPalette pal = okBtn->palette();
  pal.setColor(QPalette::ButtonText, QColor(30, 144, 255));
  okBtn->setPalette(pal);

  mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(textEdit);
  mainLayout->addWidget(buttonBox);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(buttonBox, &QDialogButtonBox::helpRequested, this, &CSVInputDialog::onHelpRequest);

  setFont(ViewStyleSheet::TEXT_EDIT_FONT);
  setWindowIcon(QIcon(":/edit/LOAD_FROM_INPUT"));
  setWindowTitle("Get record(s) from CSV input");
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void CSVInputDialog::raise() {
  QDialog::raise();
  setWindowTitle("Get record(s) from CSV input");
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void CSVInputDialog::onHelpRequest() {
  if (textEdit == nullptr) {
    qCritical("textEdit is nullptr");
    return;
  }
  const QString& text = textEdit->toPlainText();
  int nonEmptyLineCount{0};
  tempAccounts = AccountStorage::GetAccountsFromPlainString(text, &nonEmptyLineCount);

  if (nonEmptyLineCount != tempAccounts.size()) {
    QString msg;
    msg.reserve(40);
    msg += QString::number(tempAccounts.size());
    msg += "/";
    msg += QString::number(nonEmptyLineCount);
    msg += " line(s) are ok. Others need fixed at first.";
    QMessageBox::warning(this, "Some unexpected line(s) find", msg);
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    return;
  }
  const QString msg{QString{"%1/%2 line(s) are ok. no unexpected line(s) find"}
                        .arg(tempAccounts.size())
                        .arg(nonEmptyLineCount)};
  qDebug("%s", qPrintable(msg));
  setWindowTitle(msg);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
