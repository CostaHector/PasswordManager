#include "include/MyEncrypt.h"
#include "include/JsonReader.h"
#include "include/PublicVariable.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QVariantHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>

namespace FUNCS {
namespace TABLE_DATA_FUNCS {
QVariant GetIndex(const QVector<AccountInfo>& v, int i) {
  return i;
}
QVariant GetIssuer(const QVector<AccountInfo>& v, int i) {
  return v[i].Issuer;
}
QVariant GetAccount(const QVector<AccountInfo>& v, int i) {
  return v[i].Account;
}
QVariant GetWithdraw(const QVector<AccountInfo>& v, int i) {
  return v[i].Withdraw;
}
QVariant GetCVV2(const QVector<AccountInfo>& v, int i) {
  return v[i].CVV2;
}
QVariant GetValidThrough(const QVector<AccountInfo>& v, int i) {
  return v[i].ValidThrough;
}
QVariant GetCardHolder(const QVector<AccountInfo>& v, int i) {
  return v[i].CardHolder;
}
QVariant GetAnnualFee(const QVector<AccountInfo>& v, int i) {
  return v[i].AnnualFee;
}
QVariant GetNotes(const QVector<AccountInfo>& v, int i) {
  return v[i].Notes;
}
QVariant GetRecordTime(const QVector<AccountInfo>& v, int i) {
  return v[i].RecordTime;
}
QVariant GetQueryCode(const QVector<AccountInfo>& v, int i) {
  return v[i].QueryCode;
}
}  // namespace TABLE_DATA_FUNCS
namespace TABLE_SET_DATA_FUNCS {
void SetIndex(QVector<AccountInfo>& v, int i, const QVariant& val) {}
void SetIssuer(QVector<AccountInfo>& v, int i, const QVariant& val) {
  v[i].Issuer = val.toString();
}
void SetAccount(QVector<AccountInfo>& v, int i, const QVariant& val) {
  v[i].Account = val.toString();
}
void SetWithdraw(QVector<AccountInfo>& v, int i, const QVariant& val) {
  v[i].Withdraw = val.toString();
}
void SetCVV2(QVector<AccountInfo>& v, int i, const QVariant& val) {
  v[i].CVV2 = val.toInt();
}
void SetValidThrough(QVector<AccountInfo>& v, int i, const QVariant& val) {
  v[i].ValidThrough = val.toString();
}
void SetCardHolder(QVector<AccountInfo>& v, int i, const QVariant& val) {
  v[i].CardHolder = val.toString();
}
void SetAnnualFee(QVector<AccountInfo>& v, int i, const QVariant& val) {
  v[i].AnnualFee = val.toInt();
}
void SetNotes(QVector<AccountInfo>& v, int i, const QVariant& val) {
  v[i].Notes = val.toString();
}
void SetRecordTime(QVector<AccountInfo>& v, int i, const QVariant& val) {
  v[i].RecordTime = val.toInt();
}
void SetQueryCode(QVector<AccountInfo>& v, int i, const QVariant& val) {
  v[i].QueryCode = val.toString();
}
}  // namespace TABLE_SET_DATA_FUNCS
}  // namespace FUNCS

// ---------------- JsonReader ----------------
const QString JsonReader::PDBOOK_TABLE_NAME = "MyPDBook.json";

JsonReader::JsonReader() {}

void JsonReader::Init() {
  mAccsList.clear();
  mDecryptKey.clear();
  mEncryptKey.clear();
  mIsDecryptKeyCorrect = false;
}

void JsonReader::swap(JsonReader& rhs) {
  mAccsList.swap(rhs.mAccsList);
  mDecryptKey.swap(rhs.mDecryptKey);
  mEncryptKey.swap(rhs.mEncryptKey);
  std::swap(mIsDecryptKeyCorrect, rhs.mIsDecryptKeyCorrect);
}

bool JsonReader::IsJsonFileEmpty() const {
  return QFile{PDBOOK_TABLE_NAME}.size() == 0;
}

int JsonReader::RemoveIndexes(const std::set<int>& rows) {
  // rows is ascending
  int beforeRowCnt = size();
  for (auto rit = rows.crbegin(); rit != rows.crend(); ++rit) {
    if (*rit < 0 || *rit >= beforeRowCnt) {
      continue;
    }
    mAccsList.removeAt(*rit);
  }
  qDebug("row size changed from %d->%d(given %d row indexes)", beforeRowCnt, size(), rows.size());
  return beforeRowCnt - size();
}

bool JsonReader::InsertNRows(int indexBefore, int cnt) {
  if (cnt < 0) {
    qDebug("no row need to insert into");
    return true;
  }
  if (indexBefore < 0) {
    indexBefore = 0;
  } else if (indexBefore > size()) {
    indexBefore = size();
  }
  AccountInfo accInfo;
  accInfo.RecordTime = QDateTime::currentDateTime().toSecsSinceEpoch();
  mAccsList.insert(indexBefore, cnt, accInfo);
  return true;
}

bool JsonReader::AppendNRows(int cnt) {
  if (cnt < 0) {
    qDebug("no row need to append");
    return true;
  }
  mAccsList.append(decltype(mAccsList){cnt, AccountInfo{}});
  return true;
}

AccountInfo& JsonReader::operator[](int i) {
  if (i < 0 || i >= size()) {
    static AccountInfo INVALID_ACC_FILE_ITEM;
    return INVALID_ACC_FILE_ITEM;
  }
  return mAccsList[i];
}

const AccountInfo& JsonReader::operator[](int i) const {
  if (i < 0 || i >= size()) {
    static AccountInfo INVALID_ACC_FILE_ITEM;
    return INVALID_ACC_FILE_ITEM;
  }
  return mAccsList[i];
}

bool JsonReader::operator()(const QString& decryptKey) {
  qDebug("parse file[%s] start...", qPrintable(PDBOOK_TABLE_NAME));

  SetDecryptKey16(decryptKey);
  mAccsList.clear();

  if (PreferenceSettings().contains("DECRYPT_KEY_HASH")) {
    QCryptographicHash md5hash{QCryptographicHash::Algorithm::Md5};
    md5hash.addData(decryptKey.toUtf8());
    mIsDecryptKeyCorrect = md5hash.result() == PreferenceSettings().value("DECRYPT_KEY_HASH").toByteArray();
    qDebug("Decrypt key[size:%d] user provide is %s", decryptKey.size(), mIsDecryptKeyCorrect ? "correct" : "INCORRECT");
  } else {
    mIsDecryptKeyCorrect = true;
    qDebug("Preference setting .ini file is deleted");
  }

  if (!mIsDecryptKeyCorrect) {
    return false;
  }

  if (PDBOOK_TABLE_NAME.isEmpty()) {
    qWarning("file path[%s] is empty", qPrintable(PDBOOK_TABLE_NAME));
    return false;
  }
  QFile fi{PDBOOK_TABLE_NAME};
  if (!fi.exists()) {
    qWarning("file[%s] not exist", qPrintable(PDBOOK_TABLE_NAME));
    return false;
  }
  if (!fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning("Open file[%s] to read failed", qPrintable(PDBOOK_TABLE_NAME));
    return false;
  }

  QTextStream in(&fi);
  in.setCodec("UTF-8");
  QString jsonStr = in.readAll();
  fi.close();

  QJsonParseError jsonErr;
  QJsonDocument json_doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &jsonErr);
  if (jsonErr.error != QJsonParseError::NoError) {
    qWarning("Parse json from %d char(s) failed, error information: %s.", jsonStr.size(), qPrintable(jsonErr.errorString()));
    return false;
  }

  MyEncrypt encrypt{mDecryptKey};

  const QJsonArray& arrs = json_doc.array();
  for (const auto& ele : arrs) {
    const QJsonObject& accEleVar = ele.toObject();
    const auto& accJson = accEleVar.toVariantHash();
    QString accDecryptedStr = encrypt.DecryptStringFromBase64Str(accJson["Account"].toString());
    QString withdrawDecryptedStr = encrypt.DecryptStringFromBase64Str(accJson["Withdraw"].toString());
    int cvv2DecryptedInt = encrypt.DecryptIntFromBase64Str(accJson["CVV2"].toString());
    QString queryCodeDecryptedStr = encrypt.DecryptStringFromBase64Str(accJson.value("QueryCode", "").toString());

    mAccsList << AccountInfo{
        accJson["Issuer"].toString(),       /* string */
        accJson["Type"].toString(),         /* string */
        accDecryptedStr,                    /* base64 string->QByteArray->string */
        withdrawDecryptedStr,               /* base64 string->QByteArray->string */
        cvv2DecryptedInt,                   /* QByteArray->int number */
        accJson["ValidThrough"].toString(), /* string */
        accJson["CardHolder"].toString(),   /* string */
        accJson["AnnualFee"].toInt(),       /* int number */
        accJson["Notes"].toString(),        /* string */
        accJson["RecordTime"].toInt(),      /* string */
        queryCodeDecryptedStr               /* base64 string->QByteArray->string */
    };
  }
  qDebug("%d records read in total", mAccsList.size());
  return true;
}

// key used to decrypt password from json base64 string
bool JsonReader::SetDecryptKey16(const QString& decryptKey) {
  mDecryptKey = decryptKey;
  if (mEncryptKey.isEmpty()) {  // if EncryptKey16 is empty use key same as DecryptKey16.
    SetEncryptKey16(decryptKey);
  }
  return true;
}

// key used to encrypt password to json base64 string
bool JsonReader::SetEncryptKey16(const QString& newEncryptKey) {
  mEncryptKey = newEncryptKey;
  return true;
}

// 1. when nothing load, reject save directly
// 2. If Decrypt Key Incorrect, reject save directly
bool JsonReader::Save() {
  if (PDBOOK_TABLE_NAME.isEmpty()) {
    qWarning("jsonPath[%s] is empty, not load now", qPrintable(PDBOOK_TABLE_NAME));
    return false;
  }

  if (!IsDecryptKeyCorrect()) {
    qWarning("Decrypt key is incorrect, reject save");
    return false;
  }

  if (!isEncryptKeyLengthValid()) {
    qWarning("Encrypt key length:%d is invalid.", mEncryptKey.size());
    return false;
  }

  QFile jsonFile(PDBOOK_TABLE_NAME);
  if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    jsonFile.close();
    qCritical("Open file[%s] to write failed", qPrintable(PDBOOK_TABLE_NAME));
    return false;
  }

  const bool decryptKeyHashInFile = PreferenceSettings().contains("DECRYPT_KEY_HASH");
  const bool encryptKeyChange = IsEncryptKeyDiffersFromDecryptKey();
  if (!decryptKeyHashInFile || encryptKeyChange) {
    QCryptographicHash md5hash{QCryptographicHash::Algorithm::Md5};
    md5hash.addData(mEncryptKey.toUtf8());
    PreferenceSettings().setValue("DECRYPT_KEY_HASH", md5hash.result());
    qDebug("DECRYPT_KEY_HASH update succeed decryptKeyHashInFile: %d, encryptKeyChange: %d", decryptKeyHashInFile, encryptKeyChange);
  }

  MyEncrypt encrypt{mEncryptKey};
  QJsonArray jsonArray;
  for (const AccountInfo& acc : mAccsList) {
    QString accEncyptedStr = encrypt.Encrypt2Base64Str(acc.Account);
    QString withdrawEncyptedStr = encrypt.Encrypt2Base64Str(acc.Withdraw);
    QString cvv2EncyptedStr = encrypt.Encrypt2Base64Str(acc.CVV2);
    QString queryCode2EncyptedStr = encrypt.Encrypt2Base64Str(acc.QueryCode);
    QVariantHash vh{
        {"Issuer", acc.Issuer},               //
        {"Type", acc.Type},                   //
        {"Account", accEncyptedStr},          // string->QByteArray->base64 string
        {"Withdraw", withdrawEncyptedStr},    // string->QByteArray->base64 string
        {"CVV2", cvv2EncyptedStr},            // int->QByteArray
        {"ValidThrough", acc.ValidThrough},   //
        {"CardHolder", acc.CardHolder},       //
        {"AnnualFee", acc.AnnualFee},         //
        {"Notes", acc.Notes},                 //
        {"RecordTime", acc.RecordTime},       //
        {"QueryCode", queryCode2EncyptedStr}  //
    };
    QJsonValue jsonVal{QJsonObject::fromVariantHash(vh)};
    jsonArray.append(jsonVal);
  }

  QJsonDocument document;
  document.setArray(jsonArray);
  const auto& byteArray = document.toJson(QJsonDocument::JsonFormat::Indented);
  QTextStream out(&jsonFile);
  out.setCodec("UTF-8");
  out << byteArray;
  jsonFile.close();
  qDebug("%d record update into %s succced", jsonArray.size(), qPrintable(PDBOOK_TABLE_NAME));
  return true;
}
