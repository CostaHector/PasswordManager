#include "AccountStorage.h"
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include "SimpleAES.h"

const QStringList AccountInfo::HORIZONTAL_HEAD{"Index", "Type", "Name", "Account"};

QString AccountInfo::toCsvLine() const {
  // in others fields there are "comma, new line"
  QString escapedOthers = othersStr;
  escapedOthers.replace("\\", "\\\\").replace("\n", "\\n").replace("\r", "\\r").replace(",", "\\,");
  return typeStr + ',' + nameStr + ',' + accountStr + ',' + pwdStr + ',' + escapedOthers;
}

bool AccountInfo::FromCsvLine(const QString& csvLine, AccountInfo& acc) {
  static const QRegularExpression COMMA_SPLITTER{"(?<!\\\\),"};
  QStringList parts = csvLine.split(COMMA_SPLITTER);
  if (parts.size() != 5) {
    return false;
  }
  const QString& othersStr = parts[4].replace("\\,", ",").replace("\\r", "\r").replace("\\n", "\n").replace("\\\\", "\\");
  acc = AccountInfo{parts[0], parts[1], parts[2], parts[3], othersStr};
  return true;
}

const QString AccountStorage::ENC_CSV_FILE {"accounts.csv"};
const QString AccountStorage::PLAIN_CSV_FILE {"plainAccounts.csv"};

// one can save to file to plain text or encrypted by their willing
bool AccountStorage::SaveAccounts(bool bEncrypt) const {
  QString fullPlainCSVContents;
  fullPlainCSVContents.reserve(2048);
  for (const AccountInfo& acc : mAccounts) {
    fullPlainCSVContents += acc.toCsvLine();
    fullPlainCSVContents += '\n';
  }
  QString contentNeedDumped;

  QFile csvFile;
  if (bEncrypt) {
    csvFile.setFileName(ENC_CSV_FILE);
    bool encryptedResult = SimpleAES::encrypt_GCM(fullPlainCSVContents, contentNeedDumped);
    if (!encryptedResult) {
      qCritical("Encrypt file[%s] failed! Skip write into file!", qPrintable(csvFile.fileName()));
      return false;
    }
  } else {
    csvFile.setFileName(PLAIN_CSV_FILE);
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
  qDebug("%d byte(s) of %d record(s) dumped into %s succced", fullPlainCSVContents.size(), size(), qPrintable(csvFile.fileName()));
  return true;
}

// when start on, data is from plain or encrypted is determined
bool AccountStorage::LoadAccounts() {
  const bool bEncrypt = SimpleAES::getFromEncrypt();
  QFile csvFile;
  if (bEncrypt) {
    csvFile.setFileName(ENC_CSV_FILE);
  } else {
    csvFile.setFileName(PLAIN_CSV_FILE);
  }
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
  if (bEncrypt) {
    bool decryptResult = SimpleAES::decrypt_GCM(contents, plainContents);
    if (!decryptResult) {
      qCritical("Decrypt file[%s] failed! Skip load", qPrintable(csvFile.fileName()));
      return false;
    }
  } else {
    plainContents.swap(contents);
  }
  decltype(mAccounts) tempAccounts;
  AccountInfo acc;
  for (const QString& line: plainContents.split("\n")) {
    if (!AccountInfo::FromCsvLine(line, acc)) {
      continue;
    }
    tempAccounts.push_back(acc);
  }
  mAccounts.swap(tempAccounts);
  qDebug("%d item(s) of account was loaded", mAccounts.size());
  return true;
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
