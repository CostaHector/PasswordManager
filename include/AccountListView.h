#ifndef ACCOUNTLISTVIEW_H
#define ACCOUNTLISTVIEW_H

#include <QSortFilterProxyModel>
#include "CustomTableView.h"
#include "PwdTableModel.h"

class AccountListView : public CustomTableView {
public:
  explicit AccountListView(QWidget* parent = nullptr);
  void Subscribe();
  void RemoveSelectedRows();
  void InsertNRows(int rowCnt = 1);
  void AppendNRows(int rowCnt);
  AccountInfo* GetAccountInfoByCurrentIndex(const QModelIndex& proxyIndex);
  void SetTableDirty() {
    if (mPwdModel != nullptr) {
      mPwdModel->SetDirty();
    }
  }
  bool IsLoadSucceed() const {
    return mPwdModel->GetLoadResult();
  }
private:
  bool ExportPlainCSV();
  PwdTableModel* mPwdModel{nullptr};
  QSortFilterProxyModel* mSortProxyModel{nullptr};
};

#endif // ACCOUNTLISTVIEW_H
