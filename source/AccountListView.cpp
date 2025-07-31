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

  mSortProxyModel = new QSortFilterProxyModel{this};
  mSortProxyModel->setSourceModel(mPwdModel);

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
      qWarning("User input invalid");
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
  connect(ins.EXPORT_TO_PLAIN_CSV, &QAction::triggered, this, &AccountListView::ExportPlainCSV);
  connect(ins.SAVE_CHANGES, &QAction::triggered, mPwdModel, [this]() {
    bool saveResult = mPwdModel->onSave();
    if (!saveResult) {
      QMessageBox::information(this, "Save record(s) Failed", "see details in logs");
    }
  });
}

void AccountListView::RemoveSelectedRows() {
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

AccountInfo* AccountListView::GetAccountInfoByCurrentIndex(const QModelIndex& proxyIndex) {
  if (!proxyIndex.isValid()) {
    return nullptr;
  }
  QModelIndex srcCurrentIndex = mSortProxyModel->mapToSource(proxyIndex);
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
    // failedMsgBox.setIconPixmap(QPixmap(":/edit/FAILURE"));
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
  succeedMsgBox.setIconPixmap(QPixmap(":/edit/SUCCESS"));
  succeedMsgBox.setStandardButtons(QMessageBox::Ok);
  succeedMsgBox.exec();
  return true;
}
