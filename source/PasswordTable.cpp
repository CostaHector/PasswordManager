#include "include/PasswordTable.h"
#include "include/TableEditActions.h"
#include <set>
#include <QHeaderView>
#include <QInputDialog>

PasswordTable::PasswordTable(QWidget* parent)
    : CustomTableView{"PASSWORD_TABLEVIEW", parent},
      mPwdModel{new PwdTableModel{this}},                    //
      mSortProxyModel{new QSortFilterProxyModel{this}} {  // for sort
  mSortProxyModel->setSourceModel(mPwdModel);
  setModel(mSortProxyModel);
  setEditTriggers(QAbstractItemView::EditKeyPressed);
  InitTableView();
  subscribe();
}

void PasswordTable::subscribe() {
  auto& ins = GetTableEditActionsInst();
  connect(ins.INSERT_A_ROW, &QAction::triggered, this, [this]() { InsertNRows(); });
  connect(ins.INSERT_ROWS, &QAction::triggered, this, [this]() {
    bool isOk{false};
    int userInputCount = QInputDialog::getInt(this, "Input Insert Rows Count", "count", 1, 0, 255, 1, &isOk);
    if (!isOk) {
      qWarning("User input invalid");
      return;
    }
    InsertNRows(userInputCount);
  });
  connect(ins.APPEND_ROWS, &QAction::triggered, this, [this]() {
    bool isOk{false};
    int userInputCount = QInputDialog::getInt(this, "Input Append Rows Count", "count", 1, 0, 255, 1, &isOk);
    if (!isOk) {
      qWarning("User input invalid");
      return;
    }
    AppendNRows(userInputCount);
  });
  connect(ins.DELETE_ROWS, &QAction::triggered, this, &PasswordTable::RemoveSelectedRows);
}

void PasswordTable::RemoveSelectedRows() {
  if (!selectionModel()->hasSelection()) {
    qDebug("nothing were selected. skip remove.");
    return;
  }
  std::set<int> selectedRows;
  for (const auto& rowIndex : selectionModel()->selectedRows()) {
    const auto& srcIndex = mSortProxyModel->mapToSource(rowIndex);
    selectedRows.insert(srcIndex.row());
  }
  mPwdModel->RemoveIndexes(selectedRows);
}

void PasswordTable::InsertNRows(int rowCnt) {
  const QModelIndex verHeaderIndex = verticalHeader()->currentIndex();
  if (!verHeaderIndex.isValid()) {
    qDebug("no row selected");
    return;
  }
  int insertIndexBefore = verHeaderIndex.row();
  qDebug("insert at index %d with %d row", insertIndexBefore, rowCnt);
  mPwdModel->InsertNRows(insertIndexBefore, rowCnt);
}

void PasswordTable::AppendNRows(int rowCnt) {
  int lastRowIndex = verticalHeader()->count();
  qDebug("insert at index %d with %d row", lastRowIndex, rowCnt);
  mPwdModel->InsertNRows(lastRowIndex, rowCnt);
}

bool PasswordTable::SetKey16(const QString& key16) {
  return mPwdModel->SetKey16(key16);
}

bool PasswordTable::SetEncryptKey16(const QString& newKey16) {
  return mPwdModel->SetEncryptKey16(newKey16);
}
