#ifndef ACCOUNTLISTVIEW_H
#define ACCOUNTLISTVIEW_H

#include <QSortFilterProxyModel>
#include "CustomTableView.h"
#include "PwdTableModel.h"

class AccountListView : public CustomTableView {
public:
  friend class PasswordManager;
  explicit AccountListView(QWidget* parent = nullptr);
  void Subscribe();
  void RemoveSelectedRows();
  void InsertNRows(int rowCnt = 1);
  void AppendNRows(int rowCnt);
  QModelIndex GetSourceIndex(const QModelIndex& proxyIndex) const;
  AccountInfo* GetAccountInfoByCurrentIndex(const QModelIndex& proxyIndex);
  bool IsLoadSucceed() const {
    return mPwdModel->GetLoadResult();
  }
  void SetFilter(const QString& keywords) {
    if (mSortProxyModel == nullptr) {
      return;
    }
    mSortProxyModel->setFilterFixedString(keywords);
  }

private:
  bool ExportPlainCSV();
  PwdTableModel* mPwdModel{nullptr};
  AccountSortFilterProxyModel* mSortProxyModel{nullptr};
};

#endif // ACCOUNTLISTVIEW_H
