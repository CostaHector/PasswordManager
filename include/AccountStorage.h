#ifndef ACCOUNTSTORAGE_H
#define ACCOUNTSTORAGE_H

#include <QStringList>
#include <QVariant>
#include <set>

struct AccountInfo {
public:
  AccountInfo(const QString& _typeStr,
              const QString& _nameStr,
              const QString& _accountStr,
              const QString& _pwdStr,
              const QString& _othersStr)
    : typeStr{_typeStr}
    , nameStr{_nameStr}
    , accountStr{_accountStr}
    , pwdStr{_pwdStr}
    , othersStr{_othersStr} {}
  AccountInfo() = default;

  bool IsContainsKeyWords(const QString& keywords) const {
    return keywords.isEmpty() || typeStr.contains(keywords, Qt::CaseSensitivity::CaseInsensitive)
           || nameStr.contains(keywords, Qt::CaseSensitivity::CaseInsensitive)
           || accountStr.contains(keywords, Qt::CaseSensitivity::CaseInsensitive)
           || pwdStr.contains(keywords, Qt::CaseSensitivity::CaseInsensitive)
           || othersStr.contains(keywords, Qt::CaseSensitivity::CaseInsensitive);
  }

  QString toCsvLine() const;
  static bool FromCsvLine(const QString& csvLine, AccountInfo& acc);
  static const QStringList HORIZONTAL_HEAD;

  QString typeStr;
  QString nameStr;
  QString accountStr;
  QString pwdStr;
  QString othersStr;

  void SetDetailModified() const { mIsDetailModified = true; }
  void ClearDetailModified() const { mIsDetailModified = false; }
  bool IsDetailModified() const { return mIsDetailModified; }

private:
  mutable bool mIsDetailModified{false};
};

struct AccountStorage {
public:
  static const QString ENC_CSV_FILE;
  static const QString PLAIN_CSV_FILE;

  // plain Utf8 -> list<Account>
  static QVector<AccountInfo> GetAccountsFromPlainString(const QString& contents,
                                                         int* pNonEmptyLine = nullptr);
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

  void SetListModified() const { mIsListModifed = true; }

private:
  mutable bool mIsListModifed{false};
  void ClearListModified() const { mIsListModifed = false; }

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
