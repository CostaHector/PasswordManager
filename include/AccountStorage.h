#ifndef ACCOUNTSTORAGE_H
#define ACCOUNTSTORAGE_H

#include <QStringList>
#include <QVariant>
#include <set>

struct AccountInfo {
  QString typeStr;
  QString nameStr;
  QString accountStr;
  QString pwdStr;
  QString othersStr;

  bool IsContainsKeyWords(const QString& keywords) const {
    return keywords.isEmpty()
           || typeStr.contains(keywords, Qt::CaseSensitivity::CaseInsensitive)
           || nameStr.contains(keywords, Qt::CaseSensitivity::CaseInsensitive)
           || accountStr.contains(keywords, Qt::CaseSensitivity::CaseInsensitive)
           || pwdStr.contains(keywords, Qt::CaseSensitivity::CaseInsensitive)
           || othersStr.contains(keywords, Qt::CaseSensitivity::CaseInsensitive);
  }

  QString toCsvLine() const;
  static bool FromCsvLine(const QString& csvLine, AccountInfo& acc);
  static const QStringList HORIZONTAL_HEAD;
};

struct AccountStorage {
  static const QString ENC_CSV_FILE;
  static const QString PLAIN_CSV_FILE;
  bool mAllowOperation;

  bool DeleteAccountCSVFile();
  bool SaveAccounts(bool bEncrypt = true) const;
  bool LoadAccounts();

  int RemoveIndexes(const std::set<int>& rows);
  bool InsertNRows(int indexBefore, int cnt);
  bool AppendNRows(int cnt);

  AccountInfo& operator[](int i);
  const AccountInfo& operator[](int i) const;

  inline int size() const { return mAccounts.size(); }
  inline bool empty() const { return mAccounts.empty(); }
  void swap(AccountStorage& rhs) { mAccounts.swap(rhs.mAccounts); }

  QVector<AccountInfo> mAccounts;
};
#endif // ACCOUNTSTORAGE_H
