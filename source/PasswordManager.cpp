#include "PasswordManager.h"
#include <QMessageBox>
#include "PublicVariable.h"
#include "TableEditActions.h"

typedef void (*SET_WINDOW_TITLE)(QWidget* pWid, const QString& pwdBookName);

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
  mAccountDetailView->setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea
                                      | Qt::DockWidgetArea::RightDockWidgetArea);
  addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, mAccountDetailView);

  mToolBar = new QToolBar{"EditToolbar", this};
  mToolBar->setObjectName(mToolBar->windowTitle());
  mToolBar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
  const auto& tblEditInst = GetTableEditActionsInst();
  mToolBar->addActions(tblEditInst.ROW_EDIT_AG->actions());
  mToolBar->addSeparator();
  mToolBar->addAction(tblEditInst.SAVE_CHANGES);
  addToolBar(Qt::ToolBarArea::TopToolBarArea, mToolBar);

  mStatusBar = new QStatusBar{this};
  setStatusBar(mStatusBar);

  Subscribe();

  ReadSettings();
  setWindowIcon(QIcon(":/PASSWORD_TABLE"));
  SetPWBookName(AccountStorage::PLAIN_CSV_FILE);
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
    mAccountDetailView->restoreGeometry(
        PreferenceSettings().value("ACCOUNT_DETAIL_VIEW_GEOMETRY").toByteArray());
  } else {
    setGeometry(QRect(0, 0, 1024, 768));
  }
}

void PasswordManager::Subscribe() {
  connect(mAccountListView->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
          [this](const QModelIndex& proxyIndex) {
            auto* pData = mAccountListView->GetAccountInfoByCurrentIndex(proxyIndex);
            mAccountDetailView->UpdateDisplay(pData);
          });
  mAccountDetailView->BindSetTableDirtyCallback([this]() { mAccountListView->SetTableDirty(); });
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
