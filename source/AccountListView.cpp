#include "AccountListView.h"
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include "PublicVariable.h"
#include "TableEditActions.h"
#include <qdatetime.h>
#include <qpushbutton.h>
#include <set>

AccountListView::AccountListView(QWidget* parent)
  : CustomTableView{"PASSWORD_TABLEVIEW", parent} //
{
  mPwdModel = new PwdTableModel{this};

  mSortProxyModel = new AccountSortFilterProxyModel{this};
  mSortProxyModel->setSourceModel(mPwdModel);
  mSortProxyModel->BindAccountsList(mPwdModel->mAccountsList);

  setModel(mSortProxyModel);
  setEditTriggers(QAbstractItemView::EditKeyPressed);
  InitTableView();
  setFont(ViewStyleSheet::TEXT_EDIT_FONT);
  Subscribe();
}

void AccountListView::Subscribe() {
  static const auto GetRowsCountFromUserInput = [this](const QString& method) -> int {
    bool isOk{false};
    int userInputCount
        = QInputDialog::getInt(this, method + " Rows Count", "number", 1, 0, 255, 1, &isOk);
    if (!isOk) {
      qWarning("User input row count[%d] invalid", userInputCount);
      return 0;
    }
    return userInputCount;
  };

  auto& ins = GetTableEditActionsInst();
  connect(ins.INSERT_A_ROW, &QAction::triggered, this, [this]() {
    int cnt = 1;
    InsertNRows(cnt);
  });
  connect(ins.INSERT_ROWS, &QAction::triggered, this, [this]() {
    int cnt = GetRowsCountFromUserInput("INSERT");
    if (cnt > 0) {
      InsertNRows(cnt);
    }
  });
  connect(ins.APPEND_ROWS, &QAction::triggered, this, [this]() {
    int cnt = GetRowsCountFromUserInput("APPEND");
    if (cnt > 0) {
      AppendNRows(cnt);
    }
  });
  connect(ins.DELETE_ROWS, &QAction::triggered, this, &AccountListView::RemoveSelectedRows);
}

void AccountListView::RemoveSelectedRows() {
  if (!selectionModel()->hasSelection()) {
    qDebug("Nothing were selected. skip remove.");
    return;
  }
  const int nSelectedRowsCnt = selectionModel()->selectedRows().size();
  QString msg{QString{"Are you sure to remove the %1 row(s) selected(NOT RECOVERABLE)"}.arg(nSelectedRowsCnt)};
  QMessageBox deleteConfirm;
  deleteConfirm.setWindowTitle("Delete Confirm");
  deleteConfirm.setWindowIcon(QIcon(":/edit/DELETE_ROWS"));
  deleteConfirm.setIcon(QMessageBox::Icon::Warning);
  deleteConfirm.setText(msg);
  deleteConfirm.addButton(QMessageBox::StandardButton::Ok);
  deleteConfirm.addButton(QMessageBox::StandardButton::Cancel);
  if (deleteConfirm.exec() != QMessageBox::StandardButton::Ok) {
    qDebug("User cancel delete. skip remove.");
    return;
  }
  std::set<int> selectedRows;
  for (const auto& rowIndex : selectionModel()->selectedRows()) {
    const auto& srcIndex = mSortProxyModel->mapToSource(rowIndex);
    selectedRows.insert(srcIndex.row());
  }
  const int rowsDeleted = mPwdModel->RemoveIndexes(selectedRows);
  if (rowsDeleted != nSelectedRowsCnt) {
    qWarning("%d/%d row(s) were deleted", rowsDeleted, nSelectedRowsCnt);
    return;
  }
  qWarning("All %d row(s) selected were deleted successfully", nSelectedRowsCnt);
}

void AccountListView::InsertNRows(int rowCnt) {
  const QModelIndex verHeaderIndex = verticalHeader()->currentIndex();
  if (!verHeaderIndex.isValid()) {
    qDebug("no row selected");
    return;
  }
  int insertIndexBefore = verHeaderIndex.row();
  qDebug("insert at index %d with %d row", insertIndexBefore, rowCnt);
  mPwdModel->InsertNRows(insertIndexBefore, rowCnt);
}

void AccountListView::AppendNRows(int rowCnt) {
  int lastRowIndex = verticalHeader()->count();
  qDebug("insert at index %d with %d row", lastRowIndex, rowCnt);
  mPwdModel->InsertNRows(lastRowIndex, rowCnt);
}

QModelIndex AccountListView::GetSourceIndex(const QModelIndex& proxyIndex) const {
  if (!proxyIndex.isValid()) {
    return {};
  }
  return mSortProxyModel->mapToSource(proxyIndex);
}

AccountInfo* AccountListView::GetAccountInfoByCurrentIndex(const QModelIndex& proxyIndex) {
  QModelIndex srcCurrentIndex = GetSourceIndex(proxyIndex);
  if (!srcCurrentIndex.isValid()) {
    return nullptr;
  }
  return mPwdModel->rowDataAt(srcCurrentIndex.row());
}

bool AccountListView::ExportPlainCSV() {
  QMessageBox confirmDialog(this);
  confirmDialog.setWindowTitle("Export Plaintext List?");
  confirmDialog.setWindowIcon(QIcon(":/edit/EXPORT"));
  confirmDialog.setText(
      "WARNING: This may cause <b>information leakage</b>. Please keep the exported file secure.");
  confirmDialog.setIcon(QMessageBox::Warning);

  QPushButton* exportButton = confirmDialog.addButton("Continue Export", QMessageBox::AcceptRole);
  confirmDialog.addButton(QMessageBox::StandardButton::Cancel);
  exportButton->setIcon(QIcon(":/edit/EXPORT"));
  confirmDialog.setDefaultButton(QMessageBox::StandardButton::Cancel);
  confirmDialog.exec();

  if (confirmDialog.clickedButton() != exportButton) {
    qDebug("User canceled plaintext export");
    return true;
  }
  bool exportResult = mPwdModel->ExportToPlainCSV();
  if (!exportResult) {
    qInfo("Failed to export plaintext CSV at %s",
          qPrintable(QDateTime::currentDateTime().toString()));
    QMessageBox failedMsgBox(this);
    failedMsgBox.setWindowTitle("Export failed!");
    failedMsgBox.setWindowIcon(QIcon(":/edit/EXPORT"));
    failedMsgBox.setText("Please check disk permissions or space.");
    failedMsgBox.setIcon(QMessageBox::Icon::Critical);
    failedMsgBox.setStandardButtons(QMessageBox::Ok);
    failedMsgBox.exec();
    return false;
  }
  qInfo("Plaintext CSV exported at %s successfully",
        qPrintable(QDateTime::currentDateTime().toString()));
  QMessageBox succeedMsgBox(this);
  succeedMsgBox.setWindowTitle("Export succeeded");
  succeedMsgBox.setWindowIcon(QIcon(":/edit/EXPORT"));
  succeedMsgBox.setText("Please <b>store the file securely</b>.");
  succeedMsgBox.setIconPixmap(QPixmap(":/imgs/SAVED"));
  succeedMsgBox.setStandardButtons(QMessageBox::Ok);
  succeedMsgBox.exec();
  return true;
}
