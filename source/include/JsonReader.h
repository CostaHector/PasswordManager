#ifndef JSONREADER_H
#define JSONREADER_H

#include <QStringList>
#include <set>
#include <QVariant>

struct AccountInfo {
  QString Issuer;
  QString Type;
  QString Account;
  QString Withdraw;
  int CVV2 = 0;          // %03d
  QString ValidThrough;  // %02d/%02d, i.e, month/year
  QString CardHolder;
  int AnnualFee = 0;
  QString Notes;
  int RecordTime = 0;
  QString QueryCode;
};

const QStringList HORIZONTAL_HEAD{"Index", "Issuer", "Type", "Account", "Withdraw", "CVV2", "Valid through", "Card Holder", "Annual fee", "Notes", "Record Time", "QueryCode"};

class JsonReader {
 public:
  JsonReader();
  void Init();
  void swap(JsonReader& rhs);
  bool IsJsonFileEmpty() const;

  bool operator()(const QString& decryptKey);
  bool SetDecryptKey16(const QString& decryptKey);
  bool SetEncryptKey16(const QString& newEncryptKey);
  bool IsDecryptKeyCorrect() const { return mIsDecryptKeyCorrect; }
  bool IsEncryptKeyDiffersFromDecryptKey() const { return mDecryptKey != mEncryptKey; }

  AccountInfo& operator[](int i);
  const AccountInfo& operator[](int i) const;

  bool Save();
  int size() const { return mAccsList.size(); }
  bool empty() const { return mAccsList.empty(); }

  int RemoveIndexes(const std::set<int>& rows);
  bool InsertNRows(int indexBefore, int cnt);
  bool AppendNRows(int cnt);

  bool isEncryptKeyLengthValid() const { return IsKeyLengthValid(mEncryptKey); }
  bool isDecryptKeyLengthValid() const { return IsKeyLengthValid(mDecryptKey); }
  static const QString PDBOOK_TABLE_NAME;
  static bool IsKeyLengthValid(const QString& key) { return 0 < key.size() && key.size() <= 16; }

 private:
  QVector<AccountInfo> mAccsList;
  QString mDecryptKey;
  QString mEncryptKey;
  bool mIsDecryptKeyCorrect{false};
};
#endif  // JSONREADER_H
