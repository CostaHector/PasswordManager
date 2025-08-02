#include "PasswordManager.h"
#include <QDateTime>
#include <QDesktopServices>
#include <QFileInfo>
#include <QMessageBox>
#include <QPlainTextEdit>
#include "Notificator.h"
#include "PublicVariable.h"
#include "SimpleAES.h"
#include "TableEditActions.h"

PasswordManager::PasswordManager(QWidget* parent)
  : QMainWindow{parent} {
  setObjectName("PasswordManager");

  mAccountListView = new AccountListView{this};
  if (!mAccountListView->IsLoadSucceed()) {
    QMessageBox::critical(this, "Load failed", "Key error");
    this->setEnabled(false);
  }
  setCentralWidget(mAccountListView);
  // QSignalBlocker blocker(mAccountListView->selectionModel());

  mAccountDetailView = new AccountDetailView{"AccountDetailView", this};
  mAccountDetailView->setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea | Qt::DockWidgetArea::RightDockWidgetArea);
  addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, mAccountDetailView);

  const auto& tblEditInst = GetTableEditActionsInst();
  mSearchText = new QLineEdit{""};
  mSearchText->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
  mSearchText->setPlaceholderText("Input to search accounts by keywords here");
  mSearchText->setClearButtonEnabled(true);
  mSearchText->addAction(tblEditInst.SEARCH_BY, QLineEdit::ActionPosition::LeadingPosition);

  QToolBar* insertRowsTB = new QToolBar{"Insert Rows", this};
  insertRowsTB->setOrientation(Qt::Orientation::Vertical);
  insertRowsTB->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
  insertRowsTB->addAction(tblEditInst.INSERT_A_ROW);
  insertRowsTB->addAction(tblEditInst.INSERT_ROWS);

  QToolBar* exportTB = new QToolBar{"Export Contents", this};
  exportTB->setOrientation(Qt::Orientation::Vertical);
  exportTB->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
  exportTB->addAction(tblEditInst.SHOW_PLAIN_CSV_CONTENT);
  exportTB->addAction(tblEditInst.EXPORT_TO_PLAIN_CSV);

  mToolBar = new QToolBar{"EditToolbar", this};
  mToolBar->setObjectName(mToolBar->windowTitle());
  mToolBar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
  mToolBar->addWidget(insertRowsTB);
  mToolBar->addAction(tblEditInst.APPEND_ROWS);
  mToolBar->addAction(tblEditInst.DELETE_ROWS);
  mToolBar->addSeparator();
  mToolBar->addWidget(mSearchText);
  mToolBar->addSeparator();
  mToolBar->addAction(tblEditInst.LOAD_FROM_INPUT);
  mToolBar->addWidget(exportTB);
  mToolBar->addAction(tblEditInst.OPEN_DIRECTORY);
  mToolBar->addAction(tblEditInst.SAVE_CHANGES);
  addToolBar(Qt::ToolBarArea::TopToolBarArea, mToolBar);
  mToolBar->setFont(ViewStyleSheet::TEXT_EDIT_FONT);

  mStatusBar = new QStatusBar{this};
  setStatusBar(mStatusBar);

  Subscribe();

  ReadSettings();
  setWindowIcon(QIcon(":/PASSWORD_TABLE"));
  SetPWBookName();
}

PasswordManager::~PasswordManager() {}

void PasswordManager::closeEvent(QCloseEvent* event) {
  PreferenceSettings().setValue("PASSWORD_TABLEVIEW_GEOMETRY", saveGeometry());
  PreferenceSettings().setValue("PASSWORD_TABLEVIEW_STATE", saveState());
  PreferenceSettings().setValue("ACCOUNT_DETAIL_VIEW_GEOMETRY", mAccountDetailView->saveGeometry());
  QMainWindow::closeEvent(event);
}

void PasswordManager::ReadSettings() {
  if (PreferenceSettings().contains("PASSWORD_TABLEVIEW_GEOMETRY")) {
    restoreGeometry(PreferenceSettings().value("PASSWORD_TABLEVIEW_GEOMETRY").toByteArray());
    restoreState(PreferenceSettings().value("PASSWORD_TABLEVIEW_STATE").toByteArray());
    mAccountDetailView->restoreGeometry(PreferenceSettings().value("ACCOUNT_DETAIL_VIEW_GEOMETRY").toByteArray());
  } else {
    setGeometry(QRect(0, 0, 1024, 768));
  }
}

void PasswordManager::Subscribe() {
  connect(mAccountListView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &PasswordManager::onUpdateDetailView);
  connect(mSearchText, &QLineEdit::returnPressed, this, [this]() { mAccountListView->SetFilter(mSearchText->text()); });

  auto& ins = GetTableEditActionsInst();
  connect(ins.SHOW_PLAIN_CSV_CONTENT, &QAction::triggered, this, &PasswordManager::ShowPlainCSVContents);
  connect(ins.EXPORT_TO_PLAIN_CSV, &QAction::triggered, mAccountListView, &AccountListView::ExportPlainCSV);
  connect(ins.LOAD_FROM_INPUT, &QAction::triggered, this, &PasswordManager::onGetRecordsFromInput);
  connect(ins.OPEN_DIRECTORY, &QAction::triggered, this, [this]() {
    const QFileInfo fi{"./"};
    const QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
    bool bRet = QDesktopServices::openUrl(url);
    Notificator::information("Open local path", fi.absoluteFilePath() + "\nbResult:" + QString::number(bRet));
  });
  connect(ins.SAVE_CHANGES, &QAction::triggered, this, &PasswordManager::onSave);
}

void PasswordManager::SetPWBookName() {
  QString title;
  title.reserve(30);
  title += "Password Manager";
  title += " | ";
  if (SimpleAES::getFromEncrypt()) {
    title += AccountStorage::ENC_CSV_FILE;
  } else {
    title += AccountStorage::EXPORTED_PLAIN_CSV_FILE;
  }
  setWindowTitle(title);
}

void PasswordManager::onUpdateDetailView(const QModelIndex& proxyIndex) {
  auto* pData = mAccountListView->GetAccountInfoByCurrentIndex(proxyIndex);
  mAccountDetailView->UpdateDisplay(pData);
}

void PasswordManager::onSave() {
  QString changedMessage;
  SAVE_RESULT saveResult = mAccountListView->mPwdModel->onSave(&changedMessage);

  QString message;
  message.reserve(30);
  message += SAVE_RESULT_STR[(int) saveResult];
  message += ' ';
  message += "Try save record(s) at ";
  message += QDateTime::currentDateTime().toString();
  switch (saveResult) {
    case SAVE_RESULT::FAILED:
      Notificator::badNews("Failed to save", message);
      break;
    case SAVE_RESULT::SKIP:
      Notificator::warning("Nothing changed(Skip save)", message);
      break;
    default:
      Notificator::goodNews("Save successfully", changedMessage);
      break;
  }
  mStatusBar->showMessage(message);
}

void PasswordManager::onGetRecordsFromInput() {
  if (mTextInputDialog == nullptr) {
    mTextInputDialog = new CSVInputDialog{this};
    connect(mTextInputDialog, &CSVInputDialog::accepted, this, &PasswordManager::onLoadRecordsFromCSVInput);
  }
  mTextInputDialog->raise();
  mTextInputDialog->show();
}

void PasswordManager::onLoadRecordsFromCSVInput() {
  if (mTextInputDialog->tempAccounts.isEmpty()) {
    Notificator::information("Skip", "No record in CSV plain text input");
    return;
  }
  mAccountListView->mPwdModel->AppendAccountRecords(mTextInputDialog->tempAccounts);
}

void PasswordManager::ShowPlainCSVContents() {
  if (mPlainCSVContentWid == nullptr) {
    mPlainCSVContentWid = new QTextEdit;
    mPlainCSVContentWid->setAttribute(Qt::WA_DeleteOnClose);
    mPlainCSVContentWid->setWindowFlags(Qt::Window);
    mPlainCSVContentWid->setWindowTitle("Plain CSV Contents Here");
    mPlainCSVContentWid->setWindowIcon(QIcon{":/edit/SHOW_CSV_CONTENTS"});
    mPlainCSVContentWid->setReadOnly(true);
    mPlainCSVContentWid->setMinimumSize(600, 400);
  }
  mPlainCSVContentWid->setPlainText(mAccountListView->mPwdModel->GetExportCSVRecords());
  mPlainCSVContentWid->raise();
  mPlainCSVContentWid->show();
}
