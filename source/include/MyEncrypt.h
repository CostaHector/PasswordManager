#ifndef MYENCRYPT_H
#define MYENCRYPT_H

#include "../qaesencryption.h"

class MyEncrypt {
 public:
  MyEncrypt(const QString& key);
  QByteArray Encrypt(const QString& rawText);
  QByteArray Encrypt(int rawInt);
  QString Encrypt2Base64Str(const QString& rawText);
  QString Encrypt2Base64Str(int rawInt);

  QString DecryptString(const QByteArray& outECB128);
  int DecryptInt(const QByteArray& outECB128);
  QString DecryptStringFromBase64Str(const QString& encryptedBase64Str);
  int DecryptIntFromBase64Str(const QString& encryptedBase64Str);

 private:
  QByteArray mkey16;
  QAESEncryption encryption{QAESEncryption::AES_128, QAESEncryption::ECB};
};

#endif // MYENCRYPT_H
