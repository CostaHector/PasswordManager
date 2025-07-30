#include "include/MyEncrypt.h"

MyEncrypt::MyEncrypt(const QString& key) {
  if (key.size() > 16) {
    qCritical("key length[%d] longer than 16", key.size());
    return;
  }
  mkey16 = QByteArray{key.toUtf8()};
  mkey16.append(16 - mkey16.size(), 0);
}

QByteArray MyEncrypt::Encrypt(const QString& rawText) {
  if (mkey16.isEmpty()) {
    qCritical("mkey16 is empty");
    return {};
  }
  int min2Power = 8;  // 大于等于in_text.size()的最小2的整数幂次
  while (rawText.size() > min2Power) {
    min2Power *= 2;
  }
  QByteArray in{rawText.toUtf8()};
  in.append(min2Power - in.size(), 0);
  QByteArray encrypted = encryption.encode(in, mkey16);
  return encrypted;
}

QByteArray MyEncrypt::Encrypt(int rawInt) {
  if (mkey16.isEmpty()) {
    qCritical("mkey16 is empty");
    return {};
  }
  QByteArray inBytes;
  inBytes.setNum(rawInt);
  inBytes.append(16 - inBytes.size(), 0);
  return encryption.encode(inBytes, mkey16);
}

QString MyEncrypt::Encrypt2Base64Str(const QString& rawText) {
  if (rawText.isEmpty()) {
    return {};
  }
  QByteArray encryptedBA = Encrypt(rawText);
  return encryptedBA.toBase64();
}

QString MyEncrypt::Encrypt2Base64Str(int rawInt) {
  if (rawInt == 0) {
    return {};
  }
  QByteArray encryptedBA = Encrypt(rawInt);
  return encryptedBA.toBase64();
}

QString MyEncrypt::DecryptString(const QByteArray& outECB128) {
  if (mkey16.isEmpty()) {
    qCritical("mkey16 is empty");
    return {};
  }
  QByteArray decoded = encryption.decode(outECB128, mkey16);
  return QString::fromUtf8(decoded);
}

int MyEncrypt::DecryptInt(const QByteArray& outECB128) {
  if (mkey16.isEmpty()) {
    qCritical("mkey16 is empty");
    return -1;
  }
  QByteArray decoded = encryption.decode(outECB128, mkey16);
  return decoded.toInt();
}

QString MyEncrypt::DecryptStringFromBase64Str(const QString& encryptedBase64Str) {
  if (encryptedBase64Str.isEmpty()) {
    return {};
  }
  QByteArray accEncryptedBA = QByteArray::fromBase64(encryptedBase64Str.toUtf8(), QByteArray::Base64Encoding);
  return DecryptString(accEncryptedBA);
}

int MyEncrypt::DecryptIntFromBase64Str(const QString& encryptedBase64Str) {
  if (encryptedBase64Str.isEmpty()) {
    return 0;
  }
  QByteArray cvv2EncryptedBA = QByteArray::fromBase64(encryptedBase64Str.toUtf8(), QByteArray::Base64Encoding);
  return DecryptInt(cvv2EncryptedBA);
}
