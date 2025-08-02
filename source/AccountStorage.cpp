#include "AccountStorage.h"
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include "SimpleAES.h"

const QString& AccountStorage::GetFullEncCsvFilePath() {
  static const QString homePath = QDir::homePath();
  if (!QDir{homePath}.exists(PROJECT_NAME)) {
    QDir{homePath}.mkpath(PROJECT_NAME);
  }
  static constexpr char ENC_CSV_FILE[] {"accounts.csv"};
  static const QString absEncFilePath = QDir::toNativeSeparators(homePath + '/' + PROJECT_NAME + '/' + ENC_CSV_FILE);
  return absEncFilePath;
}

const QString& AccountStorage::GetFullPlainCsvFilePath() {
  static const QString homePath = QDir::homePath();
  if (!QDir{homePath}.exists(PROJECT_NAME)) {
    QDir{homePath}.mkpath(PROJECT_NAME);
  }
  static constexpr char EXPORTED_PLAIN_CSV_FILE[] {"exportedPlainAccounts.csv"};
  static const QString absPlainFilePath = QDir::toNativeSeparators(homePath + '/' + PROJECT_NAME + '/' + EXPORTED_PLAIN_CSV_FILE);;
  return absPlainFilePath;
}

const bool AccountStorage::IsAccountCSVFileInExistOrEmpty() {
  QFile csvFile{GetFullEncCsvFilePath()};
  return !csvFile.exists() || csvFile.size() == 0;
}

QString AccountStorage::GetExportCSVRecords() const{
  QString fullPlainCSVContents;
  fullPlainCSVContents.reserve(2048);
  for (const AccountInfo& acc : mAccounts) {
    fullPlainCSVContents += acc.toCsvLine();
    fullPlainCSVContents += '\n';
  }
  return fullPlainCSVContents;
}


// one can save to file to plain text or encrypted by their willing
bool AccountStorage::SaveAccounts(bool bEncrypt) const {
  QString fullPlainCSVContents = GetExportCSVRecords();
  QString contentNeedDumped;

  QFile csvFile;
  if (bEncrypt) {
    csvFile.setFileName(GetFullEncCsvFilePath());
    bool encryptedResult = SimpleAES::encrypt_GCM(fullPlainCSVContents, contentNeedDumped);
    if (!encryptedResult) {
      qCritical("Encrypt file[%s] failed! Skip write into file!", qPrintable(csvFile.fileName()));
      return false;
    }
  } else {
    csvFile.setFileName(GetFullPlainCsvFilePath());
    contentNeedDumped.swap(fullPlainCSVContents);
  }
  if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    csvFile.close();
    qCritical("Open file[%s] to write failed", qPrintable(csvFile.fileName()));
    return false;
  }
  QTextStream out(&csvFile);
  out.setCodec("UTF-8");
  out << contentNeedDumped;
  csvFile.close();
  AfterSave();
  qDebug("%d byte(s) of %d record(s) dumped into %s succced", fullPlainCSVContents.size(), size(), qPrintable(csvFile.fileName()));
  return true;
}

// when start on, data is from plain or encrypted is determined
bool AccountStorage::LoadAccounts() {
  const QString s1 = GetFullEncCsvFilePath();

  QFile csvFile{GetFullEncCsvFilePath()};
  if (!csvFile.exists()) {
    qWarning("File[%s] not exist. Create a new one when save them", qPrintable(csvFile.fileName()));
    return true;
  }
  if (!csvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning("Open file[%s] to read failed", qPrintable(csvFile.fileName()));
    return false;
  }

  QTextStream in(&csvFile);
  in.setCodec("UTF-8");
  QString contents = in.readAll();
  csvFile.close();

  QString plainContents;
  bool decryptResult = SimpleAES::decrypt_GCM(contents, plainContents);
  if (!decryptResult) {
    qCritical("Decrypt file[%s] failed! Skip load", qPrintable(csvFile.fileName()));
    return false;
  }
  int nonEmptyLine{0};
  decltype(mAccounts) tempAccounts = GetAccountsFromPlainString(plainContents, &nonEmptyLine);
  mAccounts.swap(tempAccounts);
  // SetListModified(); here mAccounts is same as file contents, we consider it has no modification
  qDebug("%d account record(s) was loaded from %d non empty lines", mAccounts.size(), nonEmptyLine);
  return true;
}

QVector<AccountInfo> AccountStorage::GetAccountsFromPlainString(const QString& contents, int* pNonEmptyLine) {
  decltype(mAccounts) tempAccounts;
  AccountInfo acc;
  int nonEmptyLine {0};
  for (const QString& line: contents.split('\n', Qt::SkipEmptyParts)) {
    if (line.isEmpty()) {
      continue;
    }
    ++nonEmptyLine;
    if (!AccountInfo::FromCsvLine(line, acc)) {
      continue;
    }
    tempAccounts.push_back(acc);
  }
  if (pNonEmptyLine != nullptr) {
    *pNonEmptyLine = nonEmptyLine;
  }
  return tempAccounts;
}

int AccountStorage::RemoveIndexes(const std::set<int>& rows) {
  // rows is ascending
  int beforeRowCnt = size();
  for (auto rit = rows.crbegin(); rit != rows.crend(); ++rit) {
    if (*rit < 0 || *rit >= beforeRowCnt) {
      continue;
    }
    mAccounts.removeAt(*rit);
  }
  qDebug("row size changed from %d->%d(given %lluu row indexes)", beforeRowCnt, size(), rows.size());
  SetListModified();
  return beforeRowCnt - size();
}

bool AccountStorage::InsertNRows(int indexBefore, int cnt) {
  if (cnt <= 0) {
    qDebug("no row need to insert into");
    return true;
  }
  if (indexBefore < 0) {
    indexBefore = 0;
  } else if (indexBefore > size()) {
    indexBefore = size();
  }
  mAccounts.insert(indexBefore, cnt, AccountInfo{});
  SetListModified();
  return true;
}

bool AccountStorage::AppendNRows(int cnt) {
  return InsertNRows(size(), cnt);
}

AccountInfo& AccountStorage::operator[](int i) {
  if (i < 0 || i >= size()) {
    static AccountInfo INVALID_ACC_FILE_ITEM;
    return INVALID_ACC_FILE_ITEM;
  }
  return mAccounts[i];
}

const AccountInfo& AccountStorage::operator[](int i) const {
  if (i < 0 || i >= size()) {
    static AccountInfo INVALID_ACC_FILE_ITEM;
    return INVALID_ACC_FILE_ITEM;
  }
  return mAccounts[i];
}
