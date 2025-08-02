#ifndef ACCOUNTSTORAGE_H
#define ACCOUNTSTORAGE_H

#include <QStringList>
#include <QVariant>
#include "AccountInfo.h"
#include <set>

struct AccountStorage {
public:
  static const QString& GetFullEncCsvFilePath();
  static const QString& GetFullPlainCsvFilePath();
  static const bool IsAccountCSVFileInExistOrEmpty();

  // plain Utf8 -> list<Account>
  static QVector<AccountInfo> GetAccountsFromPlainString(const QString& contents, int* pNonEmptyLine = nullptr);
  // encrypted base64 -> encrypted binary -> plain binary -> plain Utf8 -> list<Account>
  bool LoadAccounts();

  // list<Account> -> plain Utf8
  QString GetExportCSVRecords() const;
  // list<Account> -> plain Utf8 -> plain binary -> encrypted binary -> plain base64
  bool SaveAccounts(bool bEncrypt = true) const;

  int RemoveIndexes(const std::set<int>& rows);
  bool InsertNRows(int indexBefore, int cnt);
  bool AppendNRows(int cnt);

  AccountInfo& operator[](int i);
  AccountStorage& operator+=(const QVector<AccountInfo>& tempAccounts) {
    mAccounts += tempAccounts;
    SetListModified();
    return *this;
  }
  void swap(AccountStorage& rhs) {
    mAccounts.swap(rhs.mAccounts);
    SetListModified();
  }

  const AccountInfo& operator[](int i) const;
  inline int size() const { return mAccounts.size(); }
  inline bool empty() const { return mAccounts.empty(); }
  QVector<AccountInfo> mAccounts;

  bool IsDataDirty() const { return mIsListModifed || IsAnyDetailModified(); }

  void AfterSave() const {
    ClearListModified();
    ClearAllDetailModified();
  }

  void SetListModified(int n = 1) const {
    mIsListModifed = true;
    mRowsModifiedCount += n;
  }

  QString GetRowChangeDetailMessage() const {
    static const QString FORMAT_STRING = QStringLiteral("%1 row(s) changed and %2 detail(s) changed.");
    return FORMAT_STRING.arg(mRowsModifiedCount).arg(mDetailModifiedIndexes.size());
  }

private:
  mutable bool mIsListModifed{false};
  mutable int mRowsModifiedCount{0};
  void ClearListModified() const {
    mIsListModifed = false;
    mRowsModifiedCount = 0;
  }

  mutable std::set<int> mDetailModifiedIndexes;
  bool IsAnyDetailModified() const {
    if (!mIsListModifed) { // index still works, not invalid
      return !mDetailModifiedIndexes.empty();
    }
    int count = mAccounts.size();
    for (int index = 0; index < count; ++index) {
      if (mAccounts[index].IsDetailModified()) {
        mDetailModifiedIndexes.insert(index);
      }
    }
    return !mDetailModifiedIndexes.empty();
  }

  void ClearAllDetailModified() const {
    for (const int index : mDetailModifiedIndexes) {
      mAccounts[index].ClearDetailModified();
    }
    mDetailModifiedIndexes.clear();
  }
};
#endif // ACCOUNTSTORAGE_H
