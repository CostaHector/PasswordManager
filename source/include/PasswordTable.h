#ifndef PASSWORDTABLE_H
#define PASSWORDTABLE_H

#include "CustomTableView.h"
#include "PwdTableModel.h"
#include <QSortFilterProxyModel>

class PasswordTable: public CustomTableView
{
 public:
  explicit PasswordTable(QWidget* parent = nullptr);
  void subscribe();
  void RemoveSelectedRows();
  void InsertNRows(int rowCnt = 1);
  void AppendNRows(int rowCnt);
  bool SetKey16(const QString& key16);
  bool SetEncryptKey16(const QString& newKey16);
  bool IsDirty() const {
    return mPwdModel != nullptr && mPwdModel->IsDirty();
  }
  bool IsDecryptKeyCorrect() const {
    return mPwdModel->IsDecryptKeyCorrect();
  }
  bool IsEncryptKeyCorrect() const {
    return mPwdModel->IsEncryptKeyCorrect();
  }
 private:
  PwdTableModel* mPwdModel;
  QSortFilterProxyModel* mSortProxyModel;
};

#endif // PASSWORDTABLE_H
