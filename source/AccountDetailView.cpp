#include "AccountDetailView.h"
#include "PublicVariable.h"

AccountDetailView::AccountDetailView(const QString &title, QWidget *parent) //
  : QDockWidget{parent} {
  setObjectName(title);
  mFormWid = new QWidget{this};

  etType = new QLineEdit{mFormWid};
  etName = new QLineEdit{mFormWid};
  etAccount = new QLineEdit{mFormWid};
  etPwd = new QLineEdit{mFormWid};
  etOthers = new QTextEdit{mFormWid};

  mBtnRecover = new QPushButton{"恢复", nullptr};
  mBtnApply = new QPushButton{"应用", nullptr};
  mBtnApply->setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key::Key_S));
  mBtnApply->setToolTip(QString("<b>%1 (%2)</b><br/> Apply changes right now.") //
                            .arg(mBtnApply->text(), mBtnApply->shortcut().toString()));

  mForm = new QFormLayout;
  mForm->addRow("类型:", etType);
  mForm->addRow("名称:", etName);
  mForm->addRow("账户:", etAccount);
  mForm->addRow("密码:", etPwd);
  mForm->addRow("其他:", etOthers);
  mForm->addRow(mBtnRecover, mBtnApply);

  mFormWid->setLayout(mForm);
  setWidget(mFormWid);
  setFont(ViewStyleSheet::TEXT_EDIT_FONT);
  mBtnApply->setStyleSheet(ViewStyleSheet::SUBMIT_BTN_STYLE);
  Subscribe();
  editNotHappen();
  setWindowTitle("Detail View");
}

void AccountDetailView::Subscribe() {
  connect(mBtnRecover, &QPushButton::clicked, this, &AccountDetailView::onRecoverModify);
  connect(mBtnApply, &QPushButton::clicked, this, &AccountDetailView::onApplyModify);
  connect(etType, &QLineEdit::textEdited, this, &AccountDetailView::editHappen);
  connect(etName, &QLineEdit::textEdited, this, &AccountDetailView::editHappen);
  connect(etAccount, &QLineEdit::textEdited, this, &AccountDetailView::editHappen);
  connect(etPwd, &QLineEdit::textEdited, this, &AccountDetailView::editHappen);
  connect(etOthers, &QTextEdit::textChanged, this, &AccountDetailView::editHappen);
}

void AccountDetailView::editHappen() {
  mBtnApply->setEnabled(true);
  mBtnRecover->setEnabled(true);
}

void AccountDetailView::editNotHappen() {
  mBtnApply->setEnabled(false);
  mBtnRecover->setEnabled(false);
}

void AccountDetailView::UpdateDisplay(AccountInfo *pAcc) {
  pAccount = pAcc;
  // QSignalBlocker blocker{this};
  if (pAcc == nullptr) {
    etType->clear();
    etName->clear();
    etAccount->clear();
    etPwd->clear();
    etOthers->clear();
    return;
  }
  etType->setText(pAccount->typeStr);
  etName->setText(pAccount->nameStr);
  etAccount->setText(pAccount->accountStr);
  etPwd->setText(pAccount->pwdStr);
  etOthers->setPlainText(pAccount->othersStr);
  editNotHappen();
}

void AccountDetailView::onRecoverModify() {
  if (pAccount == nullptr) {
    return;
  }
  etType->setText(pAccount->typeStr);
  etName->setText(pAccount->nameStr);
  etAccount->setText(pAccount->accountStr);
  etPwd->setText(pAccount->pwdStr);
  etOthers->setPlainText(pAccount->othersStr);
  editNotHappen();
}
void AccountDetailView::onApplyModify() {
  if (pAccount == nullptr) {
    return;
  }
  pAccount->typeStr = etType->text();
  pAccount->nameStr = etName->text();
  pAccount->accountStr = etAccount->text();
  pAccount->pwdStr = etPwd->text();
  pAccount->othersStr = etOthers->toPlainText();
  if (pSetTableDirty != nullptr) {
    pSetTableDirty();
  }
  editNotHappen();
}
